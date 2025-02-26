
#ifdef HAVE_DIX_CONFIG_H
#include <dix-config.h>
#endif

#ifndef __MIOVERLAY_H
#define __MIOVERLAY_H

typedef void (*miOverlayTransFunc) (ScreenPtr, int, BoxPtr);
typedef Bool (*miOverlayInOverlayFunc) (WindowPtr);

#endif                          /* __MIOVERLAY_H */
