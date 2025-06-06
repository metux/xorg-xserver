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
 * Request to change the button mapping of an extension device.
 *
 */

#include <dix-config.h>

#include <X11/extensions/XI.h>
#include <X11/extensions/XIproto.h>

#include "dix/input_priv.h"

#include "inputstr.h"           /* DeviceIntPtr      */
#include "exevents.h"
#include "exglobals.h"
#include "setbmap.h"

/***********************************************************************
 *
 * This procedure lists the input devices available to the server.
 *
 */

int
ProcXSetDeviceButtonMapping(ClientPtr client)
{
    int ret;
    DeviceIntPtr dev;

    REQUEST(xSetDeviceButtonMappingReq);
    REQUEST_AT_LEAST_SIZE(xSetDeviceButtonMappingReq);

    if (client->req_len !=
        bytes_to_int32(sizeof(xSetDeviceButtonMappingReq) + stuff->map_length))
        return BadLength;

    ret = dixLookupDevice(&dev, stuff->deviceid, client, DixManageAccess);
    if (ret != Success)
        return ret;

    ret =
        ApplyPointerMapping(dev, (CARD8 *) &stuff[1], stuff->map_length,
                            client);
    if (ret == -1)
        return BadValue;

    if ((ret != Success) && (ret != MappingBusy))
        return ret;

    xSetDeviceButtonMappingReply rep = {
        .repType = X_Reply,
        .RepType = X_SetDeviceButtonMapping,
        .sequenceNumber = client->sequence,
        .status = (ret == Success ? MappingSuccess : MappingBusy),
    };

    if (client->swapped) {
        swaps(&rep.sequenceNumber);
    }
    WriteToClient(client, sizeof(xSetDeviceButtonMappingReply), &rep);

    return Success;
}
