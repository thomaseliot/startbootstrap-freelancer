/*----------------------------------------------------------------------
 * $Revision: 1.12 $    $Date: 2011-09-23 16:38:25 $
 * Copyright (c) 1991-2015 Altia, Inc.
 *
 * Permission to use, copy, and modify this software for any purpose is
 * hereby granted, provided that this copyright notice appears in all
 * copies and that the entity using, copying, or modifying this software
 * has purchased an Altia DeepScreen license. Permission to sell or
 * distribute this source code is denied. Altia makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * ALTIA INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL ALTIA INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * FILE         : altiaAPIServer.c
 *
 * ONELINER     : Altia API server functionality for Altia DeepScreen 
 *
 * DESCRIPTION  : Implements a server for the lan (socket) Altia API that
 *                integrates with DeepScreen generated code to support
 *                communications to/from another application using the
 *                Altia API functions.
 *
 *                One way to think of it is that it allows a DeepScreen
 *                generated process to behave like an instance of Altia
 *                Runtime.  Client applications can connect, communicate
 *                and disconnect.
 *
 *                This implementation uses the term "Class" in the comments
 *                when describing or referring to the components of the
 *                implementation; however, the code itself is C code, not C++.
 *
 * HOW TO USE   : To use this code for a supported DeepScreen target,
 *                place it in the same directory as the design (.dsn) file
 *                and name this file the same as the design file, but with
 *                a .c extension instead of a .dsn extension.  For example,
 *                if the design file is example.dsn, name this file
 *                example.c.  Compile the DeepScreen code as usual.  A
 *                standard DeepScreen generated Makefile script (usually
 *                altmake.bat or altmake_unixcc.bat for a UNIX host using
 *                its native compiler) will automatically compile the 
 *                code into the executable if it is named the same as
 *                the design file, but with a .c extension.
 *
 *         NOTE:  Only a DeepScreen 32-bit Windows native (GDI32) target (the
 *                "Windows" target option in the Altia Design Code Generation
 *                Options dialog) or a X11 target (the "X11" target option in
 *                the Altia Design Code Generation Options dialog) support the
 *                following command line arguments and built-in animation
 *                functions.  In general, these have to do with manipulated
 *                the style of the main window and/or opening extra views:
 * 
 *                 Command line argument:  -nowin
 *                 Built-in animation:     altiaOpenView
 *                 Built-in animation:     altiaCloseView
 *                 Built-in animation:     altiaSetViewName
 *                 Built-in animation:     altiaSetView
 *                 Built-in animation:     altiaSetViewStyle
 *
 *                If WIN32_GDI32 (for a "Windows" target) or UNIX (for a "X11"
 *                target) is not defined at compile time, the above features
 *                are disabled.
 *
 *                Here are the required compile options for this code:
 *
 *                Compile with -DWIN32 for 32-bit Windows and if it is native
 *                    (GDI32 Windows, not altiaGL), also -DWIN32_GDI32
 *                Compile with -DWIN32 -DUNDER_CE -DUNICODE -DUSERWIN for
 *                    Windows CE and if it is GDI32 WinCE, also -DWIN32_GDI32.
 *                Compile with -DVXWORKS for a VXWORKS target.
 *                Compile with -DQNX -DUSERWIN for a QNX Photon target.
 *                Compile with -DQNX for a QNX target without Photon.
 *                Compile with -DUNICODE for a Unicode system.
 *                For X11, compile with -DUNIX and then...
 *                For a specific flavor of UNIX, compile with -DLINUX (Linux),
 *                -DVXWORKS (VxWorks), -Dsun (Sun4) or -DSOL (Solaris),
 *                -DSGI (SGI Irix), -DPMAX (Concurrent PowerMax), or
 *                -DIBM (IBM Aix),
 */

#define int_owner vsecnct_int_owner

#if defined(sun) && !defined(SOL)
#define SOL
#endif

#ifdef VXWORKS
#include <vxWorks.h>
#endif /* VXWORKS */

#include <stdlib.h>
#include <stdio.h>      /* KKF/tjw:  needed for perror() on SGI and VXWORKS */
#include <string.h>

/* BEGIN INCLUDES THAT ARE PLATFORM SPECIFIC STARTING WITH WIN32 */
#ifdef WIN32
#include <windows.h>
#include <winsock.h>
#ifndef UNDER_CE
#include <io.h>
#define write _write
#endif

#else /* INCLUDES NOT FOR WIN32, BUT FLAVORS OF UNIX/LINUX/QNX/VXWORKS */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <errno.h>
#include <signal.h>

#if defined(GNUSOLARIS) || defined(SOL)
#include <sys/filio.h>
#endif

#ifdef QNX
#include <unix.h>
#ifdef USERWIN
#include <Pt.h>
#endif
#endif

#ifdef VXWORKS
#define PIPSIZ 4096
#include <hostLib.h>
#include <sockLib.h>
#include <pipeDrv.h>
#include <selectLib.h>
#include <inetLib.h>
#include <ioLib.h>
#include <netinet/in.h>
#include <time.h>

#else /* NOT WIN32, NOT VXWORKS, BUT STILL A FLAVOR OF UNIX/LINUX/QNX */

#if defined(sun)
#if defined(__cplusplus)
#include <sysent.h>     /* On SUN, mknod and gethostname declarations. */
#endif
#include <sys/param.h>  /* tjw 11/5/91:  On SUN, PIPE_BUF is in sys/param.h */
#define PIPSIZ PIPE_BUF

#elif defined(hpux)
#include <sys/inode.h>      /* On HP, PIPSIZ comes from sys/inode.h */

#elif defined(PMAX)
#include <sys/param.h>      /* tjw 11/10/98:  On PMAX, provides PIPE_BUF. */
#include <sys/filio.h>      /* tjw 11/10/98:  On PMAX, provides FIONREAD. */
#define PIPSIZ PIPE_BUF
extern int gethostname(char*, int);

#else                       /* All other UNIX/QNX/LINUX, PIPE_BUF in limits */
#include <limits.h>
#ifndef PIPE_BUF
#define PIPE_BUF 4096
#endif
#define PIPSIZ PIPE_BUF
#endif

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
 
#if !defined(SYSV) || defined(sun) || defined(SGI) || defined(QNX)
#include <sys/time.h>       /* tjw 11/5/91:  On SUN, time.h in sys subdir. */
#else
#include <time.h>
#endif /* sun || SGI || QNX */

#if defined(SGI_IRIX4)
#include "/usr/irix4/usr/include/netinet/in.h"
#include <sys/signal.h>
#if AltiaFunctionPrototyping
extern int gethostname(char*, int);
extern int select(int, fd_set*, fd_set*, fd_set*, struct timeval*);
extern int mknod(const char*, mode_t, dev_t);
extern int ioctl(int, int, ...);
extern int writev(int, const struct iovec*, int);
#else
extern int gethostname();
extern int select();
extern int mknod();
extern int ioctl();
extern int writev();
#endif /* AltiaFunctionPrototyping */

#elif defined(QNX)
#include <netinet/in.h>

#else 
#include <netinet/in.h>
#endif /* SGI_IRIX4 */

#endif /* FINISHED VXWORKS VERSUS OTHER FLAVORS OF UNIX/LINUX/QNX */
#endif /* FINISHED WIN32 VERSUS FLAVORS OF UNIX/LINUX/QNX/VXWORKS */

/* just in case standard header didn't */
#ifndef netinet_in_h
#define netinet_in_h
#endif

/* This code uses select() and it is necessary to know the upper bounds
 * for file descriptor numbers.  If none of the include files gave us
 * an upper limit, define one now for non-Windows targets.
 */
#ifndef FD_SETSIZE
#define FD_SETSIZE 64
#endif

/*-------------------- TO ENABLE DEBUG MESSAGES ------------------------
 * To enable debug messages, define ALTIAAPIDEBUG. For a Windows console
 * program our UNIX style application, messages will go to stderr.  For
 * a TRUE Windows (WinMain) application, messages go to a command prompt
 * window created just for the messages.  Must define this before any local
 * includes otherwise PRINTERR[1-N] macros will do nothing.
 */
/* #define ALTIAAPIDEBUG */
#ifdef ALTIAAPIDEBUG
int _altiaDebug = 1;
#else
int _altiaDebug = 0;
#endif

/* Altia DeepScreen generated code dependencies */
#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"


/***************************************************************************
 ***************************************************************************
 **
 ** BEGIN data type and function declarations now.  This content could
 **       reside in a header file; however, it makes the code easier to
 **       transport if the declarations reside with the code itself.
 **
 ***************************************************************************
 ***************************************************************************/

#ifndef aClIeNtserver_h
#define aClIeNtserver_h

/* Establish value for NULL and nil if not already done */
#ifndef NULL
#define NULL 0
#endif

#ifndef nil
#define nil 0
#endif

/* Determine const capabilities and define CONST appropriately */
#if defined(__cplusplus) || defined(c_plusplus)
#undef CONST
#define CONST const
#else
#undef CONST
#define CONST
#endif /* __cplusplus */

/* Define TRUE/FALSE and asvr_bool. */
#ifndef TRUE
#define TRUE 1
#endif /* TRUE */
#ifndef FALSE
#define FALSE 0
#endif /* FALSE */
typedef unsigned int asvr_bool;

#ifndef ALTIA_BASE_STUB
    extern int TargetCheckEvent(int *status);
    extern void TargetAltiaInitialize(void);
    extern int TargetAltiaAnimate(ALTIA_CHAR *funcName, AltiaEventType value);
    extern void TargetAltiaUpdate(void);
    extern void TargetAltiaClose(void);
    extern ALTIA_BOOLEAN _altiaConnectionToFloat(ALTIA_CHAR *funcName);
    extern ALTIA_BOOLEAN _altiaConnectionFromFloat(ALTIA_CHAR *funcName);
    extern void TargetAltiaSetWindowStyle(int style);
    extern void TargetAltiaPositionWindow(int x, int y, int /* Center? */);
    extern void TargetAltiaShowWindow(int yes);
    extern void TargetAltiaSetWindowName(ALTIA_CHAR *windowName);
#else
    /* Stubs to DeepScreen functions for testing without DeepScreen.*/
    typedef void (*AltiaReportFuncType)(ALTIA_CHAR *name,
                  AltiaEventType value);
    static AltiaReportFuncType AltiaReportFuncPtr = NULL;
    static ALTIA_BOOLEAN _altiaConnectionToFloat(ALTIA_CHAR *funcName)
    {
        return FALSE;
    }
    static ALTIA_BOOLEAN _altiaConnectionFromFloat(ALTIA_CHAR *funcName)
    {
        return FALSE;
    }
    static AltiaEventType _altiaFindCurVal(ALTIA_CHAR *funcName)
    {
        if (ALT_STRCMP(funcName, ALT_TEXT("altiaQuit")) == 0
            || ALT_STRCMP(funcName, ALT_TEXT("clientQuit")) == 0)
            return (AltiaEventType) 0;
        else
            return (AltiaEventType) 1;
    }
    static int TargetAltiaAnimate(ALTIA_CHAR *funcName, AltiaEventType value)
    {
        printf("Routing \"%s\" = %g\n", (char *) funcName, (double) value);
        return 1;
    }
    static void TargetAltiaUpdate(void)
    {}
    static void TargetAltiaInitialize(void)
    {}
    static void TargetAltiaClose(void)
    {}
    static void TargetAltiaSetWindowStyle(int style)
    {}
    static void TargetAltiaPositionWindow(int x, int y, int center)
    {}
    static void TargetAltiaShowWindow(int yes)
    {}
    static void TargetAltiaSetWindowName(ALTIA_CHAR *windowName)
    {}
#endif


/*========================================================================
 *
 *  printerr - defines to direct debug messages to an output device
 *             (or nowhere by default).
 *
 *========================================================================*/
#ifndef printErrorOutput
#define printErrorOutput

#ifndef ALTIAAPIDEBUG
static char localPrintNothing = 0;
#define PRINTPERROR(a)          localPrintNothing = 0
#define PRINTERR1(a)            localPrintNothing = 0
#define PRINTERR2(a,b)          localPrintNothing = 0
#define PRINTERR3(a,b,c)        localPrintNothing = 0
#define PRINTERR4(a,b,c,d)      localPrintNothing = 0
#define PRINTERR5(a,b,c,d,e)    localPrintNothing = 0
#define PRINTERR6(a,b,c,d,e,f)  localPrintNothing = 0

#elif defined(_WINDLL) || defined(WIN16) || defined(UNDER_CE)
extern FILE *_altiaServerOutfile;
#define PRINTPERROR(a) (_altiaServerOutfile?(fprintf(_altiaServerOutfile,a),fprintf(_altiaServerOutfile,"\n"),fflush(_altiaServerOutfile)):0)
#define PRINTERR1(a) (_altiaServerOutfile?(fprintf(_altiaServerOutfile,a),fflush(_altiaServerOutfile)):0)
#define PRINTERR2(a,b) (_altiaServerOutfile?(fprintf(_altiaServerOutfile,a,b),fflush(_altiaServerOutfile)):0)
#define PRINTERR3(a,b,c) (_altiaServerOutfile?(fprintf(_altiaServerOutfile,a,b,c),fflush(_altiaServerOutfile)):0)
#define PRINTERR4(a,b,c,d) (_altiaServerOutfile?(fprintf(_altiaServerOutfile,a,b,c,d),fflush(_altiaServerOutfile)):0)
#define PRINTERR5(a,b,c,d,e) (_altiaServerOutfile?(fprintf(_altiaServerOutfile,a,b,c,d,e),fflush(_altiaServerOutfile)):0)
#define PRINTERR6(a,b,c,d,e,f) (_altiaServerOutfile?(fprintf(_altiaServerOutfile,a,b,c,d,e,f),fflush(_altiaServerOutfile)):0)

#elif defined(WIN32)
extern FILE *_altiaServerOutfile;
#define PRINTPERROR(a) perror(a)
#define PRINTERR1(a) (_altiaServerOutfile?(fprintf(_altiaServerOutfile,a),fflush(_altiaServerOutfile)):printf(a))
#define PRINTERR2(a,b) (_altiaServerOutfile?(fprintf(_altiaServerOutfile,a,b),fflush(_altiaServerOutfile)):printf(a,b))
#define PRINTERR3(a,b,c) (_altiaServerOutfile?(fprintf(_altiaServerOutfile,a,b,c),fflush(_altiaServerOutfile)):printf(a,b,c))
#define PRINTERR4(a,b,c,d) (_altiaServerOutfile?(fprintf(_altiaServerOutfile,a,b,c,d),fflush(_altiaServerOutfile)):printf(a,b,c,d))
#define PRINTERR5(a,b,c,d,e) (_altiaServerOutfile?(fprintf(_altiaServerOutfile,a,b,c,d,e),fflush(_altiaServerOutfile)):printf(a,b,c,d,e))
#define PRINTERR6(a,b,c,d,e,f) (_altiaServerOutfile?(fprintf(_altiaServerOutfile,a,b,c,d,e,f),fflush(_altiaServerOutfile)):printf(a,b,c,d,e,f))

#else
#define PRINTPERROR(a) perror(a)
#define PRINTERR1(a) fprintf(stderr,a)
#define PRINTERR2(a,b) fprintf(stderr,a,b)
#define PRINTERR3(a,b,c) fprintf(stderr,a,b,c)
#define PRINTERR4(a,b,c,d) fprintf(stderr,a,b,c,d)
#define PRINTERR5(a,b,c,d,e) fprintf(stderr,a,b,c,d,e)
#define PRINTERR6(a,b,c,d,e,f) fprintf(stderr,a,b,c,d,e,f)

#endif /* ALTIAAPIDEBUG */
#endif /* printErrorOutput not already defined */


/*========================================================================
 *
 *  Class altiaCnct declarations:
 *  -----------------------------
 *  These are the lowest level functions for opening, closing, reading
 *  and writing on a socket.
 *
 *========================================================================*/

/*------------------------------------------------------------------------
 *  Declarations and definitions required only for Windows for
 *  socket access through a DLL.
 */
#ifndef clientWinSock
#define clientWinSock
#ifdef WIN32

/* Windows GetProcAddress does not work with
 * wchars so if not under WINCE use normal text for GetProcAddress
 */
#if defined(UNICODE) && defined(UNDER_CE)
#define PADDR_TEXT(x) L ## x
#else
#define PADDR_TEXT(x) x
#endif

/* Process socket activity on windows through a user message */
#define WM_SOCKET (WM_USER + 2)

#ifndef WINSOCKAPI
#define WINSOCKAPI PASCAL FAR
#endif

typedef  char FAR * (WINSOCKAPI * PINET_NTOA)(struct in_addr in);
typedef  unsigned long (WINSOCKAPI * PINET_ADDR)(const char FAR *);
typedef  int (WINSOCKAPI * PWSASTARTUP)(WORD, LPWSADATA); 
typedef  int (WINSOCKAPI* PWSACLEANUP)(VOID);
typedef  int (WINSOCKAPI* PWSAGETLASTERROR)(VOID);
typedef  int (WINSOCKAPI* PWSAASYNCSELECT)(SOCKET, HWND, u_int, long);
typedef  SOCKET (WINSOCKAPI* PACCEPT)(SOCKET, struct sockaddr FAR *, int FAR*);
typedef  int (WINSOCKAPI* PBIND)(SOCKET, const struct sockaddr FAR *, int);
typedef  int (WINSOCKAPI* PCLOSESOCKET)(SOCKET);
typedef  int (WINSOCKAPI* PCONNECT)(SOCKET, const struct sockaddr FAR *, int);
typedef  struct hostent FAR* (WINSOCKAPI* PGETHOSTBYNAME) (const char FAR *);
typedef  int (WINSOCKAPI* PGETHOSTNAME) (char FAR *, int);
typedef  struct servent FAR* (WINSOCKAPI* PGETSERVBYNAME) (const char FAR *,
                                                           const char FAR *);
typedef  struct hostent FAR* (WINSOCKAPI* PGETHOSTBYADDR) (const char FAR *,
                                                           int, int);
typedef  u_short (WINSOCKAPI* PHTONS) (u_short);
typedef  u_long (WINSOCKAPI* PHTONL) (u_long);
typedef  int (WINSOCKAPI* PIOCTLSOCKET) (SOCKET, long, u_long FAR *);
typedef  int (WINSOCKAPI* PLISTEN) (SOCKET, int);
typedef  u_short (WINSOCKAPI* PNTOHS) (u_short);
typedef  int (WINSOCKAPI* PRECV) (int, char FAR *, int, int);
typedef  long (WINSOCKAPI* PSELECT) (int, fd_set FAR*, fd_set FAR*, 
                                     fd_set FAR*,
                                     const struct timeval FAR *);
typedef  int (WINSOCKAPI* PSEND) (SOCKET, const char FAR *, int, int);
typedef  int (WINSOCKAPI* PSETSOCKOPT) (SOCKET, int, int, const char FAR *,
                                        int);
typedef  SOCKET (WINSOCKAPI* PSOCKET) (int, int, int);
typedef  int (WINSOCKAPI * P__WSAFDISSET)(SOCKET, fd_set FAR*);


#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern  PINET_NTOA pinet_ntoa;
extern  PINET_ADDR pinet_addr;
extern  PWSASTARTUP pWSAStartup;
extern  PWSACLEANUP pWSACleanup;
extern  PWSAGETLASTERROR pWSAGetLastError;
extern  PWSAASYNCSELECT pWSAAsyncSelect;
extern  PACCEPT paccept;
extern  PBIND pbind;
extern  PCLOSESOCKET pclosesocket;
extern  PCONNECT pconnect;
extern  PGETHOSTBYNAME pgethostbyname;
extern  PGETHOSTBYADDR pgethostbyaddr;
extern  PGETHOSTNAME pgethostname;
extern  PGETSERVBYNAME pgetservbyname;
extern  PHTONS phtons;
extern  PHTONL phtonl;
extern  PIOCTLSOCKET pioctlsocket;
extern  PLISTEN plisten;
extern  PNTOHS pntohs;
extern  PRECV precv;
extern  PSELECT pselect;
extern  PSEND psend;
extern  PSETSOCKOPT psetsockopt;
extern  PSOCKET psocket;
extern  P__WSAFDISSET p__WSAFDIsSet;
extern  HINSTANCE sockdll;

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#undef FD_ISSET

#define FD_ISSET(fd, set) (*p__WSAFDIsSet)((SOCKET)fd, (fd_set FAR *)set)

#endif /* WIN32 for socket access through a DLL */
#endif /* clientWinSock not already defined */


/*------------------------------------------------------------------------
 *  Very basic declarations and definitions required for socket
 *  connection functions.
 */
#ifndef aPpconnect_h

/*
 * Create a string pointer type that is the same
 * as the native string pointer type for the
 * Windows call to load a DLL library.
 */
#ifdef WIN32
#undef CharPtr
#if defined(UNDER_CE) || defined(UNICODE)
typedef unsigned short *CharPtr;
#else
typedef char *CharPtr;
#endif /* UNDER_CE || UNICODE */
#endif /* WIN32 */

/* Declaration for iovec on and sockaddr_un (if necessary) on Windows */
#ifdef WIN32
struct iovec
{
    char *iov_base;
    int iov_len;
};

#ifdef __GNUC__
struct sockaddr_un
{
    int dummy;
};
#endif
#endif

/* Declaration for sockaddr_un on VXWORKS */
#ifdef VXWORKS
struct sockaddr_un
{
};
#endif

struct altiaCnctData
{
    int _fd;
    int _fdWrite;
    int _domain;
    CONST char* _name;
    int _port;
    char *_errorString;
};


#ifdef WIN32
extern asvr_bool altiaCnctOpenSockets(CONST CharPtr libname); 
extern void    altiaCnctCloseSockets(void);
#endif /* WIN32 */


extern struct altiaCnctData*   altiaCnctNewConnect(void);

extern void      altiaCnctDeleteConnect(struct altiaCnctData* acd);

extern asvr_bool altiaCnctCreatePort(struct altiaCnctData* acd,
                      CONST char* hostname, int num);

extern asvr_bool altiaCnctCreateService(struct altiaCnctData* acd,
                      CONST char* hostname, CONST char* service);

extern asvr_bool altiaCnctCreateLocal(struct altiaCnctData* acd,
                      CONST char* fileName);

extern asvr_bool altiaCnctOpenPort(struct altiaCnctData* acd,
                      CONST char* hostname, int num);

extern asvr_bool altiaCnctOpenService(struct altiaCnctData* acd,
                      CONST char* hostname, CONST char* service);

extern asvr_bool altiaCnctOpenLocal(struct altiaCnctData* acd,
                      CONST char* fileName);

extern struct altiaCnctData* altiaCnctAcceptClient(
                                        struct altiaCnctData* acd);

extern int       altiaCnctPoll(struct altiaCnctData* acd,
                      asvr_bool mustHaveData, int timeoutSec);

extern int       altiaCnctWritePad(struct altiaCnctData* acd,
                      CONST void* buffer, int n, int padto);

extern void      altiaCnctClose(struct altiaCnctData* acd);

extern int       altiaCnctRead(struct altiaCnctData* acd,
                      void* buffer, int count);

extern int       altiaCnctWrite(struct altiaCnctData* acd,
                      CONST void* buffer, int count);

extern int       altiaCnctWriteNotify(struct altiaCnctData* acd,
                      CONST void* buffer, int count);

extern int       altiaCnctWritev(struct altiaCnctData* acd,
                      struct iovec* iovBuffer, int iovCount);

extern int       altiaCnctGetReadFD(struct altiaCnctData* acd);

extern int       altiaCnctGetWriteFD(struct altiaCnctData* acd);

extern CONST char* altiaCnctGetErrorString(struct altiaCnctData* acd);

extern asvr_bool altiaCnctMakeSocket(struct altiaCnctData* acd,
                      struct sockaddr_in* s);

extern int       altiaCnctMakeLocalSocket(struct altiaCnctData* acd,
                      struct sockaddr_un* s);

extern int       altiaCnctWordAlign(int n);

extern void      altiaEnableSigBlock(int yes);

extern void      altiaBlockSignals(int yes);

#endif /* aPpconnect_h not already defined */


/*========================================================================
 *
 *  Class altiaDispatcher declarations:
 *  -----------------------------------
 *  The dispatcher has the difficult task of actually waiting for
 *  input from clients as well as from user input devices and
 *  distributing the input to the appropriate destination.
 *
 *========================================================================*/
#ifndef dp_dispatcher_h

#define MAXIOFILES 16

typedef int (*altiaIOHandler)(int inputFD, void *data);
typedef int (*altiaTimerHandler)(long sec, long usec);

typedef struct _altiaFdMaskData
{
    fd_set _theSet;
    int    _numSet;
} altiaFdMaskData;

typedef struct _altiaIOHandlerMapData
{
    altiaIOHandler _handler[MAXIOFILES];
    void           *_handlerData[MAXIOFILES];
    int            _fd[MAXIOFILES];
} altiaIOHandlerMapData;

typedef struct _altiaTimerData
{
    struct timeval timerValue;
    altiaTimerHandler handler;
    struct _altiaTimerData *next;
#ifdef WIN32
    unsigned int id;
#endif
    int mark;
    int priority;
} altiaTimerData;

typedef struct _altiaTimerQueueData
{
    altiaTimerData *_first;
#ifdef WIN32
    int _timedout;
#endif
    int _priority;
} altiaTimerQueueData;

enum altiaDispatcherMask { DispatcherReadMask,
                           DispatcherWriteMask,
                           DispatcherExceptMask };

typedef struct _altiaDispatcherData
{
    int                    _nfds;
    int                    _anyready;
    altiaFdMaskData       *_rmask;
    altiaFdMaskData       *_wmask;
    altiaFdMaskData       *_emask;
    altiaFdMaskData       *_rmaskready;
    altiaFdMaskData       *_wmaskready;
    altiaFdMaskData       *_emaskready;
    altiaFdMaskData       *_rmasklast;
    altiaFdMaskData       *_wmasklast;
    altiaFdMaskData       *_emasklast;
    altiaIOHandlerMapData *_rtable;
    altiaIOHandlerMapData *_wtable;
    altiaIOHandlerMapData *_etable;
    altiaTimerQueueData   *_queue;
} altiaDispatcherData;


/*------------------------------------------------------------------------
 *  Declarations for dispatcher functions. There can only be one instance
 *  of a dispatcher in an executable space hence the reason why a pointer
 *  to the dispatcher is not passed to any of the dispatcher functions.
 *  Call altiaDispatcher_instance() to create and initialize the
 *  dispatcher the first time and keep calling it to just get the single
 *  instance of the dispatcher thereafter.
 */
altiaDispatcherData *altiaDispatcher_instance(void);
altiaDispatcherData *altiaDispatcher_create(void);
void                 altiaDispatcher_delete(void);

void altiaDispatcher_link(int fd, int mask,
                          altiaIOHandler handler, void *handlerData);
void altiaDispatcher_unlink(int fd);

altiaIOHandler altiaDispatcher_handler(int fd, int mask, void **handlerData);

void altiaDispatcher_startTimer(long sec, long usec,
                                altiaTimerHandler handler, int priority);
void altiaDispatcher_stopTimer(altiaTimerHandler handler);

void altiaDispatcher_setTimerPriority(int priority);
int  altiaDispatcher_getTimerPriority(void);

#define altiaDispatcher_anyReady() (_altiaDispatcher->_anyready)

int altiaDispatcher_dispatchTime(long *sec, long *usec);
int altiaDispatcher_dispatchTimeVal(struct timeval*);
int altiaDispatcher_dispatched(int fd, int mask);

#ifdef WIN32
int altiaDispatcher_BlockSocket(int fd);
int altiaDispatcher_UnblockSocket(int fd);
int altiaDispatcher_winMessage(int fd, int type);
int altiaDispatcher_timerMessage(unsigned int id);
int altiaDispatcher_timedOut(void); 
#endif

void altiaDispatcher_attach(int fd, int mask,
                            altiaIOHandler handler, void *handlerData);
void altiaDispatcher_detach(int fd);

int altiaDispatcher_fillInReady(altiaFdMaskData *rmask,
                                altiaFdMaskData *wmask,
                                altiaFdMaskData *emask);
int altiaDispatcher_waitFor(altiaFdMaskData *rmask,
                            altiaFdMaskData *wmask,
                            altiaFdMaskData *emask,
                            struct timeval *howlong);
void altiaDispatcher_notify(int nfound,
                            altiaFdMaskData *rmask,
                            altiaFdMaskData *wmask,
                            altiaFdMaskData *emask,
                            int checktimers);

struct timeval* altiaDispatcher_calculateTimeout(struct timeval *howlong);

void altiaDispatcher_handleError(void);

void altiaDispatcher_checkConnections(void);

#endif /* dp_dispatcher_h not already defined */


/*========================================================================
 *
 *  Altia API Default Socket Name/Number and Message Types:
 *  -------------------------------------------------------
 *  A client application communicates to an Altia graphics engine
 *  over a socket using a set of simple messages.  If not instructed
 *  otherwise, the altiaClientServer (see declaration later in this
 *  file) opens a default socket as a server.  A client application
 *  uses the Altia API library to connect to the server.  The client
 *  server creates an instance of an altiaClientAgent for each connected
 *  client.  The client application transmits messages with its instance
 *  of a client agent to control the graphics and respond to user input.
 *
 *========================================================================*/

/*----------------------------------------------------------------------
 * Below is the default base for the pathname used to create
 * domain sockets used to connect Altia to an application program.
 * The domain socket name created for host-to-host communications is
 * built by appending the host's name (as returned by gethostname(2))
 * to this default basename.
 */
#define DEFAULTBASENAME "/usr/tmp/vSe."
#define MAXBASELENGTH   20

/*----------------------------------------------------------------------
 * For Win32, Domain sockets not supported so define a default
 * socket number.
 */
#undef DEFAULT_PORT_STRING
#define DEFAULT_PORT_STRING "5100"

#define ALTIA_DOUBLE_TEST_VALUE 0.1234567

/*----------------------------------------------------------------------
 * Below are the message types/flags/masks for transactions between
 * Altia and an application program.  It is not important that application
 * writers be familiar with these.  They are here as a reference for HP
 * internal use.
 */

/* Message types for the type field in a client/Altia message */
#define NullMsg              0x00
#define RouteEventMsg        0x01
#define PollEventMsg         0x02
#define MoreEventsMsg        0x03
#define CheckEventsMsg       0x04
#define SelectEventMsg       0x05
#define UnselectEventMsg     0x06
#define FlushDisplayMsg      0x07
#define UpdateDisplayMsg     0x08
#define CheckEventMsg        0x09
#define EventSelectedMsg     0x0a
#define FloatInterfaceMsg    0x0b
#define SelectExternEventMsg   0x0c
#define UnselectExternEventMsg 0x0d
#define SyncClientsMsg       0x0e
#define StringFloatInterfaceMsg 0x0f
#define UndeadClientMsg      0x10
#define UndefinedMsg         0x7f

/* Flags for the type field in a client/Altia message */
#define ErrorInMsg      0x80

/* Masks for the type field in a client/Altia message */
#define MsgMask         0x7f
#define FlagsMask       0x80
#define ErrorInMsgMask  0x80


/*========================================================================
 *
 *  Class altiaClientQueue declarations:
 *  ------------------------------------
 *  A client queue is a circular double-linked list of events waiting
 *  to be sent to a client by the altiaClientAgent (see declaration
 *  later in this file).  Client applications never get events
 *  asynchronously.  Instead, the client agent buffers the events that
 *  a client selected to receive and then sends the events to the client
 *  when it requests new events.  The first element of a client queue
 *  does not contain data.  It is just a place holder for the queue so that
 *  there is always a valid queue even when there are no events in the queue.
 *
 *========================================================================*/

/*------------------------------------------------------------------------
 *  This is the data structure for a client queue element.
 */
typedef struct _altiaClientQueueData
{
    struct _altiaClientQueueData *_next;
    struct _altiaClientQueueData *_prev;
    AltiaEventType  _value;
    int  _nameLength;
    char *_name;
} altiaClientQueueData;

/*----------------------------------------------------------------------
 *  Function: altiaClientQueueData *altiaClientQueueCreate(
 *                                       char *eventName,
 *                                       AltiaEventType eventValue);
 *
 *    Creator for client queue cell takes an event name and its
 *    value.  A client queue is just a circular double-linked list of
 *    data elements.  The first element should be just a place holder
 *    for the queue and not contain any actual data.
 *
 *  Parameters:
 *    eventName             pointer to event name.
 *    eventValue            value for event.
 *
 *  Returns:
 *    Pointer to client queue data structure.
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
altiaClientQueueData *altiaClientQueueCreate(
                           char *eventName,
                           AltiaEventType eventValue);

/*----------------------------------------------------------------------
 *  Function: void altiaClientQueueDelete(altiaClientQueueData *element);
 *
 *  Deletes a client queue element.  It is assumed that the element
 *  has already been removed from a queue with altiaClientQueueRemove().
 *
 *  Parameters:
 *    element               pointer to data structure to delete.
 *
 *  Returns:
 *    Not appropriate
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
void altiaClientQueueDelete(altiaClientQueueData *element);

/*----------------------------------------------------------------------
 *  Function: void altiaClientQueueDeleteList(
 *                      altiaClientQueueData *head);
 *
 *  Goes through a linked list of client queue elements and deletes
 *  each one including the element passed in as the head.
 *
 *  Parameters:
 *    head                  pointer to data structure at the head of the
 *                          queue.
 *
 *  Returns:
 *    Not appropriate
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
void altiaClientQueueDeleteList(altiaClientQueueData *head);

/*----------------------------------------------------------------------
 *  Function: int altiaClientQueueEmpty(altiaClientQueueData *head);
 *
 *  Returns 1 (TRUE) if queue is empty.  If the head element is the only
 *  element in the queue, then it is empty.
 *
 *  Parameters:
 *    head                  pointer to data structure at the head of the
 *                          queue.
 *
 *  Returns:
 *    TRUE (1) if queue is empty, FALES (0) otherwise.
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
int altiaClientQueueEmpty(altiaClientQueueData *head);

/*----------------------------------------------------------------------
 *  Function: int altiaClientQueueSize(altiaClientQueueData *head);
 *
 *  Counts the number of elements linked to the head element.  The head
 *  element itself is not counted.  The head element is assumed to be
 *  just a placeholder for the queue and not an element that contains
 *  actual data.
 *
 *  Parameters:
 *    head                  pointer to data structure at the head of the
 *                          queue.
 *
 *  Returns:
 *    Number of linked elements not counting the head.
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
int altiaClientQueueSize(altiaClientQueueData *head);

/*----------------------------------------------------------------------
 *  Function: void altiaClientQueuePrepend(
 *                      altiaClientQueueData *head,
 *                      altiaClientQueueData *element);
 *
 *  Insert a new element before the head of the list.  Since the list
 *  is circular and double-linked, a prepend at the head is a shortcut
 *  for appending to the end.
 *
 *  Parameters:
 *    head                  pointer to data structure at the head of the
 *                          queue.
 *    element               pointer to new data structure to insert
 *                          before the head.
 *
 *  Returns:
 *    None
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
void altiaClientQueuePrepend(
          altiaClientQueueData *head,
          altiaClientQueueData *element);

/*----------------------------------------------------------------------
 *  Function: void altiaClientQueueAppend(
 *                      altiaClientQueueData *head,
 *                      altiaClientQueueData *element);
 *
 *  Insert a new element after the head of the list.  Since the head
 *  element is just a placeholder, an append at the head is really
 *  an insert at the head.
 *
 *  Parameters:
 *    head                  pointer to data structure at the head of the
 *                          queue.
 *    element               pointer to new data structure to insert
 *                          after the head.
 *
 *  Returns:
 *    None
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
void altiaClientQueueAppend(
          altiaClientQueueData *head,
          altiaClientQueueData *element);

/*----------------------------------------------------------------------
 *  Function: void altiaClientQueueRemove(altiaClientQueueData *element);
 *
 *  Remove the given element from a list.  An element has pointers
 *  to the previous and next elements in the list so this simply
 *  involves adjusting the next and previous pointers for the
 *  previous and next elements to point to eachother.
 *
 *  Parameters:
 *    element               pointer to element to be removed.
 *
 *  Returns:
 *    None
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
void altiaClientQueueRemove(altiaClientQueueData *element);

/*----------------------------------------------------------------------
 *  Function: altiaClientQueueData *altiaClientQueueNext(
 *                                       altiaClientQueueData *element);
 *
 *  Return the next element after the element given.
 *
 *  Parameters:
 *    element               pointer to an element in a queue list.
 *
 *  Returns:
 *    Pointer to next element in the list.
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
altiaClientQueueData *altiaClientQueueNext(altiaClientQueueData *element);

/*----------------------------------------------------------------------
 *  Function: void altiaClientQueueDeleteEvents(
 *                      altiaClientQueueData *head, char *eventName);
 *
 *    Remove all elements from the queue that have the given event name.
 *
 *  Parameters:
 *    head                  pointer to element at the head of the queue.
 *    eventName             name of events to be removed from queue.
 *
 *  Returns:
 *    None.
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
void altiaClientQueueDeleteEvents(
          altiaClientQueueData *head, char *eventName);


/*----------------------------------------------------------------------
 *  Function: int altiaClientQueueNameLength(altiaClientQueueData *element);
 *
 *    Return length of event name string for this client queue element.
 *
 *  Parameters:
 *    element               pointer to element.
 *
 *  Returns:
 *    String length of event name INCLUDING end of string character.
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
int altiaClientQueueNameLength(altiaClientQueueData *element);


/*----------------------------------------------------------------------
 *  Function: char *altiaClientQueueName(altiaClientQueueData *element);
 *
 *    This function returns the event name for this client queue element.
 *  
 *  Parameters:
 *    element               pointer to element.
 *
 *  Returns:
 *    Pointer to character string containing event name.
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
char *altiaClientQueueName(altiaClientQueueData *element);


/*----------------------------------------------------------------------
 *  Function: AltiaEventType altiaClientQueueValue(
 *                                altiaClientQueueData *element);
 *
 *    Returns the value for the event stored in this queue cell.
 *
 *  Parameters:
 *    None
 *
 *  Returns:
 *    Value.
 *
 *  Sideeffects:
 *    None
 *    
 *----------------------------------------------------------------------*/
AltiaEventType altiaClientQueueValue(altiaClientQueueData *element);


