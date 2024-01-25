#ifndef __XSERVER_NAMESPACE_H
#define __XSERVER_NAMESPACE_H

#include <stdio.h>
#include <X11/Xmd.h>

#define XNS_LOG(...) do { printf("XNS "); printf(__VA_ARGS__); } while (0)

static inline Bool streq(const char *a, const char *b)
{
    if (!a && !b)
        return TRUE;
    if (!a || !b)
        return FALSE;
    return (strcmp(a,b) == 0);
}

#endif /* __XSERVER_NAMESPACE_H */
