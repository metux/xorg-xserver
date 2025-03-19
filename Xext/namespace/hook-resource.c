#define HOOK_NAME "resource"

#include <dix-config.h>

#include "dix/dix_priv.h"
#include "Xext/xacestr.h"

#include "namespace.h"
#include "hooks.h"

void hookResourceAccess(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XNS_HOOK_HEAD(XaceResourceAccessRec);

    // special filtering for windows: block transparency for untrusted clients
    if (param->rtype == X11_RESTYPE_WINDOW) {
        WindowPtr pWindow = (WindowPtr) param->res;
        if (param->access_mode & DixCreateAccess) {
            if (!subj->ns->allowTransparency) {
                pWindow->forcedBG = TRUE;
                goto pass;
            }
        }
    }

pass:
    // request is passed as it is (or already had been rewritten)
    param->status = Success;
}
