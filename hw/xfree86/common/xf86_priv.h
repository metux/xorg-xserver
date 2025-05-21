/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 */
#ifndef _XSERVER_XF86_PRIV_H
#define _XSERVER_XF86_PRIV_H

#include "xf86.h"

extern Bool xf86DoConfigure;
extern Bool xf86DoConfigurePass1;
extern Bool xf86ProbeIgnorePrimary;

/*
 * Parameters set ONLY from the command line options
 * The global state of these things is held in xf86InfoRec (when appropriate).
 */
/* globals.c */
extern Bool xf86AllowMouseOpenFail;
extern Bool xf86AutoBindGPUDisabled;
extern Bool xf86VidModeDisabled;
extern Bool xf86VidModeAllowNonLocal;
extern Bool xf86fpFlag;
extern Bool xf86bsEnableFlag;
extern Bool xf86bsDisableFlag;
extern Bool xf86silkenMouseDisableFlag;
extern Bool xf86xkbdirFlag;
extern Bool xf86acpiDisableFlag;

extern char *xf86LayoutName;
extern char *xf86ScreenName;
extern char *xf86PointerName;
extern char *xf86KeyboardName;

extern rgb xf86Weight;
extern Bool xf86FlipPixels;
extern Gamma xf86Gamma;

extern const char *xf86ModulePath;
extern MessageType xf86ModPathFrom;

extern const char *xf86LogFile;
extern MessageType xf86LogFileFrom;
extern Bool xf86LogFileWasOpened;

/* xf86Cursor.c */
void xf86LockZoom(ScreenPtr pScreen, int lock);
void xf86InitViewport(ScrnInfoPtr pScr);
void xf86ZoomViewport(ScreenPtr pScreen, int zoom);
void xf86InitOrigins(void);

/* xf86Events.c */
InputHandlerProc xf86SetConsoleHandler(InputHandlerProc handler, void *data);
void xf86ProcessActionEvent(ActionEvent action, void *arg);
Bool xf86VTOwner(void);
void xf86VTEnter(void);
void xf86EnableInputDeviceForVTSwitch(InputInfoPtr pInfo);

/* xf86Helper.c */
void xf86DeleteDriver(int drvIndex);
void xf86DeleteScreen(ScrnInfoPtr pScrn);

/* xf86Mode.c */
const char * xf86ModeStatusToString(ModeStatus status);
ModeStatus xf86CheckModeForDriver(ScrnInfoPtr scrp, DisplayModePtr mode, int flags);

/* xf86RandR.c */
Bool xf86RandRInit(ScreenPtr pScreen);

/* xf86Extensions.c */
void xf86ExtensionInit(void);

#endif /* _XSERVER_XF86_PRIV_H */
