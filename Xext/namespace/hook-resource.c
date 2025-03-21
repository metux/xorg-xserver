#define HOOK_NAME "resource"

#include <dix-config.h>

#include "dix/dix_priv.h"
#include "dix/window_priv.h"
#include "Xext/xacestr.h"

#include "namespace.h"
#include "hooks.h"

void hookResourceAccess(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XNS_HOOK_HEAD(XaceResourceAccessRec);
    ClientPtr owner = dixLookupXIDOwner(param->id);
    struct XnamespaceClientPriv *obj = XnsClientPriv(owner);

    // special filtering for windows: block transparency for untrusted clients
    if (param->rtype == X11_RESTYPE_WINDOW) {
        WindowPtr pWindow = (WindowPtr) param->res;
        if (param->access_mode & DixCreateAccess) {
            if (!subj->ns->allowTransparency) {
                pWindow->forcedBG = TRUE;
            }
        }
    }

    // resource access inside same container is always permitted
    if (XnsClientSameNS(subj, obj))
        goto pass;

    char accModeStr[128];
    LookupDixAccessName(param->access_mode, (char*)&accModeStr, sizeof(accModeStr));

    XNS_HOOK_LOG("BLOCKED access 0x%07lx %s to %s 0x%06lx of client %d @ %s\n",
        (unsigned long)param->access_mode,
        accModeStr,
        LookupResourceName(param->rtype),
        (unsigned long)param->id,
        owner->index, // resource owner
        obj->ns->name);

    param->status = BadAccess;
    return;

pass:
    // request is passed as it is (or already had been rewritten)
    param->status = Success;
}
