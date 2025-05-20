/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 */
#ifndef _XORG_XF86OPTION_PRIV_H
#define _XORG_XF86OPTION_PRIV_H

#include "xf86Opt.h"

void xf86OptionListReport(XF86OptionPtr parm);
void xf86MarkOptionUsed(XF86OptionPtr option);
void xf86MarkOptionUsedByName(XF86OptionPtr options, const char *name);

#endif /* _XORG_XF86OPTION_PRIV_H */
