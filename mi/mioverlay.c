
#include <dix-config.h>

#include <X11/X.h>

#include "dix/cursor_priv.h"
#include "mi/mi_priv.h"

#include "scrnintstr.h"
#include <X11/extensions/shapeproto.h>
#include "validate.h"
#include "windowstr.h"
#include "gcstruct.h"
#include "regionstr.h"
#include "privates.h"
#include "mivalidate.h"
#include "mioverlay.h"
#include "migc.h"

#include "globals.h"

typedef struct {
    RegionRec exposed;
    RegionRec borderExposed;
    RegionPtr borderVisible;
    DDXPointRec oldAbsCorner;
} miOverlayValDataRec, *miOverlayValDataPtr;

typedef struct _TreeRec {
    WindowPtr pWin;
    struct _TreeRec *parent;
    struct _TreeRec *firstChild;
    struct _TreeRec *lastChild;
    struct _TreeRec *prevSib;
    struct _TreeRec *nextSib;
    RegionRec borderClip;
    RegionRec clipList;
    unsigned visibility;
    miOverlayValDataPtr valdata;
} miOverlayTreeRec, *miOverlayTreePtr;

typedef struct {
    miOverlayTreePtr tree;
} miOverlayWindowRec, *miOverlayWindowPtr;

typedef struct {
    CloseScreenProcPtr CloseScreen;
    CreateWindowProcPtr CreateWindow;
    DestroyWindowProcPtr DestroyWindow;
    UnrealizeWindowProcPtr UnrealizeWindow;
    RealizeWindowProcPtr RealizeWindow;
    miOverlayTransFunc MakeTransparent;
    miOverlayInOverlayFunc InOverlay;
    Bool underlayMarked;
    Bool copyUnderlay;
} miOverlayScreenRec, *miOverlayScreenPtr;

static DevPrivateKeyRec miOverlayWindowKeyRec;

#define miOverlayWindowKey (&miOverlayWindowKeyRec)
static DevPrivateKeyRec miOverlayScreenKeyRec;

#define miOverlayScreenKey (&miOverlayScreenKeyRec)

static void MarkUnderlayWindow(WindowPtr);
static Bool CollectUnderlayChildrenRegions(WindowPtr, RegionPtr);

#define MIOVERLAY_GET_SCREEN_PRIVATE(pScreen) ((miOverlayScreenPtr) \
	dixLookupPrivate(&(pScreen)->devPrivates, miOverlayScreenKey))
#define MIOVERLAY_GET_WINDOW_PRIVATE(pWin) ((miOverlayWindowPtr) \
	dixLookupPrivate(&(pWin)->devPrivates, miOverlayWindowKey))
#define MIOVERLAY_GET_WINDOW_TREE(pWin) \
	(MIOVERLAY_GET_WINDOW_PRIVATE(pWin)->tree)

#define MARK_UNDERLAY(w) MarkUnderlayWindow(w)

/*  We need this as an addition since the xf86 common code doesn't
    know about the second tree which is static to this file.  */

void
miOverlaySetRootClip(ScreenPtr pScreen, Bool enable)
{
    WindowPtr pRoot = pScreen->root;
    miOverlayTreePtr pTree = MIOVERLAY_GET_WINDOW_TREE(pRoot);

    MARK_UNDERLAY(pRoot);

    if (enable) {
        BoxRec box;

        box.x1 = 0;
        box.y1 = 0;
        box.x2 = pScreen->width;
        box.y2 = pScreen->height;

        RegionReset(&pTree->borderClip, &box);
    }
    else
        RegionEmpty(&pTree->borderClip);

    RegionBreak(&pTree->clipList);
}

/****************************************************************/

/* not used */
Bool
miOverlayGetPrivateClips(WindowPtr pWin,
                         RegionPtr *borderClip, RegionPtr *clipList)
{
    miOverlayTreePtr pTree = MIOVERLAY_GET_WINDOW_TREE(pWin);

    if (pTree) {
        *borderClip = &(pTree->borderClip);
        *clipList = &(pTree->clipList);
        return TRUE;
    }

    *borderClip = *clipList = NULL;

    return FALSE;
}