/*========================================================================
 *
 *  Class altiaClientFilter declarations:
 *  -------------------------------------
 *  A client filter is created by each altiaClientAgent (see declaration
 *  later in this file) to keep track of the names of events that a client
 *  has selected to receive.  When a new event is generated by the
 *  graphics engine, a client agent uses the client filter to determine if
 *  the event should go into the client queue for future transmission to
 *  the client.
 *
 *  A client application can tell the client agent it wants to ignore most
 *  events with some exceptions, or it is interested in most events with
 *  some exceptions.
 *
 *  When the client agent is notified of an event, it checks with the
 *  client filter to insure the event is one that the client is interested
 *  in.  It then sends the event on to the client if applicable.
 *
 *  The mode following client filter construction is to consider all events
 *  to be of interest.
 *
 *========================================================================*/

/*------------------------------------------------------------------------
 * First a list of events used by the ClientFilter class.
 *--------------------------------------------------------------------*/
typedef struct _altiaClientFilterListData
{
    char **_list;
    int _listSize;
    int _numEntries;
} altiaClientFilterListData;

/*--------------------------------------------------------------------*/
altiaClientFilterListData *altiaClientFilterListCreate(void);

/*--------------------------------------------------------------------*/
void altiaClientFilterListDelete(altiaClientFilterListData *data);

/*--------------------------------------------------------------------
 * Function: void altiaClientFilterListAddEntry(
 *                     altiaClientFilterListData *list, char* string);
 *
 *   Add entry to list
 *--------------------------------------------------------------------*/
void altiaClientFilterListAddEntry(
          altiaClientFilterListData *list, char* string);

/*--------------------------------------------------------------------
 * Function: void altiaClientFilterListDeleteEntry(
 *                     altiaClientFilterListData *list, char* string);
 *
 *   Delete entry from list
 *--------------------------------------------------------------------*/
void altiaClientFilterListDeleteEntry(
          altiaClientFilterListData *list, char* string);

/*--------------------------------------------------------------------
 * Function: void altiaClientFilterListDeleteAllEntries(
 *                     altiaClientFilterListData *list);
 *
 *   Delete all entries from list
 *--------------------------------------------------------------------*/
void altiaClientFilterListDeleteAllEntries(altiaClientFilterListData *list);

/*--------------------------------------------------------------------
 * Function: int altiaClientFilterListFindEntry(
 *                    altiaClientFilterListData *list, char* string);
 *
 *   Find entry in list and return 1 (TRUE) if it is found
 *--------------------------------------------------------------------*/
int altiaClientFilterListFindEntry(
         altiaClientFilterListData *list, char* string);

/*------------------------------------------------------------------------
 *  The client filter data structure.
 *------------------------------------------------------------------------*/
typedef struct _altiaClientFilterData
{
    altiaClientFilterListData *_currentList;
    int _interestedInAll;
    int _interestedExtern;
} altiaClientFilterData;

/*--------------------------------------------------------------------*/
altiaClientFilterData *altiaClientFilterCreate(void);

/*--------------------------------------------------------------------*/
void altiaClientFilterDelete(altiaClientFilterData *data);

/*--------------------------------------------------------------------
 * Function: int altiaClientFilterInteresting(
 *                    altiaClientFilterData *data, char *eventName);
 *
 *   For checking if a client is interested in an event
 *   before sending the event to the client.
 *--------------------------------------------------------------------*/
int altiaClientFilterInteresting(
         altiaClientFilterData *data, char *eventName);

/*--------------------------------------------------------------------
 * Function: void altiaClientFilterUnignoreAll(altiaClientFilterData *data);
 *
 *   To force interest in all events in the future
 *--------------------------------------------------------------------*/
void altiaClientFilterUnignoreAll(altiaClientFilterData *data);

/*--------------------------------------------------------------------
 * Function: void altiaClientFilterIgnoreAll(altiaClientFilterData *data);
 *
 *   To force disinterest in all events in the future
 *--------------------------------------------------------------------*/
void altiaClientFilterIgnoreAll(altiaClientFilterData *data);

/*--------------------------------------------------------------------
 * Function: void altiaClientFilterUnignore(
 *                     altiaClientFilterrData *data, char *eventName);
 *
 *   To force interest in a particular event if it is not
 *   already of interest to the client.
 *--------------------------------------------------------------------*/
void altiaClientFilterUnignore(altiaClientFilterData *data, char *eventName);

/*--------------------------------------------------------------------
 * Function: void altiaClientFilterUnignoreExtern(altiaClientFilterData *data)
 *   Force interest in all external events
 *--------------------------------------------------------------------*/
void altiaClientFilterUnignoreExtern(altiaClientFilterData *data);

/*--------------------------------------------------------------------
 * Function: void altiaClientFilterIgnoreExtern(altiaClientFilterData *data)
 *   Unforce interest in all external events
 *--------------------------------------------------------------------*/
void altiaClientFilterIgnoreExtern(altiaClientFilterData *data);

/*--------------------------------------------------------------------
 * Function: void altiaClientFilterIgnore(
 *                     altiaClientFilterData *data, char *eventName);
 *
 *   To force disinterest in a particular event if it is
 *   currently of interest to the client.
 *--------------------------------------------------------------------*/
void altiaClientFilterIgnore(altiaClientFilterData *data, char *eventName);

/*--------------------------------------------------------------------
 * Function: int altiaClientFilterNameInList(
 *                    altiaClientFilterData *data, char *eventName);
 *
 *   Return 1 (TRUE) if name is in filter list.
 *--------------------------------------------------------------------*/
int altiaClientFilterNameInList(altiaClientFilterData *data, char *eventName);

/*--------------------------------------------------------------------
 * Function: int altiaClientFilterInterestedInExtern(
 *                    altiaClientFilterData *data)
 *
 *   Return 1 (TRUE) if interested in extern events
 *--------------------------------------------------------------------*/
int altiaClientFilterInterestedInExtern(altiaClientFilterData *data);


/*========================================================================
 *
 *  Class altiaClientServer declarations:
 *  -------------------------------------
 *  A client server establishes the primary socket and is responsible
 *  for serving that socket to receive requests from new clients to
 *  connect or to clean up after a client disconnects.  For each
 *  connecting client, the client server creates an instance of an
 *  altiaClientAgent (see declaration later in this file).  The client
 *  agent is actually responsible for transmitting messages to/from
 *  the client.
 *
 *  The client server establishes a handler with the altiaDispatcher
 *  to get called when a file descriptor is ready for input as determined
 *  by select(2).
 *
 *  There can only be one instance of a client server in an executable
 *  space hence the reason why a pointer to the client server is not
 *  passed to any of the client server functions.
 *
 *  Call altiaClientServerInstance() to create and initialize the
 *  client server the first time and keep calling it to just get the
 *  single instance of the client server thereafter.
 *
 *========================================================================*/

/*------------------------------------------------------------------------
 *  Declaration for a data structure used to hold a delayed message to
 *  a client.
 *------------------------------------------------------------------------*/
typedef struct _altiaDelayedMessageData
{
    unsigned char *_field1;
    unsigned char *_field2;
    unsigned char *_field3;
    unsigned char *_field4;
    int _size1;
    int _size2;
    int _size3;
    int _size4;
    int _msgType;
} altiaDelayedMessageData;


/*------------------------------------------------------------------------
 *  Declarations for the client server data structures.
 *
 *  Note: Data structure declaration for client agent is embedded in
 *        data structure declaration for client server.
 *------------------------------------------------------------------------*/
typedef struct altiaCnctData altiaClientConnectData;

struct _altiaClientServerData
{
    altiaClientConnectData         *_domainConnect;
    char                           *_domainName;
    altiaClientConnectData         *_netConnect;

    struct _altiaClientAgentData
    {
        struct _altiaClientAgentData  *_next;
        struct _altiaClientServerData *_server;

        int                            _readFD;
        int                            _writeFD;
        int                            _partialStart;
        int                            _readSize;
        int                            _clientQueueSpace;
        int                            _lastMsgType;
        int                            _doUpdate;
        int                          (*_routeEvent)(ALTIA_CHAR *name,
                                                    AltiaEventType value);
        void                         (*_update)(void);
        altiaClientConnectData        *_connection;

        altiaIOHandler                 _rHandler;
        altiaIOHandler                 _wHandler;
        altiaIOHandler                 _eHandler;
        altiaIOHandler                 _messageHandler;
        altiaClientFilterData         *_eventFilter;
        altiaClientQueueData          *_eventQueue;

        /* Others can ask us to route events just before we destruct
         * ourselves.  Those requests are kept track of in a list.
         */
        altiaClientQueueData          *_lastRouteList;

        unsigned char                 *_inputBuf;
        int                            _inputBufSize;
        unsigned char                 *_nextInputPtr;
        unsigned char                 *_outputBuf;
        int                            _outputBufSize;
        asvr_bool                      _floatInterface;
        asvr_bool                      _stringFloat;
        asvr_bool                      _swapRValue;
        asvr_bool                      _notify;
        asvr_bool                      _delay;
        asvr_bool                      _undead;
        altiaDelayedMessageData       *_delayMess;

    }                              *_clientAgents;

    /* Keep track if caller wants to start DeepScreen without a window */
    asvr_bool                      _nowin;

    /* Keep track if an altiaQuit has been detected. */
    asvr_bool                      _quit;

    /* Next data element indicates that a delay is on and we have
     * to route stuff to turn it off or visa/versa.
     */
    asvr_bool                     _syncDelayOn;

    /* Next data element indicates that a client has requested that
     * all clients' event queue sizes be synced.
     */
    asvr_bool                     _syncClients;

    /* If an event originates from a client, the client agent can
     * identify itself and the event name and the event value so that the
     * client server ignores the event if it comes back to the client
     * server to retransmit to the client.
     */
    struct _altiaClientAgentData *_ignoreClient;
    char *                        _ignoreName;
    AltiaEventType                _ignoreValue;
};

typedef struct _altiaClientServerData altiaClientServerData;
typedef struct _altiaClientAgentData  altiaClientAgentData;

/*----------------------------------------------------------------------
 *  Function: altiaClientServerData *altiaClientServerInstance(
 *                                        int argc, char **argv);
 *
 *    Faster way of querying for the single instance of the client
 *    server.  On the first call, pass argc and argv to associate a
 *    specific socket with the server (instead of the default domain
 *    socket on UNIX/Linux or the default network socket (5100)
 *    on Windows, VxWorks, etc.).  After the first call, can pass
 *    0 for argc and NULL for argv.
 *
 *  Parameters:
 *    argc, argv             For parsing command line parameters to
 *                           determine if user has provided a specific
 *                           socket service name.
 *
 *  Returns:
 *    Pointer to new client server data structure
 *
 *  Sideeffects:
 *    None that might be classified as !gotchas!
 *----------------------------------------------------------------------*/
extern altiaClientServerData *altiaClientServerInstance(int argc, char **argv);

/*----------------------------------------------------------------------
 *  Function: altiaClientServerData *altiaClientServerCreate(
 *                                        int argc, char **argv);
 *
 *    Opens a socket to accept multiple client connections.  This
 *    function parses argv[] looking for a custom socket service
 *    name.  If no socket service name is found, the default is used.
 *
 *  Parameters:
 *    argc, argv             For parsing command line parameters to
 *                           determine if user has provided a specific
 *                           socket service name.
 *
 *  Returns:
 *    Pointer to new client server data structure
 *
 *  Sideeffects:
 *    None that might be classified as !gotchas!
 *----------------------------------------------------------------------*/
extern altiaClientServerData *altiaClientServerCreate(int argc, char **argv);

/*----------------------------------------------------------------------
 *  Function: void altiaClientServerDelete(altiaClientServerData *server);
 *
 *    Closes socket that accepts new client connects and also deletes
 *    all instances of ClientAgent known by the server.
 *
 *  Parameters:
 *    Pointer to client server data structure
 *
 *  Returns:
 *    None
 *
 *  Sideeffects
 *    None that might be classified as !gotchas!
 *----------------------------------------------------------------------*/
extern void altiaClientServerDelete(altiaClientServerData *server);

/*----------------------------------------------------------------------
 *  Function: void altiaClientServerDebugOutputOpen(void)
 *
 *    Creates a debug window if necessary for debug output.  For
 *    Windows CE where a debug window is not supported, opens "cerror.log"
 *    instead and initializes a pointer to the opened file.
 *
 *  Parameters:
 *    None
 *
 *  Returns:
 *    None
 *
 *  Sideeffects
 *    Might create a console window on Windows or a file named
 *    "cerror.log" on Windows CE.
 *----------------------------------------------------------------------*/
extern void altiaClientServerDebugOutputOpen(void);

/*----------------------------------------------------------------------
 *  Function: int altiaClientServerQuitDetected(altiaClientServerData *server);
 *
 *    Closes socket that accepts new client connects and also deletes
 *    all instances of ClientAgent known by the server.
 *
 *  Parameters:
 *    Pointer to client server data structure
 *
 *  Returns:
 *    None
 *
 *  Sideeffects
 *    None that might be classified as !gotchas!
 *----------------------------------------------------------------------*/
#define altiaClientServerQuitDetected(server) ((int) ((server)->_quit))

/*----------------------------------------------------------------------
 *  Function: void altiaClientServerDisconnectClients(void);
 *
 *    This function deletes all the clientagents causing all the 
 *    clients to be disconnected.
 *
 *  Parameters:
 *    None
 *
 *  Returns:
 *    Not appropriate
 *
 *  Sideeffects
 *    If the clients have high retry counts they will try to reconnect.
 *----------------------------------------------------------------------*/
extern void altiaClientServerDisconnectClients(void);

/*----------------------------------------------------------------------
 *  Function: void altiaClientServerHandleEvent(
 *                      ALTIA_CHAR *eventName, AltiaEventType eventValue)
 *
 *    This function can be hooked into the DeepScreen AltiaReportFuncPtr
 *    variable to allow the client server to receive events from DeepScreen.
 *
 *  Parameters:
 *    eventName             Name for new event.
 *    eventValue            Value for new event.
 *
 *  Returns:
 *    None
 *
 *  Sideeffects:
 *    May generate events for one or more clients.
 *----------------------------------------------------------------------*/
extern void altiaClientServerHandleEvent(
                 ALTIA_CHAR *eventName, AltiaEventType eventValue);

/*----------------------------------------------------------------------
 *  Function: int altiaClientServerInputReady(int inputFD, void *data);
 *
 *    Called if a descriptor known to us (the server's or a client
 *    agent's) possibly needs service.  A descriptor may need service
 *    if it has data available or the client has disconnected.  On
 *    a socket disconnect, we delete the associated agent.
 *
 *  Parameters:
 *    inputFD               Descriptor that may need service
 *    data                  Extra data (usually agent pointer)
 *
 *  Returns:
 *    0 if service handled ok, -1 if service is closed.
 *
 *  Sideeffects:
 *    May create new instance of ClientAgent if a new client
 *    connection is accepted and may delete one if a client disconnects.
 *----------------------------------------------------------------------*/
extern int altiaClientServerInputReady(int inputFD, void *data);


/*----------------------------------------------------------------------
 *  Function: int altiaClientServerOutputReady(int outputFD, void *data);
 *
 *    Relative only for agents using pipe services.  This function is
 *    called by the dispatcher when a reader is detected on an outbound
 *    pipe.  This indicates to us that a client has connected to the
 *    pipe.  We can attach the related agent to the BehaviorRouter and
 *    add a handler (i.e., InputReady()) to the system dispatcher to
 *    detect data on the service's inbound pipe.
 *
 *  Parameters:
 *    outputFD        Descriptor for pipe that has become writeable
 *    data                  Extra data (usually agent pointer)
 *
 *  Returns:
 *    -1 if we want this handler removed from dispatcher's list.  This is
 *   the typical return. Otherwise, 0.
 *
 *  Sideeffects:
 *    May reattach an existing ClientAgent to the BehaviorRouter.
 *----------------------------------------------------------------------*/
extern int altiaClientServerOutputReady(int outputFD, void *data);


/*----------------------------------------------------------------------
 *  Function: int altiaClientServerEventSelected(char *name);
 *
 *    Returns TRUE if the given event name is selected for receipt by
 *    one of the currently connected clients.  Used to determine if a
 *    view should be closed immediately or an event should be routed
 *    to allow a client application to close the view.
 *
 *  Parameters:
 *    name        Name of event to check
 *
 *  Returns:
 *    1 (TRUE) if event is selected for receipt by a client application.
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
extern int altiaClientServerEventSelected(char *name);

/*----------------------------------------------------------------------
 *  Function: int altiaClientServerNumClients(void);
 *
 *    Returns the number of currently connected clients.
 *
 *  Parameters:
 *    none
 *
 *  Returns:
 *    the number of client applications currently connected.
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
extern int altiaClientServerNumClients(void);

/*----------------------------------------------------------------------
 *  Function: altiaClientAgentData *altiaClientServerFindAgent(char *event);
 *
 *    Returns the first client agent that has selected to recieve
 *    the passed in event.  If none, then it returns NULL.
 *
 *  Parameters:
 *    event name
 *
 *  Returns:
 *    altiaClientAgent pointer
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
extern altiaClientAgentData *altiaClientServerFindAgent(char *event);

/*----------------------------------------------------------------------
 *  Function: altiaClientAgentData *altiaClientServerGetClientAgents(void);
 *
 *    Returns the head of the client agent linked list.  If there are
 *    no client agains, then it returns NULL.
 *
 *  Parameters:
 *    event name
 *
 *  Returns:
 *    altiaClientAgent pointer
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
extern altiaClientAgentData *altiaClientServerGetClientAgents(void);



/*========================================================================
 *
 *  Class altiaClientAgent declarations:
 *  ------------------------------------
 *  These are the client agent functions.  They are called by
 *  the altiaClientServer when there are indications that a client
 *  needs attention (e.g. select(2) indicates there is data to read on
 *  the socket descriptor associated with the client).
 *
 *========================================================================*/

/*----------------------------------------------------------------------
 *  Function: altiaClientAgentData *altiaClientAgentCreate(
 *                                       AltiaClientServerData *server,
 *                                       AltiaClientConnectData *connection,
 *                                       altiaIOHandler rHandler,
 *                                       altiaIOHandler wHandler,
 *                                       altiaIOHandler eHandler);
 *
 *    Creates a Client Agent data structure saving server and connection
 *    data for communicating with client.
 *
 *  Parameters:
 *    server                pointer to ClientServer data.
 *    connection            pointer to ClientConnect data.
 *
 *  Returns:
 *    Pointer to a new ClientAgent data structure.
 *
 *  Sideeffects:
 *    None that might be classified as !gotchas!
 *----------------------------------------------------------------------*/
extern altiaClientAgentData *altiaClientAgentCreate(
                                  altiaClientServerData *server,
                                  altiaClientConnectData *connection,
                                  altiaIOHandler rHandler,
                                  altiaIOHandler wHandler,
                                  altiaIOHandler eHandler);


/*----------------------------------------------------------------------
 *  Function: void altiaClientAgentDelete(altiaClientAgentData *agent);
 *
 *    Destroys a Client Agent.  It frees the instance of
 *    ClientConnection that it is using to talk to the client app.
 *
 *  Parameters:
 *    None
 *
 *  Returns:
 *    Not appropriate
 *
 *  Sideeffects:
 *    Client connection it is closed.
 *----------------------------------------------------------------------*/
extern void altiaClientAgentDelete(altiaClientAgentData *agent);


/*----------------------------------------------------------------------
 *  Function: void altiaClientAgentAddLastRoute(altiaClientAgentData *agent,
 *                      char *eventName, AltiaEventType value);
 *
 *    Others can ask Client Agent to route a particular event before it
 *    deletes itself.  This way, they can be made aware that the particular
 *    client program has disconnected.  This function takes an event
 *    name and value.  It will add it to its list of events to
 *    route when it is deleted.
 *
 *  Parameters:
 *    eventName            Name of event to route on destruction
 *    value                value to route on destruction.
 *
 *  Returns:
 *    None
 *
 *  Sideeffects:
 *    Events may be routed sometime in the distant future.
 *----------------------------------------------------------------------*/
extern void altiaClientAgentAddLastRoute(altiaClientAgentData *agent,
                         char *eventName, AltiaEventType value);


/*----------------------------------------------------------------------
 *  Function: void altiaClientAgentRemoveLastRoute(
 *                      altiaClientAgentData *agent,
 *                      char *eventName, AltiaEventType value);
 *
 *    Others can ask Client Agent to route a particular event before it
 *    deletes itself.  This way, they can be made aware that the particular
 *    client program has disconnected.  They may want to remove such
 *    a request given certain circumstances.  This function takes an event
 *    name and value.  It will remove all instances in the "last route"
 *    list that match the name and value.
 *
 *  Parameters:
 *    eventName            Name of event to NOT route on destruction
 *    value                value to NOT route on destruction.
 *
 *  Returns:
 *    None
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
extern void altiaClientAgentRemoveLastRoute(altiaClientAgentData *agent,
                            char *eventName, AltiaEventType value);


/*----------------------------------------------------------------------
 *  Function: int altiaClientAgentGetReadFD(altiaClientAgentData *agent)
 *
 *    Returns the value of the file descriptor used for reading by the
 *    ClientConnection or -1 if this agent is in trouble.
 *    If -1 is returned, it doesn't necessarily mean that the
 *    connection is closed.  It can also indicate that the agent
 *    had trouble registering with the BehaviorRouter or
 *    initializing its behavior event queue or filter.
 *
 *  Parameters:
 *    None
 *
 *  Returns:
 *    File descriptor value or -1
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
#define altiaClientAgentGetReadFD(agent) ((agent)->_readFD)


/*----------------------------------------------------------------------
 *  Function: int altiaClientAgentGetWriteFD(altiaClientAgentData *agent)
 *
 *    Returns the value of the file descriptor used for writing by the
 *    ClientConnection or -1 if this agent is in trouble.
 *    If -1 is returned, it doesn't necessarily mean that the
 *    connection is closed.  It can also indicate that the agent
 *    had trouble registering with the BehaviorRouter or
 *    initializing its behavior event queue or filter.
 *
 *    The descriptor returned by this function may be identical
 *   to the descriptor returned by GetReadFD() if the service
 *    being used is bidirectional.
 *
 *  Parameters:
 *    None
 *
 *  Returns:
 *    File descriptor value or -1
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
#define altiaClientAgentGetWriteFD(agent) ((agent)->_writeFD)

/*----------------------------------------------------------------------
 *  Function: int altiaClientAgentCheckClient(altiaClientAgentData *agent);
 *
 *    This function is called when there are indications that the client
 *    application associated with this agent needs attention.  For
 *    instance, a select(2) indicates the file desciptor for the
 *    connection needs reading. This function reads in data and then
 *    calls _handleMessage() to handle the messages in the read buffer.
 *    
 *    This function returns >=0 if the client connection truly had
 *    data.  A return of -1 indicates the client connection closed
 *    (i.e. had no data) or there was an I/O error.
 *
 *  Parameters:
 *    None
 *
 *  Returns:
 *    -1 if no data available.  Otherwise, returns the value returned
 *    by _handleMessage.
 *
 *  Sideeffects:
 *    If connection becomes broken, this routine essentially shuts
 *    down this instance of ClientAgent until it is destructed.
 *    Subsequent calls to this function will always return FALSE.
 *    
 *----------------------------------------------------------------------*/
extern int altiaClientAgentCheckClient(altiaClientAgentData *agent);

/*----------------------------------------------------------------------
 *  Function: void altiaClientAgentAttachToRouter(altiaClientAgentData *agent);
 *
 *    NOT IMPLEMENTED FOR DEEPSCREEN.  In Altia verion, attaches this
 *    agent's event receiver to the event router.
 *
 *  Parameters:
 *    None
 *
 *  Returns:
 *    None
 *
 *  Sideeffects:
 *    None
 *    
 * extern void altiaClientAgentAttachToRouter(altiaClientAgentData *agent);
 *
 *----------------------------------------------------------------------*/


/*----------------------------------------------------------------------
 *  Function: void altiaClientAgentDetachFromRouter(
 *                      altiaClientAgentData *agent);
 *
 *    NOT IMPLEMENTED FOR DEEPSCREEN.  In Altia verion, detaches
 *    this agent's event receiver from the event router.
 *
 *  Parameters:
 *    None
 *
 *  Returns:
 *    None
 *
 *  Sideeffects:
 *    None
 *    
 * extern void altiaClientAgentDetachFromRouter(altiaClientAgentData *agent);
 *
 *----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
 *  Function: void altiaClientAgentClearEventQueue(
 *                      altiaClientAgentData *agent, char *eventName);
 *
 *    Clears out specific events from pending event queue.  If eventName
 *    points to "*", all events are cleared.
 *
 *  Parameters:
 *    None
 *
 *  Returns:
 *    None
 *
 *  Sideeffects:
 *    None
 *    
 *----------------------------------------------------------------------*/
extern void altiaClientAgentClearEventQueue(altiaClientAgentData *agent,
                                            char *eventName);

/*----------------------------------------------------------------------
 *  Function: void altiaClientAgentSelectEvent(
 *                      altiaClientAgentData *agent, char *eventName);
 *
 *    Select receive routes of a particular event.  If eventName points
 *    to "*", all events are received.
 *
 *  Parameters:
 *    eventName       Points to name of events to select ("*" for all)
 *
 *  Returns:
 *    None
 *
 *  Sideeffects:
 *    None
 *    
 *----------------------------------------------------------------------*/
extern void altiaClientAgentSelectEvent(
                 altiaClientAgentData *agent, char *eventName);

/*----------------------------------------------------------------------
 *  Function: void altiaClientAgentSelectExternEvent(
 *                      altiaClientAgentData *agent);
 *
 *    Select receive routes of all events that are not labeled as
 *    internal
 *
 *  Parameters:
 *    None
 *
 *  Returns:
 *    None
 *
 *  Sideeffects:
 *    All internal events will be sent even if they are on the ignore 
 *    list
 *    
 *----------------------------------------------------------------------*/
extern void altiaClientAgentSelectExternEvent(altiaClientAgentData *agent);

/*----------------------------------------------------------------------
 *  Function: void altiaClientAgentUnselectEvent(
 *                      altiaClientAgentData *agent, char *eventName);
 *
 *    Unselect to receive routes of a particular event.  If eventName
 *    points to "*", all events are unreceived.
 *
 *  Parameters:
 *    eventName       Points to name of events to unselect ("*" for all)
 *
 *  Returns:
 *    None
 *
 *  Sideeffects:
 *    None
 *    
 *----------------------------------------------------------------------*/
extern void altiaClientAgentUnselectEvent(
                 altiaClientAgentData *agent, char *eventName);

/*----------------------------------------------------------------------
 *  Function: extern void altiaClientAgentUnselectExternEvent(
 *                             altiaClientAgentData *agent);
 *
 *    Unselect to receive all events that are not internal
 *
 *  Parameters:
 *    None
 *
 *  Returns:
 *    None
 *
 *  Sideeffects:
 *    None
 *    
 *----------------------------------------------------------------------*/
extern void altiaClientAgentUnselectExternEvent(altiaClientAgentData *agent);

/*----------------------------------------------------------------------
 *  Function: int altiaClientAgentGetPendingCount(altiaClientAgentData *agent);
 *
 *    Get number of events in event queue that are pending
 *
 *  Parameters:
 *    None
 *
 *  Returns:
 *    Value from 0 to n
 *
 *  Sideeffects:
 *    None
 *    
 *----------------------------------------------------------------------*/
extern int altiaClientAgentGetPendingCount(altiaClientAgentData *agent);

/*----------------------------------------------------------------------
 *  Function: int altiaClientAgentGetNextEvent(altiaClientAgentData *agent,
 *                     char *nameStringInOut, int stringSize, 
 *                     AltiaEventType *eventValueOut);
 *    Get next available event from event queue if any are available
 *
 *  Parameters:
 *    nameStringInOut            character array to put new event name
 *    stringSize                 size of array
 *    eventValueOut              pointer to int to hold value for event
 *
 *  Returns:
 *    0 if event found or -1 if queue is empty
 *
 *  Sideeffects:
 *    Event returned is removed from queue
 *    
 *----------------------------------------------------------------------*/
extern int altiaClientAgentGetNextEvent(altiaClientAgentData *agent,
                char *nameStringInOut, int stringSize, 
                AltiaEventType *eventValueOut);

/*----------------------------------------------------------------------
 *  Function: int altiaClientAgentCheckEvent(altiaClientAgentData *agent,
 *                     char *eventName, AltiaEventType *nextValueOut);
 *
 *    Looks in queue for an instance of an event of the given name.
 *    search is done whether or not event is at the head of the queue.
 *    If an instance exists, event is pulled from the queue and caller
 *    is returned the value of the event in nextValueOut.  The function
 *    itself returns TRUE if an instance is found or FALSE otherwise.
 *
 *  Parameters:
 *    eventName                 name of event to look for.
 *    nextValueOut              pointer to int to hold value for event
 *
 *  Returns:
 *    TRUE (1) if an instance found, FALSE (0) otherwise.
 *
 *  Sideeffects:
 *    Instance of event removed from queue if one is found
 *    
 *----------------------------------------------------------------------*/
extern int altiaClientAgentCheckEvent(altiaClientAgentData *agent,
                char *eventName, AltiaEventType *nextValueOut);

/*----------------------------------------------------------------------
 *  Function: int altiaClientAgentEventSelected(
 *                     altiaClientAgentData *agent, char *eventName)
 *
 *    Checks to see if we are selected to recieve specified event name.
 *
 *  Parameters:
 *    eventName         name of event we want to check selection for
 *
 *  Returns:
 *    TRUE if we are interested in event, FALSE otherwise.
 *
 *  Sideeffects:
 *    None
 *    
 *----------------------------------------------------------------------*/
extern int altiaClientAgentEventSelected(
                altiaClientAgentData *agent, char *eventName);

/*----------------------------------------------------------------------
 *  Function: int altiaClientAgentEventSelectedByName(
 *                     altiaClientAgentData *agent, char *eventName)
 *
 *    Checks to see if the client explicitly requested to select
 *    this event name.  (ie it will return FALSE if client selected
 *    all events).
 *
 *  Parameters:
 *    eventName         name of event we want to check selection for
 *
 *  Returns:
 *    TRUE if we are interested in event, FALSE otherwise.
 *
 *  Sideeffects:
 *    None
 *    
 *----------------------------------------------------------------------*/
int altiaClientAgentEventSelectedByName(
         altiaClientAgentData *agent, char *eventName);

/*----------------------------------------------------------------------
 *  Function: altiaClientConnectData *altiaClientAgentGetConnection(
 *                                         altiaClientAgentData *agent)
 *
 *    Return the client connection that this agent is using
 *
 *  Parameters:
 *
 *  Returns:
 *    The ClientConnect
 *
 *  Sideeffects:
 *    None
 *    
 *----------------------------------------------------------------------*/
#define altiaClientAgentGetConnection(agent) ((agent)->_connection)

/*----------------------------------------------------------------------
 *  Function: void altiaClientAgentSetClientNotify(
 *                      altiaClientAgentData *agent, int flag)
 *
 *    Set the client notify flag based upon flag passed in.
 *    If FALSE is sent then the client will not be notified via 
 *    NULL messages that events are available.  If TRUE the
 *    the default behavior of notifying the client is done.
 *
 *  Parameters:
 *    value to set the notify flag
 *
 *  Returns:
 *    None
 *
 *  Sideeffects:
 *    Changes client agents behavior
 *    
 *----------------------------------------------------------------------*/
#define altiaClientAgentSetClientNotify(agent,flag) ((agent)->_notify = (flag))

/*----------------------------------------------------------------------
 *  Function: void altiaClientAgentRouteDelay(
 *                      altiaClientAgentData *agent, int flag);
 *
 *    Route the delay message via router to other clientagents.
 *    If the flag is TRUE other clientagents are requested to delay
 *    until the client agent can reduce its queue.  If the flag
 *    is FALSE then the other clientagents can resume normal operation.
 *
 *  Parameters:
 *    value to set the notify flag
 *
 *  Returns:
 *    None
 *
 *  Sideeffects:
 *    Changes client agents behavior
 *    
 *----------------------------------------------------------------------*/
extern void altiaClientAgentRouteDelay(
                 altiaClientAgentData *agent, int flag);

/*----------------------------------------------------------------------
 *  Function: void altiaClientAgentSendDelayMess(altiaClientAgentData *agent);
 *
 *    Send the delayed message to the client if one exists. This
 *    also set the lastMsgType and sends the notify message as 
 *    required.
 *
 *  Parameters:
 *    none
 *
 *  Returns:
 *    None
 *
 *  Sideeffects:
 *    Changes client agents behavior
 *    
 *----------------------------------------------------------------------*/
extern void altiaClientAgentSendDelayMess(altiaClientAgentData *agent);


/*----------------------------------------------------------------------
 *  Function: void altiaClientAgentSetDelay(
 *                      altiaClientAgentData *agent, unsigned char value);
 *
 *    Process a request to enable or disable delay. If request is to
 *    disable a delay and the given agent is delayed, a delay message
 *    is sent to the client.
 *
 *  Parameters:
 *    value to enable (1) or disable (0) delay
 *
 *  Returns:
 *    None
 *
 *  Sideeffects:
 *    Changes client agents behavior
 *    
 *----------------------------------------------------------------------*/
extern void altiaClientAgentSetDelay(
                 altiaClientAgentData *agent, int value);

/*----------------------------------------------------------------------
 *  Function: int altiaClientAgentUndead(altiaClientAgentData *agent)
 *
 *    If the client cannot be killed then return TRUE (1).
 *
 *  Parameters:
 *    none
 *
 *  Returns:
 *    0 or 1
 *
 *  Sideeffects:
 *    Changes client agents behavior
 *    
 *----------------------------------------------------------------------*/
#define altiaClientAgentUndead(agent) ((agent)->_undead)

/*----------------------------------------------------------------------
 *  Function: asvr_bool altiaClientAgent_handleWindowChanges(
 *                          altiaClientServerData *server, char *name,
 *                          AltiaEventType value)
 *
 *    This function is called by function altiaClientAgent_handleMessage()
 *    to check to see if the event from the client is a request to
 *    manipulate the DeepScreen main window (such as open/show or close/hide
 *    it, move it, or set its style).  DeepScreen "Target" functions are
 *    directly invoked to handle a change.
 *
 *  Parameters:
 *    name             name of event
 *    value            value of event
 *
 *  Returns:
 *   True if a main view window change was processed.
 *
 *  Sideeffects:
 *   If the event name matches one of the recognized event names and
 *   the value is OK, a change to the DeepScreen main window is performed
 *   using a call to one (or more) DeepScreen "Target" functions.
 *
 *----------------------------------------------------------------------*/
extern asvr_bool altiaClientAgent_handleWindowChanges(
                     altiaClientServerData *server, char *name,
                     AltiaEventType value);

/*----------------------------------------------------------------------
 *  Function: extern int altiaClientAgent_handleMessage(int readFD,
 *                                                      void *data);
 *
 *    This function is called by function altiaClientAgentCheckClient()
 *    immediately after data is read into our input buffer, or it is
 *    called from the dispatcher to handle multiple messages in the
 *    input buffer until they are exhausted.
 *
 *  Parameters:
 *    inputFD               Descriptor that may need service
 *    data                  Extra data (usually agent pointer)
 *
 *  Returns:
 *   0 if all messages in the input buffer have been processed.  If
 *   messages remain, 1 is returned.  This tells the dispatcher that
 *   it should continue to notify us despite the fact that our socket
 *   descriptor is not ready for reading.
 *
 *  Sideeffects:
 *   This routine will replace the ClientServer's io handler for our
 *   descriptor while messages are still available in our input
 *   buffer.  It is attached to the dispatcher by CheckClient().  It
 *   will reestablish the ClientServer's io handler after all messages
 *   in our buffer have been processed.
 *
 *----------------------------------------------------------------------*/
extern int altiaClientAgent_handleMessage(int readFD, void *data);

/*----------------------------------------------------------------------
 *  Function: int altiaClientAgent_getNextMessage(
 *                     altiaClientAgentData *agent,
 *                     unsigned char *bufPtr, int dataLen,
 *                     int *msgTypeOut, unsigned char **msgPtrOut,
 *                     int *msgSizeOut, unsigned char **nextBufPtrInOut);
 *
 *    This function is called when altiaClientAgent_handleMessage() wants
 *    to get a message out of the input buffer that was filled by
 *    function altiaClientAgentCheckClient().
 *
 *    The process is started by calling this function with
 *    *nextBufPtrInOut set equivalent to bufPtr.  This function
 *    returns a pointer to the next complete message in *msgPtrOut,
 *    its size in *msgSizeOut, and its type in *msgTypeOut.  It also
 *    sets *nextBufPtrInOut to point at the location following the
 *    message.  This should be passed in again on the next call so
 *    _getNextMessage() can continue the traversal.  This function
 *    returns FALSE when it cannot return another complete message.  If
 *    a partial message is left in the buffer, it is moved to the head
 *    of the buffer and *nextBufPtrInOut is set to point to the
 *    location following it.  This way, the rest of the message can be
 *    read from the connection by someone else later on.
 *
 *  Parameters:
 *    bufPtr              Start of buffer containing messages
 *    dataLen             Amount of valid data in buffer.
 *    msgTypeOut          For returning type of next complete msg.
 *    msgPtrOut           For returning pointer to next complete msg.
 *    msg_size_out        For returning size of next complete msg.
 *    nextBufPtrInOut     For passing in starting location of next
 *                        unread message and returning pointer to
 *                        location following the message being returned.
 *
 *  Returns:
 *   TRUE  (1) if we are returning a complete message. 
 *   FALSE (0) if no more complete messages are available. If a partial
 *         message remains in the buffer, it is moved to the head of
 *         the buffer and *nextBufPtrInOut is set to point to the location
 *         following it.
 *
 *  Sideeffects:
 *   If a partial message is found at the end of the buffer, it is
 *   moved to the head of the buffer.
 *
 *   This whole scheme depends on the assumption that no message is
 *   larger than the size of our input buffer (which is the protected
 *   data member _inputBuf).
 *    
 *----------------------------------------------------------------------*/
extern int altiaClientAgent_getNextMessage(
                altiaClientAgentData *agent,
                unsigned char *bufPtr, int dataLen,
                int *msgTypeOut, unsigned char **msgPtrOut,
                int *msgSizeOut, unsigned char **nextBufPtrInOut);


