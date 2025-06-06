/*
 * SBUS bus-specific declarations
 *
 * Copyright (C) 2000 Jakub Jelinek (jakub@redhat.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * JAKUB JELINEK BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _XF86_SBUSBUS_H
#define _XF86_SBUSBUS_H

#include "xf86str.h"

#define SBUS_DEVICE_CG3		0x0003
#define SBUS_DEVICE_CG6		0x0005
#define SBUS_DEVICE_CG14	0x0008
#define SBUS_DEVICE_LEO		0x0009
#define SBUS_DEVICE_TCX		0x000a
#define SBUS_DEVICE_FFB		0x000b

typedef struct sbus_prom_node {
    int node;
    /* Because of misdesigned openpromio */
    int cookie[2];
} sbusPromNode, *sbusPromNodePtr;

typedef struct sbus_device {
    int devId;
    int fbNum;
    int fd;
    int width, height;
    sbusPromNode node;
    const char *descr;
    const char *device;
} sbusDevice, *sbusDevicePtr;

extern _X_EXPORT int xf86MatchSbusInstances(const char *driverName,
                                            int sbusDevId, GDevPtr * devList,
                                            int numDevs, DriverPtr drvp,
                                            int **foundEntities);
extern _X_EXPORT sbusDevicePtr xf86GetSbusInfoForEntity(int entityIndex);
extern _X_EXPORT void xf86SbusUseBuiltinMode(ScrnInfoPtr pScrn,
                                             sbusDevicePtr psdp);
extern _X_EXPORT void *xf86MapSbusMem(sbusDevicePtr psdp,
                                        unsigned long offset,
                                        unsigned long size);
extern _X_EXPORT void xf86UnmapSbusMem(sbusDevicePtr psdp, void *addr,
                                       unsigned long size);
extern _X_EXPORT void xf86SbusHideOsHwCursor(sbusDevicePtr psdp);
extern _X_EXPORT void xf86SbusSetOsHwCursorCmap(sbusDevicePtr psdp, int bg,
                                                int fg);
extern _X_EXPORT Bool xf86SbusHandleColormaps(ScreenPtr pScreen,
                                              sbusDevicePtr psdp);

extern _X_EXPORT int sparcPromInit(void);
extern _X_EXPORT void sparcPromClose(void);
extern _X_EXPORT int sparcPromGetBool(sbusPromNodePtr pnode, const char *prop);

#endif                          /* _XF86_SBUSBUS_H */
