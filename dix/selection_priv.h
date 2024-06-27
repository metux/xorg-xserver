/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 */

#ifndef _XSERVER_DIX_SELECTION_PRIV_H
#define _XSERVER_DIX_SELECTION_PRIV_H

#include <X11/Xdefs.h>
#include <X11/Xproto.h>

#include "include/dixstruct.h"
#include "include/privates.h"

typedef struct _Selection {
    Atom selection;
    TimeStamp lastTimeChanged;
    Window window;
    WindowPtr pWin;
    ClientPtr client;
    struct _Selection *next;
    PrivateRec *devPrivates;
} Selection;

typedef enum {
    SelectionSetOwner,
    SelectionWindowDestroy,
    SelectionClientClose
} SelectionCallbackKind;

typedef struct {
    struct _Selection *selection;
    ClientPtr client;
    SelectionCallbackKind kind;
} SelectionInfoRec;


extern Selection *CurrentSelections;

extern CallbackListPtr SelectionCallback;

int dixLookupSelection(Selection **result,
                       Atom name,
                       ClientPtr client,
                       Mask access_mode);

void InitSelections(void);
void DeleteWindowFromAnySelections(WindowPtr pWin);
void DeleteClientFromAnySelections(ClientPtr client);

#endif /* _XSERVER_DIX_SELECTION_PRIV_H */
