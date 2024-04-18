/***********************************************************

Copyright 1987, 1998  The Open Group

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

Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* Author: Todd Newman  (aided and abetted by Mr. Drewry) */

#ifdef HAVE_DIX_CONFIG_H
#include <dix-config.h>
#endif

#include <X11/X.h>
#include <X11/Xprotostr.h>

#include "misc.h"
#include "gcstruct.h"
#include "pixmapstr.h"
#include "windowstr.h"
#include "scrnintstr.h"
#include "mi.h"
#include "regionstr.h"
#include <X11/Xmd.h>
#include "servermd.h"

#ifdef __MINGW32__
#define ffs __builtin_ffs
#endif

/* MIOPQSTIPDRAWABLE -- use pbits as an opaque stipple for pDraw.
 * Drawing through the clip mask we SetSpans() the bits into a
 * bitmap and stipple those bits onto the destination drawable by doing a
 * PolyFillRect over the whole drawable,
 * then we invert the bitmap by copying it onto itself with an alu of
 * GXinvert, invert the foreground/background colors of the gc, and draw
 * the background bits.
 * Note how the clipped out bits of the bitmap are always the background
 * color so that the stipple never causes FillRect to draw them.
 */
_X_COLD static void
miOpqStipDrawable(DrawablePtr pDraw, GCPtr pGC, RegionPtr prgnSrc,
                  MiBits * pbits, int srcx, int w, int h, int dstx, int dsty)
{
    int oldfill, i;
    unsigned long oldfg;
    int *pwidth, *pwidthFirst;
    ChangeGCVal gcv[6];
    PixmapPtr pStipple, pPixmap;
    DDXPointRec oldOrg;
    GCPtr pGCT;
    DDXPointPtr ppt, pptFirst;
    xRectangle rect;
    RegionPtr prgnSrcClip;

    pPixmap = (*pDraw->pScreen->CreatePixmap)
        (pDraw->pScreen, w + srcx, h, 1, CREATE_PIXMAP_USAGE_SCRATCH);
    if (!pPixmap)
        return;

    /* Put the image into a 1 bit deep pixmap */
    pGCT = GetScratchGC(1, pDraw->pScreen);
    if (!pGCT) {
        (*pDraw->pScreen->DestroyPixmap) (pPixmap);
        return;
    }
    /* First set the whole pixmap to 0 */
    gcv[0].val = 0;
    ChangeGC(NullClient, pGCT, GCBackground, gcv);
    ValidateGC((DrawablePtr) pPixmap, pGCT);
    miClearDrawable((DrawablePtr) pPixmap, pGCT);
    ppt = pptFirst = xallocarray(h, sizeof(DDXPointRec));
    pwidth = pwidthFirst = xallocarray(h, sizeof(int));
    if (!pptFirst || !pwidthFirst) {
        free(pwidthFirst);
        free(pptFirst);
        FreeScratchGC(pGCT);
        return;
    }

    /* we need a temporary region because ChangeClip must be assumed
       to destroy what it's sent.  note that this means we don't
       have to free prgnSrcClip ourselves.
     */
    prgnSrcClip = RegionCreate(NULL, 0);
    RegionCopy(prgnSrcClip, prgnSrc);
    RegionTranslate(prgnSrcClip, srcx, 0);
    (*pGCT->funcs->ChangeClip) (pGCT, CT_REGION, prgnSrcClip, 0);
    ValidateGC((DrawablePtr) pPixmap, pGCT);

    /* Since we know pDraw is always a pixmap, we never need to think
     * about translation here */
    for (i = 0; i < h; i++) {
        ppt->x = 0;
        ppt++->y = i;
        *pwidth++ = w + srcx;
    }

    (*pGCT->ops->SetSpans) ((DrawablePtr) pPixmap, pGCT, (char *) pbits,
                            pptFirst, pwidthFirst, h, TRUE);
    free(pwidthFirst);
    free(pptFirst);

    /* Save current values from the client GC */
    oldfill = pGC->fillStyle;
    pStipple = pGC->stipple;
    if (pStipple)
        pStipple->refcnt++;
    oldOrg = pGC->patOrg;

    /* Set a new stipple in the drawable */
    gcv[0].val = FillStippled;
    gcv[1].ptr = pPixmap;
    gcv[2].val = dstx - srcx;
    gcv[3].val = dsty;

    ChangeGC(NullClient, pGC,
             GCFillStyle | GCStipple | GCTileStipXOrigin | GCTileStipYOrigin,
             gcv);
    ValidateGC(pDraw, pGC);

    /* Fill the drawable with the stipple.  This will draw the
     * foreground color wherever 1 bits are set, leaving everything
     * with 0 bits untouched.  Note that the part outside the clip
     * region is all 0s.  */
    rect.x = dstx;
    rect.y = dsty;
    rect.width = w;
    rect.height = h;
    (*pGC->ops->PolyFillRect) (pDraw, pGC, 1, &rect);

    /* Invert the tiling pixmap. This sets 0s for 1s and 1s for 0s, only
     * within the clipping region, the part outside is still all 0s */
    gcv[0].val = GXinvert;
    ChangeGC(NullClient, pGCT, GCFunction, gcv);
    ValidateGC((DrawablePtr) pPixmap, pGCT);
    (void) (*pGCT->ops->CopyArea) ((DrawablePtr) pPixmap, (DrawablePtr) pPixmap,
                            pGCT, 0, 0, w + srcx, h, 0, 0);

    /* Swap foreground and background colors on the GC for the drawable.
     * Now when we fill the drawable, we will fill in the "Background"
     * values */
    oldfg = pGC->fgPixel;
    gcv[0].val = pGC->bgPixel;
    gcv[1].val = oldfg;
    gcv[2].ptr = pPixmap;
    ChangeGC(NullClient, pGC, GCForeground | GCBackground | GCStipple, gcv);
    ValidateGC(pDraw, pGC);
    /* PolyFillRect might have bashed the rectangle */
    rect.x = dstx;
    rect.y = dsty;
    rect.width = w;
    rect.height = h;
    (*pGC->ops->PolyFillRect) (pDraw, pGC, 1, &rect);

    /* Now put things back */
    if (pStipple)
        pStipple->refcnt--;
    gcv[0].val = oldfg;
    gcv[1].val = pGC->fgPixel;
    gcv[2].val = oldfill;
    gcv[3].ptr = pStipple;
    gcv[4].val = oldOrg.x;
    gcv[5].val = oldOrg.y;
    ChangeGC(NullClient, pGC,
             GCForeground | GCBackground | GCFillStyle | GCStipple |
             GCTileStipXOrigin | GCTileStipYOrigin, gcv);

    ValidateGC(pDraw, pGC);
    /* put what we hope is a smaller clip region back in the scratch gc */
    (*pGCT->funcs->ChangeClip) (pGCT, CT_NONE, NULL, 0);
    FreeScratchGC(pGCT);
    (*pDraw->pScreen->DestroyPixmap) (pPixmap);

}

