/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 */
#ifndef _XSERVER_XKB_XKBRULES_PRIV_H
#define _XSERVER_XKB_XKBRULES_PRIV_H

#include <stdio.h>
#include <X11/Xdefs.h>

#include "include/xkbrules.h"

struct _XkbComponentNames;

Bool XkbRF_GetComponents(XkbRF_RulesPtr rules,
                         XkbRF_VarDefsPtr var_defs,
                         struct _XkbComponentNames *names);

Bool XkbRF_LoadRules(FILE *file, XkbRF_RulesPtr rules);

#endif /* _XSERVER_XKB_XKBRULES_PRIV_H */
