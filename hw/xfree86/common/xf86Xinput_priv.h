/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 */
#ifndef _XSERVER__XF86XINPUT_H
#define _XSERVER__XF86XINPUT_H

#include "xf86Xinput.h"

extern InputInfoPtr xf86InputDevs;

int xf86NewInputDevice(InputInfoPtr pInfo, DeviceIntPtr *pdev, BOOL is_auto);
InputInfoPtr xf86AllocateInput(void);

#endif /* _XSERVER__XF86XINPUT_H */
