/*
 * Copyright © 2000 Compaq Computer Corporation
 * Copyright © 2002 Hewlett-Packard Company
 * Copyright © 2006 Intel Corporation
 * Copyright © 2008 Red Hat, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 *
 * Author:  Jim Gettys, Hewlett-Packard Company, Inc.
 *	    Keith Packard, Intel Corporation
 */

#ifndef _XSERVER_RANDRSTR_PRIV_H_
#define _XSERVER_RANDRSTR_PRIV_H_

#include <X11/Xdefs.h>

#include "randrstr.h"

extern int RREventBase, RRErrorBase;

extern RESTYPE RRClientType, RREventType;     /* resource types for event masks */
extern DevPrivateKeyRec RRClientPrivateKeyRec;

#define RRClientPrivateKey (&RRClientPrivateKeyRec)

#define VERIFY_RR_OUTPUT(id, ptr, a)\
    {\
	int rc = dixLookupResourceByType((void **)&(ptr), id,\
	                                 RROutputType, client, a);\
	if (rc != Success) {\
	    client->errorValue = id;\
	    return rc;\
	}\
    }

#define VERIFY_RR_CRTC(id, ptr, a)\
    {\
	int rc = dixLookupResourceByType((void **)&(ptr), id,\
	                                 RRCrtcType, client, a);\
	if (rc != Success) {\
	    client->errorValue = id;\
	    return rc;\
	}\
    }

#define VERIFY_RR_MODE(id, ptr, a)\
    {\
	int rc = dixLookupResourceByType((void **)&(ptr), id,\
	                                 RRModeType, client, a);\
	if (rc != Success) {\
	    client->errorValue = id;\
	    return rc;\
	}\
    }

#define VERIFY_RR_PROVIDER(id, ptr, a)\
    {\
        int rc = dixLookupResourceByType((void **)&(ptr), id,\
                                         RRProviderType, client, a);\
        if (rc != Success) {\
            client->errorValue = id;\
            return rc;\
        }\
    }

#define VERIFY_RR_LEASE(id, ptr, a)\
    {\
        int rc = dixLookupResourceByType((void **)&(ptr), id,\
                                         RRLeaseType, client, a);\
        if (rc != Success) {\
            client->errorValue = id;\
            return rc;\
        }\
    }

#define GetRRClient(pClient)    ((RRClientPtr)dixLookupPrivate(&(pClient)->devPrivates, RRClientPrivateKey))
#define rrClientPriv(pClient)	RRClientPtr pRRClient = GetRRClient(pClient)

void RRConstrainCursorHarder(DeviceIntPtr, ScreenPtr, int, int *, int *);

/* rrlease.c */
void RRDeliverLeaseEvent(ClientPtr client, WindowPtr window);

void RRTerminateLease(RRLeasePtr lease);

Bool RRLeaseInit(void);

/* rrprovider.c */
#define PRIME_SYNC_PROP         "PRIME Synchronization"

void RRMonitorInit(ScreenPtr screen);

Bool RRMonitorMakeList(ScreenPtr screen, Bool get_active, RRMonitorPtr *monitors_ret, int *nmon_ret);

int RRMonitorCountList(ScreenPtr screen);

void RRMonitorFreeList(RRMonitorPtr monitors, int nmon);

void RRMonitorClose(ScreenPtr screen);

RRMonitorPtr RRMonitorAlloc(int noutput);

int RRMonitorAdd(ClientPtr client, ScreenPtr screen, RRMonitorPtr monitor);

void RRMonitorFree(RRMonitorPtr monitor);

/*
 * Deliver a ScreenChangeNotity event to given client
 *
 * @param pClient the client to notify
 * @param pWin    the window to refer to in the event
 * @param pScreen the screen where the change happened
 */
void RRDeliverScreenEvent(ClientPtr pClient, WindowPtr pWin, ScreenPtr pScreen);

/*
 * Mark screen resources as changed, so listeners will get updates on them.
 *
 * @param pScreen the screen where changes occoured
 */
void RRResourcesChanged(ScreenPtr pScreen);

/*
 * Initialize randr subsystem
 *
 * @return TRUE on success
 */
Bool RRInit(void);

