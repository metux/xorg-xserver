#include <dix-config.h>

#include <X11/Xmd.h>

#include "include/os.h"
#include "miext/extinit_priv.h"

#include "namespace.h"

Bool noNamespaceExtension = TRUE;

void
NamespaceExtensionInit(void)
{
    XNS_LOG("initializing namespace extension ...\n");
}
