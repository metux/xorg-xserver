/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 */
#ifndef _XSERVER_XF86_PRIV_H
#define _XSERVER_XF86_PRIV_H

#include "xf86.h"

extern Bool xf86DoConfigure;
extern Bool xf86DoConfigurePass1;
extern Bool xf86ProbeIgnorePrimary;

void xf86LockZoom(ScreenPtr pScreen, int lock);
void xf86InitViewport(ScrnInfoPtr pScr);
void xf86ZoomViewport(ScreenPtr pScreen, int zoom);

#endif /* _XSERVER_XF86_PRIV_H */
