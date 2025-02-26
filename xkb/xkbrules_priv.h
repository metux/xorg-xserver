/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 */
#ifndef _XSERVER_XKB_XKBRULES_PRIV_H
#define _XSERVER_XKB_XKBRULES_PRIV_H

#include <stdio.h>
#include <X11/Xdefs.h>

#include "include/xkbrules.h"

typedef struct _XkbRF_Group {
    int number;
    const char *name;
    char *words;
} XkbRF_GroupRec, *XkbRF_GroupPtr;

typedef struct _XkbRF_Rules {
    unsigned short sz_rules;
    unsigned short num_rules;
    XkbRF_RulePtr rules;
    unsigned short sz_groups;
    unsigned short num_groups;
    XkbRF_GroupPtr groups;
} XkbRF_RulesRec, *XkbRF_RulesPtr;

struct _XkbComponentNames;

Bool XkbRF_GetComponents(XkbRF_RulesPtr rules,
                         XkbRF_VarDefsPtr var_defs,
                         struct _XkbComponentNames *names);

Bool XkbRF_LoadRules(FILE *file, XkbRF_RulesPtr rules);

XkbRF_RulesPtr XkbRF_Create(void);

void XkbRF_Free(XkbRF_RulesPtr rules, Bool freeRules);

#endif /* _XSERVER_XKB_XKBRULES_PRIV_H */
