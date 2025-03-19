#include <dix-config.h>

#include <stdio.h>
#include <X11/Xmd.h>

#include "dix/dix_priv.h"
#include "dix/selection_priv.h"
#include "include/os.h"
#include "miext/extinit_priv.h"

#include "namespace.h"
#include "hooks.h"

Bool noNamespaceExtension = TRUE;

DevPrivateKeyRec namespaceClientPrivKeyRec = { 0 };

void
NamespaceExtensionInit(void)
{
    XNS_LOG("initializing namespace extension ...\n");

    /* load configuration */
    if (!XnsLoadConfig()) {
        XNS_LOG("No config file. disabling Xns extension\n");
        return;
    }

    if (!(dixRegisterPrivateKey(&namespaceClientPrivKeyRec, PRIVATE_CLIENT,
            sizeof(struct XnamespaceClientPriv)) &&
          AddCallback(&ClientStateCallback, hookClientState, NULL) &&
          AddCallback(&PostInitRootWindowCallback, hookInitRootWindow, NULL) &&
          AddCallback(&SelectionFilterCallback, hookSelectionFilter, NULL)))
        FatalError("NamespaceExtensionInit: allocation failure\n");

    /* Do the serverClient */
    struct XnamespaceClientPriv *srv = XnsClientPriv(serverClient);
    *srv = (struct XnamespaceClientPriv) { .isServer = TRUE };
    XnamespaceAssignClient(srv, &ns_root);
}

void XnamespaceAssignClient(struct XnamespaceClientPriv *priv, struct Xnamespace *newns)
{
    if (priv->ns != NULL)
        priv->ns->refcnt--;

    priv->ns = newns;

    if (newns != NULL)
        newns->refcnt++;
}

void XnamespaceAssignClientByName(struct XnamespaceClientPriv *priv, const char *name)
{
    struct Xnamespace *newns = XnsFindByName(name);

    if (newns == NULL)
        newns = &ns_anon;

    XnamespaceAssignClient(priv, newns);
}

struct Xnamespace* XnsFindByAuth(size_t szAuthProto, const char* authProto, size_t szAuthToken, const char* authToken)
{
    struct Xnamespace *walk;
    xorg_list_for_each_entry(walk, &ns_list, entry) {
        int protoLen = walk->authProto ? strlen(walk->authProto) : 0;
        if ((protoLen == szAuthProto) &&
            (walk->authTokenLen == szAuthToken) &&
            (memcmp(walk->authTokenData, authToken, szAuthToken)==0) &&
            (memcmp(walk->authProto, authProto, szAuthProto)==0))
            return walk;
    }

    // default to anonymous if credentials aren't assigned to specific NS
    return &ns_anon;
}
