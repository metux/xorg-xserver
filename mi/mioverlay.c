
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