/* MIPUTIMAGE -- public entry for the PutImage request
 * Here we benefit from knowing the format of the bits pointed to by pImage,
 * even if we don't know how pDraw represents them.
 * Three different strategies are used depending on the format
 * XYBitmap Format:
 * 	we just use the Opaque Stipple helper function to cover the destination
 * 	Note that this covers all the planes of the drawable with the
 *	foreground color (masked with the GC planemask) where there are 1 bits
 *	and the background color (masked with the GC planemask) where there are
 *	0 bits
 * XYPixmap format:
 *	what we're called with is a series of XYBitmaps, but we only want
 *	each XYPixmap to update 1 plane, instead of updating all of them.
 * 	we set the foreground color to be all 1s and the background to all 0s
 *	then for each plane, we set the plane mask to only effect that one
 *	plane and recursive call ourself with the format set to XYBitmap
 *	(This clever idea courtesy of RGD.)
 * ZPixmap format:
 *	This part is simple, just call SetSpans
 */
_X_COLD void
miPutImage(DrawablePtr pDraw, GCPtr pGC, int depth,
           int x, int y, int w, int h, int leftPad, int format, char *pImage)
{
    DDXPointPtr pptFirst, ppt;
    int *pwidthFirst, *pwidth;
    RegionPtr prgnSrc;
    BoxRec box;
    unsigned long oldFg, oldBg;
    ChangeGCVal gcv[3];
    unsigned long oldPlanemask;
    unsigned long i;
    long bytesPer;

    if (!w || !h)
        return;
    switch (format) {
    case XYBitmap:

        box.x1 = 0;
        box.y1 = 0;
        box.x2 = w;
        box.y2 = h;
        prgnSrc = RegionCreate(&box, 1);

        miOpqStipDrawable(pDraw, pGC, prgnSrc, (MiBits *) pImage,
                          leftPad, w, h, x, y);
        RegionDestroy(prgnSrc);
        break;

    case XYPixmap:
        depth = pGC->depth;
        oldPlanemask = pGC->planemask;
        oldFg = pGC->fgPixel;
        oldBg = pGC->bgPixel;
        gcv[0].val = (XID) ~0;
        gcv[1].val = (XID) 0;
        ChangeGC(NullClient, pGC, GCForeground | GCBackground, gcv);
        bytesPer = (long) h *BitmapBytePad(w + leftPad);

        for (i = (unsigned long) 1 << (depth - 1); i != 0; i >>= 1, pImage += bytesPer) {
            if (i & oldPlanemask) {
                gcv[0].val = (XID) i;
                ChangeGC(NullClient, pGC, GCPlaneMask, gcv);
                ValidateGC(pDraw, pGC);
                (*pGC->ops->PutImage) (pDraw, pGC, 1, x, y, w, h, leftPad,
                                       XYBitmap, (char *) pImage);
            }
        }
        gcv[0].val = (XID) oldPlanemask;
        gcv[1].val = (XID) oldFg;
        gcv[2].val = (XID) oldBg;
        ChangeGC(NullClient, pGC, GCPlaneMask | GCForeground | GCBackground,
                 gcv);
        ValidateGC(pDraw, pGC);
        break;

    case ZPixmap:
        ppt = pptFirst = xallocarray(h, sizeof(DDXPointRec));
        pwidth = pwidthFirst = xallocarray(h, sizeof(int));
        if (!pptFirst || !pwidthFirst) {
            free(pwidthFirst);
            free(pptFirst);
            return;
        }
        if (pGC->miTranslate) {
            x += pDraw->x;
            y += pDraw->y;
        }

        for (i = 0; i < h; i++) {
            ppt->x = x;
            ppt->y = y + i;
            ppt++;
            *pwidth++ = w;
        }

        (*pGC->ops->SetSpans) (pDraw, pGC, (char *) pImage, pptFirst,
                               pwidthFirst, h, TRUE);
        free(pwidthFirst);
        free(pptFirst);
        break;
    }
}
