/*

Copyright 1993 by Davor Matic

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation.  Davor Matic makes no representations about
the suitability of this software for any purpose.  It is provided "as
is" without express or implied warranty.

*/

#ifndef XNESTWINDOW_H
#define XNESTWINDOW_H

#include <X11/Xdefs.h>
#include <xcb/xcb.h>

typedef struct {
    xcb_window_t window;
    xcb_window_t parent;
    int x;
    int y;
    unsigned int width;
    unsigned int height;
    unsigned int border_width;
    xcb_window_t sibling_above;
    RegionPtr bounding_shape;
    RegionPtr clip_shape;
} xnestPrivWin;

typedef struct {
    WindowPtr pWin;
    xcb_window_t window;
} xnestWindowMatch;

extern DevPrivateKeyRec xnestWindowPrivateKeyRec;

#define xnestWindowPrivateKey (&xnestWindowPrivateKeyRec)

#define xnestWindowPriv(pWin) ((xnestPrivWin *) \
    dixLookupPrivate(&(pWin)->devPrivates, xnestWindowPrivateKey))

#define xnestWindow(pWin) (xnestWindowPriv(pWin)->window)

#define xnestWindowParent(pWin) \
  ((pWin)->parent ? \
   xnestWindow((pWin)->parent) : \
   xnestDefaultWindows[pWin->drawable.pScreen->myNum])

#define xnestWindowSiblingAbove(pWin) \
  ((pWin)->prevSib ? xnestWindow((pWin)->prevSib) : XCB_WINDOW_NONE)

#define xnestWindowSiblingBelow(pWin) \
  ((pWin)->nextSib ? xnestWindow((pWin)->nextSib) : XCB_WINDOW_NONE)

WindowPtr xnestWindowPtr(xcb_window_t window);
Bool xnestCreateWindow(WindowPtr pWin);
Bool xnestDestroyWindow(WindowPtr pWin);
Bool xnestPositionWindow(WindowPtr pWin, int x, int y);
void xnestConfigureWindow(WindowPtr pWin, unsigned int mask);
Bool xnestChangeWindowAttributes(WindowPtr pWin, unsigned long mask);
Bool xnestRealizeWindow(WindowPtr pWin);
Bool xnestUnrealizeWindow(WindowPtr pWin);
void xnestCopyWindow(WindowPtr pWin, xPoint oldOrigin, RegionPtr oldRegion);
void xnestClipNotify(WindowPtr pWin, int dx, int dy);
void xnestSetShape(WindowPtr pWin, int kind);
void xnestShapeWindow(WindowPtr pWin);

/* ScreenRec operations */
void xnest_screen_ClearToBackground(WindowPtr pWin, int x, int y, int w, int h, Bool generateExposures);

#endif                          /* XNESTWINDOW_H */
