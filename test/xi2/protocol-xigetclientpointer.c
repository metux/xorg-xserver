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
 * Protocol testing for XIGetClientPointer request.
 */
#include <stdint.h>
#include <X11/X.h>
#include <X11/Xproto.h>
#include <X11/extensions/XI2proto.h>
#include "inputstr.h"
#include "windowstr.h"
#include "scrnintstr.h"
#include "xigetclientpointer.h"
#include "exevents.h"

#include "protocol-common.h"

DECLARE_WRAP_FUNCTION(WriteToClient, void, ClientPtr client, int len, void *data);

static struct {
    int cp_is_set;
    DeviceIntPtr dev;
    int win;
} test_data;

extern ClientRec client_window;
static ClientRec client_request;

static void
reply_XIGetClientPointer(ClientPtr client, int len, void *data)
{
    xXIGetClientPointerReply *reply = (xXIGetClientPointerReply *) data;
    xXIGetClientPointerReply rep = *reply; /* copy so swapping doesn't touch the real reply */

    assert(len < 0xffff); /* suspicious size, swapping bug */

    if (client->swapped) {
        swapl(&rep.length);
        swaps(&rep.sequenceNumber);
        swaps(&rep.deviceid);
    }

    reply_check_defaults(&rep, len, XIGetClientPointer);

    assert(rep.set == test_data.cp_is_set);
    if (rep.set)
        assert(rep.deviceid == test_data.dev->id);
}

static void
request_XIGetClientPointer(ClientPtr client, xXIGetClientPointerReq * req,
                           int error)
{
    int rc;

    test_data.win = req->win;

    client_request.swapped = FALSE;
    rc = ProcXIGetClientPointer(&client_request);
    assert(rc == error);

    if (rc == BadWindow)
        assert(client_request.errorValue == req->win);

    client_request.swapped = TRUE;
    swapl(&req->win);
    swaps(&req->length);
    rc = SProcXIGetClientPointer(&client_request);
    assert(rc == error);

    if (rc == BadWindow)
        assert(client_request.errorValue == req->win);
}

static void
test_XIGetClientPointer(void)
{
    xXIGetClientPointerReq request;

    init_simple();
    client_window = init_client(0, NULL);

    request_init(&request, XIGetClientPointer);

    request.win = CLIENT_WINDOW_ID;

    wrapped_WriteToClient = reply_XIGetClientPointer;

    client_request = init_client(request.length, &request);

    dbg("Testing invalid window\n");
    request.win = INVALID_WINDOW_ID;
    request_XIGetClientPointer(&client_request, &request, BadWindow);

    dbg("Testing invalid length\n");
    client_request.req_len -= 4;
    request_XIGetClientPointer(&client_request, &request, BadLength);
    client_request.req_len += 4;

    test_data.cp_is_set = FALSE;

    dbg("Testing window None, unset ClientPointer.\n");
    request.win = None;
    request_XIGetClientPointer(&client_request, &request, Success);

    dbg("Testing valid window, unset ClientPointer.\n");
    request.win = CLIENT_WINDOW_ID;
    request_XIGetClientPointer(&client_request, &request, Success);

    dbg("Testing valid window, set ClientPointer.\n");
    client_window.clientPtr = devices.vcp;
    test_data.dev = devices.vcp;
    test_data.cp_is_set = TRUE;
    request.win = CLIENT_WINDOW_ID;
    request_XIGetClientPointer(&client_request, &request, Success);

    client_window.clientPtr = NULL;

    dbg("Testing window None, set ClientPointer.\n");
    client_request.clientPtr = devices.vcp;
    test_data.dev = devices.vcp;
    test_data.cp_is_set = TRUE;
    request.win = None;
    request_XIGetClientPointer(&client_request, &request, Success);
}

const testfunc_t*
protocol_xigetclientpointer_test(void)
{
    static const testfunc_t testfuncs[] = {
        test_XIGetClientPointer,
        NULL,
    };

    return testfuncs;
}