/*
 * Retrieve the first enabled CRTC on given screen
 *
 * @param pScreen the screen to query
 * @return pointer to CRTC structure or NULL
 */
RRCrtcPtr RRFirstEnabledCrtc(ScreenPtr pScreen);

/*
 * Set non-desktop property on given output. This flag should be TRUE on
 * outputs where usual desktops shouldn't expand onto (eg. head displays,
 * additional display bars in various handhelds, etc)
 */
Bool RROutputSetNonDesktop(RROutputPtr output, Bool non_desktop);

/*
 * Compute vertical refresh rate from randr mode information
 *
 * @param mode pointer to randr mode info
 * @return vertical refresh rate
 */
CARD16 RRVerticalRefresh(xRRModeInfo * mode);

/*
 * Tests if findCrtc belongs to pScreen or secondary screens
 *
 * @param pScreen the screen to check on
 * @param findCrtc the Crtc to check for
 * @return TRUE if given CRTC belongs to pScreen / secondard screens
 */
Bool RRCrtcExists(ScreenPtr pScreen, RRCrtcPtr findCrtc);

/*
 * Set whether transforms are allowed on a CRTC
 *
 * @param crtc the CRTC to set the flag on
 * @param transforms TRUE if transforms are allowed
 */
void RRCrtcSetTransformSupport(RRCrtcPtr crtc, Bool transforms);

/*
 * Deliver CRTC update event to given client
 *
 * @param pClient the client to send event to
 * @param pWin    the window whose screen had been changed
 * @param crtc    the CRTC that had been changed
 */
void RRDeliverCrtcEvent(ClientPtr pClient, WindowPtr pWin, RRCrtcPtr crtc);

/*
 * Return the area of the frame buffer scanned out by the crtc,
 * taking into account the current mode and rotation
 *
 * @param crtc    the CRTC to query
 * @param width   return buffer for width value
 * @param height  return buffer for height value
 */
void RRCrtcGetScanoutSize(RRCrtcPtr crtc, int *width, int *height);

/*
 * Retrieve CRTCs current transform
 *
 * @param crtc    the CRTC to query
 * @return        pointer to CRTCs current transform
 */
RRTransformPtr RRCrtcGetTransform(RRCrtcPtr crtc);

/*
 * Destroy a Crtc at shutdown
 *
 * @param crtc    the CRTC to destroy
 */
void RRCrtcDestroy(RRCrtcPtr crtc);

/*
 * Initialize crtc resource type
 */
Bool RRCrtcInit(void);


/*
 * Initialize crtc type error value
 */
void RRCrtcInitErrorValue(void);

/*
 * Detach and free a scanout pixmap
 *
 * @param crtc    the CRTC to act on
 */
void RRCrtcDetachScanoutPixmap(RRCrtcPtr crtc);

/*
 * Handler for the ReplaceScanoutPixmap screen proc
 * Should not be called directly.
 */
Bool RRReplaceScanoutPixmap(DrawablePtr pDrawable, PixmapPtr pPixmap, Bool enable);

/*
 * Check whether given screen has any scanout pixmap attached
 *
 * @param pScreen the screen to check
 * @return TRUE if the screen has a scanout pixmap attached
 */
Bool RRHasScanoutPixmap(ScreenPtr pScreen);

/*
 * Check whether client is operating on recent enough protocol version
 * to know about refresh rates. This has influence on reply packet formats
 *
 * @param pClient the client to check
 * @return TRUE if client using recent enough protocol version
 */
Bool RRClientKnowsRates(ClientPtr pClient);

/*
 * Called by DIX to notify RANDR extension that a lease had been terminated.
 *
 * @param lease   the lease that had been terminated
 */
void RRLeaseTerminated(RRLeasePtr lease);

/*
 * Free a RRLease structure
 *
 * @param lease   pointer to the lease to be freed
 */
void RRLeaseFree(RRLeasePtr lease);

/*
 * Check whether given CRTC has an active lease
 *
 * @param crtc    the CRTC to check
 * @return TRUE if there is any active lease on that CRTC
 */
Bool RRCrtcIsLeased(RRCrtcPtr crtc);

#endif /* _XSERVER_RANDRSTR_PRIV_H_ */