Bool
miOverlayCopyUnderlay(ScreenPtr pScreen)
{
    return MIOVERLAY_GET_SCREEN_PRIVATE(pScreen)->copyUnderlay;
}

void
miOverlayComputeCompositeClip(GCPtr pGC, WindowPtr pWin)
{
    miOverlayTreePtr pTree = MIOVERLAY_GET_WINDOW_TREE(pWin);
    RegionPtr pregWin;
    Bool freeTmpClip, freeCompClip;

    if (!pTree) {
        miComputeCompositeClip(pGC, &pWin->drawable);
        return;
    }

    if (pGC->subWindowMode == IncludeInferiors) {
        pregWin = RegionCreate(NullBox, 1);
        freeTmpClip = TRUE;
        if (pWin->parent || (screenIsSaved != SCREEN_SAVER_ON) ||
            !HasSaverWindow(pGC->pScreen)) {
            RegionIntersect(pregWin, &pTree->borderClip, &pWin->winSize);
        }
    }
    else {
        pregWin = &pTree->clipList;
        freeTmpClip = FALSE;
    }
    freeCompClip = pGC->freeCompClip;
    if (!pGC->clientClip) {
        if (freeCompClip)
            RegionDestroy(pGC->pCompositeClip);
        pGC->pCompositeClip = pregWin;
        pGC->freeCompClip = freeTmpClip;
    }
    else {
        RegionTranslate(pGC->clientClip,
                        pWin->drawable.x + pGC->clipOrg.x,
                        pWin->drawable.y + pGC->clipOrg.y);

        if (freeCompClip) {
            RegionIntersect(pGC->pCompositeClip, pregWin, pGC->clientClip);
            if (freeTmpClip)
                RegionDestroy(pregWin);
        }
        else if (freeTmpClip) {
            RegionIntersect(pregWin, pregWin, pGC->clientClip);
            pGC->pCompositeClip = pregWin;
        }
        else {
            pGC->pCompositeClip = RegionCreate(NullBox, 0);
            RegionIntersect(pGC->pCompositeClip, pregWin, pGC->clientClip);
        }
        pGC->freeCompClip = TRUE;
        RegionTranslate(pGC->clientClip,
                        -(pWin->drawable.x + pGC->clipOrg.x),
                        -(pWin->drawable.y + pGC->clipOrg.y));
    }
}

Bool
miOverlayCollectUnderlayRegions(WindowPtr pWin, RegionPtr *region)
{
    miOverlayTreePtr pTree = MIOVERLAY_GET_WINDOW_TREE(pWin);

    if (pTree) {
        *region = &pTree->borderClip;
        return FALSE;
    }

    *region = RegionCreate(NullBox, 0);

    CollectUnderlayChildrenRegions(pWin, *region);

    return TRUE;
}

static Bool
CollectUnderlayChildrenRegions(WindowPtr pWin, RegionPtr pReg)
{
    WindowPtr pChild;
    miOverlayTreePtr pTree;
    Bool hasUnderlay;

    if (!(pChild = pWin->firstChild))
        return FALSE;

    hasUnderlay = FALSE;

    while (1) {
        if ((pTree = MIOVERLAY_GET_WINDOW_TREE(pChild))) {
            RegionAppend(pReg, &pTree->borderClip);
            hasUnderlay = TRUE;
        }
        else if (pChild->firstChild) {
            pChild = pChild->firstChild;
            continue;
        }

        while (!pChild->nextSib && (pWin != pChild))
            pChild = pChild->parent;

        if (pChild == pWin)
            break;

        pChild = pChild->nextSib;
    }

    if (hasUnderlay) {
        Bool overlap;

        RegionValidate(pReg, &overlap);
    }

    return hasUnderlay;
}

static void
MarkUnderlayWindow(WindowPtr pWin)
{
    miOverlayTreePtr pTree = MIOVERLAY_GET_WINDOW_TREE(pWin);

    if (pTree->valdata)
        return;
    pTree->valdata =
        (miOverlayValDataPtr) XNFalloc(sizeof(miOverlayValDataRec));
    pTree->valdata->oldAbsCorner.x = pWin->drawable.x;
    pTree->valdata->oldAbsCorner.y = pWin->drawable.y;
    pTree->valdata->borderVisible = NullRegion;
}
