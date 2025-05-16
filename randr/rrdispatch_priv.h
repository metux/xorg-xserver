/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 *
 * @brief: prototypes for the individual request handlers
 */
#ifndef _XSERVER_RANDR_RRDISPATCH_H
#define _XSERVER_RANDR_RRDISPATCH_H

#include "include/dix.h"

/* screen related dispatch */
int ProcRRGetScreenSizeRange(ClientPtr client);
int ProcRRSetScreenSize(ClientPtr client);
int ProcRRGetScreenResources(ClientPtr client);
int ProcRRGetScreenResourcesCurrent(ClientPtr client);
int ProcRRSetScreenConfig(ClientPtr client);
int ProcRRGetScreenInfo(ClientPtr client);

#endif /* _XSERVER_RANDR_RRDISPATCH_H */
