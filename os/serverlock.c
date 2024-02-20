/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 * Copyright © 1987, 1998  The Open Group
 * Copyright © 1987 Digital Equipment Corporation, Maynard, Massachusetts,
 * Copyright © 1994 Quarterdeck Office Systems.
 */
#include <dix-config.h>

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dix/dix_priv.h"
#include "os/serverlock.h"
#include "os/osdep.h"

#include "os.h"
#include "opaque.h"

/*
 * Explicit support for a server lock file like the ones used for UUCP.
 * For architectures with virtual terminals that can run more than one
 * server at a time.  This keeps the servers from stomping on each other
 * if the user forgets to give them different display numbers.
 */
#define LOCK_DIR "/tmp"
#define LOCK_TMP_PREFIX "/.tX"
#define LOCK_PREFIX "/.X"
#define LOCK_SUFFIX "-lock"

#ifdef LOCK_SERVER

static Bool StillLocking = FALSE;
static char LockFile[PATH_MAX];
static Bool nolock = FALSE;

/*
 * LockServer --
 *      Check if the server lock file exists.  If so, check if the PID
 *      contained inside is valid.  If so, then die.  Otherwise, create
 *      the lock file containing the PID.
 */
void
LockServer(void)
{
    char tmp[PATH_MAX], pid_str[12];
    int lfd, i, haslock, l_pid, t;
    const char *tmppath = LOCK_DIR;
    int len;
    char port[20];

    if (nolock || NoListenAll)
        return;
    /*
     * Path names
     */
    snprintf(port, sizeof(port), "%d", atoi(display));
    len = strlen(LOCK_PREFIX) > strlen(LOCK_TMP_PREFIX) ? strlen(LOCK_PREFIX) :
        strlen(LOCK_TMP_PREFIX);
    len += strlen(tmppath) + strlen(port) + strlen(LOCK_SUFFIX) + 1;
    if (len > sizeof(LockFile))
        FatalError("Display name `%s' is too long\n", port);
    (void) sprintf(tmp, "%s" LOCK_TMP_PREFIX "%s" LOCK_SUFFIX, tmppath, port);
    (void) sprintf(LockFile, "%s" LOCK_PREFIX "%s" LOCK_SUFFIX, tmppath, port);

    /*
     * Create a temporary file containing our PID.  Attempt three times
     * to create the file.
     */
    StillLocking = TRUE;
    i = 0;
    do {
        i++;
        lfd = open(tmp, O_CREAT | O_EXCL | O_WRONLY, 0644);
        if (lfd < 0)
            sleep(2);
        else
            break;
    } while (i < 3);
    if (lfd < 0) {
        unlink(tmp);
        i = 0;
        do {
            i++;
            lfd = open(tmp, O_CREAT | O_EXCL | O_WRONLY, 0644);
            if (lfd < 0)
                sleep(2);
            else
                break;
        } while (i < 3);
    }
    if (lfd < 0)
        FatalError("Could not create lock file in %s\n", tmp);
    snprintf(pid_str, sizeof(pid_str), "%10lu\n", (unsigned long) getpid());
    if (write(lfd, pid_str, 11) != 11)
        FatalError("Could not write pid to lock file in %s\n", tmp);
    (void) fchmod(lfd, 0444);
    (void) close(lfd);

    /*
     * OK.  Now the tmp file exists.  Try three times to move it in place
     * for the lock.
     */
    i = 0;
    haslock = 0;
    while ((!haslock) && (i++ < 3)) {
        haslock = (link(tmp, LockFile) == 0);
        if (haslock) {
            /*
             * We're done.
             */
            break;
        }
        else if (errno == EEXIST) {
            /*
             * Read the pid from the existing file
             */
            lfd = open(LockFile, O_RDONLY | O_NOFOLLOW);
            if (lfd < 0) {
                unlink(tmp);
                FatalError("Can't read lock file %s\n", LockFile);
            }
            pid_str[0] = '\0';
            if (read(lfd, pid_str, 11) != 11) {
                /*
                 * Bogus lock file.
                 */
                unlink(LockFile);
                close(lfd);
                continue;
            }
            pid_str[11] = '\0';
            sscanf(pid_str, "%d", &l_pid);
            close(lfd);

            /*
             * Now try to kill the PID to see if it exists.
             */
            errno = 0;
            t = kill(l_pid, 0);
            if ((t < 0) && (errno == ESRCH)) {
                /*
                 * Stale lock file.
                 */
                unlink(LockFile);
                continue;
            }
            else if (((t < 0) && (errno == EPERM)) || (t == 0)) {
                /*
                 * Process is still active.
                 */
                unlink(tmp);
                FatalError
                    ("Server is already active for display %s\n%s %s\n%s\n",
                     port, "\tIf this server is no longer running, remove",
                     LockFile, "\tand start again.");
            }
        }
        else {
            unlink(tmp);
            FatalError
                ("Linking lock file (%s) in place failed: %s\n",
                 LockFile, strerror(errno));
        }
    }
    unlink(tmp);
    if (!haslock)
        FatalError("Could not create server lock file: %s\n", LockFile);
    StillLocking = FALSE;
}

/*
 * UnlockServer --
 *      Remove the server lock file.
 */
void
UnlockServer(void)
{
    if (nolock || NoListenAll)
        return;

    if (!StillLocking) {

        (void) unlink(LockFile);
    }
}

void DisableServerLock(void) {
    nolock = TRUE;
}

#else /* LOCK_SERVER */

void LockServer(void) {}
void UnlockServer(void) {}
void DisableServerLock(void) {}

#endif /* LOCK_SERVER */