/*----------------------------------------------------------------------
 *  Function: int altiaClientAgent_sendMessage(
 *                     altiaClientAgentData *agent, int msgType,
 *                     unsigned char *field1, int size1,
 *                     unsigned char *field2, int size2,
 *                     unsigned char *field3, int size3,
 *                     unsigned char *field4, int size4);
 *
 *    This function is called to transmit a message via the connection.
 *    A message always has a type, but other fields are optional to a
 *    maximum of 4.
 *
 *    A message is transmitted by first sending a 2-byte size field
 *    (which is the sum of the type and individual field sizes),
 *    a type field, and then the fields themselves.
 *
 *  Parameters:
 *    msgType          Value for message's type field.
 *    fieldN           Pointer to start of a message field or NULL.
 *    sizeN            Byte size of corresponding message field.
 *                     MUST be zero(0) if field pointer is NULL.
 *
 *  Returns:
 *   None
 *
 *  Sideeffects:
 *   None
 *    
 *----------------------------------------------------------------------*/
extern void altiaClientAgent_sendMessage(
                 altiaClientAgentData *agent, int msgType,
                 unsigned char *field1, int size1,
                 unsigned char *field2, int size2,
                 unsigned char *field3, int size3,
                 unsigned char *field4, int size4);


/*----------------------------------------------------------------------
 *  Function: void altiaClientAgent_sendQueueBytes(
 *                      altiaClientAgentData *agent, int availSpace);
 *
 *    This function transfers events from our queue to the client.
 *    The number of events transmitted depends on how much space
 *    is available on the client side as indicated by the parameter.
 *
 *    This function is called immediately following a request for
 *    events from the client if our queue isn't empty.  This function
 *    might also be called from the ClientBehaviorReceiver after an
 *    event is added to an empty queue and the client is known to be
 *    blocked waiting for a response from a request for queue events.
 *
 *  Parameters:
 *    availSpace       Amount of space client has for stashing new events.
 *
 *  Returns:
 *   None
 *
 *  Sideeffects:
 *   None
 *    
 *----------------------------------------------------------------------*/
extern void altiaClientAgent_sendQueueBytes(
                 altiaClientAgentData *agent, int availSpace);

/*----------------------------------------------------------------------
 *  Function: AltiaEventType altiaClientAgent_getRValue(
 *                                altiaClientAgentData *agent,
 *                                char *name, unsigned char **msgptrInOut);
 *
 *    Get The AltiaEventType from the msgptr and advance msgptr
 *----------------------------------------------------------------------*/
extern AltiaEventType altiaClientAgent_getRValue(
                    altiaClientAgentData *agent,
                    char *name, unsigned char **msgptrInOut);

/*----------------------------------------------------------------------
 *  Function: void altiaClientAgent_setRValue(
 *                      altiaClientAgentData *agent,
 *                      char *name, AltiaEventType value,
 *                      unsigned char **msgptrInOut, int *vlenOut);
 *
 *    Set the RValueType to msgptr and advance msgptr
 *----------------------------------------------------------------------*/
extern void altiaClientAgent_setRValue(
                 altiaClientAgentData *agent,
                 char *name, AltiaEventType value,
                 unsigned char **msgptrInOut, int *vlenOut);


/*----------------------------------------------------------------------
 *  Function: int altiaClientAgentHandleEvent(
 *                     altiaClientAgentData *agent,
 *                     char *eventName, AltiaEventType value);
 *
 *    This function is called when there is an event for the client
 *    agent.  The event being routed may or may not be of interest to
 *    the client.  If it is, the event is queued or the event itself
 *    may be transmitted to the client by this handler if the client
 *    is waiting for a new event.
 *
 *  Parameters:
 *    eventName           Name of event that is being routed.
 *    value               Value that event is being routed with.
 *
 *  Returns:
 *    This handler should return 0 if it successfully handles the event.
 *    It can return non-zero otherwise.  
 *
 *  Sideeffects:
 *    None
 *----------------------------------------------------------------------*/
extern int altiaClientAgentHandleEvent(
                altiaClientAgentData *agent,
                char *eventName, AltiaEventType value);

#endif /* aClIeNtserver_h not already defined */


/***************************************************************************
 ***************************************************************************
 **
 ** END data type and function declarations.
 ** BEGIN implementation.
 **
 ***************************************************************************
 ***************************************************************************/


/*======================================================================
 *
 * main data and funtions:
 * -----------------------
 * A special main function is required to promptly handle incoming
 * client events as well as DeepScreen window events.  To override
 * this main code, define CUSTOM_SERVER_MAIN at compile time. 
 *
 *========================================================================*/
#ifndef CUSTOM_SERVER_MAIN

/* For debug output... */
#if defined(WIN32) && !defined(UNDER_CE)
#include <io.h>
#include <fcntl.h>
#endif

