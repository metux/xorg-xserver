/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 */
#ifndef _XSERVER_DIXGRABS_PRIV_H
#define _XSERVER_DIXGRABS_PRiV_H

#include <X11/extensions/XIproto.h>

#include "misc.h"
#include "window.h"
#include "input.h"
#include "cursor.h"

struct _GrabParameters;

void PrintDeviceGrabInfo(DeviceIntPtr dev);
void UngrabAllDevices(Bool kill_client);

GrabPtr AllocGrab(const GrabPtr src);
void FreeGrab(GrabPtr grab);
Bool CopyGrab(GrabPtr dst, const GrabPtr src);

GrabPtr CreateGrab(int client,
                   DeviceIntPtr device,
                   DeviceIntPtr modDevice,
                   WindowPtr window,
                   enum InputLevel grabtype,
                   GrabMask *mask,
                   struct _GrabParameters *param,
                   int type,
                   KeyCode keybut,
                   WindowPtr confineTo,
                   CursorPtr cursor);

Bool GrabIsPointerGrab(GrabPtr grab);
Bool GrabIsKeyboardGrab(GrabPtr grab);
Bool GrabIsGestureGrab(GrabPtr grab);

int DeletePassiveGrab(void *value, XID id);

Bool GrabMatchesSecond(GrabPtr pFirstGrab,
                       GrabPtr pSecondGrab,
                       Bool ignoreDevice);

int AddPassiveGrabToList(ClientPtr client, GrabPtr pGrab);

Bool DeletePassiveGrabFromList(GrabPtr pMinuendGrab);

#endif /* _XSERVER_DIXGRABS_PRIV_H */
