/**
 * Copyright © 2009 Red Hat, Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice (including the next
 *  paragraph) shall be included in all copies or substantial portions of the
 *  Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 */

/* Test relies on assert() */
#undef NDEBUG

#include <dix-config.h>

/*
 * Protocol testing for XISetClientPointer request.
 *
 * Tests include:
 * BadDevice of all devices except master pointers.
 * Success for a valid window.
 * Success for window None.
 * BadWindow for invalid windows.
 */
#include <stdint.h>
#include <X11/X.h>
#include <X11/Xproto.h>
#include <X11/extensions/XI2proto.h>

#include "miext/extinit_priv.h"            /* for XInputExtensionInit */

#include "inputstr.h"
#include "windowstr.h"
#include "scrnintstr.h"
#include "xisetclientpointer.h"
#include "exevents.h"
#include "exglobals.h"

#include "protocol-common.h"

extern ClientRec client_window;
static ClientRec client_request;

static void
request_XISetClientPointer(xXISetClientPointerReq * req, int error)
{
    int rc;

    client_request = init_client(req->length, req);

    rc = ProcXISetClientPointer(&client_request);
    assert(rc == error);

    if (rc == BadDevice)
        assert(client_request.errorValue == req->deviceid);

    client_request.swapped = TRUE;

    /* MUST NOT swap req->length here !

       The handler proc's don't use that field anymore, thus also SProc's
       wont swap it. But this test program uses that field to initialize
       client->req_len (see above). We previously had to swap it here, so
       that SProcXIPassiveGrabDevice() will swap it back. Since that's gone
       now, still swapping itself would break if this function is called
       again and writing back a errornously swapped value
    */

    swapl(&req->win);
    swaps(&req->deviceid);
    rc = SProcXISetClientPointer(&client_request);
    assert(rc == error);

    if (rc == BadDevice)
        assert(client_request.errorValue == req->deviceid);

}

static void
test_XISetClientPointer(void)
{
    int i;
    xXISetClientPointerReq request;

    init_simple();
    client_window = init_client(0, NULL);

    request_init(&request, XISetClientPointer);

    request.win = CLIENT_WINDOW_ID;

    dbg("Testing BadDevice error for XIAllDevices and XIMasterDevices.\n");
    request.deviceid = XIAllDevices;
    request_XISetClientPointer(&request, BadDevice);

    request.deviceid = XIAllMasterDevices;
    request_XISetClientPointer(&request, BadDevice);

    dbg("Testing Success for VCP and VCK.\n");
    request.deviceid = devices.vcp->id; /* 2 */
    request_XISetClientPointer(&request, Success);
    assert(client_window.clientPtr->id == 2);

    request.deviceid = devices.vck->id; /* 3 */
    request_XISetClientPointer(&request, Success);
    assert(client_window.clientPtr->id == 2);

    dbg("Testing BadDevice error for all other devices.\n");
    for (i = 4; i <= 0xFFFF; i++) {
        request.deviceid = i;
        request_XISetClientPointer(&request, BadDevice);
    }

    dbg("Testing window None\n");
    request.win = None;
    request.deviceid = devices.vcp->id; /* 2 */
    request_XISetClientPointer(&request, Success);
    assert(client_request.clientPtr->id == 2);

    dbg("Testing invalid window\n");
    request.win = INVALID_WINDOW_ID;
    request.deviceid = devices.vcp->id;
    request_XISetClientPointer(&request, BadWindow);

}

const testfunc_t*
protocol_xisetclientpointer_test(void)
{
    static const testfunc_t testfuncs[] = {
        test_XISetClientPointer,
        NULL,
    };

    return testfuncs;
}