/* For Unicode, main() still just takes char strings in argv[] */
int main(int argc, char *argv[])
{
    altiaClientServerData *server;
    altiaDispatcherData   *dispatcher;

    dispatcher = altiaDispatcher_instance();
    server = altiaClientServerInstance(argc, argv);

    TargetAltiaInitialize();

#ifdef WIN32_GDI32
    /* This is a native (GDI32) Windows target so we need to have
     * a direct look inside the message queue for socket messages.
     */
    {
        MSG msg;

        /* Continue to process messages as long as there are messages
         * and an altiaQuit has not been detected.
         */
        while(!altiaClientServerQuitDetected(server)
              && GetMessage(&msg, NULL, 0, 0))
        {
            if (msg.message == WM_SOCKET)
            {
                if (_altiaDebug > 1)
                {
                    static int msgCount = 0;
                    if (msgCount++ % 1000 == 0)
                        PRINTERR2("Passed %d WM_SOCKET msg(s) to dispatcher\n",
                                  msgCount);
                }
                altiaDispatcher_winMessage(msg.wParam, msg.lParam);
            }
            else if (msg.message == WM_QUIT)
            {
                if (_altiaDebug > 1)
                {
                    PRINTERR1("Detected WM_QUIT msg\n");
                }
                break;
            }
            else
            {
                if (_altiaDebug > 1)
                {
                    static int msgCount = 0;
                    if (msgCount++ % 1000 == 0)
                        PRINTERR2("Translated/Dispatched %d msg(s)\n",
                                  msgCount);
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

#else
    {
        int retVal;
        long sec, usec;

        while(!altiaClientServerQuitDetected(server))
        {
            sec = 0;

            /* RTC 840 -- this is the execution loop time.  It should be small
            ** so responsiveness to timer stimulus, keyboard input, and mouse
            ** events is fast and smooth.  Otherwise choppy behavior can
            ** occur.
            */
            usec = 5000;

            altiaDispatcher_dispatchTime(&sec, &usec);
            /* fprintf(stderr,
             *        "dispatch wait returned %d with %d.%03d secs remaining\n",
             *        retVal, (int) sec, (int) usec/1000);
             */
            if (TargetCheckEvent(&retVal) < 0)
                break;
        }
    }
#endif /* WIN32 */

    altiaClientServerDelete(server);
    altiaDispatcher_delete();
    TargetAltiaClose();
    
    return 0;
}

#ifdef UNICODE
/*----------------------------------------------------------------------
 * For Unicode support, it is frequently necessary to convert wide strings
 * to byte strings where the wide character values are actually always
 * just ANSI characters (not really wide).  This is especially true for
 * program arguments and event names.
 */
#define MAX_UNICODE_LEN 256
static void localCopyWideToChar(ALTIA_CHAR *src, char *dest, int maxLen)
{
    if (src != NULL)
    {
        int i;
        for (i = 0; i < maxLen; i++)
        {
            if (*src == '\0')
                break;
            *dest++ = (char) (*src++ & 0x0ff);
        }
    }
    *dest = '\0';
}
#endif

#ifdef UNICODE
/*----------------------------------------------------------------------
 * For Unicode support, convert byte strings to wide strings.
 */
static void localCopyCharToWide(char *src, ALTIA_CHAR *dest, int maxLen)
{
    if (src != NULL)
    {
        int i;
        for (i = 0; i < maxLen; i++)
        {
            if (*src == '\0')
                break;
            *dest++ = ((ALTIA_CHAR) (*src++)) & 0x0ff;
        }
    }
    *dest = '\0';
}
#endif

/*----------------------------------------------------------------------
 * Windows Main function.  The entry point for a Windows application.
 * Every Windows application needs one of these.  However, if this is
 * an MFC APP, MFCs need to use their own WinMain() so don't define this
 * WinMain.  This way, the linker will pull WinMain() from the MFC libraries
 * (specifically mfcs42.lib or one of its derivatives) and not from the
 * DeepScreen library.
 */
#ifdef WIN32
#if defined(UNDER_CE)
/* Windows CE always has a Unicode WinMain */
#define UNICODE_WINMAIN
typedef ALTIA_CHAR* CMDLINESTR;
#else
/* Desktop Windows never has a Unicode WinMain even if UNICODE is defined */
typedef char* CMDLINESTR;
#endif /* UNICODE && UNDER_CE */
static int WindowsFetchArgcArgv(void *hInstance, char *lpszCmdLine,
                             int argvsize, char **argv, int *totalargs);
#ifndef MFCAPP
int PASCAL WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   CMDLINESTR lpszCmdLine,
                   int nCmdShow)
{
    int retval;
    char *argv[20];  /* The main() always takes char strings in argv[] */
    int argc;
    int totalargs;
    char *cmdline;

#ifdef UNICODE_WINMAIN
    char cmdlinechar[512];
#endif

    /* The WinMain command line is converted to an argument
     * list and count in the standard C main(argc, argv) style.
     * and argc, argv are passed to the custom main loop function.
     */
    if (lpszCmdLine != NULL && lpszCmdLine[0] != '\0')
    {
#ifdef UNICODE_WINMAIN
        localCopyWideToChar(lpszCmdLine, cmdlinechar, 512);
        cmdline = cmdlinechar;
#else
        cmdline = lpszCmdLine;
#endif
        argc = WindowsFetchArgcArgv(hInstance, cmdline,
                                   20, argv, &totalargs);
    }
    else
        argc = 0;

    retval = main(argc, argv);
    return retval;
}
#endif /* NOT MFCAPP */
#endif /* WIN32 */

/*----------------------------------------------------------------------
 * Windows function to convert WinMain command line string to argv[] style
 * list and argc count.  Caller provides an ALTIA_CHAR **argv list and
 * tells how long it is with argvsize.  Function fills argv list with
 * elements.  The number of elements put into argv is the return value of
 * the function itself while the integer pointed to by argc is given the
 * total number of arguments encountered in the line.
 */
#ifdef WIN32
static int WindowsFetchArgcArgv(void *hInstance, char *lpszCmdLine,
                                int argvsize, char **argv,
                                int *totalargs)
{
    #define MAX_FETCH_LEN  256 
    static ALTIA_CHAR fmodule[MAX_FETCH_LEN];
#ifdef UNICODE
    static char fmodulechar[MAX_FETCH_LEN];
#endif
    char *arg;
    int argcount = 0;
    int len;
    char *endcmd;

    if (lpszCmdLine != NULL)
        endcmd = &lpszCmdLine[strlen(lpszCmdLine)-1];


    *totalargs = 0;

     /* Linux Microwindows implementation of WIN32 has no GetModuleFileName. */
#ifndef MICROWIN
    GetModuleFileName((HINSTANCE)hInstance, fmodule, MAX_FETCH_LEN);
    if (argcount < argvsize &&  argv != NULL)
    {
#ifdef UNICODE
        /* main() only takes char strings so convert for Unicode */
        localCopyWideToChar(fmodule, fmodulechar, MAX_FETCH_LEN);
        argv[argcount++] = fmodulechar;
#else
        argv[argcount++] = fmodule;
#endif
    }
    (*totalargs)++;
#endif


    if (lpszCmdLine == NULL)
        return argcount;
    arg = strtok(lpszCmdLine, " ");
    if (arg) {
         do
         {
             /* If we have quotes then put everything between quotes as its
              * own arg.
              */
             if (arg[0] == '"' || arg[0] == '\'')
             {
                 char *ptr = arg;
                 len = strlen(arg);
                 if (arg[len-1] == '"' || arg[len-1] == '\'')
                 {
                     arg[len-1] = '\0';
                     if (argcount < argvsize && argv != NULL)
                         argv[argcount++] = &arg[1];
                     (*totalargs)++;
                 }else if (&arg[len-1] != endcmd)
                 {
                 /* Put the blank back so we have one large token delimited by
                  * quotes.
                  */
                     arg[len] = ' ';
                     if (strchr(arg,'"') != NULL 
                         || strchr(arg,'\'') != NULL)
                         arg = strtok(NULL, "\"'");
                     else 
                         arg = NULL;
                     if (arg == NULL)
                     {
                         ptr[len] = '\0';
                         if (argcount < argvsize && argv != NULL)
                             argv[argcount++] = ptr;
                         (*totalargs)++;
                     }else
                     {
                         if (argcount < argvsize && argv != NULL)
                             argv[argcount++] = ptr + 1;
                         (*totalargs)++;
                     }
                 }else
                 {
                     if (argcount < argvsize && argv != NULL)
                         argv[argcount++] = arg;
                     (*totalargs)++;
                 }
             }else
             {
                 if (argcount < argvsize && argv != NULL)
                     argv[argcount++] = arg;
                 (*totalargs)++;
             }
         } while ((arg = strtok(NULL, " ")) != NULL);
    }
    if (argcount < argvsize && argv != NULL)
        argv[argcount] = NULL;
    else if (argv != NULL)
    {
        argv[argvsize - 1] = NULL;
        argcount = argvsize - 1;
    }
    return argcount;
}
#endif /* WIN32 */

#endif /* NOT CUSTOM_SERVER_MAIN */


/*----------------------------------------------------------------------
 * Utility function for general use to get a string from an error number.
 */
#ifdef ALTIAAPIDEBUG
static char *_errorStart = "Altia ClientServer: ";
static char *_errorMore  = "                    ";
static char * _errnoString(void)
{
    static char empty = '\0';
#ifdef WIN32
#ifndef UNDER_CE
    int err = (*pWSAGetLastError)();
    static char estring[32];
    if (err != 0)
    {
        sprintf(estring,"Win Socket Error %d",err);
        return estring;
    }else
#endif /* NOT UNDER_CE */
        return &empty;
#else
#ifndef SUN4
    if (strerror(errno) != NULL)
        return strerror(errno);
#else
    if (errno < sys_nerr && errno > 0)
        return sys_errlist[errno];
#endif /* NOT SUN4 */
    else
        return &empty;
#endif /* WIN32 */
}
#endif /* ALTIAAPIDEBUG */


/*======================================================================
 *
 * Class altiaClientServer implementation:
 * ---------------------------------------
 * Create and manipulate a client server.  There is just one instance
 * of a client server in an executable space hence the reason why a
 * pointer to the client server is not passed to any of the client
 * server functions.  Also the reason for the static _server variable
 *
 *========================================================================*/

/* As stated in the previous comment, only one instance of a client server */
static altiaClientServerData *_server = NULL;

/* Define environment variable name that will reflect the socket port we
 * have opened for child processes to determine the socket we are serving.
 */
#if !defined(UNDER_CE) && !defined(VXWORKS)
static char *ALTIAPORT = "ALTIAPORT";
#endif

/*----------------------------------------------------------------------
 * Definition for rounding function.  History preserved for reference.
 *
 * NA/NA/NA Needed to change this since even if negative we want to
 *          round up on a .5.
 * 05/25/95 15 digits of percision is all the compilers will give us.
 * 10/02/01 In order to round the largest possible positive number,
 *          we want to cast the addition operations to unsigned()
 *          instead of int().  For the 4.0 and 4.5 releases, the int()
 *          cast has caused us to incorrectly handle X key sym values
 *          for the function keys on UNIX because they are values like
 *          0xffbe0000.  These come out of stimulus and (as of 4.0
 *          release) are put into the router as doubles (not a problem)
 *          and then the client agent uses altia_round() to get them
 *          into an integer for an integer client (as of 4.0 release),
 *          but we end up with 0x7ffffffff whenever the double value is
 *          0x800000000 or larger.  For the Concurrent PMAX platform
 *          port on 11/21/00, I actually discovered the solution while
 *          fixing the exception generated by PMAX in this case.  I
 *          didn't know it at the time, but the fix should've been
 *           applied on all platforms.  Unsigned arithmetic works OK
 *          for neg numbers as well (tested it on all platforms) so
 *          we will use it just like we did on the PMAX platform.
 *          Original code was:
 *
 *     inline int altia_round (double x) { return x > 0 ? int(x+0.5) :
 *                    -int(-x+(0.499999999999999));}
 *
 * 11/15/04 Change was made to suppress warnings in MSVC.
 *          Tested the change on all platforms (Solaris, HP, SGI, Linux
 *          and Windows) to confirm identical behavior with last version.
 *          Last version was:
 *
 *     inline int altia_round (double x) { return x > 0 ? unsigned(x+0.5) :
 *                    -unsigned(-x+(0.499999999999999));}
 */
int altiaClientServer_round (double x)
{
    /* TODO:  May want to support fixed point some day */

    return x > 0 ? (unsigned int)(x+0.5)
                 : -(int)((unsigned int)(-x+(0.499999999999999)));
}

/*----------------------------------------------------------------------
 * Function to test if 2 floating point values are equal
 */
#define altiaClientServer_RVALUE_ERR_TOL 1e-6
#define altiaClientServer_TVALUE_ERR_TOL 1e-3
int altiaClientServer_REQUAL(AltiaEventType a, AltiaEventType b)
{
    /* TODO:  May want to support fixed point some day */

    AltiaEventType diff = a - b;
    if (-altiaClientServer_RVALUE_ERR_TOL > diff
        || diff > altiaClientServer_RVALUE_ERR_TOL)
        return FALSE;
    else
        return TRUE;
}

/*----------------------------------------------------------------------*/
altiaClientServerData *altiaClientServerInstance(int argc, char *argv[])
{
    if (_server == NULL)
        _server = altiaClientServerCreate(argc, argv);

    return _server;
}

/*----------------------------------------------------------------------*/
altiaClientServerData* altiaClientServerCreate(int argc, char *argv[])
{
#ifdef WIN32
    static unsigned char firstTime = 0;
#endif
    static unsigned char nowin = 0;
    int thisHostLen;
    int i;

    /* Use a socket and bind it to the given file.
     * This code will find an available domain socket if one
     * was not provided.
     */
    int     netNumber = 0;
    char    *domainName = NULL;
    char    *serviceBase = NULL;

    /* Get host name - up to 99 characters long - for this routine to use. */
    char thisHost[100];

    /* Hold the string name of the socket actually opened so we can set
     * environment variable ALTIAPORT.  Then, our child processes can
     * know our port!
     */
    char *altiaPort = NULL;
#if defined(WIN32) || defined(VXWORKS)
    char *defaultPortString;
#endif

    /* Initialize socket interface if necessary (such as on Windows) */
#ifdef WIN32
    if (firstTime == 0)
    {
#ifdef UNDER_CE
        if (!altiaCnctOpenSockets(ALT_TEXT("winsock.dll")))
#else
        if (!altiaCnctOpenSockets(ALT_TEXT("wsock32.dll")))
#endif
        {
            if (_altiaDebug)
                PRINTERR1("Could not load Windows sockets DLL\n");

            return NULL;
        }
    }
#endif /* WIN32 */

    /* Make sure an instance of the dispatcher exists.  This should
     * create it if it doesn't.  Do this after initializing the socket
     * interface so that the dispatcher has a chance to initialize
     * its own dependencies on the socket interface (WIN32 only).
     */
    if (altiaDispatcher_instance() == NULL)
        return NULL;

    if (_server != NULL)
        return _server;

#if defined(WIN32) || defined(VXWORKS)
    defaultPortString
          = (char *) ALTIA_MALLOC((strlen(DEFAULT_PORT_STRING) + 10)
                                  * sizeof(char));
    *defaultPortString = ':';
    strcpy(defaultPortString + 1, DEFAULT_PORT_STRING);
#endif

    /* Search for user preference for service name */
    for (i = 0; i < argc; i++)
    {
        if ((serviceBase == NULL) && (strcmp("-port", argv[i]) == 0))
        {
            /* If service name doesn't immediately follow option string,
             * then we do nothing.  If service name begins with a '\',
             * assume it is being used to escape a name that
             * begins with '-'.
             */
            if (((i+1) < argc) && (argv[i+1][0] != '-'))
            {
                if (argv[++i][0] == '\\')
                {
                    serviceBase = argv[i] + 1;
                }
                else
                {
                    serviceBase = argv[i];
                }
            }
            else
            {
                PRINTERR1("usage: program [-port NAME] [-debug [1|2|3]]\n");
                PRINTERR1("       use -port NAME to select a socket to open for client\n");
                PRINTERR1("       communications where NAME has one of the formats:\n");
                PRINTERR1("       :NUMBER  - open inet socket NUMBER\n");
                PRINTERR1("       :STRING  - open inet socket with service name STRING\n");
                PRINTERR1("\n");
                PRINTERR2("If \"-port NAME\" is not given, socket %s is used by default\n", DEFAULT_PORT_STRING);
                PRINTERR1("\n");
            }
        }
        else if (strcmp("-debug", argv[i]) == 0)
        {
            /* Note:  Debug will only show if ALTIAAPIDEBUG is defined
             *        at compile time.  Otherwise the PRINTERR macros 
             *        are just NOOPs and no debug will show.
             */
            if (((i+1) < argc) && (argv[i+1][0] != '-'))
                _altiaDebug = ((int) (argv[++i][0]) - (int) '0') & 0x0ff;
            else
                _altiaDebug = 1;
        }
        else if (strcmp("-nowin", argv[i]) == 0)
        {
            /* Caller wants DeepScreen to start without showing its window */
            nowin = 1;

#if defined(WIN32_GDI32) || defined(UNIX)
            TargetAltiaShowWindow(0);
#endif
        }
    }

    /* First, set up an error output facility such as for Windows. */
    if (_altiaDebug)
        altiaClientServerDebugOutputOpen();

    /* Get memory for client server data structure */
    _server = (altiaClientServerData *)
              ALTIA_MALLOC(sizeof(altiaClientServerData));
    if (_server == NULL)
        return NULL;
    
    _server->_clientAgents = NULL;
    _server->_quit = FALSE;
    _server->_syncDelayOn = FALSE;
    _server->_syncClients = FALSE;
    _server->_ignoreClient = NULL;
    _server->_ignoreName = NULL;
    _server->_ignoreValue = 0;
    _server->_nowin = nowin;

    /* Create connect instance */
    _server->_domainConnect = altiaCnctNewConnect();
    _server->_netConnect = altiaCnctNewConnect();
#ifndef WIN32
    _server->_domainName = NULL;
#endif

    /* Not applicable
     *  _urlRef = new URLRef(vse, this);
     */

    thisHost[0] = thisHost[sizeof(thisHost) - 1] = '\0';
#ifdef WIN32
    if (((*pgethostname)(thisHost, sizeof(thisHost) - 1)) == SOCKET_ERROR)
    {
        if (_altiaDebug)
            PRINTERR2("Could not get hostname error: %d\n",
                      (*pWSAGetLastError)());
    }
#else
    gethostname(thisHost, sizeof(thisHost) - 1);
#endif
    thisHostLen = strlen(thisHost);

    /* Copy default or user provided base name for service into service
     * name.
     */
#if defined(WIN32) || defined(VXWORKS)
    if (FALSE)
#else
    if (serviceBase == NULL)
#endif
    {
#if !defined(WIN32) && !defined(VXWORKS)
        int i;
        char *defaultBaseName = DEFAULTBASENAME;

        domainName
            = (char *) ALTIA_MALLOC((MAXBASELENGTH + 2 + thisHostLen + 1)
                                    * sizeof(char));
        strcpy(domainName, defaultBaseName);

        /* Keep track of domain socket base name. */
        altiaPort = (char *) ALTIA_MALLOC((MAXBASELENGTH + 2 + 1)
                                          * sizeof(char));
        strcpy(altiaPort, domainName);

        /* Append host name to guarantee some level of uniqueness. */
        strcat(domainName, thisHost);

        /* Try to find a domain socket that isn't in use.
         * Try <defaultBaseName>, then <defaultBaseName>1 through
         * <defaultBaseName>99.
         */
        for (i = 1;
             i < 100 && altiaCnctOpenLocal(_server->_domainConnect, domainName);
             i++)
        {
            altiaCnctClose(_server->_domainConnect);
            sprintf(domainName, "%s%d", defaultBaseName, i);
            strcpy(altiaPort, domainName);
            strcat(domainName, thisHost);
        }

        /* If nothing worked, clean things up to look like
         * we are going to use the standard domain socket.
         */
        if (i == 100)
        {
            strcpy(domainName, defaultBaseName);
            strcat(domainName, thisHost);
            ALTIA_FREE(altiaPort);
            altiaPort = NULL;
        }
        else if (i > 1 && _altiaDebug)
        {
            PRINTERR2("%sthe default domain socket is already in use.\n",
                      _errorStart);
            PRINTERR3("%sclients must use port \"%s\" to connect\n",
                      _errorMore, altiaPort);
        }
#endif
    }
    else
    {
        char *delimiter;
#if defined(WIN32) || defined (VXWORKS)
        if (serviceBase == NULL)
            delimiter = NULL;
        else
#endif
        delimiter = strchr(serviceBase, ':');

#if defined(WIN32) || defined(VXWORKS)
        if (FALSE)
#else
        /* Name is only a domain name if no ':' appears */
        if (delimiter == NULL)
#endif
        {
            domainName
                = (char *) ALTIA_MALLOC((strlen(serviceBase) + thisHostLen + 1)
                                        * sizeof(char));
            strcpy(domainName, serviceBase);

           /* tjw, 10/20/99:  Keep track of domain socket base name. */
           altiaPort = (char *) ALTIA_MALLOC((strlen(domainName) + 1)
                                             * sizeof(char));
           strcpy(altiaPort, domainName);

            /* Append host name to guarantee some level of uniqueness. */
            strcat(domainName, thisHost);
        }
        else
        {
            int res = 0;

            /* Name has a ':' so we need to open net and domain sockets.
             * We don't use the hostname preceding the ':' because we are
             * the server so the hostname is our name by definition. We will
             * warn caller that it is ignored if it doesn't match.
             */
#if defined(WIN32) || defined(VXWORKS)
            if ((delimiter != NULL) && (serviceBase != NULL))
#endif
            if (delimiter != serviceBase
                && strncmp(serviceBase, thisHost, delimiter-serviceBase) != 0
                && _altiaDebug)
            {
                PRINTERR3("%shost name in \"%s\" ignored\n",
                           _errorStart, serviceBase);
                PRINTERR3("%shost is %s by definition\n",
                           _errorMore, thisHost);
            }

            /* Domain name will be standard base (e.g. /usr/tmp/vSe.)
             * + hostname + user's input.  For example, a "-port :xyzzy"
             * would yield a domain socket name like:
             * "/usr/tmp/vSe.hplsdcixyzzy".
             */

#if !defined(WIN32) && !defined(VXWORKS)
            domainName = ALTIA_MALLOC((MAXBASELENGTH + thisHostLen
                                      + strlen(delimiter + 1) + 1)
                                      * sizeof(char));
           
            strcpy(domainName, DEFAULTBASENAME);
            strcat(domainName, thisHost);
            strcat(domainName, delimiter + 1);
#endif

            /* OK, did user give us a number or a name. */
#if defined(WIN32) || defined(VXWORKS)
            if (delimiter == NULL)
            {
                if (serviceBase == NULL)
                {
                    res = sscanf(defaultPortString + 1, "%d", &netNumber);
                    delimiter = defaultPortString;
                }
                else
                {
                    delimiter = serviceBase;
                    delimiter--;
                }
            }
            if (res != 1)
#endif
            res = sscanf(delimiter + 1, "%d", &netNumber);

            /* NOTE THAT AT THIS POINT, delimiter + 1 is an
             * address of a valid string if res == 1.  That's very important
             * for the upcoming code!
             */
            if (res == 1)
            {
#ifdef WIN32
                /* It would be nice to check the default
                 * network socket on Windows like we do the default domain
                 * socket on Unix, but it introduces long time delays on
                 * startup so this code is ifdef'd out for now.
                 */
#ifdef CHECKSOCKETONWIN32
                /* If we are using sockets on Windows and we
                 * didn't get a port name from the user, then check the
                 * port we have to make sure it isn't in use by someone else.
                 * If it is in use, then try to find the next available port
                 * that isn't in use.  Try netNumber to netNumber + 99
                 * ports at most.
                 */
                if (serviceBase == NULL)
                {
                    int i;
                    int origNumber = netNumber;

                    for (i = 0; i < 100; i++)
                    {
                        if (altiaCnctOpenPort(_server->_netConnect,
                                             thisHost, netNumber))
                        {
                            altiaCnctClose(_server->_netConnect);
                            netNumber++;
                        }
                        else
                            break;
                    }

                    /* If nothing worked, clean things up to look like
                     * we are going to use the standard socket number.
                     */
                    if (i == 100)
                        netNumber = origNumber;
                    else if (i > 0 && _altiaDebug)
                    {
                        PRINTERR2(
                                "%sthe default socket is already in use.\n",
                                _errorStart);
                        PRINTERR3(
                           "%sclients can use socket port \":%d\" to connect\n",
                                _errorMore, netNumber);
                    }

                    sprintf(delimiter + 1, "%d", netNumber);
                }
#endif /* CHECKSOCKETONWIN32 */

                /* Following code is ifdef'd out for now
                 * because it introduces long time delays on startup.
                 */
#ifdef CHECKSOCKETONWIN32
                /* Check socket before using it on Windows.
                 * Windows will create the service even if it is already 
                 * in use by another process so this check is necessary.
                 * It is NOT necessary on UNIX (and may be harmful).
                 */
                if (altiaCnctOpenPort(_server->_netConnect,
                                     thisHost, netNumber))
                {
                    altiaCnctClose(_server->_netConnect);
                    if (_altiaDebug)
                        PRINTERR3("%ssocket \":%d\" already in use.\n",
                                  _errorStart, netNumber);
                }
                else
#endif /* CHECKSOCKETONWIN32 */

#endif /* WIN32 */
                if (_altiaDebug > 1)
                    PRINTERR2("\nInto altiaClientServerCreate:  Setting up port on socket %d\n", netNumber);
                if (altiaCnctCreatePort(_server->_netConnect,
                                       thisHost, netNumber))
                {
                    /* Need to hook this file descriptor into wait loop */
                    altiaDispatcher_link(
                         altiaCnctGetReadFD(_server->_netConnect),
                         DispatcherReadMask,
                         altiaClientServerInputReady, NULL);

                    /* There could be some clients already waiting to connect.
                     * We'll accept them now if that's the case.
                     */
                    altiaClientServerInputReady(
                         altiaCnctGetReadFD(_server->_netConnect), NULL);

                    /* Save socket name as a string for later. */
                    altiaPort
                        = (char *) ALTIA_MALLOC((strlen(thisHost) + 1
                                                 + strlen(delimiter + 1) + 1)
                                                * sizeof(char));
                    strcpy(altiaPort, thisHost);
                    strcat(altiaPort, ":");
                    strcat(altiaPort, delimiter + 1);
                }
                else
                {
                    altiaCnctClose(_server->_netConnect);
                    if (_altiaDebug)
                    {
                        PRINTERR5("%s%s # %d: %s\n",
                                  _errorStart,
                                  altiaCnctGetErrorString(_server->_netConnect),
                                  netNumber, _errnoString());
                        PRINTERR2("%scontinuing with domain socket open\n",
                                  _errorMore);
                    }
                }
            }
            else
            {
#ifdef WIN32
                /* Following code is ifdef'd out for now
                 * because it introduces long time delays on startup.
                 */
#ifdef CHECKSOCKETONWIN32
                /* Check socket before using it on Windows.
                 * Windows will create the service even if it is already 
                 * in use by another process so this check is necessary.
                 * It is NOT necessary on UNIX (and may be harmful).
                 */
                if (altiaCnctOpenService(_server->_netConnect,
                                        thisHost, delimiter + 1))
                {
                    altiaCnctClose(_server->_netConnect);
                    if (_altiaDebug)
                        PRINTERR4("%ssocket \"%s:%s\" already in use.\n",
                                  _errorStart, thisHost, delimiter + 1);
                }
                else
#endif /* CHECKSOCKETONWIN32 */

#endif
                if (altiaCnctCreateService(_server->_netConnect,
                                          thisHost, delimiter + 1))
                {
                    /* Need to hook this file descriptor into wait loop */
                    altiaDispatcher_link(
                         altiaCnctGetReadFD(_server->_netConnect),
                         DispatcherReadMask,
                         altiaClientServerInputReady, NULL);

                    /* There could be some clients already waiting to connect.
                     * We'll accept them now if that's the case.
                     */
                    altiaClientServerInputReady(
                         altiaCnctGetReadFD(_server->_netConnect), NULL);

                    /* Save socket name as a string for later. */
                    altiaPort = (char *) ALTIA_MALLOC((strlen(thisHost) + 1
                                              + strlen(delimiter + 1) + 1)
                                              * sizeof(char));
                    strcpy(altiaPort, thisHost);
                    strcat(altiaPort, ":");
                    strcat(altiaPort, delimiter + 1);
                }
                else
                {
                    altiaCnctClose(_server->_netConnect);
                    if (_altiaDebug)
                    {
                        PRINTERR5("%s%s \"%s\": %s\n",
                                  _errorStart,
                                  altiaCnctGetErrorString(_server->_netConnect),
                                  delimiter + 1, _errnoString());
                        PRINTERR2("%scontinuing with domain socket open\n",
                                  _errorMore);
                    }
                }
            }
        }
    }

#if !defined(WIN32) && !defined(VXWORKS)
    /* Make sure domain socket isn't being served by someone
     * else by first opening it as a client. If that works,
     * we shouldn't try to open it as a server because we will
     * disconnect the current server.
     */
    if (altiaCnctOpenLocal(_server->_domainConnect, domainName))
    {
        altiaCnctClose(_server->_domainConnect);
        if (_altiaDebug)
            PRINTERR3("%sdomain socket \"%s\" already in use.\n",
                      _errorStart, domainName);
        ALTIA_FREE(domainName);
    }
    else if (altiaCnctCreateLocal(_server->_domainConnect,domainName))
    {
        /* Need to hook this file descriptor into wait loop */
        altiaDispatcher_link(
             altiaCnctGetReadFD(_server->_domainConnect),
             DispatcherReadMask,
             altiaClientServerInputReady, NULL);

        if (altiaCnctGetReadFD(_server->_domainConnect) != -1)
            _server->_domainName = domainName;
        else
            ALTIA_FREE(domainName);

        /* There could be some clients already waiting to connect.
         * We'll accept them now if that's the case.
         */
        altiaClientServerInputReady(
             altiaCnctGetReadFD(_server->_domainConnect), NULL);
    }
    else
    {
        altiaCnctClose(_server->_domainConnect);
        if (_altiaDebug)
        PRINTERR5("%s%s \"%s\": %s\n",
                  _errorStart, altiaCnctGetErrorString(_server->_netConnect),
                  domainName, _errnoString());
        ALTIA_FREE(domainName);
    }


   if (altiaCnctGetReadFD(_server->_domainConnect) == -1
       && altiaCnctGetReadFD(_server->_netConnect) == -1)
   {
        if (altiaPort != NULL)
        {
            ALTIA_FREE(altiaPort);
            altiaPort = NULL;
        }
        if (_altiaDebug)
        {
            PRINTERR2("%sall socket opens failed\n", _errorMore);
            PRINTERR2(
                    "%sclients attempting to connect via sockets will fail\n",
                    _errorMore);
        }
   }
#endif /* !WIN32 && !VXWORKS */

    /* If we were able to open a socket port, set the
     * ALTIAPORT environment variable so that clients can find out the
     * port we are using.
     */
   if (altiaPort != NULL)
   {
#if defined(UNDER_CE) || defined(VXWORKS)
       ALTIA_FREE(altiaPort);
#else
       char *envString
           = (char *) ALTIA_MALLOC((strlen(ALTIAPORT) + strlen(altiaPort) + 2)
                                   * sizeof(char));
       strcpy(envString, ALTIAPORT);
       strcat(envString, "=");
       strcat(envString, altiaPort);

#ifdef WIN32
#ifdef UNICODE
       SetEnvironmentVariableA(ALTIAPORT, altiaPort);
#else
       SetEnvironmentVariable(ALTIAPORT, altiaPort);
#endif
       /* Don't delete altiaPort because the environment is now using it? */
#else
       ALTIA_FREE(altiaPort);
#endif
       putenv(envString);
       /* Don't delete envString because the environment is now using it! */
#endif /* NOT UNDER_CE and NOT VXWORKS */
   }

#ifdef WIN32
   if (defaultPortString != NULL)
       ALTIA_FREE(defaultPortString);
#endif

    /* Hook us into DeepScreen event loop. */
    AltiaReportFuncPtr = altiaClientServerHandleEvent;

   return _server;
}

/*----------------------------------------------------------------------*/
void altiaClientServerDelete(altiaClientServerData *server)
{
    altiaClientAgentData *next;
    altiaClientAgentData *nextNext;

    if (_server == NULL)
        return;

    /* Unhook us from DeepScreen event loop. */
    if (AltiaReportFuncPtr == altiaClientServerHandleEvent)
        AltiaReportFuncPtr = NULL;

    /* Deleting the list of agents deletes each entry in the list
     * which in-turn deletes each instance of ClientAgent known to
     * this server.  A client agent automatically deletes its
     * ClientConnect when it is destructed.  That's why you'll never
     * see this server do a delete on a ClientConnection other than
     * its own _domainConnect.  A client agent also will unlink itself
     * from the dispatcher when destructed.
     */
    for (next = _server->_clientAgents; next != NULL; next = nextNext)
    {
         nextNext = next->_next;
         altiaClientAgentDelete(next);
    }

    /* Not applicable
     * _urlRef->Clear();
     * delete _urlRef;
     */

    /* Unlink master connection from dispatcher */
    altiaDispatcher_unlink(altiaCnctGetReadFD(_server->_domainConnect));
    altiaDispatcher_unlink(altiaCnctGetReadFD(_server->_netConnect));

#ifndef WIN32
    if (_server->_domainName != NULL)
    {
        unlink(_server->_domainName);
        ALTIA_FREE(_server->_domainName);
    }
#endif

    altiaCnctDeleteConnect(_server->_domainConnect);
    altiaCnctDeleteConnect(_server->_netConnect);

    ALTIA_FREE(_server);
    _server = NULL;
}

/*----------------------------------------------------------------------
 * Creates a debug window if necessary for debug output.  For
 * Windows CE where a debug window is not supported, opens "cerror.log"
 * instead and initializes a pointer to the opened file.
 */
#ifdef WIN32
FILE *_altiaServerOutfile = 0L;
#endif
void altiaClientServerDebugOutputOpen(void)
{
#ifndef WIN32
    /* For non-Windows, assume no special action required */
    return;

#else
    /* For Windows, this function only supports a one-time call. */
    static unsigned char alreadyCalled = 0;

    if (alreadyCalled)
        return;

    alreadyCalled = 1;

    {
#ifndef UNDER_CE
        int hCrt;
        FILE *fp;
        HANDLE herr;
        unsigned long bcount;

        /* Open a console window for debug messages */
        AllocConsole();
        hCrt = _open_osfhandle
                   (
                       (long)GetStdHandle(STD_OUTPUT_HANDLE),
                       _O_TEXT
                   );
        fp = _fdopen(hCrt, "w");
        *stdout = *fp;
        setvbuf(stdout, NULL, _IONBF, 0);

        hCrt = _open_osfhandle
                   (
                       (long)GetStdHandle(STD_ERROR_HANDLE),
                       _O_TEXT
                   );
        fp = _fdopen(hCrt, "w");
        *stderr = *fp;
        setvbuf(stderr, NULL, _IONBF, 0);

        /* Confirm that there is an output.  If not, open cerror.log
         * for debug output.
         */
        herr = GetStdHandle(STD_ERROR_HANDLE);
        if (herr < 0 || (!WriteFile(herr, "\n", 1, &bcount, FALSE)))
#endif /* NOT UNDER_CE */
        {
            /* We don't have a valid stderr. Try opening a file. */
            if (_altiaServerOutfile != 0L)
                fclose(_altiaServerOutfile);

            _altiaServerOutfile = fopen("cerror.log","wtc");
            if (_altiaServerOutfile != 0L)
            {
#ifndef UNDER_CE
                setvbuf(_altiaServerOutfile, NULL, _IONBF, 0);
#endif
                return;
            }
            else
                return;
        }
    }
#endif /* WIN32 */
}

/*----------------------------------------------------------------------*/
static void altiaClientServerCheckClientExit(void)
{
    /* Let's check to see if we are to exit if the client
     * does.
     */

    /* PORTING NOTES:
     * In the Altia version, this is estabilished with
     * a WindowSystem "clientExit" application defaults entry.
     * No implementation to-date for DeepScreen.  If it is
     * implemented, its action is to force a quit perhaps by
     * routing an "altiaQuit" event.
     */
}

/*----------------------------------------------------------------------*/
int altiaClientServerEventSelected(char *name)
{
    altiaClientAgentData *next;

    if (_server == NULL)
        return FALSE;

    if (name == NULL)
        return FALSE;

    /* Check for clients that have selected to receive the given event. */
    for (next = _server->_clientAgents; next != NULL; next = next->_next)
    {
        if (altiaClientAgentEventSelected(next, name))
            return TRUE;

    }

    return FALSE;
}

/*----------------------------------------------------------------------*/
int altiaClientServerNumClients(void)
{
    int count = 0;
    altiaClientAgentData *next;

    if (_server == NULL)
        return 0;

    /* Count the client agents. */
    for (next = _server->_clientAgents; next != NULL; next = next->_next)
        count++;

    return count;
}

/*----------------------------------------------------------------------*/
void altiaClientServerHandleEvent(ALTIA_CHAR *inEventName,
                                  AltiaEventType eventValue)
{
    altiaClientAgentData *agent;

#ifndef UNICODE
    char *eventName = inEventName;
#else
    /* For Unicode, convert wide event name to 8-bit character string */
    char eventName[MAX_UNICODE_LEN];
    localCopyWideToChar(inEventName, eventName, MAX_UNICODE_LEN);
#endif

    /* The caller doesn't provide a server.  However, there can only
     * be one active server in process space and the _server global
     * identifies it.
     */
    if (_server == NULL)
        return;

    /* If an altiaQuit just went through the graphics code, assume we
     * need to quit as soon as possible.
     */
    if (_server->_quit || strcmp(eventName, "altiaQuit") == 0)
    {
        _server->_quit = 1;
        return;
    }

    for (agent = _server->_clientAgents; agent != NULL; agent = agent->_next)
    {
        /* If this specific event originated from the current client
         * agent, do not give it back to the client agent.
         */
        if (agent != _server->_ignoreClient
            || eventValue != _server->_ignoreValue
            || strcmp(eventName, _server->_ignoreName) != 0)
            altiaClientAgentHandleEvent(agent, eventName, eventValue);
    }
}

/*----------------------------------------------------------------------*/
int altiaClientServerInputReady(int inputFD, void *data)
{
    altiaClientAgentData *nextAgent;
    altiaClientAgentData *prevAgent;
    int retValue;

    if (_server == NULL)
        return 0;

    /* Check for new clients if necessary. */
    if (inputFD == altiaCnctGetReadFD(_server->_domainConnect)
        || inputFD == altiaCnctGetReadFD(_server->_netConnect))
    {
        altiaClientConnectData *domConnect = NULL, *netConnect = NULL;

        if (_altiaDebug > 1)
            PRINTERR2("\nInto altiaClientServerInputReady: checking for new client on server fd %d\n", inputFD);

        /* Attempt to connect personal socket to one or
         * more new clients.
         */
        while ((domConnect
                = altiaCnctAcceptClient(_server->_domainConnect)) != NULL
               || (netConnect
                   = altiaCnctAcceptClient(_server->_netConnect)) != NULL)
        {
            altiaClientAgentData* newAgent;

            /* Create a new client agent for the connection. It
             * needs the behavior router instance pointer so it
             * can register with the router as appropriate.
             */
            if (domConnect != NULL)
            {
                PRINTERR1("Altia domain socket connect...");
                newAgent
                    = altiaClientAgentCreate(_server,
                                             domConnect,
                                             altiaClientServerInputReady,
                                             altiaClientServerOutputReady,
                                             NULL);
            }
            else
            {
#ifdef WIN32
                PRINTERR1("Altia inet socket connect...");
#else
                PRINTERR1("Altia inet socket connect...");
#endif
                newAgent
                    = altiaClientAgentCreate(_server,
                                             netConnect,
                                             altiaClientServerInputReady,
                                             altiaClientServerOutputReady,
                                             NULL);
            }


            /* If client agent was successfully created, then
             * it knows its service descriptor.  Add client
             * agent to our list if descriptor is ok; otherwise,
             * delete the client agent.
             */
            if (altiaClientAgentGetReadFD(newAgent) > 0)
            {
                altiaClientAgentData **next;

                /* PORTING NOTES:
                 * In the Altia version of this code, now is the time to
                 * attach the agent to the router with code like:
                 *
                 *     newAgent->AttachToRouter();
                 *
                 * This effectively registers the agent's event handler
                 * with the router for any and all events.
                 *
                 * For DeepScreen, the client server hooks itself into
                 * the DeepScreen event loop in altiaClientServerCreate()
                 * by assigning the address of altiaClientServerHandleEvent()
                 * to the DeepScreen AltiaReportFuncPtr function pointer.
                 * Whenever the handler gets called, it passes the event on
                 * to each client agent via the altiaClientAgentHandleEvent()
                 * function.
                 */

                /* Add agent to our list of agents */
                for (next =  &(_server->_clientAgents);
                     *next != NULL;
                     next = &((*next)->_next));

                *next = newAgent;

                altiaDispatcher_link(
                         altiaClientAgentGetReadFD(newAgent),
                         DispatcherReadMask,
                         altiaClientServerInputReady, (void *) newAgent);
#ifdef WIN32
                PRINTERR1("ok\n");
#else
                PRINTERR1("ok\n");
#endif
            }
            else
            {
                altiaClientAgentDelete(newAgent);
                PRINTERR1("FAILED\n");
            }
        }

        if (_altiaDebug > 1)
            PRINTERR1("\tReturning from altiaClientServerInputReady\n");

        /* A return of 0 tells caller everything went ok. */
        return 0;
    }

    if (_altiaDebug > 1)
        PRINTERR2("\nInto altiaClientServerInputReady: for fd %d\n", inputFD);

    /* Now check for clients that need service. Note that we never test
     * the contents in the first element of the list.  We don't have
     * to - it is just a placeholder to the start of the list.  It will
     * never contain data.
     */
    nextAgent = _server->_clientAgents;
    prevAgent = NULL;
    while (nextAgent != NULL)
    {
        if (inputFD == altiaClientAgentGetReadFD(nextAgent))
        {

            if (_altiaDebug > 1)
                PRINTERR2("\tfd %d does belong to a client\n", inputFD);

            /* checkClient() returns FALSE if client has
             * disconnected.  If that's the case, then we need
             * to do something with the client agent
             */
            if ((retValue = altiaClientAgentCheckClient(nextAgent)) < 0)
            {
                PRINTERR1("Altia socket disconnect...ok\n");

                /* Remove entry that is now defunct. */
                if (prevAgent != NULL)
                    prevAgent->_next = nextAgent->_next;
                else if (nextAgent == _server->_clientAgents)
                    _server->_clientAgents = nextAgent->_next;

                /* Tell urlRef that this agent is going away */
                /* _urlRef->Clear(nextAgent); */

                /* Now it can be deleted.*/
                altiaClientAgentDelete(nextAgent);

                altiaClientServerCheckClientExit();

                /* A return of -1 instructs caller to stop
                 * dispatching events to us for the given
                 * descriptor.
                 */
                return -1;
            }

            /* Return non-negative value from ClientAgent::ClientCheck. */
            return retValue;
        }

        prevAgent = nextAgent;
        nextAgent = nextAgent->_next;
    }

    /* Should never get to here.  If we do, the descriptor passed
     * to us does not belong to us or any of our agents.  A return
     * of -1 instructs caller to stop dispatching events to us for
     * the given descriptor.
     */
    return -1;
}

/*----------------------------------------------------------------------*/
int altiaClientServerOutputReady(int outputFD, void *data)
{

    /* Match outputFD with a write descriptor from one of our client agents.
     * This is only used for agents connected to a client with a pipe service.
     * When the output descriptor becomes writeable, we have a newly
     * connected client on the pipe.
     */

    /* We can attach the client agent to the Behavior Router and link
     * the agent's input descriptor to our InputReady() routine to enable
     * the receipt of incoming requests.
     */

    /* A client connecting with a pipe is not supported in this
     * implementation.
     */

    PRINTERR1("Altia unexpected output ready\n");

    /* Should never get to here.  If we do, the descriptor passed
     * to us does not belong to us or any of our agents.  A return
     * of -1 instructs caller to stop dispatching events to us for
     * the given descriptor.
     */
    return -1;
}

/*----------------------------------------------------------------------*/
void altiaClientServerDisconnectClients(void)
{
    altiaClientAgentData* nextAgent;
    altiaClientAgentData* prevAgent;

    if (_server == NULL)
        return;

    /* We can't just delete the client agents.  If we have
     * an undead one then keep it around.
     */
    nextAgent = _server->_clientAgents;
    prevAgent = NULL;

    while (nextAgent != NULL)
    {
        if (altiaClientAgentUndead(nextAgent))
        {   /* Don't delete the undead */
            prevAgent = nextAgent;
            nextAgent = nextAgent->_next;
        }
        else
        {   /* Delete this one and move to the next */
            prevAgent->_next = nextAgent->_next;
            /* _urlRef->Clear(nextAgent); */
            altiaClientAgentDelete(nextAgent);
            nextAgent = prevAgent->_next;
        }
    }
}

/*----------------------------------------------------------------------*/
altiaClientAgentData *altiaClientServerFindAgent(char *event)
{
    altiaClientAgentData* nextAgent;

    if (_server == NULL)
        return 0;

    nextAgent = _server->_clientAgents;
    while (nextAgent != NULL)
    {
        if (altiaClientAgentEventSelectedByName(nextAgent, event))
            return nextAgent;
        nextAgent = nextAgent->_next;
    }
    return NULL;
}


/*========================================================================
 *
 * Class altiaClientAgent implementation:
 * --------------------------------------
 * Manage a connection to a specific client.
 *
 *========================================================================*/

typedef union
{
    int ival;
    float fval;
} IntFloat_type;


typedef union
{
    int   ival[2];
    double rval;
} IntRValue_type;

/* Max number of messages in a queue before we stop answering
 * client requests for to send more data
 */
#define MAXSYNCSIZE 4
#define MINSYNCSIZE 2
#define DELAY_EVENT_NAME "altiaInternalDelay"


/*---------------------------------------------------------------------- */
static altiaDelayedMessageData *altiaDelayedMessageCreate(
                                     int msgType,
                                     unsigned char* field1, int size1,
                                     unsigned char* field2, int size2,
                                     unsigned char* field3, int size3,
                                     unsigned char* field4, int size4)
{
    altiaDelayedMessageData *data = (altiaDelayedMessageData *)
        ALTIA_MALLOC(sizeof(altiaDelayedMessageData));

    data->_size1 = size1;
    data->_size2 = size2;
    data->_size3 = size3;
    data->_size4 = size4;
    data->_msgType = msgType;
    if (size1 > 0 && field1 != NULL)
    {
        data->_field1
            = (unsigned char *) ALTIA_MALLOC(size1 * sizeof(unsigned char));
        memcpy(data->_field1, field1, size1);
    }else
        data->_field1 = NULL;
    if (size2 > 0 && field2 != NULL)
    {
        data->_field2
            = (unsigned char *) ALTIA_MALLOC(size2 * sizeof(unsigned char));
        memcpy(data->_field2, field2, size2);
    }else
        data->_field2 = NULL;
    if (size3 > 0 && field3 != NULL)
    {
        data->_field3
            = (unsigned char *) ALTIA_MALLOC(size3 * sizeof(unsigned char));
        memcpy(data->_field3, field3, size3);
    }else
        data->_field3 = NULL;
    if (size4 > 0 && field4 != NULL)
    {
        data->_field4
            = (unsigned char *) ALTIA_MALLOC(size4 * sizeof(unsigned char));
        memcpy(data->_field4, field4, size4);
    }else
        data->_field4 = NULL;

    return data;
}

/*---------------------------------------------------------------------- */
static void altiaDelayedMessageDelete(altiaDelayedMessageData *data)
{
    if (data->_field1 != NULL)
        ALTIA_FREE(data->_field1);
    if (data->_field2 != NULL)
        ALTIA_FREE(data->_field2);
    if (data->_field3 != NULL)
        ALTIA_FREE(data->_field3);
    if (data->_field4 != NULL)
        ALTIA_FREE(data->_field4);
}

/*---------------------------------------------------------------------- */
altiaClientAgentData *altiaClientAgentCreate(
                           altiaClientServerData *server,
                           altiaClientConnectData* connection,
                           altiaIOHandler rHandler,
                           altiaIOHandler wHandler,
                           altiaIOHandler eHandler)
{
    altiaClientAgentData *data
        = (altiaClientAgentData *) ALTIA_MALLOC(sizeof(altiaClientAgentData));

    data->_next = NULL;
    data->_server = server;
    data->_connection = connection;
    data->_floatInterface = FALSE;
    data->_swapRValue = FALSE;
    data->_stringFloat = FALSE;
    data->_undead = FALSE;
    data->_notify = TRUE;

    /* The delay flag indicates that we have delayed this client
     * while we reduce the queue on the other clients if its TRUE.
     */
    if (server->_syncDelayOn)
        data->_delay = TRUE;
    else
        data->_delay = FALSE;

    /* The delayMess is the message that we are delaying sending the
     * client.
     */
    data->_delayMess = NULL;

    /* This is the handler that is called by the dispatcher when we have
     * data available.  We need it so we can unlink and relink with the
     * dispatcher as required by our performance optimizations.
     */
    data->_rHandler = rHandler;
    data->_wHandler = wHandler;
    data->_eHandler = eHandler;

    /* If _connection is non-NULL, than this is a TRUE pipe or socket
     * connection.  Otherwise, this is a psuedo connection for a direct
     * connect (i.e., user's application is directly linked into Altia).
     */
    if (data->_connection != NULL)
    {
        /* We keep a local copy of the connection's file descriptor so that
         * we can set it to -1 if this agent becomes inoperative for some
         * reason.  We don't want to set the actual connection's file
         * descriptor to -1 because it will need to be closed by the
         * connection's destructor.
         */
        data->_readFD = altiaCnctGetReadFD(data->_connection);
        data->_writeFD = altiaCnctGetWriteFD(data->_connection);

        /* If our connection is pipe based, then we will get optimal
         * performance by making our input buffer as large as the pipe's
         * buffer space. We also need an output buffer.  But, no
         * pipe based connections in this implementation so PIPSIZ
         * used for _inputBuf size was replaced with 4096 just like
         * the 4096 used for the size of _outputBuf.
         */
        data->_inputBuf
            = (unsigned char *) ALTIA_MALLOC(4096 * sizeof(unsigned char));
        data->_inputBufSize = 4096;
        data->_outputBuf
            = (unsigned char *) ALTIA_MALLOC(4096 * sizeof(unsigned char));
        data->_outputBufSize = 4096;

       /* Ok, this is a bit funky.  We attempt to read a maximum amount of
        * data from our socket with each read call.  If we read multiply
        * messages, we need a way to force the Interviews event loop to
        * give us control back until we empty our buffer.  This IOHandler
        * object is used to attach a handler for our socket in place of the
        * ClientServer's normal handler while we have messages in our buffer.
        */
       data->_messageHandler = altiaClientAgent_handleMessage;
    }
    else
    {
        data->_readFD = -1;
        data->_writeFD = -1;
        data->_inputBuf = NULL;
        data->_inputBufSize = 0;
        data->_outputBuf = NULL;
        data->_outputBufSize = 0;
        data->_messageHandler = NULL;
    }

    /* To start with, the buffer which reads data from the connection
     * is completely empty.  Later, it may be partially filled because
     * we have read an incomplete message.
     */
    data->_nextInputPtr = data->_inputBuf;

    /* PORTING NOTES:
     * In the Altia version of this code, now is the time to create a
     * behavior event receiver for this client agent with code like:
     *
     *    _eventRcvr = new ClientBehaviorReceiver(this);
     *
     * Later, the client server attaches this client agent to the router
     * which effectively registers the behavior receiver's event handler
     * with the router for any and all events.
     *
     * For DeepScreen, the client server hooks itself into
     * the DeepScreen event loop in altiaClientServerCreate()
     * by assigning the address of altiaClientServerHandleEvent()
     * to the DeepScreen AltiaReportFuncPtr function pointer.
     * Whenever the handler gets called, it passes the event on
     * to each client agent via the altiaClientAgentHandleEvent()
     * function.
     */

    /* Initialize filter for events from behavior router.  Initially, we
     * aren't interested in any events.  That is, the client must register
     * an interest in an event or all events. 
     */
    data->_eventFilter = altiaClientFilterCreate();
    altiaClientFilterIgnoreAll(data->_eventFilter);

    /* Intialize queue for events from behavior router.  Events sit in
     * this queue until they are requested by client.  This first queue
     * instance is used only as a placeholder.
     */
    data->_eventQueue = altiaClientQueueCreate("", 0);

    /* We haven't heard from the client yet so we cannot speculate on
     * the number of queue bytes it can accept.
     */
    data->_clientQueueSpace = 0;

    /* If anyone else (like our behavior receiver) might be interested in
     * what type of message was last sent to us.
     * To start with, no messages have been sent so initial
     * last message is undefined.
     */
    data->_lastMsgType = UndefinedMsg;

    /* Default mode is to have router update the display after
     * an event is routed.
     */
    data->_doUpdate = 1;

    /* tjw, 3/8/93:  Others can give us a list of events that they would
     *               like us to route when this ClientAgent instance is
     *               destructed.
     */
    data->_lastRouteList = altiaClientQueueCreate("", 0);

    return data;
}

/*---------------------------------------------------------------------- */
void altiaClientAgentDelete(altiaClientAgentData *data)
{

    if (data->_delayMess != NULL)
        altiaDelayedMessageDelete(data->_delayMess);
    data->_delayMess = NULL;

    if (data->_server->_syncClients && data->_server->_syncDelayOn
        && data->_delay == FALSE)
    {   /* We are in delayed mode so clear it out by telling other
         * agents that we no longer are delayed.  Since we already
         * deleted the delayed message it will not get sent to the
         * client.  But since we are deleteing the connection
         * sending the message could be dangerous.
         */
        altiaClientAgentRouteDelay(data, FALSE);
    }

    /* If this is a real socket or pipe connection, need to do some
     * destructing.
     */
    if (data->_connection != NULL)
    {
        /* In case we are still hooked up to the dispatcher, so unlink. */
        altiaDispatcher_unlink(altiaCnctGetReadFD(data->_connection));
        altiaDispatcher_unlink(altiaCnctGetWriteFD(data->_connection));

        /* Delete input and output buffers. */
        ALTIA_FREE(data->_inputBuf);
        ALTIA_FREE(data->_outputBuf);

        /* Detach us from our custom handler. */
        data->_messageHandler = NULL;

        /* The client server gives us the task of destroying our 
         * client connection.
         */
        altiaCnctDeleteConnect(data->_connection);
    }

    /* PORTING NOTES:
     * In the Altia version of this code, now is the time to detach the
     * behavior event receiver from the router and delete it with code like:
     *
     *    _eventRouter->DetachFromEvent(_eventRcvr);
     *    delete _eventRcvr;
     *
     * For DeepScreen, the client server hooks itself into
     * the DeepScreen event loop in altiaClientServerCreate()
     * by assigning the address of altiaClientServerHandleEvent()
     * to the DeepScreen AltiaReportFuncPtr function pointer.
     * Whenever the handler gets called, it passes the event on
     * to each client agent via the altiaClientAgentHandleEvent()
     * function.  As soon as the client server removes this agent from
     * its list of client agents (which probably has already happened
     * if this client is getting deleted), this client agent will
     * stop receiving events.
     */

    /* Our queue and filter classes need to be deleted.  The queue class
     * will delete all connected queue instances.
     */
    altiaClientFilterDelete(data->_eventFilter);

    /* Delete the events in our queue (and this also deletes the head). */
    altiaClientQueueDeleteList(data->_eventQueue);

    /* If there are events to be routed when we are destructed,
     * do them now.
     */
    {
        altiaClientQueueData *listPtr
            = altiaClientQueueNext(data->_lastRouteList);
        altiaClientQueueData *nextPtr;
        while (listPtr != data->_lastRouteList && !(data->_server->_quit))
        {
            /* There's a good chance that the receiver of this
             * message route will ask us to remove this last route
             * request from our queue.  We need to get the next
             * entry in the list before routing the current message
             * so that we avoid an infinite loop.
             */
            nextPtr = altiaClientQueueNext(listPtr);

            /* PORTING NOTES:
             * In the Altia version of this code, now is the time to route
             * the set of events in the last route list with code like:
             *
             *    _eventRouter->RouteEvent(NULL,
             *                       listPtr->Name(), listPtr->Value(),
             *                       BehaviorRouter::InternalEvent,
             *                       NULL, NULL);
             *
             * Note that the final argument to tell the router whether or
             * not to do an update is missing so it defaults to doing an
             * update if an update is needed.
             *
             * For DeepScreen, this is done with a call to the DeepScreen
             * TargetAltiaAnimate() and TargetAltiaUpdate() functions.
             */

#ifndef UNICODE
            if (TargetAltiaAnimate(altiaClientQueueName(listPtr),
                                   altiaClientQueueValue(listPtr)))
                    TargetAltiaUpdate();
#else
             {
                 /* For Unicode, convert 8-bit event name to wide string */
                 ALTIA_CHAR wideName[MAX_UNICODE_LEN];
                 localCopyCharToWide(altiaClientQueueName(listPtr),
                                     wideName, MAX_UNICODE_LEN);
                 if (TargetAltiaAnimate(wideName,
                                        altiaClientQueueValue(listPtr)))
                         TargetAltiaUpdate();
             }
#endif
            
            listPtr = nextPtr;
        }
    }

    /* Destruct our list of last events to route. */
    altiaClientQueueDeleteList(data->_lastRouteList);
}

/*----------------------------------------------------------------------
 * Others can ask us to route events just before we destruct
 * ourselves sometime in the near or distant future.
 */
void altiaClientAgentAddLastRoute(altiaClientAgentData *agent,
                                  char *eventName, AltiaEventType value)
{
    if (agent != NULL && eventName != NULL && eventName[0] != '\0')
    {
        /* Make sure entry is not already in our list */
        altiaClientQueueData *listPtr
            = (altiaClientQueueData *) altiaClientQueueNext(
                                            agent->_lastRouteList);
        while (listPtr != agent->_lastRouteList)
        {
            if (strcmp(altiaClientQueueName(listPtr), eventName) == 0
                && altiaClientQueueValue(listPtr) == value)
            {
                return;
            }
            else
            {
                listPtr = altiaClientQueueNext(listPtr);
            }
        }
        altiaClientQueuePrepend(agent->_lastRouteList,
                                altiaClientQueueCreate(eventName, value));
    }
}

/*----------------------------------------------------------------------
 * Others can remove their request for us to route a particular
 * event and value when we destruct.
 */
void altiaClientAgentRemoveLastRoute(altiaClientAgentData *agent,
                                     char *eventName, AltiaEventType value)
{
    if (agent != NULL
        && !(altiaClientQueueEmpty(agent->_lastRouteList))
        && eventName != NULL && eventName[0] != '\0')
    {
        altiaClientQueueData *listPtr
            = altiaClientQueueNext(agent->_lastRouteList);

        while (listPtr != agent->_lastRouteList)
        {
            if (strcmp(altiaClientQueueName(listPtr), eventName) == 0
                && altiaClientQueueValue(listPtr) == value)
            {
                /* Remove entry from list before deleting it. */
                altiaClientQueueData *tempPtr = listPtr;
                listPtr = altiaClientQueueNext(listPtr);
                altiaClientQueueRemove(tempPtr);
                altiaClientQueueDelete(tempPtr);
            }
            else
            {
                altiaClientQueueNext(listPtr);
            }
        }
    }
}

/*----------------------------------------------------------------------*/
/* PORTING NOTES:
 * In the Altia version, it is necessary for each client agent to
 * attach an event receiver to the router as demonstrated by the code
 * below (which is commented out).
 *
 * For DeepScreen, the client server hooks itself into
 * the DeepScreen event loop in altiaClientServerCreate()
 * by assigning the address of altiaClientServerHandleEvent()
 * to the DeepScreen AltiaReportFuncPtr function pointer.
 * Whenever the handler gets called, it passes the event on
 * to each client agent via the altiaClientAgentHandleEvent()
 * function.
 *
 * void altiaClientAgentAttachToRouter(altiaClientAgentData *agent)
 * {
 *     // Attach receiver to all behavior events - that's the default action
 *     // when only a pointer to the receiver is passed.
 *     //
 *     _eventRouter->AttachToEvent(_eventRcvr);
 * }
 */

/*----------------------------------------------------------------------*/
/* PORTING NOTES:
 * In the Altia version, it is necessary for each client agent to
 * attach an event receiver to the router and then detach it when
 * the client agent finds it necessary to do so (for example, when
 * it destructs itself). How this is done in the Altia version is
 * demonstrated by the code below (which is commented out).
 *
 * For DeepScreen, the client server hooks itself into
 * the DeepScreen event loop in altiaClientServerCreate()
 * by assigning the address of altiaClientServerHandleEvent()
 * to the DeepScreen AltiaReportFuncPtr function pointer.
 * Whenever the handler gets called, it passes the event on
 * to each client agent via the altiaClientAgentHandleEvent()
 * function.  A client agent is automatically out of the event
 * passing process when it is removed from the client server's
 * list of client agents.
 *
 * void altiaClientAgentDetachFromRouter(altiaClientAgentData *agent)
 * {
 *     // Detach our behavior event receiver from behavior router
 *     _eventRouter->DetachFromEvent(_eventRcvr);
 * }
 */

/*----------------------------------------------------------------------*/
void altiaClientAgentClearEventQueue(altiaClientAgentData *agent,
                                     char *eventName)
{
      /* Clear out all instances of this event from our queue if it is
       * of interest to client.  The events in the queue are all
       * out-of-date because the user polled the event.
       */
      if (agent != NULL && eventName != NULL
          && altiaClientAgentEventSelected(agent, eventName))
      {
          altiaClientQueueDeleteEvents(agent->_eventQueue, eventName);
      }
}

/*----------------------------------------------------------------------*/
int altiaClientAgentEventSelected(altiaClientAgentData *agent,
                                  char *eventName)
{
    int res = altiaClientFilterInteresting(agent->_eventFilter, eventName);

    if (altiaClientFilterInterestedInExtern(agent->_eventFilter))
    {   /* We want all external events */
        if (res)
            return TRUE;
        else
        {   /* If we are not interested we still could be if its
             * not an internal event
             */
            if (strcmp(eventName, "altiaCloseViewPending") == 0)
            {   /* treat close view pending special */
                return FALSE;
#ifdef TODO_DONE
            }else if (!(agent->_server->_quit)
                      && _eventRouter->GetEventType() 
                              != BehaviorRouter::InternalEvent)
            {   /* Note:  This may not be correct if the event has
                 * not just been routed (ie client asked if the event
                 * is selected).  But we have to live with this problem
                 * until we change the router.
                 */
                return TRUE;
#endif /* TODO_DONE */
            } else
                return FALSE;
        }
    } else
        return res;
}

/*----------------------------------------------------------------------*/
int altiaClientAgentEventSelectedByName(
         altiaClientAgentData *agent, char *eventName)
{
        return altiaClientFilterNameInList(agent->_eventFilter, eventName);
}

/*----------------------------------------------------------------------*/
int altiaClientAgentCheckEvent(altiaClientAgentData *agent,
                               char *eventName, AltiaEventType *nextValueOut)
{
    altiaClientQueueData *queuePtr;

    /* Is event with given name even possibly in our queue. */
    if (eventName == NULL 
        || !(altiaClientAgentEventSelected(agent, eventName)))
        return FALSE;

    /* Look for next instance of event in our queue */
    queuePtr = agent->_eventQueue;
    while ((queuePtr = altiaClientQueueNext(queuePtr)) != agent->_eventQueue)
    {
        /* If we find an instance, return its value, delete the instance,
         * and return TRUE.
         */
        if (strcmp(eventName, altiaClientQueueName(queuePtr)) == 0)
        {
            *nextValueOut = altiaClientQueueValue(queuePtr);
            altiaClientQueueRemove(queuePtr);
            altiaClientQueueDelete(queuePtr);
            return TRUE;
        }
    }

    /* Didn't find an instance so return FALSE. */
    return FALSE;
}

/*----------------------------------------------------------------------*/
void altiaClientAgentSelectEvent(altiaClientAgentData *agent,
                                 char *eventName)
{
    if (eventName == NULL)
        return;

    if (strcmp((char *) eventName, "*") == 0)
        altiaClientFilterUnignoreAll(agent->_eventFilter);
    else
        altiaClientFilterUnignore(agent->_eventFilter, eventName);
}

/*----------------------------------------------------------------------*/
void altiaClientAgentSelectExternEvent(altiaClientAgentData *agent)
{
    altiaClientFilterUnignoreExtern(agent->_eventFilter);
}

/*----------------------------------------------------------------------*/
void altiaClientAgentUnselectExternEvent(altiaClientAgentData *agent)
{
    altiaClientFilterIgnoreExtern(agent->_eventFilter);
}

/*----------------------------------------------------------------------*/
void altiaClientAgentUnselectEvent(altiaClientAgentData *agent,
                                   char *eventName)
{
    if (eventName == NULL)
        return;

    if (strcmp(eventName, "*") == 0)
    {
        altiaClientFilterIgnoreAll(agent->_eventFilter);

        /* Entire event queue should be cleared. All events up to
         * this point have been rendered obsolete.
         */
        altiaClientQueueDeleteList(agent->_eventQueue);
        agent->_eventQueue = altiaClientQueueCreate("", 0);
    }
    else
    {
        altiaClientFilterIgnore(agent->_eventFilter, eventName);

        /* Delete all instances of this particular event from
         * event queue.
         */
        altiaClientQueueDeleteEvents(agent->_eventQueue, eventName);
    }
}

/*----------------------------------------------------------------------*/
int altiaClientAgentGetPendingCount(altiaClientAgentData *agent)
{
    altiaClientQueueData *queuePtr = agent->_eventQueue;
    int eventCount = 0;

    while ((queuePtr = altiaClientQueueNext(queuePtr)) != agent->_eventQueue)
    {
        eventCount++;
    }

    return eventCount;
}

/*----------------------------------------------------------------------*/
int altiaClientAgentGetNextEvent(altiaClientAgentData *agent,
         char *nameStringInOut, int stringSize, AltiaEventType *eventValueOut)
{
    altiaClientQueueData *queuePtr;

    if ((queuePtr = altiaClientQueueNext(agent->_eventQueue))
        == agent->_eventQueue)
        return -1;

    if (stringSize < altiaClientQueueNameLength(queuePtr))
    {
        strncpy(nameStringInOut, altiaClientQueueName(queuePtr), stringSize);
        nameStringInOut[stringSize - 1] = '\0';
    }
    else
        strcpy(nameStringInOut, altiaClientQueueName(queuePtr));
    
   *eventValueOut = altiaClientQueueValue(queuePtr);

   /* The queue entry has served its purpose.  We nolonger need. */
   altiaClientQueueRemove(queuePtr);
   altiaClientQueueDelete(queuePtr);

    return 0;
}

/*----------------------------------------------------------------------*/
int altiaClientAgentCheckClient(altiaClientAgentData *agent)
{
    /* If this is a psuedo connection (not a real pipe or socket), then
     * just return 0 so caller does nothing.  This should never get
     * called in the case of a psuedo connect - its just a precaution.
     */
    if (agent->_connection == NULL)
        return 0;

    /* This establishes if the input buffer contains partial data.
     * This can occur if a previous read truncated a message because
     * the end of _inputBuf was reached before the complete message
     * could be read.  The partial message should have been moved to
     * the head of the input buffer by the message reader so it could
     * be completed now.
     */
    agent->_partialStart = agent->_nextInputPtr - agent->_inputBuf;

    if (_altiaDebug > 1)
        PRINTERR1("\treading data from client...\n");

    /* We should always have data because we were called as a result of a
     * select(2).  If the read returns 0, it indicates a connection closure.
     * A return of less than 0 indicates a fatal error.
     */
    if ((agent->_readSize
         = altiaCnctRead(agent->_connection, agent->_nextInputPtr,
                        agent->_inputBufSize - agent->_partialStart)) <= 0)
    {
#ifdef WIN32
        if ((agent->_readSize == -1)
            && ((*pWSAGetLastError)() == WSAEWOULDBLOCK))
            return(0);
#endif

        if (_altiaDebug > 1)
            PRINTERR1("\tread from client FAILED!\n");

        return(-1);
    }

    if (_altiaDebug > 1)
        PRINTERR2("\t%d bytes read from client\n", agent->_readSize);

    /* Ok, we must have some data. We'll start at the head of the
     * input buffer and handle messages until they run out via
     * _handleMessage instead of the ClientServer's io handler.
     */
    agent->_nextInputPtr = agent->_inputBuf;

    /* Bypass client server's io handler. */
    altiaDispatcher_link(agent->_readFD, DispatcherReadMask,
                         altiaClientAgent_handleMessage, (void *) agent);

    return altiaClientAgent_handleMessage(agent->_readFD, agent);
}

/*----------------------------------------------------------------------*/
AltiaEventType altiaClientAgent_getRValue(altiaClientAgentData *agent,
                                 char *name, unsigned char **msgPtrInOut)
{
    /* TODO:  May want to support fixed point some day */
    double vtype;
    IntRValue_type irval;
    unsigned char *msgPtr = *msgPtrInOut;

    if (agent->_floatInterface)
    {
      if (agent->_stringFloat)
      {
          char strbuffer[128];
          int len = *msgPtr++;
          int i;
          if ((len + 1) > 128)
              len = 127;
          for (i = 0; i < len; i++)
          {
              strbuffer[i] = *msgPtr++;
          }
          strbuffer[len] = 0;
          sscanf(strbuffer, "%lg", &vtype);
      }else
      {
          /* First 8 bytes is value field */
          int start;
          int inc;
          int i;
          /* The socket will do the byte swapping for us.  The
           * only thing we have to worry about is what values go first
           */
          if (agent->_swapRValue)
          {
              start = 1;
              inc = -1;
          }else
          {
              start = 0;
              inc = 1;
          }
          i = start;
          irval.ival[i] = *msgPtr++ << 24;
          irval.ival[i] |= *msgPtr++ << 16;
          irval.ival[i] |= *msgPtr++ << 8;
          irval.ival[i] |= *msgPtr++;
          i += inc;
          irval.ival[i] = *msgPtr++ << 24;
          irval.ival[i] |= *msgPtr++ << 16;
          irval.ival[i] |= *msgPtr++ << 8;
          irval.ival[i] |= *msgPtr++;
          vtype = irval.rval;
      }
    }else
    {
      int value;
#ifndef UNICODE
      char *eventName = name;
#else
      ALTIA_CHAR *eventName;
      ALTIA_CHAR wideName[MAX_UNICODE_LEN];
#endif

      /* First 4 bytes is value field */
      value = *msgPtr++ << 24;
      value |= *msgPtr++ << 16;
      value |= *msgPtr++ << 8;
      value |= *msgPtr++;

      /* Next if statement originally requested a verification of the type
       * for the event name from the router as in:
       * 
       *    if (name != NULL && _eventRouter->IsType(name, ROUTER_FLOAT_TYPE))
       *
       * Now it makes a call to a DeepScreen function to do this.
       */

#ifdef UNICODE
      /* For Unicode, convert 8-bit event name to wide character string */
      localCopyCharToWide(name, wideName, MAX_UNICODE_LEN);
      eventName = wideName;
#endif

      /* Don't call into DeepScreen code if altiaQuit was already detected */
      if (!(agent->_server->_quit) && name != NULL && _altiaConnectionToFloat(eventName))
      {   /* If we are routing to a float type then don't
           * typecast, instead preserve the float ness of the number
           */
          IntFloat_type ifloat;
          ifloat.ival = value;
          vtype = ifloat.fval;
      }
      else
          vtype = value;
    }

    *msgPtrInOut = msgPtr;
    return (AltiaEventType) vtype;
}

/*----------------------------------------------------------------------*/
void altiaClientAgent_setRValue(altiaClientAgentData *agent,
                                char *name, AltiaEventType value, 
                                unsigned char** msgPtrInOut, int *lenOut)
{
     /* TODO:  May want to support fixed point some day */

     IntRValue_type irval;
     unsigned char *msgPtr = *msgPtrInOut;

     irval.rval = value;
     if (agent->_floatInterface)
     {
         if (agent->_stringFloat)
         {
             int i;
             char strBuffer[128];

             sprintf(strBuffer, "%.32g", (double) value);
             *lenOut = strlen(strBuffer);
             *msgPtr++ = *lenOut;
             for (i = 0; i < *lenOut; i++)
             {
                 *msgPtr++ = strBuffer[i];
             }
             (*lenOut)++; /* need to add length byte */
         } else
         {
             int start;
             int inc;
             int i;
             if (agent->_swapRValue)
             {
                 start = 1;
                 inc = -1;
             }else
             {
                 start = 0;
                 inc = 1;
             }
             i = start;
             *msgPtr++ = irval.ival[i] >> 24;
             *msgPtr++ = irval.ival[i] >> 16;
             *msgPtr++ = irval.ival[i] >> 8;
             *msgPtr++ = irval.ival[i];
             i += inc;
             *msgPtr++ = irval.ival[i] >> 24;
             *msgPtr++ = irval.ival[i] >> 16;
             *msgPtr++ = irval.ival[i] >> 8;
             *msgPtr++ = irval.ival[i];
             *lenOut = 8;
         }
     }else
     {
#ifndef UNICODE
          char *eventName = name;
#else
          ALTIA_CHAR *eventName;
          ALTIA_CHAR wideName[MAX_UNICODE_LEN];
#endif

          /* Next if statement originally requested a verification of the type
           * for the event name from the router as in:
           * 
           *    if (_eventRouter->IsType(name, ROUTER_FLOAT_TYPE))
           *
           * Now it makes a call to a DeepScreen function to do this.
           */

#ifdef UNICODE
          /* For Unicode, convert 8-bit event name to wide character string */
          localCopyCharToWide(name, wideName, MAX_UNICODE_LEN);
          eventName = wideName;
#endif

         /* Don't call into DeepScreen code if altiaQuit already detected */
         /* 8/31/15, DE929: For event from DeepScreen, _altiaConnectFromFloat()
          * indicates if event value is really a 32-bit float which needs to be
          * packed into a 32 bit integer for sending to an integer client.
          */
         if (!(agent->_server->_quit) && _altiaConnectionFromFloat(eventName))
         {
             IntFloat_type ifloat;
             ifloat.fval = (float)value;
             *msgPtr++ = ifloat.ival >> 24;
             *msgPtr++ = ifloat.ival >> 16;
             *msgPtr++ = ifloat.ival >> 8;
             *msgPtr++ = ifloat.ival;
         }else
         {
             int ivalue = altiaClientServer_round(value);
             *msgPtr++ = ivalue >> 24;
             *msgPtr++ = ivalue >> 16;
             *msgPtr++ = ivalue >> 8;
             *msgPtr++ = ivalue;
         }
         *lenOut = 4;
     }

    *msgPtrInOut = msgPtr;
}

/*----------------------------------------------------------------------*/
/* TODO:  Fix DeepScreen implementation for "altiaGetViewSize". */
static char *altiaGetViewSizeString = "altiaGetViewSize";
static int altiaGetViewSizeValue = 0;
asvr_bool altiaClientAgent_handleWindowChanges(altiaClientServerData *server,
                                               char *name,
                                               AltiaEventType value)
{
    static ALTIA_CHAR *windowText = NULL;
    static int windowTextSize = 0;
    static int windowTextLen = 0;
    static int windowTextReset = 1;
    static int stateOfXYPair = 0;
    static int placement = -2;     /* User Place by default */
    static int nextX, nextY;
    static int viewId = 0;
    static int viewStyle = 0;

    /* Don't check animation name if it does not start with altia[A-Z]. */
    if (strncmp(name, "altia", 5) != 0 || name[5] < 'A' || name[5] > 'Z')
        return FALSE;

    /* To work around a shortcoming in the DeepScreen implementation
     * of views (versus changing all versions of the socket API
     * library on all platforms), always handle placement changes
     * and save the new settings.
     */
    if (strcmp(name, "altiaSetViewPlacement") == 0)
    {
        /* This function may get called before a view id is specified
         * so take note of the value and do not return TRUE.
         */

        /* -2: User Place (default) */
        /* -1: RootWindowPlace      */
        /*  0: MainViewPlace        */
        placement = (int) value;
        return FALSE;
    }
    else if (strcmp(name, "altiaSetViewPlacementX") == 0)
    {
        /* This function may get called before a view id is specified
         * so take note of the value and do not return TRUE.
         */

        nextX = (int) value;
        stateOfXYPair |= 1;
        return FALSE;
    }
    else if (strcmp(name, "altiaSetViewPlacementY") == 0)
    {
        /* This function may get called before a view id is specified
         * so take note of the value and do not return TRUE.
         */

        nextY = (int) value;
        stateOfXYPair |= 2;
        return FALSE;
    }

    /* TODO:  Fix DeepScreen implementation for "altiaGetViewSize".
     * To work around a current shortcoming in the DeepScreen
     * implementation of views, keep track of the view id internally
     * from a client request to get a view's size so that we can
     * return it correctly if the client poll's this animation.
     * Client implementations of altiaGetViewSize()/AtGetViewSize()
     * expect to poll the view id and get back the value sent
     * otherwise they get stuck in an infinite look.
     */
    if (strcmp(name, altiaGetViewSizeString) == 0)
    {
        altiaGetViewSizeValue = (int) value;
        return FALSE;
    }

    /* To work around a shortcoming in the DeepScreen implementation
     * of views (versus changing all versions of the socket API
     * library on all platforms), always handle open view requests.
     */
    if (strcmp(name, "altiaOpenView") == 0)
    {
        /* If this process was started with -nowin and the view id is 1,
         * this is a request to manipulate the main window and requires
         * special handling because it is not a normal view.
         */
        if (server->_nowin && (int) value == 1)
        {
#if defined(WIN32_GDI32) || defined(UNIX)
            TargetAltiaSetWindowStyle(viewStyle);
#endif

            if (stateOfXYPair == 3)
            {
#if defined(WIN32_GDI32) || defined(UNIX)
                TargetAltiaPositionWindow(nextX, nextY, 0 /* Do not Center */);
#endif
                stateOfXYPair = 0;
            }
#if defined(WIN32_GDI32) || defined(UNIX)
            else if (placement == -1 || placement == 0)
                TargetAltiaPositionWindow(0, 0, 1 /* Center */);
#endif

            placement = -2;  /* Reset placement to User Place (default) */

#if defined(WIN32_GDI32) || defined(UNIX)
            TargetAltiaShowWindow(1);
#endif
            return TRUE;
        }

        /* To work around a shortcoming in the DeepScreen implementation
         * of views (versus changing all versions of the socket API
         * library on all platforms), generate placement events
         * before opening normal views (i.e., views that are not the
         * main window).
         */
        if (stateOfXYPair == 3)
        {
            TargetAltiaAnimate(ALT_TEXT("altiaSetViewPlacementX"),
                               (AltiaEventType) nextX);
            TargetAltiaAnimate(ALT_TEXT("altiaSetViewPlacementY"),
                               (AltiaEventType) nextY);
            stateOfXYPair = 0;
        }

        TargetAltiaAnimate(ALT_TEXT("altiaSetViewPlacement"),
                           (AltiaEventType) placement);

        /* Reset placement to User Place (default) */
        placement = -2;

        /* This was a normal view (i.e., not the main window) so let the
         * DeepScreen implementation of normal views finish the handling
         * of the open view request.
         */
        return FALSE;
    }

    /* Any further action should only take place if this is a manipulation
     * of the main window.  If this process was not started with the -nowin
     * option, this cannot be a request to manipulate the main window.
     */
    if (!server->_nowin)
    {
        return FALSE;
    }

    if (strcmp(name, "altiaCloseView") == 0 && (int) value == 1)
    {
#if defined(WIN32_GDI32) || defined(UNIX)
        TargetAltiaShowWindow(0);
#endif
        return TRUE;
    }
    else if (strcmp(name, "altiaSetViewName") == 0 && viewId == 1)
    {
        int character = (int) value;

        /* Initialize text buffer the first time */
        if (windowText == NULL)
        {
            windowTextSize = 50;
            windowText = (ALTIA_CHAR *)
                ALTIA_MALLOC(windowTextSize * sizeof(ALTIA_CHAR));
        }
        
        /* If we should reset the text to an empty string, do it now */
        if (windowTextReset)
        {
            windowText[0] = '\0';
            windowTextLen = 0;
            windowTextReset = 0;
        }
        
        /* If there is not room for another character and terminator, make
         * the text buffer larger.
         */
        if (windowTextSize < (windowTextLen + 2))
        {
            windowTextSize += 50;
            windowText = (ALTIA_CHAR *)
                ALTIA_REALLOC(windowText, windowTextSize * sizeof(ALTIA_CHAR));
        }

        /* Add new character to text buffer */
        windowText[windowTextLen++] = (ALTIA_CHAR) character;

        /* If this is the string terminator, we have a window name */
        if (character == '\0')
        {
#if defined(WIN32_GDI32) || defined(UNIX)
            TargetAltiaSetWindowName(windowText);
#endif
            /* Next time through, restart the building of a new window name */
            windowTextReset = 1;
        }
        else
            windowText[windowTextLen] = '\0';

        return TRUE;
    }
    else if (strcmp(name, "altiaSetView") == 0)
    {
        /* We want to keep track if the view number is not our own */
        viewId = (int) value;

        /* If it is our own, let caller know that action was taken */
        if (viewId == 1)
            return TRUE;
        else
            return FALSE;
    }
    else if (strcmp(name, "altiaSetViewStyle") == 0
             && (viewId == 0 || viewId == 1))
    {
        /* View id may not be set the first time this function is called
         * which is why a viewId of 0 is allowed in addition to 1.
         */
        viewStyle = (int) value;
        return TRUE;
    }

    return FALSE;
}

/*----------------------------------------------------------------------*/
int altiaClientAgent_handleMessage(int readFD, void *data)
{
    /* When the particular client agent chose to bypass the client server's
     * io handler with this one, it set itself as the handler data element.
     */
    altiaClientAgentData *agent = (altiaClientAgentData *) data;

    /* The message size returned in msgSize is the number of bytes
     * starting at msgPtr that make up the next message. The pointer
     * in _nextInputPtr is moved to what should be the  start of the
     * next message in the buffer. When _getNextMessage() returns FALSE,
     * we have no more messages. _nextInputPtr will be set to _inputBuf
     * if no partial message was detected.  Otherwise, the partial
     * message is moved to the start of _inputBuf and _nextInputPtr
     * points to the location following it.
     */
    unsigned char* msgPtr;
    int msgSize;
    int msgType;

    if (agent == NULL 
        || !altiaClientAgent_getNextMessage(
                 agent, agent->_inputBuf,
                 agent->_readSize + agent->_partialStart,
                 &msgType, &msgPtr, &msgSize, &(agent->_nextInputPtr)))
    {
        /* No more messages. Reestablish notification via ClientServer's
         * io handler.
         */
        altiaDispatcher_link(readFD, DispatcherReadMask,
                             agent->_rHandler, agent);
        return 0;
    }

    /* If anyone else (like our behavior receiver) might be interested in
     * what type of message was last sent to us.
     */
    msgType &= MsgMask;

    switch (msgType)
    {
       /* If type is RouteEventMsg, client is sending us
        * a behavior event that should be routed.
        */
       case RouteEventMsg:
       {
          AltiaEventType vtype;
          char *ptr = (char *)msgPtr;

          if (agent->_floatInterface)
          {
              if (agent->_stringFloat)
              {
                  int flen = *ptr;
                  ptr += flen + 1;
              }else
                  ptr += 8;
          }else
              ptr += 4;

          vtype = altiaClientAgent_getRValue(agent, ptr, &msgPtr);

          if (_altiaDebug > 1)
            PRINTERR3("Received ROUTE request from client for event %s(%g)\n",
                      (char *) msgPtr, (double) vtype);


          /* Note that event name string is next in
           * buffer pointed to by msgPtr
           */

          /* PORTING NOTES:
           * In the Altia version of this code, now is the time to route
           * the event with code like:
           *
           *    _eventRouter->RouteEvent(_eventRcvr, ptr, 
           *                       vtype, BehaviorRouter::ClientEvent,
           *                       NULL, this,    // 3/8/93: new code!
           *                       _doUpdate);   // 7/20/93: new code!
           *
           * Note that the final argument to tell the router whether or
           * not to do an update comes from the client agent's _doUpdate
           * flag.
           *
           * For DeepScreen, routing is done with a call to the DeepScreen
           * TargetAltiaAnimate() function and then a call to
           * TargetAltiaUpdate() if it is OK to update.  However, before
           * doing any of this, we want to set up for the client server
           * to ignore this event if it comes back to us (i.e., we do not
           * want to retransmit the event to the client).
           */

          /* However, do not take any action if an altiaQuit has been
           * detected.
           */
          if (agent->_server->_quit)
              break;

          agent->_server->_ignoreClient = agent;
          agent->_server->_ignoreName = ptr;
          agent->_server->_ignoreValue = vtype;

          /* For DeepScreen, support main view style, hide/show and move */
          if (!altiaClientAgent_handleWindowChanges(agent->_server, ptr, vtype))
          {
#ifndef UNICODE
              char *eventName  = ptr;
#else
              /* For Unicode, convert 8-bit event name to wide event name */
              ALTIA_CHAR eventName[MAX_UNICODE_LEN];
              localCopyCharToWide(ptr, eventName, MAX_UNICODE_LEN);
#endif
              if (TargetAltiaAnimate(eventName, vtype) && agent->_doUpdate)
                  TargetAltiaUpdate();
          }

          agent->_server->_ignoreClient = NULL;
          agent->_server->_ignoreName = NULL;
          agent->_server->_ignoreValue = 0;

          break;
       }

       /* If type is FlushDisplayMsg, client is asking us to force an
        * update of the displays immediately.
        */
       case FlushDisplayMsg:
       {
          /* Event router knows how to update display already so let it
           * do the job.
           */

          /* PORTING NOTES:
           * In the Altia version of this code, now is the time to have
           * the router perform an update of the display with code like:
           *
           *    _eventRouter->UpdateDisplay();
           *
           * For DeepScreen, updating of the Display is done with a call
           * to TargetAltiaUpdate().
           */

          /* However, do not take any action if an altiaQuit has been
           * detected.
           */
          if (!agent->_server->_quit)
              TargetAltiaUpdate();

          break;
       }

       case FloatInterfaceMsg:
       {
          agent->_floatInterface = TRUE;
          if (msgSize > 0)
          {   /* We have a new float interface message that includes
               * the swap test value.
               */

              /* Save msgPtr since getRvalue will inc it.*/
              unsigned char* savePtr = msgPtr;
              AltiaEventType vtype
                  = altiaClientAgent_getRValue(agent, NULL, &msgPtr); 
              if (altiaClientServer_REQUAL(vtype, ALTIA_DOUBLE_TEST_VALUE)
                  == FALSE)
              {
                  agent->_swapRValue = TRUE;
                  vtype = altiaClientAgent_getRValue(agent, NULL, &savePtr); 
                  if (altiaClientServer_REQUAL(vtype, ALTIA_DOUBLE_TEST_VALUE)
                      == FALSE)
                      /* This should never happen */
                      agent->_swapRValue = FALSE;
              }
          }   
          break;
       }

       case StringFloatInterfaceMsg:
       {
          agent->_floatInterface = TRUE;
          agent->_stringFloat = TRUE;
          if (msgSize > 0)
          {
              AltiaEventType vtype
                  = altiaClientAgent_getRValue(agent, NULL, &msgPtr); 
              if (altiaClientServer_REQUAL(vtype, ALTIA_DOUBLE_TEST_VALUE)
                  == FALSE)
              {
                 PRINTERR1("Bad StringFloat message from client\n");
              }
          }
          break;
       }

       case UndeadClientMsg:
       {
           agent->_undead = TRUE;
           break;
       }
          
       /* If type is UpdateDisplayMsg, client is asking us to do/don't
        * update displays each time an event is routed.
        */
       case UpdateDisplayMsg:
       {
          agent->_doUpdate = (int) *msgPtr & 0x0ff;

          /* PRINTERR2("Received Update request from client of %d",
           *           agent->_doUpdate);
           */

          break;
       }

       /* If type is PollEventMsg, client wants us to
        * get the state for an event and transmit it
        * back.
        */
       case PollEventMsg: 
       {
          AltiaEventType retValue = 0;
          unsigned char msgValue[128];
          int returnMsgType = PollEventMsg;
          int len;
#ifndef UNICODE
          char *eventName  = (char *) msgPtr;
#else
          ALTIA_CHAR eventName[MAX_UNICODE_LEN];
#endif

          /* PRINTERR2("Received POLL request from client for event %s",
           *           (char *) msgPtr);
           */

          /* Get events current value.  Note that message
           * contains only the event name that needs to be
           * polled.  If event has no current value, set
           * error flag in message type field.
           */

          /* Next if statement originally requested the value from
           * the router as in:
           * 
           * if (!(_eventRouter->GetCurrentValue( (char *) msgPtr, &retValue)))
           *
           * Now it makes a call to a DeepScreen function to do this.
           */

#ifdef UNICODE
          /* For Unicode, convert 8-bit event name to wide event name */
          localCopyCharToWide((char *) msgPtr, eventName, MAX_UNICODE_LEN);
#endif

          /* TODO:  Fix DeepScreen implementation for "altiaGetViewSize".
           * If client is polling "altiaGetViewSize" animation, assume it is
           * because of an Altia API altiaGetViewSize() call.  DeepScreen
           * will not have a current value for this animation.  Instead,
           * return the last value provided by a client.
           */
          if (strcmp(msgPtr, altiaGetViewSizeString) == 0)
              retValue = (AltiaEventType) altiaGetViewSizeValue;
          else if (agent->_server->_quit)
              /* Don't call into DeepScreen if altiaQuit already detected */
              retValue = 0;
          else
              retValue = _altiaFindCurVal(eventName);

          if (0) /* Not possible to have an error when querying DeepScreen */
          {
             returnMsgType |= ErrorInMsg;
             msgValue[0]
               = msgValue[1]
               = msgValue[2]
               = msgValue[3]
               = msgValue[4]
               = msgValue[5]
               = msgValue[6]
               = msgValue[7]
               = 0;
             if (agent->_floatInterface)
             {
                 if (agent->_stringFloat)
                 {
                     msgValue[0] = 1;
                     msgValue[1] = '0';
                     len = 2;
                 }else
                     len = 8;
             }else
             {
                 len = 4;
             }
          }
          else
          {
             unsigned char *temp = msgValue;
             altiaClientAgent_setRValue(agent, (char *) msgPtr,
                                        retValue, &temp, &len);
          }

          /* PRINTERR3("\tresponding with %s(%g)",
           *           (char *) msgPtr, (double) retValue);
           */

          /* Now we want to send a message to the client. It has
           * 3 fields - type (always an int), event value
           * (4-byte array), event name (size msg_size).
           *
           * If we are to delay the client and don't have a
           * delayMess then create one to send later.  DelayMess
           * should always be NULL here but if not ignore the delay.
           */
          if (agent->_delay && agent->_delayMess == NULL)
              agent->_delayMess = 
                 altiaDelayedMessageCreate(returnMsgType,
                                           msgValue, len,
                                           msgPtr, msgSize,
                                           NULL, 0,
                                           NULL, 0);

          else
              altiaClientAgent_sendMessage(agent, returnMsgType,
                                           msgValue, len,
                                           msgPtr, msgSize,
                                           NULL, 0,
                                           NULL, 0);

          /* Clear out all instances of this event from our queue if it is
           * of interest to client.  The events in the queue are all
           * out-of-date because the user polled the event.
           */
          altiaClientAgentClearEventQueue(agent, (char *) msgPtr);

          /* 1/4/93: Added this if statement and sending of NULL message */
          if (agent->_delayMess == NULL)
          {
              if (!altiaClientQueueEmpty(agent->_eventQueue) && agent->_notify)
              {
                  /* Client program still has messages waiting.  
                   * A likely next move for the client would be to 
                   * select(2) waiting for new events.
                   * We'll send a Null message to unblock the select(2) 
                   * since we don't send events asynchronously to the client.
                   */
                  unsigned char msgBuf[2];
                  msgBuf[0] = msgBuf[1] = 0;
                  altiaCnctWriteNotify(agent->_connection, msgBuf, 2);
                  agent->_lastMsgType = NullMsg;
              }
              else
              {
                  agent->_lastMsgType = PollEventMsg;
              }
          }
          break;
       }

       /* 9/25/93: Added this new message to more easily get input label
        *          "getchar" events.
        * If type is CheckEventMsg, client wants us to
        * get the next available event that matches a particular name
        * whether or not it is at the front of the queue.  Remove it from
        * the queue as part of getting it.
        */
       case CheckEventMsg: 
       {
          AltiaEventType retValue = 0;
          unsigned char msgValue[128];
          int returnMsgType = CheckEventMsg;
          int len;

          /* PRINTERR2("Received CheckEvent request from client for event %s\n",
           *           (char *) msgPtr);
           */

          /* Get events current value.  Note that message
           * contains only the event name that needs to be
           * polled.  If event has no current value, set
           * error flag in message type field.
           */
          if (!(altiaClientAgentCheckEvent(agent, (char *) msgPtr, &retValue)))
          {
             returnMsgType |= ErrorInMsg;
             msgValue[0]
               = msgValue[1]
               = msgValue[2]
               = msgValue[3]
               = msgValue[4]
               = msgValue[5]
               = msgValue[6]
               = msgValue[7]
               = 0;
             if (agent->_floatInterface)
             {
                 if (agent->_stringFloat)
                 {
                     msgValue[0] = 1;
                     msgValue[1] = '0';
                     len = 2;
                 }else
                     len = 8;
             }else
             {
                 len = 4;
             }
          }
          else
          {
             unsigned char *temp = msgValue;
             altiaClientAgent_setRValue(agent, (char *) msgPtr,
                                        retValue, &temp, &len);
          }

          /* PRINTERR3("\tresponding with %s(%g)\n",
           *           (char *) msgPtr, (double) retValue);
           */

          /* Now we want to send a message to the client. It has
           * 3 fields - type (always an int), event value
           * (4-byte array), event name (size msg_size).
           *
           * If we are to delay the client and don't have a
           * delayMess then create one to send later.  DelayMess
           * should always be NULL here but if not ignore the delay.
           */
          if (agent->_delay && agent->_delayMess == NULL)
              agent->_delayMess = 
                 altiaDelayedMessageCreate(returnMsgType,
                                           msgValue, len,
                                           msgPtr, msgSize,
                                           NULL, 0,
                                           NULL, 0);

          else
              altiaClientAgent_sendMessage(agent, returnMsgType,
                                           msgValue, len,
                                           msgPtr, msgSize,
                                           NULL, 0,
                                           NULL, 0);

          if (agent->_delayMess == NULL)
          {
              if (!altiaClientQueueEmpty(agent->_eventQueue) && agent->_notify)
              {
                  /* Client program still has messages waiting.  
                   * A likely next move for the client would be to 
                   * select(2) waiting for new events.
                   * We'll send a Null message to unblock the select(2) 
                   * since we don't send events asynchronously to the client.
                   */
                  unsigned char msgBuf[2];
                  msgBuf[0] = msgBuf[1] = 0;
                  altiaCnctWriteNotify(agent->_connection, msgBuf, 2);
                  agent->_lastMsgType = NullMsg;
              }
              else
              {
                  agent->_lastMsgType = CheckEventMsg;
              }
          }
          break;
       }

       /* If type is SelectEventMsg, client wants us to
        * allow messages of the requested type to be queued.
        */
       case SelectEventMsg: 
       {
          /* PRINTERR2("Received Select request from client for event %s\n",
           *           (char *) msgPtr);
           */

          altiaClientAgentSelectEvent(agent, (char *) msgPtr);

          break;
       }

       /* Client wants all external events sent to it */
       case SelectExternEventMsg:
       {
          altiaClientAgentSelectExternEvent(agent);
          break;
       }

       case SyncClientsMsg:
       {
          if ((*msgPtr & 0xff) == 0)
              agent->_server->_syncClients = FALSE;
          else
              agent->_server->_syncClients = TRUE;
          break;
       }

       /* Client wants all external events not sent */
       case UnselectExternEventMsg:
       {
          altiaClientAgentUnselectExternEvent(agent);
          break;
       }

       /* If type is UnselectEventMsg, client wants us to
        * disallow messages of the requested type to be queued.
        */
       case UnselectEventMsg: 
       {
          /* PRINTERR2("Received Unselect request from client for event %s\n",
           *           (char *) msgPtr);
           */

          altiaClientAgentUnselectEvent(agent, (char *) msgPtr);

          break;
       }

       /* If type is CheckEventsMsg, client wants us to
        * send them the next sequence of events from our
        * queue up to a certain byte amount without delay.
        * If no events exist, we'll send a 0 event count.
        */
       case CheckEventsMsg: 
       {
          /* If we are to delay the client and don't have a
           * delayMess then create one to send later.  DelayMess
           * should always be NULL here but if not ignore the delay.
           */
          if (agent->_delay && agent->_delayMess == NULL)
              agent->_delayMess
                  = altiaDelayedMessageCreate(CheckEventsMsg, NULL,
                          (msgPtr[0] << 24) | (msgPtr[1] << 16)
                          | (msgPtr[2] << 8)  | msgPtr[3],
                                           NULL, 0,
                                           NULL, 0,
                                           NULL, 0);

          else
              altiaClientAgent_sendQueueBytes(agent,
                                      (msgPtr[0] << 24) | (msgPtr[1] << 16)
                                      | (msgPtr[2] << 8)  | msgPtr[3]);

          if (agent->_delayMess == NULL)
          {
              /* 1/4/93: If queue still not empty, send a NULL message. */
              if (!altiaClientQueueEmpty(agent->_eventQueue) && agent->_notify)
              {
                  /* Client program still has messages waiting. 
                   * A likely next move for the client would be to select(2) 
                   * waiting for new events.
                   * We'll send a Null message to unblock the select(2) since we
                   * don't send events asynchronously to the client.
                   */
                  unsigned char msgBuf[2];
                  msgBuf[0] = msgBuf[1] = 0;
                  altiaCnctWriteNotify(agent->_connection, msgBuf, 2);
                  agent->_lastMsgType = NullMsg;
              }
              else
              {
                  agent->_lastMsgType = CheckEventsMsg;
              }
          }
          /* If we are in syncClients mode and our queue has been reduced
           * in size below min size then remove the delay set in other clients
           */
          if (agent->_server->_syncClients && agent->_server->_syncDelayOn
              && agent->_delay == FALSE
              && altiaClientQueueSize(agent->_eventQueue) <= MINSYNCSIZE)
              altiaClientAgentRouteDelay(agent, FALSE);
          break;
       }

       /* If type is MoreEventsMsg, client wants us to
        * send them the next sequence of events from our
        * queue up to a certain byte amount.  If the queue is
        * empty, they are willing to wait until events are available.
        */
       case MoreEventsMsg: 
       {
          agent->_clientQueueSpace = (msgPtr[0] << 24) | (msgPtr[1] << 16)
                                     | (msgPtr[2] << 8) | msgPtr[3];

          /* If we are to delay the client and don't have a
           * delayMess then create one to send later.  DelayMess
           * should always be NULL here but if not ignore the delay.
           */
          if (agent->_delay && agent->_delayMess == NULL)
              agent->_delayMess = altiaDelayedMessageCreate(MoreEventsMsg,
                                              NULL, agent->_clientQueueSpace,
                                              NULL, 0,
                                              NULL, 0,
                                              NULL, 0);

          /* If there are events in queue, send them without delay.
           * Otherwise, the client behavior receiver will send them when
           * it gets an event and sees that _clientQueueSpace is non-zero.
           */
          if (agent->_delayMess == NULL)
          {
              if (!altiaClientQueueEmpty(agent->_eventQueue))
              {
                  altiaClientAgent_sendQueueBytes(agent,
                                                  agent->_clientQueueSpace);
                  agent->_clientQueueSpace = 0;

                  /* 1/4/93: If queue still not empty, send a NULL message. */
                  if (!altiaClientQueueEmpty(agent->_eventQueue)
                      && agent->_notify)
                  {
                      /* Client program still has messages waiting. Likely next
                       * move for the client would be to select(2) waiting for
                       * events. Send a Null message to unblock the select(2)
                       * since we don't send events asynchronously to clients.
                       */
                      unsigned char msgBuf[2];
                      msgBuf[0] = msgBuf[1] = 0;
                      altiaCnctWriteNotify(agent->_connection, msgBuf, 2);
                      agent->_lastMsgType = NullMsg;
                  }
                  else
                  {
                      agent->_lastMsgType = MoreEventsMsg;
                  }
              }
          }
          if (agent->_server->_syncClients && agent->_server->_syncDelayOn
              && agent->_delay == FALSE
              && altiaClientQueueSize(agent->_eventQueue) <= MINSYNCSIZE)
              altiaClientAgentRouteDelay(agent, FALSE);
          break;
       }

       /* 9/25/93: Added this new message to more easily get input label
        *          "getchar" events.
        * If type is EventSelectedMsg, client wants us to
        * confirm if we are selected to recieve events of the given type.
        */
       case EventSelectedMsg: 
       {
          unsigned char msgValue;
          int returnMsgType = EventSelectedMsg;

          /* PRINTERR2("Received EventSelected from client for event %s\n",
           *           (char *) msgPtr);
           */

          if (altiaClientAgentEventSelected(agent, (char *) msgPtr))
             msgValue = 1;
          else
             msgValue = 0;

          /* PRINTERR2("\tresponding with %d\n", msgValue[0]);
           */

          /* Now we want to send a message to the client. It has
           * 3 fields - type (always an int), flag if event is selected
           * (1 byte), event name (size msg_size).
           */
          altiaClientAgent_sendMessage(agent, returnMsgType,
                                       &msgValue, 1,
                                       msgPtr, msgSize,
                                       NULL, 0,
                                       NULL, 0);

          if (!altiaClientQueueEmpty(agent->_eventQueue) && agent->_notify)
          {
              /* Client program still has messages waiting.  A likely next move
               * for the client would be to select(2) waiting for new events.
               * We'll send a Null message to unblock the select(2) since we
               * don't send events asynchronously to the client.
               */
              unsigned char msgBuf[2];
              msgBuf[0] = msgBuf[1] = 0;
              altiaCnctWriteNotify(agent->_connection, msgBuf, 2);
              agent->_lastMsgType = NullMsg;
          }
          else
          {
              agent->_lastMsgType = EventSelectedMsg;
          }
          break;
       }

       case NullMsg: 
       {
         /* PRINTERR1("Received NULL message from client\n");
          */
         break;
       }

       default: 
       {
         PRINTERR1("Received UNKNOWN message from client\n");
         break;
       }
    }

    /* If we just handled the last message in the buffer, reestablish
     * notification via the ClientServer's io handler.  This is an
     * optimization so we don't reenter this function again only to find
     * out that the message buffer is totally empty.  We will reenter if
     * the message buffer contains a partial message - this is desirable
     * because it allows _getNextMessage() to move the partial message
     * to the head of the buffer.
     */
    if ((agent->_inputBuf + agent->_readSize + agent->_partialStart)
        == agent->_nextInputPtr)
    {
        agent->_nextInputPtr = agent->_inputBuf;
        altiaDispatcher_link(readFD, DispatcherReadMask,
                             agent->_rHandler, agent);
        return 0;
    }

    /* There are more messages in the buffer.  Continue to have us
     * immediately notified by returning a positive value.
     */
    return 1;
}

/*----------------------------------------------------------------------*/
int altiaClientAgent_getNextMessage(altiaClientAgentData *agent,
                                    unsigned char* bufPtr, int bufLen,
                                    int* typeOut, unsigned char** ptrOut,
                                    int* sizeOut, unsigned char** nextPtrInOut)
{
    unsigned int size;
    unsigned char *nextPtr = *nextPtrInOut;
    unsigned char *beyondBufPtr = bufPtr + bufLen;

    /* Need at least 2 bytes to figure out next message's size */
    if ((nextPtr + 2) > beyondBufPtr)
    {
        /* Move partial message of 0, 1, or 2 bytes to head of buffer */
        while(nextPtr < beyondBufPtr)
        {
            *bufPtr++ = *nextPtr++;
        }

        /* Adjust outgoing "next" pointer to point at, or
         * 1 or 2 bytes beyond, original start of buffer.
         */
        *nextPtrInOut = bufPtr;

        /* Set outgoing message size to zero in case caller 
         * isn't paying very good attention.
         */
        *sizeOut = 0;
    
        /* Indicate to caller that we are done getting messages */
        return FALSE;
    }
    
    /* We have at least 2 bytes, so get next message size */
    size = *nextPtr++ << 8;
    size |= *nextPtr++;

    /* Null message supported - maybe just for testing connection */
    if (!size)
    {
        *typeOut = NullMsg;
        *sizeOut = 0;
        *ptrOut = NULL;
        *nextPtrInOut = nextPtr;
        return TRUE;
    }

    /* Test if this new message would go beyond buffer limits. If
     * it would, we only have partial data for the message.
     */
    if ((nextPtr + size) > beyondBufPtr)
    {
        /* Figure out length of partial message (including size bytes) */
        int partialLen = beyondBufPtr - nextPtr + 2;

        /* Move partial message including size bytes) to head of
         * input buffer.
         */
        memcpy(bufPtr, nextPtr - 2, partialLen);

        /* Return location immediately following partial message. */
        *nextPtrInOut = bufPtr + partialLen;
        *sizeOut = 0;
        return FALSE;
    }

    /* Ok, we have a complete message. Return type, size, message pointer,
     * and pointer to next location after message.
     */
    *typeOut = *nextPtr & 0x0ff;
    *sizeOut = size - 1;
    *ptrOut = nextPtr + 1;
    *nextPtrInOut = nextPtr + size;
    return TRUE;
}

/*----------------------------------------------------------------------*/
void altiaClientAgent_sendMessage(altiaClientAgentData *agent, int type,
                               unsigned char* field1, int size1,
                               unsigned char* field2, int size2,
                               unsigned char* field3, int size3,
                               unsigned char* field4, int size4)
{

    struct iovec iov[5];
    unsigned char field0[3];

    /* Transmitted message size doesn't include size field, but does
     * include message type field (size of 1).
     */
    unsigned int size = 1 + size1 + size2 + size3 + size4;

    field0[0] = size >> 8;
    field0[1] = size;
    field0[2] = type;

    iov[0].iov_base = (char*) field0;
    iov[0].iov_len = sizeof(field0);

    if (field1 != NULL)
    {
        iov[1].iov_base = (char*) field1;
        iov[1].iov_len = size1;
    }
    else
    {
        altiaCnctWritev(agent->_connection, iov, 1);
        return;
    }

    if (field2 != NULL)
    {
        iov[2].iov_base = (char*) field2;
        iov[2].iov_len = size2;
    }
    else
    {
        altiaCnctWritev(agent->_connection, iov, 2);
        return;
    }

    if (field3 != NULL)
    {
        iov[3].iov_base = (char*) field3;
        iov[3].iov_len = size3;
    }
    else
    {
        altiaCnctWritev(agent->_connection, iov, 3);
        return;
    }

    if (field4 != NULL)
    {
        iov[4].iov_base = (char*) field4;
        iov[4].iov_len = size4;
    }
    else
    {
        altiaCnctWritev(agent->_connection, iov, 4);
        return;
    }

    altiaCnctWritev(agent->_connection, iov, 5);
    return;
}

/*----------------------------------------------------------------------*/
void altiaClientAgent_sendQueueBytes(altiaClientAgentData *agent,
                                     int availSpace)
{
    altiaClientQueueData *queuePtr;
    unsigned char *bufPtr;
    unsigned char *endPtr;
    int eventCount;
    int valSize;

    /* Adjust availSpace so we take into account size and type fields
     * added by our _sendMessage() routine.
     */
    availSpace -= 3;

     /* PRINTERR2("Sending events to client up to %d bytes\n", availSpace);
      */

    /* Of course, we can send no more than our output buffer provides. */
    if (availSpace > agent->_outputBufSize)
    {
        availSpace = agent->_outputBufSize;
    }

#ifdef WIN32
   if (altiaCnctGetReadFD(agent->_connection) != -1)
   {   /* We have a socket connection and they appear to be limited to 
        * just 1024 bytes at a time.
        */
       if (availSpace > 1000)
           availSpace = 1000;
   }
#endif

    /* Build return message. It will initially consist of an event
     * count (2 bytes).  Each event then consists of a 4 byte value,
     * a 2 byte string count (which includes a null character), and
     * the event name string itself.  An assumption is made that we
     * will never have an event name that exceeds 65,535 characters.
     */
    bufPtr = agent->_outputBuf + 2;
    endPtr = agent->_outputBuf + availSpace;
    eventCount = 0;
    if (agent->_floatInterface)
    {
        if (agent->_stringFloat)
            valSize = 34; /* 16 digits decimal pt. 16 digits + len */
        else
            valSize = 8;
    }
    else
        valSize = 4;

    while ((queuePtr
            = altiaClientQueueNext(agent->_eventQueue)) != agent->_eventQueue
           && (bufPtr + valSize  + 2 + altiaClientQueueNameLength(queuePtr))
              <= endPtr
           && eventCount <= 0x0ffff)
    {

        int rlen;
        altiaClientAgent_setRValue(agent, altiaClientQueueName(queuePtr),
                                   altiaClientQueueValue(queuePtr),
                                   &bufPtr, &rlen);

        rlen = altiaClientQueueNameLength(queuePtr);
        *bufPtr++ = rlen >> 8;
        *bufPtr++ = rlen;

        strcpy((char *) bufPtr, altiaClientQueueName(queuePtr));
        bufPtr += rlen;

        /* The queue entry has served its purpose.  We nolonger need
         * it because we are about to transmit the event to the client.
         */
        altiaClientQueueRemove(queuePtr);
        altiaClientQueueDelete(queuePtr);

        eventCount++;
    }

    /* Fill in first 2 bytes of message with event count now that we
     * have it.  Note that if queue is empty, we will send only this
     * 2 byte count, and it will be 0.
     */
    agent->_outputBuf[0] = eventCount >> 8;
    agent->_outputBuf[1] = eventCount;

    altiaClientAgent_sendMessage(agent, MoreEventsMsg,
                                 agent->_outputBuf, bufPtr - agent->_outputBuf,
                                 NULL, 0,
                                 NULL, 0,
                                 NULL, 0);
}

/*----------------------------------------------------------------------*/
void altiaClientAgentSetDelay(altiaClientAgentData *agent, int value)
{
    if (agent->_server->_syncClients)
    {   /* got a delay request from another client agent */
        if (value == 0 && agent->_delay)
        {
            /* PRINTERR2("client 0x%x delay off\n" (unsigned int) agent);
             */

            /* Our delay is done so turn off flag and sent the delay message
             */
            agent->_delay = FALSE;
            altiaClientAgentSendDelayMess(agent);
            agent->_server->_syncDelayOn = FALSE;
        }
        else if (value == 1 && agent->_delay == FALSE)
        {
            /* PRINTERR2("client 0x%x delay on\n" (unsigned int) agent);
             */
            agent->_delay = TRUE;
            agent->_server->_syncDelayOn = TRUE;
        }
    }
}

/*----------------------------------------------------------------------*/
void altiaClientAgentRouteDelay(altiaClientAgentData *agent, int value)
{
    altiaClientAgentData *clients;

    if (agent->_server->_syncDelayOn && value)
        return; 
    if (agent->_server->_syncDelayOn == FALSE
        && value == FALSE)
        return; 

    clients = agent->_server->_clientAgents;
    while (clients != NULL)
    {
        if (clients != agent)
        {
            altiaClientAgentSetDelay(clients, value);
        }
        clients = clients->_next;
    }
}

/*----------------------------------------------------------------------*/
void altiaClientAgentSendDelayMess(altiaClientAgentData *agent)
{
    if (agent->_delayMess)
    {   /* We have a delay message so sent it */
        if (agent->_delayMess->_msgType == CheckEventsMsg
            || agent->_delayMess->_msgType == MoreEventsMsg)
        {   /* We send queue bytes */
            altiaClientAgent_sendQueueBytes(agent, agent->_delayMess->_size1);
            if (agent->_delayMess->_msgType == MoreEventsMsg)
               agent->_clientQueueSpace = 0;
        }else
        {   /* We send simple reply message */
            altiaClientAgent_sendMessage(agent, agent->_delayMess->_msgType, 
                       agent->_delayMess->_field1, agent->_delayMess->_size1,
                       agent->_delayMess->_field2, agent->_delayMess->_size2,
                       agent->_delayMess->_field3, agent->_delayMess->_size3,
                       agent->_delayMess->_field4, agent->_delayMess->_size4);
        }

        /* 1/4/93: Added this if statement and sending of NULL message */
        if (!altiaClientQueueEmpty(agent->_eventQueue) && agent->_notify)
        {
            /* Client program still has messages waiting.  A likely next move
             * for the client would be to select(2) waiting for new events.
             * We'll send a Null message to unblock the select(2) since we
             * don't send events asynchronously to the client.
             */
            unsigned char msgBuf[2];
            msgBuf[0] = msgBuf[1] = 0;
            altiaCnctWriteNotify(agent->_connection, msgBuf, 2);
            agent->_lastMsgType = NullMsg;
        }
        else
        {
            agent->_lastMsgType = agent->_delayMess->_msgType;
        }

        /* int msg = agent->_delayMess->_msgType; */
        altiaDelayedMessageDelete(agent->_delayMess);
        agent->_delayMess = NULL;
    }
}

/*----------------------------------------------------------------------*/
int altiaClientAgentHandleEvent(altiaClientAgentData *agent,
                                char* eventName, AltiaEventType value)
{
    if (altiaClientAgentEventSelected(agent, eventName))
    {
        /* If we have a real client and if the client is waiting for events
         * (i.e. the queue was empty when they asked for one), send them
         * this one without delay.
         */
        if (agent->_connection && agent->_clientQueueSpace && agent->_notify)
        {
            /* For optimal performance, we won't bother with putting the event
             * in the client queue and then shipping it off.  We'll just build
             * the message and send it ourselves.  It will consist of a 2 byte
             * event count, 4/8/len byte event value, 2 byte string count (which
             * includes a null character), and the event name string itself.
             * An assumption is made that we will never have an event that
             * exceeds the size of our output buffer which is several kbytes
             * large.
             */
            unsigned char *bufPtr = agent->_outputBuf;
            int nameLength = strlen(eventName) + 1;
            int vlen;
            int size;

            *bufPtr++ = 0;
            *bufPtr++ = 1;
            altiaClientAgent_setRValue(agent, eventName, value, &bufPtr, &vlen);
            *bufPtr++ = nameLength >> 8;
            *bufPtr++ = nameLength;
            strcpy((char *) bufPtr, eventName);
            size = 4 + vlen;
            altiaClientAgent_sendMessage(agent, MoreEventsMsg,
                                         agent->_outputBuf, nameLength + size,
                                         NULL, 0,
                                         NULL, 0,
                                         NULL, 0);

            agent->_clientQueueSpace = 0;
            agent->_lastMsgType = MoreEventsMsg;

            return 0;
        }

        /* 1/4/93: Changed slightly from before.  If client isn't 
         *         specifically waiting for an event, it may be blocked
         *         in a select(2) call.  If our last transfer wasn't a
         *         Null message, then make it a Null message now so client
         *         has a chance to unblock.
         */
        if (agent->_connection && 
            agent->_lastMsgType != NullMsg && agent->_notify)
        {
            unsigned char msgBuf[2];
            msgBuf[0] = msgBuf[1] = 0;
            altiaCnctWriteNotify(agent->_connection, msgBuf, 2);

            /* Reset last message type so we only send our null message
             * once between real message transfers.
             */
            agent->_lastMsgType = NullMsg;
        }

        altiaClientQueuePrepend(agent->_eventQueue,
                                altiaClientQueueCreate(eventName, value));

        /* We just put stuff in the queue if we are syncing clients and
         * our queue is now to big and no syncDelayOn flag is set (no previous
         * sync is in effect) then lets turn one on.
         */
        if (agent->_server->_syncClients 
            && agent->_server->_syncDelayOn == FALSE
            && altiaClientQueueSize(agent->_eventQueue) > MAXSYNCSIZE)
        {
            /* PRINTERR2("client 0x%x route delay\n", (unsigned int) agent);
             */
            altiaClientAgentRouteDelay(agent, TRUE);
        }
    }

    return 0;
}


/*========================================================================
 *
 * Class altiaClientQueue implementation:
 * --------------------------------------
 * A queue of events available for the client to receive.  In this queue,
 * the first element is strictly a place holder.  It does not itself
 * contain a valid event.  This is a circular double-linked list.
 *
 *========================================================================*/

/*----------------------------------------------------------------------*/
altiaClientQueueData *altiaClientQueueCreate(
                           char *eventName, AltiaEventType eventValue)
{
    altiaClientQueueData *data
        = (altiaClientQueueData *) ALTIA_MALLOC(sizeof(altiaClientQueueData));

    /* As optimization, save string length so it can be reused without
     * having to do a strlen().
     */
    data->_nameLength = strlen(eventName) + 1;
    data->_value = eventValue;
    data->_name = (char *) ALTIA_MALLOC(data->_nameLength * sizeof(char));
    strcpy(data->_name, eventName);
    data->_next = data->_prev = data;

    return data;
}

/*----------------------------------------------------------------------*/
/* Just delete the element.  Assume that it has already been removed
 * from a list (if it was in a list) with altiaClientQueueRemove.
 */
void altiaClientQueueDelete(altiaClientQueueData *element)
{
    if (element == NULL)
        return;

    if (element->_name != NULL)
        ALTIA_FREE(element->_name);

    ALTIA_FREE(element);
}

/*----------------------------------------------------------------------*/
/* Delete the elements in the list and then the list itself */
void altiaClientQueueDeleteList(altiaClientQueueData *head)
{
    altiaClientQueueData *data;
    altiaClientQueueData *next;

    if (head == NULL)
        return;

    data = head->_next;
    while (data != head)
    {
        next = data->_next;
        altiaClientQueueDelete(data);
        data = next;
    }

    altiaClientQueueDelete(head);
}

/*----------------------------------------------------------------------*/
/* There is always at least one element in a queue that is not
 * holding an event.  If this is the only element in the queue, then
 * it is empty.
 */
int altiaClientQueueEmpty(altiaClientQueueData *head)
{
    return (head->_next == head && head->_prev == head);
}

/*----------------------------------------------------------------------*/
/* There is always at least one element in a queue that is
 * not holding an event.  Do not count this element in the size.
 */
int altiaClientQueueSize(altiaClientQueueData *head)
{
    int count = 0;
    altiaClientQueueData *next;

    if (head == NULL)
        return 0;

    for (next = head->_next; next != head; next = next->_next)
        count++;

    return count;
}

/*----------------------------------------------------------------------*/
/* Since this is a circular double-linked list, a prepend at the head is
 * a shortcut for appending to the end (very handy).
 */
void altiaClientQueuePrepend(altiaClientQueueData *head,
                             altiaClientQueueData *element)
{
    if (head == NULL)
        return;

    head->_prev->_next = element;
    element->_prev = head->_prev;
    element->_next = head;
    head->_prev = element;
}

/*----------------------------------------------------------------------*/
/* Since the head element is just a placeholder, an append at the head
 * is really an insert at the head.
 */
void altiaClientQueueAppend(altiaClientQueueData *head,
                            altiaClientQueueData *element)
{
    if (head == NULL)
        return;

    head->_next->_prev = element;
    element->_prev = head;
    element->_next = head->_next;
    head->_next = element;
}

/*----------------------------------------------------------------------*/
void altiaClientQueueRemove(altiaClientQueueData *element)
{
    element->_prev->_next = element->_next;
    element->_next->_prev = element->_prev;
    element->_next = element->_prev = element;
}

/*----------------------------------------------------------------------*/
altiaClientQueueData *altiaClientQueueNext(altiaClientQueueData *element)
{
    return element->_next;
}

/*----------------------------------------------------------------------*/
void altiaClientQueueDeleteEvents(altiaClientQueueData *head,
                                  char *eventName)
{
    altiaClientQueueData *ptr;

    if (head == NULL)
        return;

    ptr = head->_next;
    while (ptr != head)
    {
        if (strcmp(ptr->_name, eventName) == 0)
        {
            altiaClientQueueData *next = ptr->_next;
            altiaClientQueueRemove(ptr);
            altiaClientQueueDelete(ptr);
            ptr = next;
        }
        else
        {
            ptr = ptr->_next;
        }
    }
}

/*----------------------------------------------------------------------*/
int altiaClientQueueNameLength(altiaClientQueueData *element)
{
    return element->_nameLength;
}

/*----------------------------------------------------------------------*/
char *altiaClientQueueName(altiaClientQueueData *element)
{
    return element->_name;
}

/*----------------------------------------------------------------------*/
AltiaEventType altiaClientQueueValue(altiaClientQueueData *element)
{
    return element->_value;
}


/*========================================================================
 *
 * Class altiaClientFilter implementation:
 * ---------------------------------------
 * Functions to keep track of events a client has registered interest in.
 *
 *========================================================================*/

/*----------------------------------------------------------------------*/
altiaClientFilterData *altiaClientFilterCreate(void)
{
    altiaClientFilterData *data = (altiaClientFilterData *)
        ALTIA_MALLOC(sizeof(altiaClientFilterData));

    data->_currentList = altiaClientFilterListCreate();

    /* We will start by showing interest in no events - this
     * is the default for client applications.
     */
    data->_interestedInAll = 0;
    data->_interestedExtern = 0;

    return data;
}


/*----------------------------------------------------------------------*/
void altiaClientFilterDelete(altiaClientFilterData *data)
{
    altiaClientFilterListDelete(data->_currentList);
    ALTIA_FREE(data);
}

/*----------------------------------------------------------------------*/
int altiaClientFilterInteresting(altiaClientFilterData *data, char *eventName)
{
    /* If client is interested in all, then list is an ignore list and we
     * return TRUE if our name isn't in the list.  If client is ignoring 
     * all events, then list is an interest list and we return TRUE if our
     * name is in the list.  That's an exclusive-OR operation.
     */
    return data->_interestedInAll
           ^ altiaClientFilterListFindEntry(data->_currentList, eventName);
}

/*----------------------------------------------------------------------*/
void altiaClientFilterUnignoreAll(altiaClientFilterData *data)
{
    /* Clear current list and then set flag to indicate we are now
     * interested in all events.
     */
    altiaClientFilterListDeleteAllEntries(data->_currentList);
    data->_interestedInAll = 1;
}

/*----------------------------------------------------------------------*/
void altiaClientFilterIgnoreAll(altiaClientFilterData *data)
{
    /* Clear current list and then set flag to indicate we are now
     * ignoring in all events.
     */
    altiaClientFilterListDeleteAllEntries(data->_currentList);
    data->_interestedInAll = 0;
}

/*----------------------------------------------------------------------*/
void altiaClientFilterUnignore(altiaClientFilterData *data, char* eventName)
{
    if (data->_interestedInAll)
    {
        /* Already interested in all events except for a select few.
         * Remove this event from the ignore list if it exists.
         */
        altiaClientFilterListDeleteEntry(data->_currentList, eventName);
    }
    else
    {
        /* Currently ignoring all events except for a select few.
         * Add this event to the interest list.
         */
        altiaClientFilterListAddEntry(data->_currentList, eventName);
    }
}

/*----------------------------------------------------------------------*/
void altiaClientFilterIgnore(altiaClientFilterData *data, char* eventName)
{
    if (data->_interestedInAll)
    {
        /* Already interested in all events except for a select few.
         * Add this event to the ignore list if it exists.
         */
        altiaClientFilterListAddEntry(data->_currentList, eventName);
    }
    else
    {
        /* Currently ignoring all events except for a select few.
         * Delete this event from the interest list.
         */
        altiaClientFilterListDeleteEntry(data->_currentList, eventName);
    }
}

/*----------------------------------------------------------------------*/
void altiaClientFilterUnignoreExtern(altiaClientFilterData *data)
{
    data->_interestedExtern = TRUE;
}

/*----------------------------------------------------------------------*/
void altiaClientFilterIgnoreExtern(altiaClientFilterData *data)
{
    data->_interestedExtern = FALSE;
}

/*----------------------------------------------------------------------*/
int altiaClientFilterNameInList(altiaClientFilterData *data, char *eventName)
{
    return altiaClientFilterListFindEntry(data->_currentList, eventName);
}

/*----------------------------------------------------------------------*/
int altiaClientFilterInterestedInExtern(altiaClientFilterData *data)
{
    return data->_interestedExtern;
}


/*========================================================================
 *
 * Class altiaClientFilterList implementation:
 * -------------------------------------------
 * Functions to manipulate is a list of events used by the ClientFilter.
 *
 *========================================================================*/

/*----------------------------------------------------------------------*/
altiaClientFilterListData *altiaClientFilterListCreate(void)
{
    int i;
    char** next;
    altiaClientFilterListData *data = (altiaClientFilterListData *)
        ALTIA_MALLOC(sizeof(altiaClientFilterListData));

    /* Create 25 entries in lists to avoid lots of reallocs */
    data->_list = (char**) ALTIA_MALLOC(sizeof(char*) * 25);
    data->_listSize = 25;
    data->_numEntries = 0;

    /* List allows fragmentation - NULL entries can intermingle with
     * non-NULL.  Must NULL all entries now to make it work.
     */
    next = data->_list;
    for (i = 0; i < data->_listSize; i++)
        *next++ = NULL;

    return data;
}

/*----------------------------------------------------------------------*/
void altiaClientFilterListDelete(altiaClientFilterListData *data)
{
    altiaClientFilterListDeleteAllEntries(data);
    ALTIA_FREE(data->_list);
    ALTIA_FREE(data);
}
    

/*----------------------------------------------------------------------*/
void altiaClientFilterListDeleteEntry(
          altiaClientFilterListData *data, char* string)
{
    /* List is allowed to be fragmented with unused entries so
     * we must search entire list for non-NULL elements or until we
     * know we have visited all non-NULL entries.
     */
    int i;
    int entriesToVisit = data->_numEntries;
    char** next = data->_list;
    for (i = 0; (i < data->_listSize) && entriesToVisit; next++, i++)
    {
        if (*next != NULL)
        {
            if (strcmp(string, *next) == 0)
            {
                ALTIA_FREE(*next);
                *next = NULL;
                data->_numEntries--;
                return;
            }
            entriesToVisit--;
        }
    }
}

/*----------------------------------------------------------------------*/
void altiaClientFilterListDeleteAllEntries(altiaClientFilterListData *data)
{
    /* List is allowed to be fragmented with unused entries so
     * we must search entire list for non-NULL elements or until we
     * know we have visited all non-NULL entries.
     */
    int i;
    char** next = data->_list;
    for (i = 0; (i < data->_listSize) && data->_numEntries; next++, i++)
    {
        if (*next != NULL)
        {
            ALTIA_FREE(*next);
            *next = NULL;
            data->_numEntries--;
        }
    }
}

/*----------------------------------------------------------------------*/
int altiaClientFilterListFindEntry(
         altiaClientFilterListData *data, char* string)
{
    /* List is allowed to be fragmented with unused entries so
     * we must search entire list for non-NULL elements or until we
     * know we have visited all non-NULL entries.
     */
    int i;
    int entriesToVisit = data->_numEntries;
    char** next = data->_list;
    for (i = 0; (i < data->_listSize) && entriesToVisit; next++, i++)
    {
        if (*next != NULL)
        {
            if (strcmp(string, *next) == 0)
            {
                return TRUE;
            }
            entriesToVisit--;
        }
    }

    return FALSE;
}

/*----------------------------------------------------------------------*/
void altiaClientFilterListAddEntry(
          altiaClientFilterListData *data, char* string)
{
    /* List is allowed to be fragmented with unused entries so
     * we must search entire list for non-NULL elements or until we
     * know we have visited all non-NULL entries.
     */
    int i;
    char *newString;
    char** firstNilEntry = NULL;
    int entriesToVisit = data->_numEntries;
    char** next = data->_list;
    for (i = 0; i < data->_listSize; next++, i++)
    {
        if (*next != NULL)
        {
            if (strcmp(string, *next) == 0)
            {
                return;
            }
            entriesToVisit--;
        }
        else if (firstNilEntry == NULL)
        {
            firstNilEntry = next;
            if (!entriesToVisit)
                break;
        }
    }

    newString = (char *) ALTIA_MALLOC((strlen(string) + 1) * sizeof(char));
    strcpy(newString, string);

    if (firstNilEntry != NULL)
    {
        *firstNilEntry = newString;
    }
    else
    {
        data->_list = (char**) ALTIA_REALLOC((char*) data->_list,
                                    sizeof(char*) * (data->_listSize + 25));
        data->_list[data->_listSize] = newString;

        /* Need to NULL out new unused entries and set new list size. */
        next = data->_list + data->_listSize + 1;
        for (i = 0; i < 24; i++)
        {
            *next++ = NULL;
        }
        data->_listSize += 25;
    }

    data->_numEntries++;
    return;
}


/*========================================================================
 *
 * Class altiaDispatcher implementation:
 * -------------------------------------
 * Manage sockets and timers from a variable number of sources.
 *
 *========================================================================*/

#ifdef WIN32
/*==========================================================================*/
/* Need a window to process socket events, but we will never show it. */

static HWND _socketWin = NULL;
static HINSTANCE _hInstance = NULL;
static WNDCLASS _wndclass;
static int _wndclassRegistered = 0;

/*----------------------------------------------------------------------*/
/* Begin with a window procedure (i.e., handler) for the window. Cannot
 * register a window class without a window procedure and cannot make
 * a window without a window class.
 */
static long FAR PASCAL _socketWndProc(HWND hWnd,
                                      unsigned message,
                                      unsigned wParam,
                                      LONG lParam)
{
    /* When we go into a sys command (i.e., menu) we can get a socket
     * message here instead of the queue so we need to process it.
     */
    if (message == WM_SOCKET)
    {
        if (_altiaDebug > 1)
            PRINTERR4("_socketWndProc WM_SOCKET(0x%x), wParam 0x%x, lParam 0x%x\n",
                  message, wParam, lParam);
        altiaDispatcher_winMessage(wParam, lParam);
    }
    else if (_altiaDebug > 1)
        PRINTERR4("_socketWndProc message 0x%x, wParam 0x%x, lParam 0x%x\n",
                  message, wParam, lParam);

    return (DefWindowProc(hWnd, message, wParam, lParam));
}

/*----------------------------------------------------------------------*/
/* Now a function to register a window class and make a window for
 * the registered class.
 */
static void _makeSocketWindow(void)
{
    if (_socketWin != NULL)
        return;

    /* Try to create an HINSTANCE.  All indications are that this
     * is unnecessary and NULL works just fine for the HINSTANCE,
     * but do it just in case.
     */
#ifndef UNDER_CE
    if (_hInstance == NULL)
        _hInstance = (HINSTANCE) GetWindowLong(GetActiveWindow(),
                                               GWL_HINSTANCE);
#endif

    /* MUST register the window class and it MUST provide a window
     * procedure (i.e., handler) for messages from the window.
     */
    if (_wndclassRegistered == 0)
    {
#ifdef UNDER_CE
        _wndclass.style         = 0;
#else
        _wndclass.style         = CS_CLASSDC | CS_SAVEBITS;
#endif
        _wndclass.lpfnWndProc   = _socketWndProc;
        _wndclass.cbClsExtra    = 0;
        _wndclass.cbWndExtra    = 0;
        _wndclass.hInstance     = _hInstance;
        _wndclass.hIcon         = NULL;
        _wndclass.hCursor       = NULL;
        _wndclass.lpszClassName = TEXT("DSLanAPI");
        _wndclass.hbrBackground = NULL;
        _wndclass.lpszMenuName  = NULL;

        if (!RegisterClass(&_wndclass))
            return;

        _wndclassRegistered = TRUE;
    }

    /* Create the window.  Note that we will never actually show it.  It
     * only exists for receiving socket messages.  MSDN documentation
     * says that we can create a window for this purpose using
     * HWND_MESSAGE as the parent.  This may be TRUE, but the following
     * code has proven itself over many years.
     */
    _socketWin = CreateWindow(TEXT("DSLanAPI"),
                              TEXT("DSLanAPIWin"),
                              WS_POPUP,
                              0,
                              0,
                              10,
                              10,
                              HWND_DESKTOP,
                              NULL,
                              _hInstance,
                              NULL);
}

/*----------------------------------------------------------------------*/
/* Finally a function to destroy the window and then unregister the
 * class used by the window.
 */
static void _deleteSocketWindow(void)
{
    if (_socketWin != NULL)
    {
        DestroyWindow(_socketWin);
        _socketWin = NULL;
    }

    if (_wndclassRegistered)
    {
        UnregisterClass(TEXT("DSLanAPI"), _hInstance);
        _wndclassRegistered = FALSE;
    }
}

#endif /* WIN32 */


/*==========================================================================*/
/* Create and manipulate file descriptor mask data structures */

/*----------------------------------------------------------------------*/
void altiaFdMask_initialize(altiaFdMaskData *data)
{
    FD_ZERO(&(data->_theSet));
    data->_numSet = 0;
}

/*----------------------------------------------------------------------*/
altiaFdMaskData *altiaFdMask_create(void)
{
    altiaFdMaskData *data
        = (altiaFdMaskData *) ALTIA_MALLOC(sizeof(altiaFdMaskData));
    altiaFdMask_initialize(data);
    return data;
}

/*----------------------------------------------------------------------*/
void altiaFdMask_delete(altiaFdMaskData *data)
{
    ALTIA_FREE(data);
}

/*----------------------------------------------------------------------*/
fd_set *altiaFdMask_getSet(altiaFdMaskData *data)
{
    return &(data->_theSet);
}

/*----------------------------------------------------------------------*/
void altiaFdMask_zero(altiaFdMaskData *data)
{
    FD_ZERO(&(data->_theSet));
    data->_numSet = 0;
}

/*----------------------------------------------------------------------*/
#ifdef WIN32
int altiaFdMask_isSet(altiaFdMaskData *data, int fd)
{
    if (p__WSAFDIsSet != NULL)
        return (*p__WSAFDIsSet)((SOCKET)fd, (fd_set FAR *) &(data->_theSet));
    else
        return FALSE;
}
#else
int altiaFdMask_isSet(altiaFdMaskData *data, int fd)
{
    return FD_ISSET(fd, &(data->_theSet));
}
#endif /* WIN32 */

/*----------------------------------------------------------------------*/
void altiaFdMask_setBit(altiaFdMaskData *data, int fd)
{
    if (!altiaFdMask_isSet(data, fd))
    {
#ifdef WIN32
        FD_SET((unsigned int) fd, &(data->_theSet));
#else
        FD_SET(fd, &(data->_theSet));
#endif
        data->_numSet++;
    }
}

/*----------------------------------------------------------------------*/
void altiaFdMask_clrBit(altiaFdMaskData *data, int fd)
{
    if (altiaFdMask_isSet(data, fd))
    {
#ifdef WIN32
        FD_CLR((unsigned int) fd, &(data->_theSet));
#else
        FD_CLR(fd, &(data->_theSet));
#endif
        data->_numSet--;
    }
}

/*----------------------------------------------------------------------*/
int altiaFdMask_anySet(altiaFdMaskData *data)
{
    return data->_numSet != 0;
}

/*----------------------------------------------------------------------*/
int altiaFdMask_numSet(altiaFdMaskData *data)
{
    return data->_numSet;
}


/*==========================================================================*/
/* Create and manipulate io handler for a file descriptor */

/*----------------------------------------------------------------------*/
altiaIOHandlerMapData *altiaIOHandlerMapCreate(void)
{ 
    int i;
    altiaIOHandlerMapData *data
        = (altiaIOHandlerMapData *) ALTIA_MALLOC(sizeof(altiaIOHandlerMapData));

    for (i = 0; i < MAXIOFILES; i++)
    {
        data->_handler[i] = NULL;
        data->_handlerData[i] = NULL;
        data->_fd[i] = -1;
    }

    return data;
}

/*----------------------------------------------------------------------*/
void altiaIOHandlerMapDelete(altiaIOHandlerMapData *data)
{
    ALTIA_FREE(data);
}

/*----------------------------------------------------------------------*/
altiaIOHandler altiaIOHandlerMapGetHandler(altiaIOHandlerMapData *data,
                                           int fd, void **handlerData)
{
    int i;

    for (i = 0; i < MAXIOFILES; i++)
    {
        if (fd == data->_fd[i])
        {
            if (handlerData != NULL)
                *handlerData = data->_handlerData[i];
            return data->_handler[i];
        }
    }
    return NULL;
}

/*----------------------------------------------------------------------*/
void altiaIOHandlerMapSetHandler(altiaIOHandlerMapData *data, int fd,
                                 altiaIOHandler io, void *handlerData)
{
    int i;
    /* search for an existing entry */
    for (i = 0; i < MAXIOFILES; i++)
    {
        if (data->_fd[i] == fd)
        {
            data->_handler[i] = io;
            data->_handlerData[i] = handlerData;
            return;
        }
    }
    /* find a empty spot */
    for (i = 0; i < MAXIOFILES; i++)
    {
        if (data->_fd[i] == -1)
        {
            data->_fd[i] = fd;
            data->_handler[i] = io;
            data->_handlerData[i] = handlerData;
            return;
        }
    }
}

/*----------------------------------------------------------------------*/
int altiaIOHandlerMapClearHandler(altiaIOHandlerMapData *data, int fd)
{
    int i;
    for (i = 0; i < MAXIOFILES; i++)
    {
        if (data->_fd[i] == fd)
        {
            data->_fd[i] = -1;
            data->_handler[i] = NULL;
            data->_handlerData[i] = NULL;
            return TRUE;
        }
    }
    return FALSE;
}

/*----------------------------------------------------------------------*/
int altiaIOHandlerMapNextFD(altiaIOHandlerMapData *data, int *i)
{
    int j;
    for (j = *i; j < MAXIOFILES; j++)
    {
        if (data->_fd[j] != -1)
        {
            *i = j + 1;
            return data->_fd[j];
        }
    }
    return -1;
}


/*==========================================================================*/
/* Operations on timeval structures. */
static long ONE_SECOND = 1000000;

/*----------------------------------------------------------------------*/
static struct timeval addTimeVal(struct timeval src1 /* + */,
                                 struct timeval src2)
{
    struct timeval sum;
    sum.tv_sec = src1.tv_sec + src2.tv_sec;
    sum.tv_usec = src1.tv_usec + src2.tv_usec;
    if (sum.tv_usec >= ONE_SECOND)
    {
        sum.tv_usec -= ONE_SECOND;
        sum.tv_sec++;
    }
    else if (sum.tv_sec >= 1 && sum.tv_usec < 0)
    {
        sum.tv_usec += ONE_SECOND;
        sum.tv_sec--;
    }
    return sum;
}

/*----------------------------------------------------------------------*/
static struct timeval subtractTimeVal(struct timeval src1 /* - */,
                                      struct timeval src2)
{
    struct timeval delta;
    delta.tv_sec = src1.tv_sec - src2.tv_sec;
    delta.tv_usec = src1.tv_usec - src2.tv_usec;
    if (delta.tv_usec < 0)
    {
        delta.tv_usec += ONE_SECOND;
        delta.tv_sec--;
    }
    else if (delta.tv_usec >= ONE_SECOND)
    {
        delta.tv_usec -= ONE_SECOND;
        delta.tv_sec++;
    }
    return delta;
}

/*----------------------------------------------------------------------*/
static int greaterThanTimeVal(struct timeval src1 /* > */,
                              struct timeval src2)
{
    if (src1.tv_sec > src2.tv_sec)
        return TRUE;
    else if (src1.tv_sec == src2.tv_sec && src1.tv_usec > src2.tv_usec)
        return TRUE;
    return FALSE;
}

/*----------------------------------------------------------------------*/
static int greaterThanEqualTimeVal(struct timeval src1 /* >= */,
                                   struct timeval src2)
{
    if (src1.tv_sec > src2.tv_sec)
        return TRUE;
    else if (src1.tv_sec == src2.tv_sec && src1.tv_usec >= src2.tv_usec)
        return TRUE;
    return FALSE;
}

/*----------------------------------------------------------------------*/
static int lessThanTimeVal(struct timeval src1 /* < */,
                           struct timeval src2)
{
    if (src1.tv_sec < src2.tv_sec)
        return TRUE;
    else if (src1.tv_sec == src2.tv_sec && src1.tv_usec < src2.tv_usec)
        return TRUE;
    return FALSE;
}

/*----------------------------------------------------------------------*/
static int lessThanEqualTimeVal(struct timeval src1 /* <= */,
                                struct timeval src2)
{
    if (src1.tv_sec < src2.tv_sec)
        return TRUE;
    else if (src1.tv_sec == src2.tv_sec && src1.tv_usec <= src2.tv_usec)
        return TRUE;
    return FALSE;
}


/*==========================================================================*/
/* Interface to timers. */

/*----------------------------------------------------------------------*/
altiaTimerData *altiaTimerCreate(struct timeval t, altiaTimerHandler h,
                                 altiaTimerData *n, int pri)
{
    altiaTimerData *data =
        (altiaTimerData *) ALTIA_MALLOC(sizeof(altiaTimerData));
    data->timerValue = t;
    data->handler = h;
    data->next = n;
#ifdef WIN32
     data->id = 0; 
#endif
     data->mark = 0;
     if (pri >= 0)
         data->priority = pri;
     else
         data->priority = 0;

     return data;
}

/*----------------------------------------------------------------------*/
void altiaTimerDelete(altiaTimerData* data)
{
#ifdef WIN32
    if (data->id != 0)
    {
        /* Kill Timer will remove all timer events
         * with this id from the message queue.
         */
        KillTimer(NULL, data->id);
    }
#endif
    ALTIA_FREE(data);
}


/*==========================================================================*/
/* Create and manipulate a queue of timers */
static struct timeval altiaTimerQueue_zeroTime = {0,0};

/*----------------------------------------------------------------------*/
#ifdef WIN32
void altiaTimerQueue_clearTimedOut(altiaTimerQueueData *data)
{
    data->_timedout = FALSE;
}
int altiaTimerQueue_timedOut(altiaTimerQueueData *data)
{
    return data->_timedout;
}
#endif /* WIN32 */

/*----------------------------------------------------------------------*/
void altiaTimerQueue_setPriority(altiaTimerQueueData *data, int p)
{
    data->_priority = p;
}
int altiaTimerQueue_getPriority(altiaTimerQueueData *data)
{
    return data->_priority;
}

/*----------------------------------------------------------------------*/
altiaTimerQueueData *altiaTimerQueue_create(void)
{
    altiaTimerQueueData *data =
        (altiaTimerQueueData *) ALTIA_MALLOC(sizeof(altiaTimerQueueData));
    
    data->_first = NULL;
#ifdef WIN32
    data->_timedout = FALSE;
#endif
    data->_priority = 0;

    return data;
}

/*----------------------------------------------------------------------*/
void altiaTimerQueue_delete(altiaTimerQueueData *data)
{
    altiaTimerData *doomed = data->_first;
    while (doomed != NULL)
    {
        altiaTimerData* next = doomed->next;
        altiaTimerDelete(doomed);
        doomed = next;
    }
    ALTIA_FREE(data);
}

/*----------------------------------------------------------------------*/
int altiaTimerQueue_isEmpty(altiaTimerQueueData *data)
{
    return (data->_first == NULL);
}

/*----------------------------------------------------------------------*/
struct timeval altiaTimerQueue_earliestTime(altiaTimerQueueData *data)
{
    return data->_first->timerValue;
}

#ifdef WIN32
/*----------------------------------------------------------------------*/
void altiaTimerQueue_setTimer(altiaTimerQueueData *data, unsigned int time)
{
    unsigned int id;
    if (data->_first->id != 0)
    { /* We have already set a timer so kill it before we start a new one. */
        KillTimer(NULL, data->_first->id);
    }
    id = SetTimer(NULL, 0, time, NULL);
    data->_first->id = id;
}

/*----------------------------------------------------------------------*/
int altiaTimerQueue_findTimer(altiaTimerQueueData *data, unsigned int id)
{
    altiaTimerData *ptr = data->_first;
    while (ptr != NULL)
    {
        if (ptr->id == id)
        {   /* We have this timer so lets tell windows to stop it */
            KillTimer(NULL, id);
            ptr->id = 0;
            return TRUE;
        }
        ptr = ptr->next;
    }
    return FALSE;
}

#endif /* WIN32 */

/*----------------------------------------------------------------------*/
struct timeval altiaTimerQueue_currentTime(void)
{
    static struct timeval curTime;
#ifdef WIN32
    int ticks = GetTickCount();
    curTime.tv_sec = ticks / 1000;
    curTime.tv_usec = (ticks % 1000) * 1000;
#elif defined(VXWORKS)
    struct timespec tp;
    if (clock_gettime(CLOCK_REALTIME, &tp) == 0)
    {
        curTime.tv_sec = tp.tv_sec;
        curTime.tv_usec = tp.tv_nsec / 1000;
    }
    else
        curTime.tv_sec = curTime.tv_usec = 0;
#elif defined(PMAX)
    gettimeofday(&curTime);
#else
    struct timezone curZone;
    gettimeofday(&curTime, &curZone);
#endif
    return curTime;
}

/*----------------------------------------------------------------------*/
void altiaTimerQueue_insert(altiaTimerQueueData *data,
                            struct timeval futureTime,
                            altiaTimerHandler handler, int p)
{
    if (altiaTimerQueue_isEmpty(data)
        || lessThanTimeVal(futureTime, altiaTimerQueue_earliestTime(data)))
    {
        data->_first = altiaTimerCreate(futureTime, handler, data->_first, p);
    }
    else
    {
        altiaTimerData* before = data->_first;
        altiaTimerData* after = data->_first->next;
        while (after != NULL
               && greaterThanEqualTimeVal(futureTime, after->timerValue))
        {
            before = after;
            after = after->next;
        }
        before->next = altiaTimerCreate(futureTime, handler, after, p);
    }
}

/*----------------------------------------------------------------------*/
void altiaTimerQueue_remove(altiaTimerQueueData *data,
                            altiaTimerHandler handler)
{
    altiaTimerData* before = NULL;
    altiaTimerData* doomed = data->_first;
    while (doomed != NULL && doomed->handler != handler)
    {
        before = doomed;
        doomed = doomed->next;
    }
    if (doomed != NULL)
    {
        if (before == NULL)
            data->_first = doomed->next;
        else
            before->next = doomed->next;

        altiaTimerDelete(doomed);

        /* Lets see if that handler is attached to anyone else.
         * If so then delete them as well.
         */
        altiaTimerQueue_remove(data, handler);
    }
}

/*----------------------------------------------------------------------*/
/* NOTE: care must be taken when modifying this routine since a given
 * timer handler can remove and add timers at will and when we return
 * from timerExpired _first could have changed or entries added or 
 * deleted.
 * 5/16/95 - Changed so that any timers added by the expire handlers
 * will not get executed on this call.  This prevents the timers from
 * completely taking over the system.
 */
void altiaTimerQueue_expire(altiaTimerQueueData *data, struct timeval curTime)
{
    /* Go through the list and mark those timers we will execute this
     * time around
     */
    altiaTimerHandler handler;
    altiaTimerData *ptr = data->_first;
    while (ptr != NULL)
    {
        if (lessThanEqualTimeVal(ptr->timerValue , curTime))
        {
            if (ptr->priority >= data->_priority)
            {
                ptr->mark = 1;
            }
            ptr = ptr->next;
        }
        else break;
    }
    while (!altiaTimerQueue_isEmpty(data))
    {
        /* Since the list can change each time through this loop lets
         * look for the next marked timer from the beginning incase our
         * marked timers get deleted.
         */
        altiaTimerData *expired = data->_first;
        altiaTimerData *last = NULL;
        while (expired != NULL)
        {
            if (expired->mark == 1)
                break;
            else
            {
                last = expired;
                expired = expired->next;
            }
        }
        if (expired == NULL)
            break; /* No more marked timers */
        else if (expired == data->_first)
            data->_first = data->_first->next;
        else
        {   /* We are removing from the middle of the list */
            last->next = expired->next;
        }
         
        handler = expired->handler;

        /* We have to delete the expired one before we call the 
         * handler since the handler could remove it and we would
         * try and remove it again.  So lets remove it now and if
         * remove is called by the handler it will not be there!
         */
        altiaTimerDelete(expired);
        (*handler)(curTime.tv_sec, curTime.tv_usec);
#ifdef WIN32
        data->_timedout = TRUE;
#endif
    }
}


/*==========================================================================*/
/* Create and manipulate an event dispatcher for sockets and timers.
 * There is just one instance of a dispatcher in an executable space.
 */
static altiaDispatcherData *_instance = NULL;

/*----------------------------------------------------------------------*/
altiaDispatcherData *altiaDispatcher_instance(void)
{
    if (_instance == NULL)
        _instance = altiaDispatcher_create();

#ifdef WIN32
    /* On the first call, it might be the case that sockdll is not
     * initialized and this will not allow us to initialize the
     * local pointers to the socket functions needed by the dispatcher.
     * So check every time and initialize the local pointers as soon
     * as sockdll is initialized.
     */
    if (pWSAAsyncSelect == NULL && sockdll != NULL)
    {
        p__WSAFDIsSet = (P__WSAFDISSET)GetProcAddress(sockdll,
                                               PADDR_TEXT("__WSAFDIsSet"));
        pWSAAsyncSelect = (PWSAASYNCSELECT)GetProcAddress(sockdll,
                                               PADDR_TEXT("WSAAsyncSelect"));
        pWSAGetLastError = (PWSAGETLASTERROR)GetProcAddress(sockdll,
                                               PADDR_TEXT("WSAGetLastError"));
        pselect = (PSELECT)GetProcAddress(sockdll,
                                               PADDR_TEXT("select"));
        pioctlsocket = (PIOCTLSOCKET) GetProcAddress(sockdll,
                                               PADDR_TEXT("ioctlsocket"));
    }
#endif

    return _instance;
}

/*----------------------------------------------------------------------*/
altiaDispatcherData *altiaDispatcher_create(void)
{
    if (_instance != NULL)
        return _instance;

    _instance
        = (altiaDispatcherData *) ALTIA_MALLOC(sizeof(altiaDispatcherData));

    _instance->_nfds = 0;
    _instance->_anyready = FALSE;

    _instance->_rmask = altiaFdMask_create();
    _instance->_wmask = altiaFdMask_create();
    _instance->_emask = altiaFdMask_create();
    _instance->_rmaskready = altiaFdMask_create();
    _instance->_wmaskready = altiaFdMask_create();
    _instance->_emaskready = altiaFdMask_create();
    _instance->_rmasklast = altiaFdMask_create();
    _instance->_wmasklast = altiaFdMask_create();
    _instance->_emasklast = altiaFdMask_create();
    _instance->_rtable = altiaIOHandlerMapCreate();
    _instance->_wtable = altiaIOHandlerMapCreate();
    _instance->_etable = altiaIOHandlerMapCreate();
    _instance->_queue = altiaTimerQueue_create();

#ifdef WIN32
    _makeSocketWindow();
#endif
    
    return _instance;
}

/*----------------------------------------------------------------------*/
void altiaDispatcher_delete(void)
{
    if (_instance == NULL)
        return;

    altiaFdMask_delete(_instance->_rmask);
    altiaFdMask_delete(_instance->_wmask);
    altiaFdMask_delete(_instance->_emask);
    altiaFdMask_delete(_instance->_rmaskready);
    altiaFdMask_delete(_instance->_wmaskready);
    altiaFdMask_delete(_instance->_emaskready);
    altiaFdMask_delete(_instance->_rmasklast);
    altiaFdMask_delete(_instance->_wmasklast);
    altiaFdMask_delete(_instance->_emasklast);
    altiaIOHandlerMapDelete(_instance->_rtable);
    altiaIOHandlerMapDelete(_instance->_wtable);
    altiaIOHandlerMapDelete(_instance->_etable);
    altiaTimerQueue_delete(_instance->_queue);
#ifdef WIN32
    _deleteSocketWindow();
#endif /* WIN32 */

    ALTIA_FREE(_instance);
    _instance = NULL;
}

/*----------------------------------------------------------------------*/
altiaIOHandler altiaDispatcher_handler(int fd, int mask, void **handlerData)
{
    altiaIOHandler cur = NULL;

#ifdef WIN32
    if (fd == INVALID_SOCKET)
        return NULL;
#else
    if (fd < 0 || fd >= FD_SETSIZE)
        return NULL;
#endif

    if (mask == DispatcherReadMask)
        cur = altiaIOHandlerMapGetHandler(_instance->_rtable, fd, handlerData);
    else if (mask == DispatcherWriteMask)
        cur = altiaIOHandlerMapGetHandler(_instance->_wtable, fd, handlerData);
    else if (mask == DispatcherExceptMask)
        cur = altiaIOHandlerMapGetHandler(_instance->_etable, fd, handlerData);
    else
        return NULL;

    return cur;
}

/*----------------------------------------------------------------------*/
int altiaDispatcher_dispatched(int fd, int mask)
{
#ifdef WIN32
    if (fd == INVALID_SOCKET)
        return FALSE;
#else
    if (fd < 0 || fd >= FD_SETSIZE)
        return FALSE;
#endif
    if (mask == DispatcherReadMask)
        return altiaFdMask_isSet(_instance->_rmasklast, fd);
    else if (mask == DispatcherWriteMask)
        return altiaFdMask_isSet(_instance->_wmasklast, fd);
    else if (mask == DispatcherExceptMask)
        return altiaFdMask_isSet(_instance->_emasklast, fd);

    return FALSE;
}

/*----------------------------------------------------------------------*/
void altiaDispatcher_link(int fd, int mask, altiaIOHandler handler,
                          void *handlerData)
{
#ifdef WIN32
    if (fd == INVALID_SOCKET)
        return;
#else
    if (fd < 0 || fd >= FD_SETSIZE)
        return;
#endif
    altiaDispatcher_attach(fd, mask, handler, handlerData);
}

/*----------------------------------------------------------------------*/
void altiaDispatcher_unlink(int fd)
{
#ifdef WIN32
    if (fd == INVALID_SOCKET)
        return;
#else
    if (fd < 0 || fd >= FD_SETSIZE)
        return;
#endif
    altiaDispatcher_detach(fd);
}

/*----------------------------------------------------------------------*/
void altiaDispatcher_attach(int fd, int mask,
                            altiaIOHandler handler, void *handlerData)
{
    int registered = FALSE;

    if (mask == DispatcherReadMask)
    {
        altiaFdMask_setBit(_instance->_rmask, fd);
        if (altiaIOHandlerMapGetHandler(_instance->_rtable, fd, NULL) != NULL)
            registered = TRUE;
        altiaIOHandlerMapSetHandler(_instance->_rtable, fd,
                                    handler, handlerData);
    }
    else if (mask == DispatcherWriteMask)
    {
        altiaFdMask_setBit(_instance->_wmask, fd);
        if (altiaIOHandlerMapGetHandler(_instance->_wtable, fd, NULL) != NULL)
            registered = TRUE;
        altiaIOHandlerMapSetHandler(_instance->_wtable, fd,
                                    handler, handlerData);
    }
    else if (mask == DispatcherExceptMask)
    {
        altiaFdMask_setBit(_instance->_emask, fd);
        if (altiaIOHandlerMapGetHandler(_instance->_etable, fd, NULL) != NULL)
            registered = TRUE;
        altiaIOHandlerMapSetHandler(_instance->_etable, fd,
                                    handler, handlerData);
    }
    else
        return;

    if (_instance->_nfds < fd + 1)
        _instance->_nfds = fd + 1;
        
#ifdef WIN32
    if (!registered)
    {
        altiaDispatcher_UnblockSocket(fd);
    }
#endif
}

/*----------------------------------------------------------------------*/
void altiaDispatcher_detach(int fd)
{
    altiaFdMask_clrBit(_instance->_rmask, fd);
    altiaIOHandlerMapClearHandler(_instance->_rtable, fd);
    altiaFdMask_clrBit(_instance->_wmask, fd);
    altiaIOHandlerMapClearHandler(_instance->_wtable, fd);
    altiaFdMask_clrBit(_instance->_emask, fd);
    altiaIOHandlerMapClearHandler(_instance->_etable, fd);
#ifdef WIN32
    /* Tell Dll that we don't need notification for this socket */
    if (pWSAAsyncSelect != NULL && _socketWin != NULL)
        (*pWSAAsyncSelect)(fd, _socketWin, 0, 0);
#else
   /* windows does not use _nfds in its select call */
    if (_instance->_nfds == (fd+1))
    {
        while (_instance->_nfds > 0
               && !(altiaFdMask_isSet(_instance->_rmask, _instance->_nfds-1))
               && !(altiaFdMask_isSet(_instance->_wmask, _instance->_nfds-1))
               && !(altiaFdMask_isSet(_instance->_emask, _instance->_nfds-1)))
        {
            _instance->_nfds--;
        }
    }
#endif
}

/*----------------------------------------------------------------------*/
void altiaDispatcher_startTimer(long sec, long usec,
                                altiaTimerHandler handler, int p)
{
    struct timeval deltaTime;
    deltaTime.tv_sec = sec;
    deltaTime.tv_usec = usec;
    altiaTimerQueue_insert(_instance->_queue,
                         addTimeVal(altiaTimerQueue_currentTime(), deltaTime),
                         handler, p);
}

/*----------------------------------------------------------------------*/
void altiaDispatcher_stopTimer(altiaTimerHandler handler)
{
    altiaTimerQueue_remove(_instance->_queue, handler);
}

/*----------------------------------------------------------------------*/
void altiaDispatcher_setTimerPriority(int p)
{
    altiaTimerQueue_setPriority(_instance->_queue, p);
}

/*----------------------------------------------------------------------*/
int altiaDispatcher_getTimerPriority(void)
{
    return altiaTimerQueue_getPriority(_instance->_queue);
}

/*----------------------------------------------------------------------*/
int altiaDispatcher_dispatchTime(long *sec, long *usec)
{
    struct timeval howlong;
    struct timeval prevTime;
    struct timeval elapsedTime;
    int success;

    howlong.tv_sec = *sec;
    howlong.tv_usec = *usec;
    prevTime = altiaTimerQueue_currentTime();

    success = altiaDispatcher_dispatchTimeVal(&howlong);

    elapsedTime = subtractTimeVal(altiaTimerQueue_currentTime(), prevTime);
    if (greaterThanTimeVal(howlong, elapsedTime))
        howlong = subtractTimeVal(howlong, elapsedTime);
    else
        howlong = altiaTimerQueue_zeroTime; /* Used all of timeout */

    *sec = howlong.tv_sec;
    *usec = howlong.tv_usec;
    return success;
}

/*----------------------------------------------------------------------*/
int altiaDispatcher_dispatchTimeVal(struct timeval* howlong)
{
    static altiaFdMaskData rmaskret;
    static altiaFdMaskData wmaskret;
    static altiaFdMaskData emaskret;
    static int firsttime = 1;

    int nfound;

    if (firsttime)
    {
        altiaFdMask_initialize(&rmaskret);
        altiaFdMask_initialize(&wmaskret);
        altiaFdMask_initialize(&emaskret);
        firsttime = 0;
    }

    if (_instance->_anyready)
        nfound = altiaDispatcher_fillInReady(&rmaskret, &wmaskret, &emaskret);
    else
        nfound = altiaDispatcher_waitFor(&rmaskret, &wmaskret, &emaskret,
                                         howlong);

    *(_instance->_rmasklast) = rmaskret;
    *(_instance->_wmasklast) = wmaskret;
    *(_instance->_emasklast) = emaskret;

    altiaDispatcher_notify(nfound, &rmaskret, &wmaskret, &emaskret,
                           1 /* check timers */);

    return (nfound != 0);
}

/*----------------------------------------------------------------------*/
int altiaDispatcher_fillInReady(altiaFdMaskData *rmaskret,
                                altiaFdMaskData *wmaskret,
                                altiaFdMaskData *emaskret)
{
    *rmaskret = *(_instance->_rmaskready);
    *wmaskret = *(_instance->_wmaskready);
    *emaskret = *(_instance->_emaskready);
    altiaFdMask_zero(_instance->_rmaskready);
    altiaFdMask_zero(_instance->_wmaskready);
    altiaFdMask_zero(_instance->_emaskready);
    _instance->_anyready = FALSE;

    return altiaFdMask_numSet(rmaskret)
           + altiaFdMask_numSet(wmaskret)
           + altiaFdMask_numSet(emaskret);
}

/*----------------------------------------------------------------------*/
int altiaDispatcher_waitFor(altiaFdMaskData *rmaskret,
                            altiaFdMaskData *wmaskret,
                            altiaFdMaskData *emaskret,
                            struct timeval *howlong)
{
    int nfound;

    do {
        *rmaskret = *(_instance->_rmask);
        *wmaskret = *(_instance->_wmask);
        *emaskret = *(_instance->_emask);
        howlong = altiaDispatcher_calculateTimeout(howlong);

#if defined(WIN32)
        nfound = 0;

/* If Windows, but not a DS target with a WndProc (i.e., not native GDI32) */
#ifndef WIN32_GDI32
        if (pselect != NULL)
            nfound = (*pselect)(_instance->_nfds,
                                altiaFdMask_getSet(rmaskret),
                                altiaFdMask_getSet(wmaskret),
                                altiaFdMask_getSet(emaskret),
                                howlong);
#endif

#else
        nfound = select(_instance->_nfds,
                        altiaFdMask_getSet(rmaskret),
                        altiaFdMask_getSet(wmaskret),
                        altiaFdMask_getSet(emaskret),
                        howlong);
#endif

        if (_altiaDebug > 1 && nfound > 0)
        {
            PRINTERR2("\nInto altiaDispatcher_waitFor: select(max fd=%d)\n", _instance->_nfds - 1);
            PRINTERR2("\tselect() returned number of waiting descriptors %d\n", nfound);
        }

        if (nfound < 0) {
            altiaDispatcher_handleError();
        }
    } while (nfound < 0);

    return nfound;                /* Timed out or input available */
}

/*----------------------------------------------------------------------*/
void altiaDispatcher_notify(int nfound,
                            altiaFdMaskData *rmaskret,
                            altiaFdMaskData *wmaskret,
                            altiaFdMaskData *emaskret,
                            int check_timers)
{
    int i;
    int last = 0;
    altiaIOHandler handle;
    void *handleData;

    while ((nfound > 0)
           && ((i = altiaIOHandlerMapNextFD(_instance->_rtable, &last)) != -1))
    {
        if (altiaFdMask_isSet(rmaskret, i))
        {
            handle = altiaIOHandlerMapGetHandler(_instance->_rtable, i,
                                                 &handleData);
            if (handle != NULL)
            {
                int status = (*handle)(i, handleData);
                if (status < 0)
                    altiaDispatcher_detach(i);
                else if (status > 0)
                {
                    altiaFdMask_setBit(_instance->_rmaskready, i);
                    _instance->_anyready = TRUE;
                }
            }
            nfound--;
        }
    }
    last = 0;
    while ((nfound > 0)
           && ((i = altiaIOHandlerMapNextFD(_instance->_wtable, &last)) != -1))
    {
        if (altiaFdMask_isSet(wmaskret, i))
        {
            handle = altiaIOHandlerMapGetHandler(_instance->_wtable, i,
                                                 &handleData);
            if (handle != NULL)
            {
                int status = (*handle)(i, handleData);
                if (status < 0)
                    altiaDispatcher_detach(i);
                else if (status > 0)
                {
                    altiaFdMask_setBit(_instance->_wmaskready, i);
                    _instance->_anyready = TRUE;
                }
            }
            nfound--;
        }
    }
    last = 0;
    while ((nfound > 0)
           && ((i = altiaIOHandlerMapNextFD(_instance->_etable, &last)) != -1))
    {
        if (altiaFdMask_isSet(emaskret, i))
        {
            handle = altiaIOHandlerMapGetHandler(_instance->_etable, i,
                                                 &handleData);
            if (handle != NULL)
            {
                int status = (*handle)(i, handleData);
                if (status < 0)
                    altiaDispatcher_detach(i);
                else if (status > 0)
                {
                    altiaFdMask_setBit(_instance->_emaskready, i);
                    _instance->_anyready = TRUE;
                }
            }
            nfound--;
        }
    }

#ifdef WIN32
    altiaTimerQueue_clearTimedOut(_instance->_queue);
#endif
    if ((!altiaTimerQueue_isEmpty(_instance->_queue))
        && check_timers)
    {
        altiaTimerQueue_expire(_instance->_queue,
                               altiaTimerQueue_currentTime());
    }
}

#ifdef WIN32
/*----------------------------------------------------------------------*/
int altiaDispatcher_timedOut(void)
{
    return altiaTimerQueue_timedOut(_instance->_queue);
}

/*----------------------------------------------------------------------*/
int altiaDispatcher_winMessage(int fd, int type)
{
    int close = FALSE;
    altiaFdMaskData rd, wr, ex;
    int nfound;
    long sec = 0;
    long usec = 0;

    altiaFdMask_initialize(&rd);
    altiaFdMask_initialize(&wr);
    altiaFdMask_initialize(&ex);

    /* We don't want to process timer messages when we get a socket message.
     * So lets force no check of queue to be empty.  If we did process timer
     * messages and playback was on then we would just cause extra delays
     * since the timer messages have not been processed until the next
     * GetEvent call.
     */
    if (WSAGETSELECTEVENT(type) == FD_WRITE)
    {
        altiaFdMask_setBit(&wr, fd);
        altiaDispatcher_notify(1, &rd, &wr, &ex, FALSE /* no check timers */);
    }else if (WSAGETSELECTEVENT(type) == FD_CLOSE)
    {
        /* on a close treat it as a read so clientagent
         * can close the connection.
         */
        altiaFdMask_setBit(&rd, fd);
        altiaDispatcher_notify(1, &rd, &wr, &ex, FALSE /* no check timers */);
        close = TRUE;

        altiaDispatcher_detach(fd);
    }else
    {
        altiaFdMask_setBit(&rd, fd);
        altiaDispatcher_notify(1, &rd, &wr, &ex, FALSE /* no check timers */);
    }
    while (_instance->_anyready) 
    { 
        nfound = altiaDispatcher_fillInReady(&rd, &wr, &ex);
        *(_instance->_rmasklast) = rd;
        *(_instance->_wmasklast) = wr;
        *(_instance->_emasklast) = ex;
        altiaDispatcher_notify(nfound, &rd, &wr, &ex, FALSE);
    }
    return close;
}

/*----------------------------------------------------------------------*/
int altiaDispatcher_timerMessage(unsigned int id)
{
    return altiaTimerQueue_findTimer(_instance->_queue, id);
}

#endif /* WIN32 */
    

/*----------------------------------------------------------------------*/
struct timeval* altiaDispatcher_calculateTimeout(struct timeval* howlong)
{
    static struct timeval timeout;

    if (!altiaTimerQueue_isEmpty(_instance->_queue))
    {
        struct timeval curTime;

        curTime = altiaTimerQueue_currentTime();
        if (greaterThanTimeVal(
                altiaTimerQueue_earliestTime(_instance->_queue), curTime))
        {
            timeout = subtractTimeVal(
                altiaTimerQueue_earliestTime(_instance->_queue), curTime);
            if (howlong == NULL || greaterThanTimeVal(*howlong, timeout))
            {
                howlong = &timeout;
#ifdef WIN32
                altiaTimerQueue_setTimer(_instance->_queue,
                    (howlong->tv_sec * 1000) + (howlong->tv_usec / 1000));
#endif
            }
        }
        else
        {
            timeout = altiaTimerQueue_zeroTime;
            howlong = &timeout;
        }
    }
    return howlong;
}

/*----------------------------------------------------------------------*/
void altiaDispatcher_handleError(void)
{
#ifdef WIN32
    int sockError = 0;
    if (pWSAGetLastError != NULL)
    {
        sockError = (*pWSAGetLastError)();
        PRINTERR2("Dispatcher: select error %d", sockError);
    }
    else
        PRINTERR1("Socket Error functions not initialized");

    if (sockError == WSAEINTR)
        return;

    if (sockError == WSAENOTSOCK)
    {
        altiaDispatcher_checkConnections();
        return;
    }
#else
    if (errno == EINTR)
        return;

    if (errno == EBADF)
    {
        altiaDispatcher_checkConnections();
        return;
    }

    PRINTPERROR("Dispatcher: select");
#endif
    /* exit(1); */
}

/*----------------------------------------------------------------------*/
void altiaDispatcher_checkConnections(void)
{
    altiaFdMaskData rmask;
    struct timeval poll = altiaTimerQueue_zeroTime;
    int last = 0;
    int fd;

    altiaFdMask_initialize(&rmask);

    while ((fd = altiaIOHandlerMapNextFD(_instance->_rtable, &last)) != -1)
    {
        if (altiaFdMask_isSet(_instance->_rmask, fd))
        {
            altiaFdMask_setBit(&rmask, fd);
#ifdef WIN32
            if (pselect != NULL)
                if ((*pselect)(fd+1, altiaFdMask_getSet(&rmask),
                               NULL, NULL, &poll) < 0) 
#else
            if (select(fd+1,
                       altiaFdMask_getSet(&rmask), NULL, NULL, &poll) < 0) 
#endif
            {
                altiaDispatcher_detach(fd);
            }
            altiaFdMask_clrBit(&rmask, fd);
        }
    }
}

#ifdef WIN32
/*----------------------------------------------------------------------*/
int altiaDispatcher_UnblockSocket(int fd)
{
    int res;
    long emask = FD_ACCEPT | FD_CLOSE;

    if (altiaFdMask_isSet(_instance->_rmask, fd))
        emask |= FD_READ;
    if (altiaFdMask_isSet(_instance->_wmask, fd))
        emask |= FD_WRITE;

    if (pWSAAsyncSelect != NULL && _socketWin != NULL)
        res = (*pWSAAsyncSelect)(fd, _socketWin, WM_SOCKET, emask);
    else 
        res = SOCKET_ERROR;
    if (res == SOCKET_ERROR)
    {
        if (pWSAGetLastError != NULL)
            PRINTERR2("Socket Error %d", (*pWSAGetLastError)());
        else
            PRINTERR1("Socket Error functions not initialized");
        return FALSE;
    }
    return TRUE;
}

/*----------------------------------------------------------------------*/
int Dispatcher_BlockSocket(int fd)
{
    int res;

    if (pWSAAsyncSelect != NULL && _socketWin != NULL)
        res = (*pWSAAsyncSelect)(fd, _socketWin, 0, 0);
    else 
        res = SOCKET_ERROR;
    if (res != SOCKET_ERROR)
    {
        unsigned long val = 0;
        if (pioctlsocket != NULL)
            res = (*pioctlsocket)(fd, FIONBIO, &val); 
        else
            res = SOCKET_ERROR;
    }
    if (res == SOCKET_ERROR)
    {
        if (pWSAGetLastError != NULL)
            PRINTERR2("Socket Error %d", (*pWSAGetLastError)());
        else
            PRINTERR1("Socket Error functions not initialized");
        return FALSE;
    }
    else
        return TRUE;
}

#endif /* WIN32 */


/*========================================================================
 *
 * Class altiaCnct implementation:
 * ------------------------------
 * Communicate via sockets.
 *
 *========================================================================*/

/* tjw: 8/22/95: Flag for sig blocking of reads and writes */
static int _altiaSigBlockEnabled = 0;

#ifdef WIN32
PNTOHS pntohs;
PHTONS phtons;
PHTONL phtonl;
#endif

/* If we are a windows app then we need to do more than
 * just a blocking read so windows events can be processed.
 */
#ifdef WIN32
#ifdef USERWIN
#include <winbase.h>
#define WM_MYTHREADDONE (WM_APP + 1)
static void *threadBuf;
static int threadBufCnt;
static int curThread;
static int threadVal;
static HANDLE threadHandle;
#endif /* USERWIN */
#endif

#ifdef QNX
static void *threadBuf;
static int threadBufCnt;
#endif

/*----------------------------------------------------------------------*/
/* NETWORK VARIABLE CONVERSION ROUTINES FOLLOW FOR A WHILE...           */
/*----------------------------------------------------------------------*/
/*
 * Can't use the library names because
 * C header might use macros or prototype-less functions.
 */
#if defined(htons)||defined(WIN32)

/*
 * The operations are defined as macros, so we can
 * define our functions using the macros.
 */

#if AltiaFunctionPrototyping
    static u_short short_host_to_net (u_short x)
#else
    static u_short short_host_to_net (x)
    u_short x;
#endif
{
#ifdef WIN32
    return (*phtons)(x);
#else
    return htons(x);
#endif
}

#if AltiaFunctionPrototyping
    static u_long long_host_to_net (u_long x)
#else
    static u_long long_host_to_net (x)
    u_long x;
#endif
{
#ifdef WIN32
    return (*phtonl)(x);
#else
    return htonl(x);
#endif
}

#if AltiaFunctionPrototyping
    static u_short short_net_to_host (u_short x)
#else
    static u_short short_net_to_host (x)
    u_short x;
#endif
{
#ifdef WIN32
    return (*pntohs)(x);
#else
    return ntohs(x);
#endif
}

#if AltiaFunctionPrototyping
    static u_long long_net_to_host (u_long x)
#else
    static u_long long_net_to_host (x)
    u_long x;
#endif
{
    return ntohl(x);
}

#else

#if defined(vax)
/*
 * VAX Ultrix doesn't define anything yet, so add it here.
 * When Ultrix defines them, remove this #if - #endif code.
 */

extern u_short htons(), ntohs();
extern u_long htonl(), ntohl();

#endif

/*
 * The operations are defined as functions, possibly without prototypes.
 * So, we cast pointers to the functions and call indirect.
 *
 * Yuck!!
 */

typedef u_short (*_nsfunc)(u_short);
typedef u_long (*_nlfunc)(u_long);

#if AltiaFunctionPrototyping
    static u_short short_host_to_net (u_short x)
#else
    static u_short short_host_to_net (x)
    u_short x;
#endif
{
    return (*((_nsfunc)&htons))(x);
}

#if AltiaFunctionPrototyping
    static u_long long_host_to_net (u_long x)
#else
    static u_long long_host_to_net (x)
    u_long x;
#endif
{
    return (*((_nlfunc)&htonl))(x);
}

#if AltiaFunctionPrototyping
    static u_short short_net_to_host (u_short x)
#else
    static u_short short_net_to_host (x)
    u_short x;
#endif
{
    return (*((_nsfunc)&ntohs))(x);
}

#if AltiaFunctionPrototyping
    static u_long long_net_to_host (u_long x)
#else
    static u_long long_net_to_host (x)
    u_long x;
#endif
{
    return (*((_nlfunc)&ntohl))(x);
}

/*----------------------------------------------------------------------*/
/* END OF NETWORK VARIABLE CONVERSIONS ROUTINES                         */
/*----------------------------------------------------------------------*/
#endif /* htons */

/*----------------------------------------------------------------------*/
/* WIN 32S NETWORK ROUTINE INITIALIZATION...                            */
/*----------------------------------------------------------------------*/
#ifdef WIN32

static HINSTANCE sockdll = NULL;
static PWSASTARTUP pWSAStartup = NULL;
static PWSACLEANUP pWSACleanup = NULL;
static PWSAGETLASTERROR pWSAGetLastError = NULL;
static PWSAASYNCSELECT pWSAAsyncSelect = NULL;
static PACCEPT paccept = NULL;
static PBIND pbind = NULL;
static PCLOSESOCKET pclosesocket = NULL;
static PCONNECT pconnect = NULL;
static PGETHOSTBYNAME pgethostbyname = NULL;
static PGETHOSTBYADDR pgethostbyaddr = NULL;
static PINET_NTOA pinet_ntoa = NULL;
static PINET_ADDR pinet_addr = NULL;
static PGETHOSTNAME pgethostname = NULL;
static PGETSERVBYNAME pgetservbyname = NULL;
static PIOCTLSOCKET pioctlsocket = NULL;
static PLISTEN plisten = NULL;
static PRECV precv = NULL;
static PSELECT pselect = NULL;
static PSEND psend = NULL;
static PSETSOCKOPT psetsockopt = NULL;
static PSOCKET psocket = NULL;
static P__WSAFDISSET p__WSAFDIsSet = NULL;

static int altiaCnctOpenCount = 0;

#ifdef WIN16
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#endif

/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    asvr_bool altiaCnctOpenSockets(CONST CharPtr libname)
#else
    asvr_bool altiaCnctOpenSockets(libname)
    CONST CharPtr libname;
#endif
{
    WSADATA wsaData;
    WORD version;
    int err;

    altiaCnctOpenCount++;
    if (sockdll != NULL)
        return TRUE;

    sockdll = LoadLibrary(libname);
#ifdef UNDER_CE
    if (sockdll == NULL)
#else
    if (sockdll == NULL || (int)sockdll < HINSTANCE_ERROR)
#endif
    {
	altiaCnctOpenCount = 0;
        return FALSE;
    }

    pWSAStartup = (PWSASTARTUP) GetProcAddress(sockdll,
                                               PADDR_TEXT("WSAStartup"));
    pWSACleanup = (PWSACLEANUP) GetProcAddress(sockdll,
                                               PADDR_TEXT("WSACleanup"));
    pWSAGetLastError = (PWSAGETLASTERROR) GetProcAddress(sockdll, 
                                               PADDR_TEXT("WSAGetLastError"));
    pWSAAsyncSelect = (PWSAASYNCSELECT) GetProcAddress(sockdll, 
                                                PADDR_TEXT("WSAAsyncSelect"));
    paccept = (PACCEPT) GetProcAddress(sockdll, PADDR_TEXT("accept"));
    pbind = (PBIND) GetProcAddress(sockdll, PADDR_TEXT("bind"));
    pclosesocket = (PCLOSESOCKET) GetProcAddress(sockdll, 
                                                 PADDR_TEXT("closesocket"));
    pconnect = (PCONNECT) GetProcAddress(sockdll, PADDR_TEXT("connect"));
    pgethostbyname = (PGETHOSTBYNAME) GetProcAddress(sockdll, 
                                                 PADDR_TEXT("gethostbyname"));
    pgethostbyaddr = (PGETHOSTBYADDR) GetProcAddress(sockdll, 
                                                 PADDR_TEXT("gethostbyaddr"));
    pgethostname = (PGETHOSTNAME) GetProcAddress(sockdll, 
                                                 PADDR_TEXT("gethostname"));
    pgetservbyname = (PGETSERVBYNAME) GetProcAddress(sockdll, 
                                                 PADDR_TEXT("getservbyname"));
    phtons = (PHTONS) GetProcAddress(sockdll, PADDR_TEXT("htons"));
    phtonl = (PHTONL) GetProcAddress(sockdll, PADDR_TEXT("htonl"));
    pioctlsocket = (PIOCTLSOCKET) GetProcAddress(sockdll, 
                                                 PADDR_TEXT("ioctlsocket"));
    plisten = (PLISTEN) GetProcAddress(sockdll, PADDR_TEXT("listen"));
    pntohs = (PNTOHS) GetProcAddress(sockdll, PADDR_TEXT("ntohs"));
    precv = (PRECV) GetProcAddress(sockdll, PADDR_TEXT("recv"));
    pselect = (PSELECT) GetProcAddress(sockdll, PADDR_TEXT("select"));
    psend = (PSEND) GetProcAddress(sockdll, PADDR_TEXT("send"));
    psetsockopt = (PSETSOCKOPT) GetProcAddress(sockdll,
                                               PADDR_TEXT("setsockopt"));
    psocket = (PSOCKET) GetProcAddress(sockdll, PADDR_TEXT("socket"));
    pinet_ntoa = (PINET_NTOA) GetProcAddress(sockdll,
                                             PADDR_TEXT("inet_ntoa"));
    pinet_addr = (PINET_ADDR) GetProcAddress(sockdll,
                                             PADDR_TEXT("inet_addr"));
    p__WSAFDIsSet = (P__WSAFDISSET) GetProcAddress(sockdll, 
                                             PADDR_TEXT("__WSAFDIsSet"));
    version = MAKEWORD(1,1);
#ifdef _WIN32_WCE_EMULATION
    err = WSAStartup(version, &wsaData);
#else
    err = (*pWSAStartup)(version, &wsaData);
#endif
    if (err != 0)
    {
	altiaCnctOpenCount = 0;
        return FALSE;
    }
    return TRUE;
}

/*----------------------------------------------------------------------*/
void altiaCnctCloseSockets(void)
{ 
#ifdef USERWIN
    if (threadHandle != NULL)
    {
	TerminateThread(threadHandle, -1);
	CloseHandle(threadHandle);
	threadHandle = NULL;
    }
#endif /* USERWIN */

    altiaCnctOpenCount--;
    if (altiaCnctOpenCount == 0)
    {
#ifdef _WIN32_WCE_EMULATION
        WSACleanup();
#else
	(*pWSACleanup)();
#endif
	FreeLibrary(sockdll);
	sockdll = NULL;
    }
}

#endif  /* WIN32 */
/*----------------------------------------------------------------------*/
/* END WIN 32S NETWORK ROUTINE INITIALIZATION                           */
/*----------------------------------------------------------------------*/
    
/*----------------------------------------------------------------------*/
#ifndef VXWORKS
#if AltiaFunctionPrototyping
    static struct hostent* GetHostEnt(CONST char* hostname)
#else
    static struct hostent* GetHostEnt(hostname)
    CONST char* hostname;
#endif
{
#ifdef WIN32

    struct hostent *tempent = NULL;
    unsigned long address = (*pinet_addr)(hostname);
    if (address != -1)
    {
	tempent = (*pgethostbyaddr)((char *)(&address), sizeof(address), AF_INET);
    }
    if (tempent == NULL)
    {
	tempent = (*pgethostbyname)(hostname);
    }
    return tempent;
#else
    struct hostent *tempent;
    tempent = gethostbyname(hostname);
    /* tjw, 1/10/96: On SUN "#.#.#.#" is not automatically converted by
     *               gethostbyname so try doing it here if no hostent
     *               was returned.
     */
    if (tempent == NULL)
    {
        unsigned long address = inet_addr(hostname);
        if (address != -1)
            tempent = gethostbyaddr((char *)(&address), sizeof(address), AF_INET);
    }
    return tempent;
#endif
}
#endif /* VXWORKS */

/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    static u_short PortNumber(u_short p)
#else
    static u_short PortNumber(p)
    u_short p;
#endif
{
    return short_host_to_net(p);
}

/*----------------------------------------------------------------------*/
static u_long FromAddr(void)
{
    return long_host_to_net(INADDR_ANY);
}

/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    int altiaCnctGetReadFD(struct altiaCnctData *acd)
#else
    int altiaCnctGetReadFD(acd)
    struct altiaCnctData *acd;
#endif
{
    return acd->_fd;
}

/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    int altiaCnctGetWriteFD(struct altiaCnctData *acd)
#else
    int altiaCnctGetWriteFD(acd)
    struct altiaCnctData *acd;
#endif
{
    return acd->_fdWrite;
}

/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    CONST char *altiaCnctGetErrorString(struct altiaCnctData *acd)
#else
    CONST char *altiaCnctGetErrorString(acd)
    struct altiaCnctData *acd;
#endif
{
    return acd->_errorString;
}

/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    int altiaCnctWordAlign(int n)
#else
    int altiaCnctWordAlign(n)
    int n;
#endif
{
    return (n + sizeof(int) - 1) & ~(sizeof(int) - 1);
}


#ifdef NOMALLOC
#define MAXACDCOUNT 8
static int acdCount = 0;
static struct altiaCnctData acds[MAXACDCOUNT];
#endif

/*----------------------------------------------------------------------*/
struct altiaCnctData* altiaCnctNewConnect(void)
{
    struct altiaCnctData *acd;

#ifdef NOMALLOC
    if (acdCount == MAXACDCOUNT)
        return NULL;
    acd = &acds[acdCount++];
#else
    acd = (struct altiaCnctData *) malloc(sizeof(struct altiaCnctData));

#endif
    if (acd != NULL)
    {
        acd->_fd = acd->_fdWrite = -1;
        acd->_errorString = " ";
    }

    /* Assume we don't want anything to do with SIGPIPE and that our callers
     * feel the same way.  We won't bother resetting the signal on
     * destruction either.
     */
#ifndef WIN32
    signal(SIGPIPE, SIG_IGN);
#endif

    return acd;
}


/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    void altiaCnctDeleteConnect(struct altiaCnctData *acd)
#else
    void altiaCnctDeleteConnect(acd)
    struct altiaCnctData *acd;
#endif
{
    if (acd != NULL)
    {
        altiaCnctClose(acd);
#ifdef NOMALLOC
        acdCount--;
	if (acdCount < 0)
	    acdCount = 0;
#else
        free((char *) acd);
#endif
    }
}

/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    static void setsockopts(int fd)
#else
    static void setsockopts(fd)
    int fd;
#endif
{
    struct linger l;
    int one = 1;

#ifdef WIN32
    if ((*psetsockopt)(fd,SOL_SOCKET,SO_REUSEADDR,(char *) &one,sizeof(int))
         < 0)
#else
    if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(char *) &one,sizeof(int)) < 0)
#endif
    {
        if (_altiaDebug)
            PRINTPERROR("setsockopt SO_REUSEADDR failed");
    }
    l.l_onoff = 0;
    l.l_linger = 0;
#ifdef WIN32
    if ((*psetsockopt)(fd,SOL_SOCKET,SO_LINGER,(char *) &l,sizeof(l)) < 0)
#else
    if (setsockopt(fd,SOL_SOCKET,SO_LINGER,(char *) &l,sizeof(l)) < 0)
#endif
    {
        if (_altiaDebug)
            PRINTPERROR("setsockopt SO_LINGER failed");
    }
#ifdef WIN32
    // Set it so it will not delay if a write occurs while another write is
    // being processed.  Otherwise it will delay hoping for more data to
    // arrive.
    if ((*psetsockopt)(fd,IPPROTO_TCP,TCP_NODELAY,(char *) &one,sizeof(int))
         < 0)
    {
        if (_altiaDebug)
            PRINTPERROR("setsockopt TCP_NODELAY failed");
    }
#endif
}

/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    asvr_bool altiaCnctMakeSocket(struct altiaCnctData *acd,
                               struct sockaddr_in *s)
#else
    asvr_bool altiaCnctMakeSocket(acd,s)
    struct altiaCnctData *acd;
    struct sockaddr_in *s;
#endif
{
#ifndef VXWORKS
    register struct hostent* h;
#endif

    altiaCnctClose(acd);

#ifdef WIN32
    if ((acd->_fd = acd->_fdWrite = (*psocket)(acd->_domain, SOCK_STREAM, 0)) < 0)
#else
    if ((acd->_fd = acd->_fdWrite = socket(acd->_domain, SOCK_STREAM, 0)) < 0)
#endif
    {
	/* debug */
	/* PRINTERR2("socket failed %d\n", WSAGetLastError()); */
        return FALSE;
    }

#ifndef VXWORKS
    /* if ((h = GetHostEnt(acd->_name)) == NULL || h->h_addrtype != acd->_domain) */
    if ((h = GetHostEnt(acd->_name)) == NULL)
    {
	/* debug */
	/* PRINTERR3("GetHostEnt failed %d, %x\n", WSAGetLastError(), h); */
        return FALSE;
    }
#endif

    memset(s, 0, sizeof(struct sockaddr_in));
    s->sin_family = acd->_domain;
    s->sin_port = PortNumber((u_short) acd->_port);

#ifndef VXWORKS
    memcpy((char *) (&(s->sin_addr)), h->h_addr, h->h_length);
#else
    {
        unsigned long raw_addr;
	struct in_addr tmp_addr;
	raw_addr = (unsigned long) hostGetByName((char *) acd->_name);
	if (raw_addr == ERROR)
	    raw_addr = (unsigned long) inet_addr((char *) acd->_name);
	tmp_addr.s_addr = (unsigned long) raw_addr;
	s->sin_addr = tmp_addr;
    }
#endif

    setsockopts(acd->_fd);
    return TRUE;
}

/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    asvr_bool altiaCnctCreateService(struct altiaCnctData *acd,
                                  CONST char *hostname, CONST char *service)
#else
    asvr_bool altiaCnctCreateService(acd, hostname, service)
    struct altiaCnctData *acd;
    CONST char *hostname;
    CONST char *service;
#endif
{
#ifdef WIN32
    u_short temp;
#endif
#ifndef VXWORKS
    struct servent *sp;
    int service_num;
#endif
    if (acd == NULL)
	return FALSE;

#ifdef VXWORKS
    /* Always return an error - no services file available! */
    acd->_errorString = "inet services not available on VXWORKS";
    return FALSE;

#else

    /* tjw, 3/20/06: If service is a number, treat it like a socket number
     *               instead of a service.
     */
    if (service != NULL && sscanf(service, "%d", &service_num) == 1)
        return(altiaCnctCreatePort(acd, hostname, service_num));

#ifdef WIN32
    if ((sp = (*pgetservbyname)((char *) service,"tcp")) == NULL)
#else
    if ((sp = getservbyname((char *) service,"tcp")) == NULL)
#endif /* WIN32 */
    {
	acd->_errorString = "cannot find inet service";
	return FALSE;
    }
#ifdef WIN32
    /* windows getservbyname returns address in net order so change it 
     * back to host order since we will convert it in OpenService.
     */
    temp = short_net_to_host(sp->s_port);

    return(altiaCnctCreatePort(acd, hostname,temp));
#else
    return(altiaCnctCreatePort(acd, hostname,sp->s_port));
#endif /* WIN32 */
#endif /* VXWORKS */
}

/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    asvr_bool altiaCnctCreatePort(struct altiaCnctData *acd,
                               CONST char *hostname, int n)
#else
    asvr_bool altiaCnctCreatePort(acd, hostname, n)
    struct altiaCnctData *acd;
    CONST char *hostname;
    int n;
#endif
{
    struct sockaddr_in s;
#ifdef WIN32
    unsigned long val;
#elif VXWORKS
    int on;
#endif

    if (acd == NULL)
	return FALSE;

    acd->_domain = AF_INET;
    acd->_name = hostname;
    acd->_port = n;

    if (!altiaCnctMakeSocket(acd, &s))
    {
	acd->_errorString = "cannot create inet socket";
	return FALSE;
    }
    /* We want the service to listen on all addresses that are available */
    s.sin_addr.s_addr = INADDR_ANY;

#ifdef WIN32
    if ((*pbind)(acd->_fd, (struct sockaddr*) (&s), sizeof(s)) < 0)
#else
    if (bind(acd->_fd, (struct sockaddr*) (&s), sizeof(s)) < 0)
#endif
    {
	acd->_errorString = "cannot bind inet socket";
	return FALSE;
    }

#ifdef WIN32
    if ((*plisten)(acd->_fd, 5) != 0)
#else
    if (listen(acd->_fd, 5) != 0)
#endif
    {
	acd->_errorString = "cannot listen to inet socket";
	return FALSE;
    }

    /* We're going to set up this server socket in non-blocking mode so
     * that we don't block in AcceptClient() when there is no client to
     * accept.
     */
#ifdef WIN32
    val = 1;
    (*pioctlsocket)(acd->_fd, FIONBIO, &val);
#elif VXWORKS
    on = 1;
    ioctl(acd->_fd, FIONBIO, (int) &on);
#else
    fcntl(acd->_fd, F_SETFL, O_NDELAY);
#endif

    return TRUE;
}

/*----------------------------------------------------------------------*/
/*
 * Create a UNIX domain socket; return the length of the socket address.
 */
#if AltiaFunctionPrototyping
    int altiaCnctMakeLocalSocket(struct altiaCnctData *acd,
                                struct sockaddr_un *s)
#else
    int altiaCnctMakeLocalSocket(acd, s)
    struct altiaCnctData *acd;
    struct sockaddr_un *s;
#endif
{
#ifdef WIN32
    return 0;
#elif VXWORKS
    return 0;
#else
    altiaCnctClose(acd);

    if ((acd->_fd = acd->_fdWrite = socket(acd->_domain, SOCK_STREAM, 0)) < 0)
    {
        return 0;
    }

    memset(s, 0, sizeof(struct sockaddr_un));
    s->sun_family = acd->_domain;
    strncpy(s->sun_path, acd->_name, sizeof(s->sun_path) - 1);

    return sizeof(s->sun_family) + sizeof(s->sun_path);
#endif
}

/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    asvr_bool altiaCnctCreateLocal(struct altiaCnctData *acd,
                                CONST char *servname)
#else
    asvr_bool altiaCnctCreateLocal(acd, servname)
    struct altiaCnctData *acd;
    CONST char *servname;
#endif
{
#ifdef WIN32
    return FALSE;
#elif VXWORKS
    if (acd == NULL)
	return FALSE;

    /* Always return an error - no domain sockets in VXWORKS */
    acd->_errorString = "domain sockets not available in VXWORKS";
    return FALSE;
#else
    struct sockaddr_un s;
    int n;

    if (acd == NULL)
	return FALSE;

    acd->_domain = AF_UNIX;
    acd->_name = servname;
    acd->_port = -1;

    if ((n = altiaCnctMakeLocalSocket(acd, &s)) == 0)
    {
	acd->_errorString = "cannot create domain socket";
	return FALSE;
    }

    unlink(s.sun_path);
    if (bind(acd->_fd, (struct sockaddr*) (&s), n) < 0)
    {
	acd->_errorString = "cannot bind domain socket";
	return FALSE;
    }

    if (listen(acd->_fd, 5) != 0)
    {
	acd->_errorString = "cannot listen to domain socket";
	return FALSE;
    }

    /* UNIX domain sockets need to be mode 777 on 4.3 */
    chmod(s.sun_path, 0777);

    /* We're going to set up this server socket in non-blocking mode so
     * that we don't block in AcceptClient() when there is no client to
     * accept.
     */
    fcntl(acd->_fd, F_SETFL, O_NDELAY);

    return TRUE;
#endif
}

/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    asvr_bool altiaCnctOpenService(struct altiaCnctData *acd,
                                CONST char *hostname, CONST char *service)
#else
    asvr_bool altiaCnctOpenService(acd, hostname, service)
    struct altiaCnctData *acd;
    CONST char *hostname;
    CONST char *service;
#endif
{
#ifdef WIN32
    u_short temp;
#endif
#ifndef VXWORKS
    struct servent *sp;
    int service_num;
#endif

    if (acd == NULL)
	return FALSE;

#ifdef VXWORKS
    /* Always return an error - no services file available! */
    acd->_errorString = "inet services not available on VXWORKS";
    return FALSE;

#else

    /* tjw, 3/20/06: If service is a number, treat it like a socket number
     *               instead of a service.
     */
    if (service != NULL && sscanf(service, "%d", &service_num) == 1)
        return(altiaCnctOpenPort(acd, hostname, service_num));

#ifdef WIN32
    if (pgetservbyname == NULL)
        return FALSE;
    if ((sp = (*pgetservbyname)((char *) service,"tcp")) == NULL)
#else
    if ((sp = getservbyname((char *) service,"tcp")) == NULL)
#endif /* WIN32 */
    {
	acd->_errorString = "cannot find inet service";
	return FALSE;
    }
#ifdef WIN32
    /* windows getservbyname returns address in net order so change it
     * back to host order since we will convert it in OpenService.
     */
    temp = short_net_to_host(sp->s_port);

    return(altiaCnctOpenPort(acd, hostname,temp));
#else
    return(altiaCnctOpenPort(acd, hostname,sp->s_port));
#endif /* WIN32 */
#endif /* VXWORKS */
}

/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    asvr_bool altiaCnctOpenPort(struct altiaCnctData *acd,
                             CONST char *host, int n)
#else
    asvr_bool altiaCnctOpenPort(acd, host, n)
    struct altiaCnctData *acd;
    CONST char *host;
    int n;
#endif
{
    struct sockaddr_in s;
#ifdef WIN32
    unsigned long val;
#elif VXWORKS
    int on;
#endif

    if (acd == NULL)
	return FALSE;

    acd->_domain = AF_INET;
    acd->_name = host;
    acd->_port = n;

    if (!altiaCnctMakeSocket(acd, &s))
    {
	acd->_errorString = "cannot create inet socket";
	return FALSE;
    }

#ifdef WIN32
    if ((*pconnect)(acd->_fd, (struct sockaddr*) (&s), sizeof(s)) < 0)
#else
    if (connect(acd->_fd, (struct sockaddr*) (&s), sizeof(s)) < 0)
#endif
    {
	acd->_errorString = "cannot connect to inet socket";
	altiaCnctClose(acd);
	return FALSE;
    }

    /* This client-side socket is opened in blocking mode */
#ifdef WIN32
    val = 0;
    (*pioctlsocket)(acd->_fd, FIONBIO, &val);
#elif VXWORKS
    on = 0;
    ioctl(acd->_fd, FIONBIO, (int) &on);
#else
    fcntl(acd->_fd, F_SETFL, /* delay */ 0);
#endif

    return TRUE;
}

/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    asvr_bool altiaCnctOpenLocal(struct altiaCnctData *acd, CONST char *str)
#else
    asvr_bool altiaCnctOpenLocal(acd, str)
    struct altiaCnctData *acd;
    CONST char *str;
#endif
{
#ifdef WIN32
    return FALSE;
#elif VXWORKS
    if (acd == NULL)
	return FALSE;

    /* Always return an error - no domain sockets in VXWORKS */
    acd->_errorString = "domain sockets not available in VXWORKS";
    return FALSE;
#else
    struct sockaddr_un s;
    int n;

    if (acd == NULL)
	return FALSE;

    acd->_domain = AF_UNIX;
    acd->_name = str;

    if ((n = altiaCnctMakeLocalSocket(acd, &s)) == 0)
    {
	acd->_errorString = "cannot create domain socket";
	return FALSE;
    }

    if (connect(acd->_fd, (struct sockaddr*) (&s), n) < 0)
    {
	acd->_errorString = "cannot connect to domain socket";
	altiaCnctClose(acd);
	return FALSE;
    }

    /* This client-side socket is opened in blocking mode */
    fcntl(acd->_fd, F_SETFL, /* delay */ 0);

    return TRUE;
#endif
}

/*----------------------------------------------------------------------*/
/*
 * Create a pipe using mknod and return TRUE if pipe already exists or
 * mknod succeeded.
 */
#if AltiaFunctionPrototyping
    asvr_bool altiaCnctMakePipe(struct altiaCnctData *acd,
                             CONST char *fileName)
#else
    asvr_bool altiaCnctMakePipe(acd, fileName)
    struct altiaCnctData *acd;
    CONST char *fileName;
#endif
{
#ifdef WIN32
    return FALSE;
#elif VXWORKS
    if (acd == NULL)
	return FALSE;

    pipeDrv();  /* Initialize pipe device driver */
    /* Create pipe - support 200 messages in pipe or 4kbytes */
    pipeDevCreate((char *) fileName, 200, PIPSIZ);
    return TRUE;
#else
    struct stat buf;

    if (acd == NULL)
	return FALSE;

    /* Check if file already exists as a pipe. */
    if (stat(fileName, &buf) == 0)
    {
        if (buf.st_mode & S_IFIFO)
        {
            return TRUE;
        }
    }
    else if (errno == ENOENT)
    {
        /* Pipe does not exist so try to make it with read/write permission
         * for everyone.
         */
        int oldmask = umask(0);
        int retVal = mknod((char*) fileName, S_IFIFO | S_IRUSR | S_IWUSR
                                             | S_IRGRP | S_IWGRP
                                             | S_IROTH | S_IWOTH, 0);
        umask(oldmask);

        if (retVal == 0)
        {
            return TRUE;
        }
    }

    /* File exists, but not a pipe.  Or, mknod() failed to create the pipe */
    return FALSE;
#endif
}

/*----------------------------------------------------------------------*/
/*
 * Open pipe based service.  This requires a pipe for writing and one for
 * reading.  We will create the named pipes if they don't already exist.
 * The noDelay flag refers to how we want to do reading.  The pipes
 * themselves will always be open without delay.
 */
#if AltiaFunctionPrototyping
    asvr_bool altiaCnctOpenPipe(struct altiaCnctData *acd,
                             CONST char *writeName,
                             CONST char *readName, asvr_bool noDelay)
#else
    asvr_bool altiaCnctOpenPipe(acd, writeName, readName, noDelay)
    struct altiaCnctData *acd;
    CONST char *writeName;
    CONST char *readName;
    asvr_bool noDelay;
#endif
{
#ifdef WIN32
    return FALSE;
#else
    int tmpFD;

    if (acd == NULL)
	return FALSE;

    altiaCnctClose(acd);

    if (!altiaCnctMakePipe(acd, writeName))
    {
	acd->_errorString = "cannot create pipe";
	return FALSE;
    }

    if (!altiaCnctMakePipe(acd, readName))
    {
	acd->_errorString = "cannot create pipe";
    return FALSE;
    }

    /* Don't block if a reader doesn't exist for the pipe. We want
     * the open to work, however.  This can be accomplished by
     * opening the pipe for reading as well as writing and then
     * closing the open for reading.  Actual writes to the pipe
     * will be done in blocking mode.
     */
#ifndef VXWORKS
    tmpFD = open(writeName, O_RDONLY | O_NDELAY);
    acd->_fdWrite = open(writeName, O_WRONLY);
#else
    tmpFD = open(writeName, O_RDONLY | O_NDELAY, 0666);
    acd->_fdWrite = open(writeName, O_WRONLY, 0666);
#endif
    close(tmpFD);

    if (acd->_fdWrite == -1)
    {
	acd->_errorString = "cannot open pipe";
	return FALSE;
    }

    /* If user doesn't want to block on reads... */
    if (noDelay)
    {

        /* Open pipe to be used for reading without delay and in
         * non-blocking mode.
         */
#ifndef VXWORKS
        acd->_fd = open(readName, O_RDONLY | O_NDELAY);
#else
        acd->_fd = open(readName, O_RDONLY | O_NDELAY, 0666);
#endif
    }
    else
    {
        /* Open pipe to be used for reading without delay, but in
         * blocking mode.  This can be accomplished by first opening
         * the pipe for writing as well as reading, then opening it
         * for reading only and closing the first open.
         */
#ifndef VXWORKS
        tmpFD = open(readName, O_RDWR);
        acd->_fd = open(readName, O_RDONLY);
#else
        tmpFD = open(readName, O_RDWR, 0666);
        acd->_fd = open(readName, O_RDONLY, 0666);
#endif
        close(tmpFD);
    }

    if (acd->_fd == -1)
    {
	close(acd->_fdWrite);
	acd->_fdWrite = -1;

	acd->_errorString = "cannot open pipe";
	return FALSE;
    }

    return TRUE;
#endif
}

/*----------------------------------------------------------------------*/
/*
 * Accept a request by a client to connect.  Since this routine
 * is used by servers and servers usually do not want to block
 * reading from a client, the connection is made non-blocking.
 */
#if AltiaFunctionPrototyping
    struct altiaCnctData *altiaCnctAcceptClient(struct altiaCnctData *acd)
#else
    struct altiaCnctData *altiaCnctAcceptClient(acd)
    struct altiaCnctData *acd;
#endif
{
    int f;
#if defined(IBM) || defined(LINUX)
    socklen_t len;
#else
    int len;
#endif /* IBM */
    struct sockaddr_in inet;
#ifndef WIN32
#ifndef VXWORKS
    struct sockaddr_un local;
#endif
#endif
    register struct altiaCnctData *c;

    if (acd == NULL)
        return NULL;

/*  Server socket is now non-blocking - shouldn't need a select.
 *  The select was necessary before so that we wouldn't block on
 *  an accept() call when there were no clients connected.
 *  struct timeval timeout;
 *  int readfds = 1 << _fd;
 *  timeout.tv_sec = 0;
 *  timeout.tv_usec = 0;
 *
 *  if (select(_fd + 1, &readfds, 0, 0, &timeout) == -1)
 *  {
 *    PRINTPERROR("ClientConnect::AcceptClient:  select failed");
 *    return NULL;
 *  }
 *
 *  if (!readfds)
 *    return NULL;
 */

    if (acd->_domain == AF_INET)
    {
        len = sizeof(inet);
#ifdef WIN32
        f = (*paccept)(acd->_fd, (struct sockaddr*) (&inet), &len);
#else
        f = accept(acd->_fd, (struct sockaddr*) (&inet), &len);
#endif
	/* setsockopts(f); */
    }
    else
    {
#ifdef WIN32
        f = -1;
#elif VXWORKS
        f = -1;
#else
        len = sizeof(local);
        f = accept(acd->_fd, (struct sockaddr*) (&local), &len);
#endif
    }

    if (f >= 0)
    {
#ifdef WIN32
	unsigned long val = 1;
	(*pioctlsocket)(f, FIONBIO, &val);
#elif VXWORKS
	int on = 1;
	ioctl(f, FIONBIO, (int) &on);
#else
        fcntl(f, F_SETFL, O_NDELAY);
#endif
        if ((c = altiaCnctNewConnect()) != NULL)
        {
            c->_domain = acd->_domain;
            c->_name = acd->_name;
            c->_port = acd->_port;
            c->_fd = c->_fdWrite = f;
        }
    }
    else
    {
        if (_altiaDebug > 1 && acd->_domain == AF_INET)
            PRINTERR1("\tNo network client waiting to accept a connect at this time\n");
        else if (_altiaDebug > 1)
            PRINTERR1("\tNo domain  client waiting to accept a connect at this time\n");
        c = NULL;
    }

    return c;
}

/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    void altiaCnctClose(struct altiaCnctData *acd)
#else
    void altiaCnctClose(acd)
    struct altiaCnctData *acd;
#endif
{
    if (acd == NULL)
        return;

    if (acd->_fd != -1)
    {
#ifdef WIN32
        (*pclosesocket)(acd->_fd);
#else
        close(acd->_fd);
#endif
    }
    if (acd->_fdWrite != acd->_fd)
    {
#ifdef WIN32
        (*pclosesocket)(acd->_fdWrite);
#else
        close(acd->_fdWrite);
#endif
    }
    acd->_fd = acd->_fdWrite = -1;
}

/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    int altiaCnctPoll(struct altiaCnctData *acd,
                     asvr_bool mustHaveData, int timeoutSec)
#else
    int altiaCnctPoll(acd, mustHaveData, timeoutSec)
    struct altiaCnctData *acd;
    asvr_bool mustHaveData;
    int timeoutSec;
#endif
{
    int nbytes;

    if (acd == NULL)
        return 0;

    if (acd->_fd == -1)
        return 0;


    /* If caller is certain the socket has data (e.g., a previous
     * read resulted in an incomplete message so we know more data must
     * still be coming), they can skip this select by setting mustHaveData
     * to TRUE.  By default, it is passed in as FALSE (i.e. the select is
     * performed) which can never hurt anything, but it adds overhead if
     * the caller knows it is unnecessary.
     */
    if (!mustHaveData)
    {
        struct timeval timeout;

#ifdef WIN32
        struct fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET((SOCKET) acd->_fd, &readfds);
#else
        int readfds = 1<< acd->_fd;
#endif

        timeout.tv_sec = timeoutSec;
        timeout.tv_usec = 0;

#if defined(hpux)
        if (select(acd->_fd + 1, &readfds, 0, 0, &timeout) < 0)
#elif !defined(WIN32)
        /* tjw, 11/5/91:On SUN and UNIX likes,mask params are (fd_set*) */
        if (select(acd->_fd + 1, (fd_set *) &readfds, 0, 0, &timeout) < 0)
#else
        if ((*pselect)(acd->_fd + 1, (fd_set *) &readfds, 0, 0, &timeout) < 0)
#endif
        {
	    if (_altiaDebug)
#ifdef WIN32
                PRINTERR2("altiaCnctPoll:  select failed error: %d",
                          (*pWSAGetLastError)());
#else
                PRINTPERROR("altiaCnctPoll:  select failed");
#endif
            return -1;
        }

#ifdef WIN32
        if (FD_ISSET(acd->_fd, &readfds) == 0)
#else
        if (!readfds)
#endif
        {
            return 0;
        }
    }

    /* We got through select. For a pipe,  always just return the
     * maximum available from a pipe.  The fact that we do a non-blocking
     * read saves our butts.
     */
    if (acd->_fd != acd->_fdWrite)
    {
#ifdef WIN32
        return 0;
#else
        return PIPSIZ;
#endif
    }

#ifdef WIN32
    if ((*pioctlsocket)(acd->_fd, FIONREAD, (unsigned long *)&nbytes) < 0)
#elif VXWORKS
    if (ioctl(acd->_fd, FIONREAD, (int) &nbytes) < 0)
#else
    if (ioctl(acd->_fd, FIONREAD, &nbytes) < 0)
#endif
    {
	if (_altiaDebug)
            PRINTPERROR("altiaCnctPoll:  ioctl failed");
        return -1;
    }

    /* We got through select, but there are actually no bytes
     * available.  This indicates the client has disconnected.
     */
    if (!nbytes)
    {
        return -1;
    }
    
    return nbytes;
}

#ifndef UNDER_CE
/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    int altiaCnctWritePad(struct altiaCnctData *acd,
                         CONST void* msg, int n, int padto)
#else
    int altiaCnctWritePad(acd, msg, n, padto)
    struct altiaCnctData *acd;
    CONST void* msg;
    int n;
    int padto;
#endif
{
    static int zero[32];
    register int r;
    register int left;

    if (acd == NULL)
        return -1;

#ifdef VXWORKS
    r = write(acd->_fdWrite, (char*) msg, n);
#elif sun
    /* tjw 11/5/91:  On SUN, pointer is (CONST char*) */
    r = write(acd->_fdWrite, (CONST char*) msg, n);
#else
    r = write(acd->_fdWrite, msg, n);
#endif

    left = padto - n;
    while (left > sizeof(zero))
    {
#ifdef VXWORKS
        r += write(acd->_fdWrite, (char*) zero, sizeof(zero));
#elif sun
        /* tjw 11/5/91:  On SUN, pointer is (CONST char*) */
        r += write(acd->_fdWrite, (CONST char*) zero, sizeof(zero));
#else
        r += write(acd->_fdWrite, zero, sizeof(zero));
#endif
        left -= sizeof(zero);
    }

#ifdef VXWORKS
    return r + write(acd->_fdWrite, (char*) zero, left);
#elif sun
    /* tjw 11/5/91:  On SUN, pointer is (CONST char*) */
    return r + write(acd->_fdWrite, (CONST char*) zero, left);
#else
    return r + write(acd->_fdWrite, zero, left);
#endif
}
#endif /* UNDER_CE */


#ifdef WIN32
#ifdef USERWIN
/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    DWORD WINAPI _altiaReadThread(LPVOID parm)
#else
    int _altiaReadThread(parm)
    LPVOID parm;
#endif
{
    DWORD retVal;
    struct altiaCnctData *acd = (struct altiaCnctData *)parm;
    retVal = (*precv)(acd->_fd, (char*)threadBuf, threadBufCnt, 0);
    PostThreadMessage(curThread, WM_MYTHREADDONE, 0, retVal);
    threadVal = retVal;
    return retVal;
}
#endif /* USERWIN */
#endif /* WIN32 */

#ifdef QNX
#if AltiaFunctionPrototyping
static int qnx_read(int fd, void *data, unsigned mode)
#else
static int qnx_read(fd, data, mode)
int fd;
void *data;
unsigned mode;
#endif
{
#ifdef USERWIN
    PtModalCtrl_t *ctl = (PtModalCtrl_t*)data;
#endif
    int retVal;
    while ((retVal = read(fd, threadBuf, threadBufCnt)) < 0)
    {
        if (errno != EINTR)
            break;
    }
#ifdef USERWIN
    PtModalUnblock(ctl, (void*)retVal);
    return Pt_END;
#else
    return 0;
#endif
}
#endif

/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    int altiaCnctRead(struct altiaCnctData *acd, void *buffer, int count)
#else
    int altiaCnctRead(acd, buffer, count)
    struct altiaCnctData *acd;
    void *buffer;
    int count;
#endif
{
    int retVal;
#ifdef WIN32
#ifdef USERWIN
    DWORD threadId;
    DWORD res;
    asvr_bool gotThreadMess = FALSE;
    MSG msg;
    MSG tempmsg;
#endif /* USERWIN */
#endif /* WIN32 */

    if (acd == NULL)
        return -1;

    /* tjw 8/22/95: If application wants to block signals during
     *              critical sections, do it.  Then unblock them
     *              after the read call.  The flag tested is set by
     *              calling altiaEnableSigBlock().
     *
     *              Also, added retry if read call fails do to EINTR
     *              so we have better recovery when signals are not
     *              blocked.
     *
     *              Finally, added testing of debug flag so we can
     *              print error messages.  Flag comes from vseapp.c.
     */

#ifdef WIN32
#ifdef USERWIN
    /* See if we have data if we do just go get it else create a thread
     * that will block while we process window messages.
     */
    if (altiaCnctPoll(acd, FALSE, 0) > 0)
    {
	retVal = (*precv)(acd->_fd, (char*)buffer, count, 0);
	return retVal;
    }
    threadBuf = buffer;
    threadBufCnt = count;
    curThread = GetCurrentThreadId();
    threadHandle = CreateThread(NULL, 0, 
                        (LPTHREAD_START_ROUTINE)_altiaReadThread, 
			acd, 0, &threadId);
    if (threadHandle == NULL)
        return -1;
    threadVal = 0;
    /* We wait until the thread signals that the read is done */
    while (GetMessage(&msg, NULL, 0, 0))
    {
	if (msg.message == WM_MYTHREADDONE)
	{
	    gotThreadMess = TRUE;
	    retVal = msg.lParam;
	    break;
	}
	TranslateMessage(&msg);
	DispatchMessage(&msg);
	/* If we have a message in the queue the wince emulator windows
	 * code might remove it from the queue without telling us.
	 * So lets check for it now. By seeing if the read thread got
	 * a read.  If it did then it set threadVal and we can
	 * remove it from the message queue and continue.
	 * This code should not be required, but it works.
	 */
	if (threadVal)
	{
	    gotThreadMess = TRUE;
	    /* If the message did not get thrown away then we better remove
	     * it.
	     */
	    if (PeekMessage(&tempmsg, NULL, WM_MYTHREADDONE, WM_MYTHREADDONE,
	                PM_REMOVE))
	    {
		retVal = tempmsg.lParam;
	    }else
		retVal = threadVal;
	    break;
	}
    }
    if (gotThreadMess == FALSE)
    { // We did not get a thread mess so we got a quit
	TerminateThread(threadHandle, -1);
	CloseHandle(threadHandle);
	threadHandle = NULL;
	return -1;
    }
    CloseHandle(threadHandle);
    threadHandle = NULL;
#else
    retVal = (*precv)(acd->_fd, (char*) buffer, count, 0);
#endif /* USERWIN */
#elif defined(QNX)
#ifdef USERWIN
    /* We need to get windows events while we wait */
    {
	int esize = 1024;
	PtModalCtrl_t ctrl;
	PhEvent_t event[1024];
	threadBuf = buffer;
	threadBufCnt = count;
        if (PtAppAddFd(NULL, acd->_fd, Pt_FD_READ,
                       qnx_read, &ctrl)) 
        {
            if (_altiaDebug)
		PRINTPERROR("Could not add fd");
            return -1;
        }
        return PtModalBlock(&ctrl, (void*)Pt_EVENT_PROCESS_ALLOW);

    }
#else
    /* If executing standalone, it is OK to just block. */
    return read (acd->_fd, buffer, count);
#endif /* USERWIN for QNX */
#else
    if (_altiaSigBlockEnabled)
        altiaBlockSignals(1);
#if defined(sun) || defined(VXWORKS)
    while ((retVal = read(acd->_fd, (char*) buffer, count)) < 0)
#else
    while ((retVal = read(acd->_fd, buffer, count)) < 0)
#endif
    {

        if (_altiaDebug)
        {
            PRINTPERROR("Evaluating read retry");
        }

        if (errno != EINTR)
            break;
    }

    if (_altiaSigBlockEnabled)
        altiaBlockSignals(0);

#endif /* WIN32 */

    return retVal;
}

/*----------------------------------------------------------------------*/
/* tjw 8/22/95:  Created this routine to add read retry if interrupted by
 *               a signal using pipes.
 */
#if AltiaFunctionPrototyping
    int altiaPipeRead(int fd, void *buffer, int count)
#else
    int altiaPipeRead(fd, buffer, count)
    int fd;
    void *buffer;
    int count;
#endif
{
    int retVal;

    /* tjw 8/22/95: If application wants to block signals during
     *              critical sections, do it.  Then unblock them
     *              after the read call.  The flag tested is set by
     *              calling altiaEnableSigBlock().
     *
     *              Also, added retry if read call fails do to EINTR
     *              so we have better recovery when signals are not
     *              blocked.
     *
     *              Finally, added testing of debug flag so we can
     *              print error messages.  Flag comes from vseapp.c.
     */

#ifdef WIN32
    /* Our Win 32 library doesn't do pipes at this point. */
    retVal = -1;
#else
    if (_altiaSigBlockEnabled)
        altiaBlockSignals(1);
#if defined(VXWORKS) || defined(sun)
    while ((retVal = read(fd, (char*) buffer, count)) < 0)
#else
    while ((retVal = read(fd, buffer, count)) < 0)
#endif
    {
        if (_altiaDebug)
        {
            PRINTPERROR("Evaluating read retry");
        }
        if (errno != EINTR)
            break;
    }

    if (_altiaSigBlockEnabled)
        altiaBlockSignals(0);

#endif /* WIN32 */

    return retVal;
}

/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    int altiaCnctWrite(struct altiaCnctData *acd,
                      CONST void* buffer, int count)
#else
    int altiaCnctWrite(acd, buffer, count)
    struct altiaCnctData *acd;
    CONST void* buffer;
    int count;
#endif
{
    int retVal;

    if (acd == NULL)
        return -1;

    /* tjw 8/22/95: If application wants to block signals during
     *              critical sections, do it.  Then unblock them
     *              after the write call.  The flag tested is set by
     *              calling altiaEnableSigBlock().
     *
     *              Also, added retry if write call fails do to EINTR
     *              so we have better recovery when signals are not
     *              blocked.
     *
     *              Finally, added testing of debug flag so we can
     *              print error messages.  Flag comes from vseapp.c.
     */

#ifdef WIN32
    retVal = (*psend)(acd->_fdWrite, (char*) buffer, count, 0);
#else
    if (_altiaSigBlockEnabled)
        altiaBlockSignals(1);
#if defined (sun) || defined(VXWORKS)
    while ((retVal = write(acd->_fdWrite, (char*) buffer, count)) < 0)
#else
    while ((retVal = write(acd->_fdWrite, buffer, count)) < 0)
#endif
    {
        if (_altiaDebug)
        {
            PRINTPERROR("Evaluating write retry");
        }
        if (errno != EINTR)
            break;
    }

    if (_altiaSigBlockEnabled)
        altiaBlockSignals(0);

#endif /* WIN32 */

    return retVal;
}

/*----------------------------------------------------------------------*/
/* tjw 8/22/95:  Created this routine to add write retry if interrupted by
 *               a signal using pipes.
 */
#if AltiaFunctionPrototyping
    int altiaPipeWrite(int fd, CONST void* buffer, int count)
#else
    int altiaPipeWrite(fd, buffer, count)
    int fd;
    CONST void* buffer;
    int count;
#endif
{
    int retVal;

#ifdef WIN32
    /* Our Win 32 library doesn't do pipes at this point. */
    retVal = -1;
#else
    if (_altiaSigBlockEnabled)
        altiaBlockSignals(1);
#if defined(sun) || defined(VXWORKS)
    while ((retVal = write(fd, (char*) buffer, count)) < 0)
#else
    while ((retVal = write(fd, buffer, count)) < 0)
#endif
    {
        if (_altiaDebug)
        {
            PRINTPERROR("Evaluating write retry");
        }
        if (errno != EINTR)
            break;
    }

    if (_altiaSigBlockEnabled)
        altiaBlockSignals(0);

#endif /* WIN32 */

    return retVal;
}

/*----------------------------------------------------------------------*/
#if AltiaFunctionPrototyping
    int altiaCnctWriteNotify(struct altiaCnctData *acd,
                            CONST void* buffer, int count)
#else
    int altiaCnctWriteNotify(acd, buffer, count)
    struct altiaCnctData *acd;
    CONST void* buffer;
    int count;
#endif
{
    /* Default is to just write the data */
    return altiaCnctWrite(acd, buffer, count);
}

/*----------------------------------------------------------------------*/
#define ARRSIZE 1024
#if AltiaFunctionPrototyping
    int altiaCnctWritev(struct altiaCnctData *acd,
                       struct iovec *iovBuffer, int iovCount)
#else
    int altiaCnctWritev(acd, iovBuffer, iovCount)
    struct altiaCnctData *acd;
    struct iovec *iovBuffer;
    int iovCount;
#endif
#ifdef WIN32
    {
        int i;
	int val;
	int res = 0;
	char arr[ARRSIZE];
	int arrcount = 0;

    if (acd == NULL)
        return SOCKET_ERROR;

	for (i = 0; i < iovCount; i++)
	{

	    if (iovBuffer[i].iov_len < (ARRSIZE - arrcount))
	    { /* put it in buffer to send all at once */
	        memcpy(&arr[arrcount], iovBuffer[i].iov_base, 
		        iovBuffer[i].iov_len);
	        arrcount += iovBuffer[i].iov_len;
	    }else
	    {/* could not fit into buffer */
	        if (arrcount > 0)
		{
		    val = (*psend)(acd->_fdWrite, arr, arrcount, 0);
		    if (val == SOCKET_ERROR)
		        return val;
		    else
		        res += val;
		}
		val = (*psend)(acd->_fdWrite, iovBuffer[i].iov_base, 
		           iovBuffer[i].iov_len, 0);
		if (val == SOCKET_ERROR)
		    return val;
		else
		    res += val;
            }
	}
	if (arrcount > 0)
	{
	    val = (*psend)(acd->_fdWrite, arr, arrcount, 0);
	    if (val == SOCKET_ERROR)
		return val;
	    else
		res += val;
	}
	return res;
    }
#else
    {
        int retVal;

        if (acd == NULL)
            return -1;

        /* tjw 8/22/95: If application wants to block signals during
         *              critical sections, do it.  Then unblock them
         *              after the write call.  The flag tested is set by
         *              calling altiaEnableSigBlock().
         *
         *              Also, added retry if write call fails do to EINTR
         *              so we have better recovery when signals are not
         *              blocked.
         *
         *              Finally, added testing of debug flag so we can
         *              print error messages.  Flag comes from vseapp.c.
         */

        if (_altiaSigBlockEnabled)
            altiaBlockSignals(1);

        while ((retVal = writev(acd->_fdWrite, iovBuffer, iovCount)) < 0)
        {
            if (_altiaDebug)
            {
                PRINTPERROR("Evaluating write retry");
            }
            if (errno != EINTR)
                break;
        }

        if (_altiaSigBlockEnabled)
            altiaBlockSignals(0);

        return retVal;
    }
#endif /* WIN32 */

/*----------------------------------------------------------------------*/
/* tjw 8/22/95:  Created this routine to allow blocking of signals around
 *               critical code sections.
 */
#if AltiaFunctionPrototyping
    void altiaEnableSigBlock(int flag)
#else
    void altiaEnableSigBlock(flag)
    int flag;
#endif
{
    _altiaSigBlockEnabled = flag;
}

/*----------------------------------------------------------------------*/
/*
 * tjw 8/22/95:  Moved this routine from vseapp.c because it is very
 *               OS sensitive and this file is very OS sensitive as well.
 * Function: Permanently block or unblock signals.
 *           Most applications shouldn't need this, but here is the
 *           example of the code if it becomes necessary.
 */
#if defined(GNUSOLARIS) || defined(SOL) || defined(__GNUC__)
sigset_t _sigSetIn;
sigset_t _sigSetOut;
#else
static long _sigBlockMask = 0;
#endif
static int  _signalsBlocked = 0;

#if AltiaFunctionPrototyping
    void altiaBlockSignals(int flag)
#else
    void altiaBlockSignals(flag)
    int flag;
#endif
{
#ifdef WIN32
    return;
#else
    if (flag)
    {
        if (!_signalsBlocked)
        {
            _signalsBlocked = 1;
#ifndef ALTIANOSIGBLOCK
#if defined(GNUSOLARIS) || defined(SOL) || defined(__GNUC__)
            sigfillset(&_sigSetIn);
            sigprocmask(SIG_SETMASK, &_sigSetIn, &_sigSetOut);
#else
#if defined(sun) || defined(VXWORKS)
            _sigBlockMask = sigblock(~(0));
#else
            _sigBlockMask = sigblock(~(0L));
#endif /* sun || VXWORKS */
#endif /* GNUSOLARIS || SOLARIS || __GNUC__ */
#endif /* ALTIANOSIGBLOCK */
        }
    }
    else if (_signalsBlocked)
    {
#ifndef ALTIANOSIGBLOCK
#if defined(GNUSOLARIS) || defined(SOL) || defined(__GNUC__)
            sigprocmask(SIG_SETMASK, &_sigSetOut, NULL);
#else
#if defined(sun) || defined(VXWORKS)
        sigsetmask((int) _sigBlockMask);
#else
        sigsetmask(_sigBlockMask);
#endif /* sun || VXWORKS */
#endif /* GNUSOLARIS || SOLARIS || __GNUC__ */
#endif /* ALTIANOSIGBLOCK */
        _signalsBlocked = 0;
    }
#endif
}
