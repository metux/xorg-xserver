#include <dix-config.h>

#include <string.h>
#include <X11/Xdefs.h>

#include "namespace.h"

struct Xnamespace ns_root = {
    .allowMouseMotion = TRUE,
    .builtin = TRUE,
    .name = NS_NAME_ROOT,
    .refcnt = 1,
    .superPower = TRUE,
};

struct Xnamespace ns_anon = {
    .builtin = TRUE,
    .name = NS_NAME_ANONYMOUS,
    .refcnt = 1,
};

struct xorg_list ns_list = { 0 };

Bool XnsLoadConfig(void)
{
    xorg_list_init(&ns_list);
    xorg_list_add(&ns_root.entry, &ns_list);
    xorg_list_add(&ns_anon.entry, &ns_list);

    return TRUE;
}

struct Xnamespace *XnsFindByName(const char* name) {
    struct Xnamespace *walk;
    xorg_list_for_each_entry(walk, &ns_list, entry) {
        if (strcmp(walk->name, name)==0)
            return walk;
    }
    return NULL;
}
