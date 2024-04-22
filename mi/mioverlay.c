
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

void
miOverlaySetTransFunction(ScreenPtr pScreen, miOverlayTransFunc transFunc)
{
    MIOVERLAY_GET_SCREEN_PRIVATE(pScreen)->MakeTransparent = transFunc;
}

Bool
miOverlayCopyUnderlay(ScreenPtr pScreen)
{
    return MIOVERLAY_GET_SCREEN_PRIVATE(pScreen)->copyUnderlay;
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
