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

#ifndef XNESTFONT_H
#define XNESTFONT_H

#include <X11/Xdefs.h>

#include <xcb/xcb.h>

extern int xnestFontPrivateIndex;

#define xnestFontPriv(pFont) \
  ((xnestPrivFont *)FontGetPrivate(pFont, xnestFontPrivateIndex))

Bool xnestRealizeFont(ScreenPtr pScreen, FontPtr pFont);
Bool xnestUnrealizeFont(ScreenPtr pScreen, FontPtr pFont);

#endif                          /* XNESTFONT_H */
