/************************************************************

Copyright 1989, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.

Copyright 1989 by Hewlett-Packard Company, Palo Alto, California.

			All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Hewlett-Packard not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

HEWLETT-PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
HEWLETT-PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

********************************************************/

/***********************************************************************
 *
 * Function to return the dont-propagate-list for an extension device.
 *
 */

#include <dix-config.h>

#include <X11/extensions/XI.h>
#include <X11/extensions/XIproto.h>

#include "dix/dix_priv.h"

#include "inputstr.h"           /* DeviceIntPtr      */
#include "windowstr.h"          /* window structs    */
#include "exglobals.h"
#include "swaprep.h"
#include "getprop.h"

extern XExtEventInfo EventInfo[];
extern int ExtEventIndex;

/***********************************************************************
 *
 * Handle a request from a client with a different byte order.
 *
 */

int _X_COLD
SProcXGetDeviceDontPropagateList(ClientPtr client)
{
    REQUEST(xGetDeviceDontPropagateListReq);
    REQUEST_SIZE_MATCH(xGetDeviceDontPropagateListReq);
    swapl(&stuff->window);
    return (ProcXGetDeviceDontPropagateList(client));
}

/***********************************************************************
 *
 * This procedure lists the input devices available to the server.
 *
 */

int
ProcXGetDeviceDontPropagateList(ClientPtr client)
{
    CARD16 count = 0;
    int i, rc;
    XEventClass *buf = NULL, *tbuf;
    WindowPtr pWin;
    OtherInputMasks *others;

    REQUEST(xGetDeviceDontPropagateListReq);
    REQUEST_SIZE_MATCH(xGetDeviceDontPropagateListReq);

    xGetDeviceDontPropagateListReply rep = {
        .repType = X_Reply,
        .RepType = X_GetDeviceDontPropagateList,
        .sequenceNumber = client->sequence,
    };

    rc = dixLookupWindow(&pWin, stuff->window, client, DixGetAttrAccess);
    if (rc != Success)
        return rc;

    if ((others = wOtherInputMasks(pWin)) != 0) {
        for (i = 0; i < EMASKSIZE; i++)
            ClassFromMask(NULL, others->dontPropagateMask[i], i, &count, COUNT);
        if (count) {
            rep.count = count;
            buf = calloc(rep.count, sizeof(XEventClass));
            if (!buf)
                return BadAlloc;
            rep.length = bytes_to_int32(rep.count * sizeof(XEventClass));

            tbuf = buf;
            for (i = 0; i < EMASKSIZE; i++)
                tbuf = ClassFromMask(tbuf, others->dontPropagateMask[i], i,
                                     NULL, CREATE);
        }
    }

    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.length);
        swaps(&rep.count);
    }
    WriteToClient(client, sizeof(xGetDeviceDontPropagateListReply), &rep);

    if (count) {
        client->pSwapReplyFunc = (ReplySwapPtr) Swap32Write;
        WriteSwappedDataToClient(client, count * sizeof(XEventClass), buf);
        free(buf);
    }
    return Success;
}

/***********************************************************************
 *
 * This procedure gets a list of event classes from a mask word.
 * A single mask may translate to more than one event class.
 *
 */

XEventClass
    * ClassFromMask(XEventClass * buf, Mask mask, int maskndx, CARD16 *count,
                    int mode)
{
    int i, j;
    int id = maskndx;
    Mask tmask = 0x80000000;

    for (i = 0; i < 32; i++, tmask >>= 1)
        if (tmask & mask) {
            for (j = 0; j < ExtEventIndex; j++)
                if (EventInfo[j].mask == tmask) {
                    if (mode == COUNT)
                        (*count)++;
                    else
                        *buf++ = (id << 8) | EventInfo[j].type;
                }
        }
    return buf;
}
