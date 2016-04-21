/* FILE        altia.h
 *
 * VERSION      Release 11.2
 *
 * LANGUAGE     C or C++
 *
 * AUTHOR       Altia, Incorporated
 *              Copyright 1991-2015 by Altia, Incorporated
 *
 *              Contains declarations/descriptions for functions
 *              provided by Altia client application library.
 *
 *              !!!!!!!!!!!!!!!!!!!!!WARNING!!!!!!!!!!!!!!!!!!!!!!!!
 *              The functions described in this file may use signal(2).
 *              Specifically, for host applications, SIGPIPE is permanently
 *              disabled with the call signal(SIGPIPE, SIG_IGN).
 *              This may be incompatible with code that uses sigvector(2),
 *              or other signal handling routines.  See the manual pages
 *              for signal(2), sigvector(2), etc for details.
 *
 * ONELINER     Altia client application library function declarations
 *
 *
 * (C/C++ GLOBAL)
 *
 * Please note that "const" is a type reserved for C++.  If you are using
 * Standard C, you should ignore it as well as instances of "CONST" in this
 * file.
 * Altia DeepScreen Function Declarations:
 *               int  altiaLocalAddEvent(AltiaNativeChar *eventName,
 *                                       LocalEventType value);
 *               int  altiaLocalAddEventId(int nameId, LocalEventType value);
 *               int  TargetAltiaInitialize(void);
 *               int  TargetAltiaAnimate(AltiaNativeChar *funcName,
 *                                       LocalEventType value);
 *               void TargetAltiaUpdate(void);
 *               void TargetAltiaLoop(AltiaNativeChar *portName);
 *               void TargetAltiaClose(void);
 *               void TargetAltiaSemGet(int index);
 *               void TargetAltiaSemRelease(int index);
 *
 * Altia Base Library Declarations:
 *               int  altiaSendEvent(const AltiaCharType *eventName, 
 *                                   AltiaEventType eventValue);
 *               int  altiaSendText(const AltiaCharType *eventName,
 *                                  const AltiaCharType *text);
 *               int  altiaGetText(const AltiaCharType *textName,
 *                                 AltiaCharType *buffer, int size);
 *               int  altiaGetTextExternalConn(
 *                                 const AltiaCharType *externalInputName,
 *                                 const AltiaCharType *externalOutputName,
 *                                 AltiaCharType *buffer, int size);
 *               int  altiaCacheOutput(int onoff);
 *               int  altiaFlushOutput(void);
 *               int  altiaPollEvent(const AltiaCharType *eventName, 
 *                                   AltiaEventType  *eventValueOut);
 *               int  altiaLocalPollEvent(const AltiaCharType *eventName,
 *                                        AltiaEventType *eventValueOut);
 *               int  altiaNextEvent(AltiaCharType **eventNameOut, 
 *                                   AltiaEventType *eventValueOut);
 *               int  altiaCheckEvent(const AltiaCharType *eventName, 
 *                                    AltiaEventType *eventValueOut);
 *               int  altiaPending(void);
 *               int  altiaLocalPending(void);
 *               int  altiaSelectEvent(const AltiaCharType *eventName);
 *               int  altiaSelectAllEvents(void);
 *               int  altiaSelectAllExternEvents(void);
 *               int  altiaSyncClients(int onoff);
 *               int  altiaUnselectEvent(const AltiaCharType *eventName);
 *               int  altiaUnselectAllEvents(void);
 *               int  altiaUnselectAllExternEvents(void);
 *               int  altiaEventSelected(const AltiaCharType *eventName);
 *               int  altiaMoveObject(int objNum, int xOffset, int yOffset);
 *               int  altiaInputNumber(void);
 *               int  altiaOutputNumber(void);
 *               int  altiaConnect(const AltiaCharType *portName);
 *               void altiaSelectConnect(int index);
 *               int  altiaGetConnect(void);
 *               void altiaDisconnect(void);
 *               void altiaStopInterface(void);
 *               void altiaSuppressErrors(int yes);
 *               void altiaRetryCount(int count);
 *               void altiaSuppressExit(int yes);
 *               void altiaSleep(unsigned long millisecs);
 *               const AltiaCharType *altiaNativeStr2AltiaStr(
 *                                                const AltiaNativeChar *str,
 *                                                AltiaCharType *buffer,
 *                                                int bufferLength)
 *               const AltiaNativeChar *altiaStr2NativeStr(
 *                                                const AltiaCharType *str,
 *                                                AltiaNativeChar *buffer,
 *                                                int bufferLength)
 *               int  _altiaDoNotDisconnect(void);
 *
 *            For WINDOWS ONLY
 *               int  altiaFetchArgcArgv(void *hInstance,
 *                                       AltiaNativeChar *lpCmdLine, int argsize,
 *                                       AltiaNativeChar **argv, int *argc);
 *               AtConnectId altiaStartInterface(const AltiaCharType *dsnfile,
 *                                        int editMode,
 *                                        const AltiaNativeChar *cmdLine);
 *
 * Altia Toolkit Functions/Procedures Declarations:
 *   int         AtSendEvent(AtConnectId, const AltiaCharType *name,
 *                           AltiaEventType value);
 *   int         AtSendText(AtConnectId, const AltiaCharType *name,
 *                          const AltiaCharType *text);
 *   int         AtGetText(AtConnectId, const AltiaCharType *textName,
 *                         AltiaCharType *buffer, int size);
 *   int         AtGetTextExternalConn(AtConnectId,
 *                                   const AltiaCharType *externalInputName,
 *                                   const AltiaCharType *externalOutputName,
 *                                   AltiaCharType *buffer, int size);
 *   int         AtCacheOutput(AtConnectId, int onoff);
 *   int         AtFlushOutput(AtConnectId);
 *   int         AtPollEvent(AtConnectId, const AltiaCharType *name,
 *                           AltiaEventType *valueOut);
 *   int         AtLocalPollEvent(AtConnectId, const AltiaCharType *name,
 *                                AltiaEventType *valueOut);
 *   int         AtCheckEvent(AtConnectId, const AltiaCharType *name,
 *                            AltiaEventType *valueOut);
 *   int         AtNextEvent(AtConnectId, const AltiaCharType **nameOut,
 *                           AltiaEventType *valueOut);
 *   int         AtPending(AtConnectId);
 *   int         AtLocalPending(AtConnectId);
 *   int         AtSelectEvent(AtConnectId, const AltiaCharType *name);
 *   int         AtSelectAllEvents(AtConnectId);
 *   int         AtSelectAllExternEvents(AtConnectId);
 *   int         AtSyncClients(AtConnectId, int onoff);
 *   int         AtUnselectEvent(AtConnectId, const AltiaCharType *name);
 *   int         AtUnselectAllEvents(AtConnectId);
 *   int         AtUnselectAllExternEvents(AtConnectId);
 *   int         AtEventSelected(AtConnectId, const AltiaCharType *name);
 *   int         AtMoveObject(AtConnectId, int objNum,int xOffset,int yOffset);
 *   int         AtInputNumber(AtConnectId);
 *   int         AtOutputNumber(AtConnectId);
 *   void        AtSuppressErrors(int yes);
 *   void        AtRetryCount(AtConnectId, int count);
 *   void        AtSuppressExit(AtConnectId, int yes);
 *   void        AtDispatchEvent(AtConnectId, const AltiaCharType *nameOut,
 *                               int value);
 *   AtConnectId AtOpenConnection(const AltiaCharType *portName,
 *                                const AltiaCharType *optionName,
 *                                int argc, const AltiaNativeChar *argv[]);
 *   void        AtCloseConnection(AtConnectId);
 *   #ifndef DEEPSCREEN
 *   AtConnectId AtStartInterface(const char *dsnFile,
 *                                const char *rtmFile,
 *                                int editMode, int argc,
 *                                const char *argv[]);
 *   #else
 *   AtConnectId AtStartInterface(const AltiaCharType *dsnFile,
 *                                const AltiaCharType *rtmFile,
 *                                int editMode, int argc,
 *                                const AltiaNativeChar *argv[]);
 *   #endif
 *   void        AtStopInterface(AtConnectId);
 *   const AltiaCharType *AtGetPortName(AtConnectId);
 *   void        AtAddCallback(AtConnectId, const AltiaCharType *name,
 *                             AtCallbackProc proc, AtPointer clientData);
 *   void        AtRemoveCallback(AtConnectId, const AltiaCharType *name,
 *                                AtCallbackProc proc, AtPointer clientData);
 *   void        AtAddTextCallback(AtConnectId, const AltiaCharType *name,
 *                                 AtTextProc proc, AtPointer clientData);
 *   void        AtRemoveTextCallback(AtConnectId, const AltiaCharType *name,
 *                                    AtTextProc proc, AtPointer clientData);
 *   void        AtRemoveAllCallbacks(AtConnectId, const AltiaCharType *name);
 *   AtInputId   AtAddInput(int source, AtInputType,
 *                          AtInputProc proc, AtPointer clientData);
 *   void        AtRemoveInput(AtInputId);
 *   AtTimerId   AtAddTimer(long msecs, AtTimerProc proc,
 *                          AtPointer clientData);
 *   void        AtRemoveTimer(AtTimerId);
 *   AtEventMask AtAllPending(AtConnectId);
 *   void        AtProcessEvent(AtConnectId, AtEventMask);
 *   void        AtMainLoop(void);
 *
 * Altia DeepScreen ID Based Animation API Declarations:
 *   int         altiaGetAnimationId(const AltiaCharType *name);
 *   int         AtGetAnimationId(AtConnectId, const AltiaCharType *name);
 *   int         altiaSendEventId(int nameId, AltiaEventType eventValue);
 *   int         AtSendEventId(AtConnectId, int nameId, AltiaEventType value);
 *   int         altiaSendTextId(int nameId, const AltiaCharType *text);
 *   int         AtSendTextId(AtConnectId, int nameId, const AltiaCharType *text);
 *   int         TargetAltiaAnimateId(int nameId, AltiaEventType value);
 *
 * Altia DeepScreen Tasking API Declarations:
 *   int         altiaTaskRun(unsigned long milliSeconds);
 *   int         altiaTaskStatus(AltiaTaskStatusType * status);
 *
 * Altia DeepScreen Font API Declarations:
 *   int         altiaGetTextWidth(const AltiaCharType *name, AltiaCharType *text);
 *   int         altiaGetTextWidthId(int nameId, AltiaCharType *text);
 *   int         altiaGetFont(const AltiaCharType *name);
 *   int         altiaGetFontId(int nameId);
 *   int         altiaSetFont(const AltiaCharType *name, int fontId);
 *   int         altiaSetFontId(int nameId, int fontId);
 *
 * CHANGELOG:
 *               01/21/91    Created
 *               02/02/93    Added Toolkit declarations
 *               02/09/99    Added floating point event value support
 *               07/15/08    Added DeepScreen ID based APIs (JJM)
 *               08/05/13    Added DeepScreen Tasking APIs (JJM)
 */

#ifndef vSeApP_h
#define vSeApP_h

#if defined(WIN16) && defined(_WINDLL)
/* Compiling for a WIN16 DLL */
#define CALLP _far
#define CALLTYPE _pascal

#else
#if defined(WIN32DLL) || defined(ALTIADLLAPI)
/* Compiling for a WIN32 DLL */

#ifndef WIN32DLL
#define WIN32DLL
#endif

#ifdef CALLTYPE
#undef CALLTYPE
#endif

/* For DeepScreen, even if WIN32DLL is defined, define nothing
 * for CALLP or CALLTYPE so that Drawing Area object works.
 */
#ifdef DEEPSCREEN
#define CALLP
#define CALLTYPE 
#else
#define CALLP 
#define CALLTYPE  __stdcall
#endif

#else
/* Compiling for non-Windows or a Windows executable or object library */
#define CALLP
#define CALLTYPE 
#endif

#endif


/******************************************************************
 * Trivial user code that has a "main" entry point and is being compiled
 * with DeepScreen code can return ALTIA_NO_USER_MAIN and DeepScreen
 * will then execute its own main loop to process events.
 ******************************************************************/
#define ALTIA_NO_USER_MAIN -12345


/******************************************************************
 * For backwards compatibility with Beta releases of DeepScreen:
 * If DEEPSCREENUSERMAIN and CUSTOM_MAIN_LOOP are both defined,
 * user code that has a "main" entry point and is being compiled with
 * DeepScreen code must use the entry point "CustomMainLoop".  The
 * DeepScreen code defines the "main" entry point and calls
 * "CustomMainLoop" after initialization is complete.
 ******************************************************************/
#if defined(DEEPSCREENUSERMAIN) && defined(CUSTOM_MAIN_LOOP)
#define main CustomMainLoop
#endif


#ifdef WIN16

/******************************************************************
 * Programs that have as their entry point "main" does not work
 * since they must be windows programs.  WinMain calls altiaMain
 * so we have to redefine main to be altiaMain.
 ******************************************************************/
#define main altiaMain

#endif

/******************************************************************
 * If this is a float version of altia.h for use with a float
 * version of the Altia API library, then ALTIAFLOAT will be
 * defined below.  Otherwise, ALTIAINT will be defined.  These
 * defines are used in this file and can be used in source
 * code to create a client that works with both the integer
 * or floating point versions of the Altia API library.
 ******************************************************************/
#ifndef ALTIAINT
#define ALTIAINT
#endif
/* #ifndef ALTIAFLOAT */
/* #define ALTIAFLOAT */
/* #endif */

#ifdef ALTIAINTAPI
#ifndef ALTIAINT
#define ALTIAINT
#endif
#ifdef ALTIAFLOAT
#undef ALTIAFLOAT
#endif
#endif /* ALTIAINTAPI */

/******************************************************************
 * AltiaEventType definition follows.  Based on the definition
 * of ALTIAFLOAT, the event type is either a double or the
 * traditional integer.  If event values passed to the library
 * functions or returned from library functions are declared as
 * AltiaEventType (instead of just double or int), then a client
 * can easily compile to a float version of the Altia API library
 * as well as a traditional integer version.
 ******************************************************************/
#ifdef ALTIAFLOAT
typedef double AltiaEventType;
#define EVENTSIZE 8
#else
typedef int AltiaEventType;
#define EVENTSIZE 4
#endif

/******************************************************************
 * LocalEventType is used only for Deepscreen and indicates what
 * the DeepScreen event type is.
 ******************************************************************/
#ifndef LocalEventTypeDefined
#ifdef ALTIAFIXPOINT
typedef int LocalEventType;
#else
typedef double LocalEventType;
#endif /* ALTIAFIXPOINT */
#endif /* LocalEventTypeDefined */

#ifdef ALTIAFIXPOINT
typedef long ALT_DOUBLE;
#else
typedef double ALT_DOUBLE;
#endif /* ALTIAFIXPOINT */

/******************************************************************
 * UNICODE definitions
 ******************************************************************/
#ifdef UNICODE

#if defined(ALTIA_CHAR_UNICODE) && defined(WCHAR_IS_WCHAR_T)
/* For activex integration only */
typedef wchar_t AltiaNativeChar;
#elif defined(WCHAR_IS_WCHAR_T)
/* For C99 compliant C compiler */
#include <wchar.h>
#include <wctype.h>
typedef wchar_t AltiaNativeChar;
#elif defined(WIN32) || defined(WCHAR_IS_USHORT)
typedef unsigned short AltiaNativeChar;
#elif defined(WCHAR_IS_UINT)
typedef unsigned int AltiaNativeChar;
#elif defined(WCHAR_IS_ULONG)
typedef unsigned long AltiaNativeChar;
#elif defined(WCHAR_IS_SHORT)
typedef short AltiaNativeChar;
#elif defined(WCHAR_IS_LONG)
typedef long AltiaNativeChar;
#else
typedef int AltiaNativeChar;
#endif

/*********************************************************************
 * If UNICODE is defined and ALTIA_WCHAR_SUPPORT is defined, Altia
 * DeepScreen's implementation of wide char functions is enabled. This
 * requires compiling the DeepScreen additional wCharStrings.c file.
 * By default, macros below redefine DeepScreen wCharStrings.c funcs
 * to custom names beginning with _altia_ (e.g., _altia_wcscmp).  This
 * allows them to co-exist with C library wide char funcs.  This is
 * necessary if the target C library actually has wide char funcs
 * available and some application code uses them (but not application
 * code interfacing to DeepScreen).  If a #include of wCharStrings.h
 * and/or altia.h is in application code, occurences of wide char
 * function names (e.g., wcscmp) are redefined to be Altia's version
 * (e.g., _altia_wcscmp).  To force Altia wCharStrings.c func names to
 * exactly match standard C library wide char func names (for example,
 * because the target C library really has NO wide char funcs), define
 * ALTIA_WCHAR_NAMES=0 for DeepScreen and application code compilation.
 ********************************************************************/
#ifndef ALTIA_WCHAR_NAMES
#define ALTIA_WCHAR_NAMES 1
#endif
#if defined(UNICODE) && defined(ALTIA_WCHAR_SUPPORT) && (0 != ALTIA_WCHAR_NAMES) && !defined(ALTIAWCHARNAMES_H)
#define ALTIAWCHARNAMES_H
#define _wfopen   _altia_wfopen
#define fgetwc    _altia_fgetwc
#define wcscmp    _altia_wcscmp
#define wcscpy    _altia_wcscpy
#define wcscat    _altia_wcscat
#define wcsncpy   _altia_wcsncpy
#define wcsncmp   _altia_wcsncmp
#define wcslen    _altia_wcslen
#define wcschr    _altia_wcschr
#define wcsrchr   _altia_wcsrchr
#define wcstod    _altia_wcstod
#define wcstol    _altia_wcstol
#define wcstoul   _altia_wcstoul
#define swprintf  _altia_swprintf
#define iswspace  _altia_iswspace
#define iswpunct  _altia_iswpunct
#define iswprint  _altia_iswprint
#define iswalpha  _altia_iswalpha
#define swscanf   _altia_swscanf
#define wcsstr    _altia_wcsstr
#endif /* UNICODE && ALTIA_WCHAR_SUPPORT && ALTIA_WCHAR_NAMES */

#ifndef ALT_TEXT
#define ALT_STRCMP wcscmp
#define ALT_STRCPY wcscpy
#define ALT_STRCAT wcscat
#define ALT_STRNCMP wcsncmp
#define ALT_STRNCPY wcsncpy
#define ALT_STRLEN wcslen
#define ALT_STRTOD wcstod
#define ALT_STRTOL wcstol
#define ALT_SSCANF swscanf

#if !defined(WCHAR_IS_WCHAR_T) || defined(ALTIA_CHAR_UNICODE)
/* WCHAR_IS_WCHART_T is not defined or ALTIA_CHAR_UNICODE (for activex
 * integration) is defined.  In this case, assume compiler is not C99
 * compliant and swprintf() arguments are identical to sprintf() arguments.
 */
#define ALT_SPRINTF swprintf
#else
/* WCHAR_IS_WCHART_T is defined and ALTIA_CHAR_UNICODE is not defined.
 * Assume compiler is compliant with C99 standard for swprintf() func.
 * C99 compliant swprintf(buf, size, format, ...) has extra size arg
 * thereby requiring a custom wrapper function to convert legacy
 * swprintf() argument list to C99 compliant argument list.
 */
#define ALT_SPRINTF _altiaSwprintf
extern int _altiaSwprintf(AltiaNativeChar *buf, AltiaNativeChar *format, ...);
#endif

#define ALT_ISSPACE iswspace
#define ALT_ISPRINT iswprint
#define ALT_ISASCII iswascii
#define ALT_STRTOK  wcstok
#define ALT_STRCHR  wcschr
#define ALT_STRCHR_TYPE AltiaCharType
#define ALT_CHARMASK 0xffffU
#define ALT_TEXT(x) L ## x
#endif /* ALT_TEXT */

#ifdef ALTIAUNICODEAPI
typedef AltiaNativeChar AltiaCharType;
#define CharPtr AltiaNativeChar*
#else
typedef char AltiaCharType;
#define CharPtr char*
#endif /* ALTIAUNICODEAPI */

/* For API users, define additional macros based on ALTIAUNICODEAPI */
#ifdef ALTIAUNICODEAPI
#define API_STRCMP wcscmp
#define API_STRNCMP wcsncmp
#define API_STRNCPY wcsncpy
#define API_STRCPY wcscpy
#define API_STRCAT wcscat
#define API_STRLEN wcslen
#define API_SCANF wscanf
#define API_STRCHR wcschr
#define API_STRCHR_TYPE AltiaCharType
#define API_STRTOK wcstok
#define API_STRTOD wcstod
#define API_STRTOL wcstol
#define API_SSCANF swscanf

#if !defined(WCHAR_IS_WCHAR_T) || defined(ALTIA_CHAR_UNICODE)
/* WCHAR_IS_WCHART_T is not defined or ALTIA_CHAR_UNICODE (for activex
 * integration) is defined.  In this case, assume compiler is not C99
 * compliant and swprintf() arguments are identical to sprintf() arguments.
 */
#define API_SPRINTF swprintf
#else
/* WCHAR_IS_WCHART_T is defined and ALTIA_CHAR_UNICODE is not defined.
 * Assume compiler is compliant with C99 standard for swprintf() func.
 * C99 compliant swprintf(buf, size, format, ...) has extra size arg
 * thereby requiring a custom wrapper function to convert legacy
 * swprintf() argument list to C99 compliant argument list.
 */
#define API_SPRINTF _altiaSwprintf
extern int _altiaSwprintf(AltiaNativeChar *buf, AltiaNativeChar *format, ...);
#endif

#define API_ISSPACE iswspace
#define API_ISPRINT iswprint
#define API_ISASCII iswascii
#define API_CHARMASK 0xffffU
#define API_TEXT(x) L ## x
#else
#define API_STRCMP strcmp
#define API_STRNCMP strncmp
#define API_STRNCPY strncpy
#define API_STRCPY strcpy
#define API_STRCAT strcat
#define API_STRLEN strlen
#define API_SCANF sscanf
#define API_STRCHR strchr
#define API_STRCHR_TYPE int
#define API_STRTOK strtok
#define API_STRTOD strtod
#define API_STRTOL strtol
#define API_SSCANF sscanf
#define API_SPRINTF sprintf
#define API_ISSPACE isspace
#define API_ISPRINT isprint
#define API_ISASCII isascii
#define API_CHARMASK 0xffU
#define API_TEXT(x) x
#endif

#else /* NOT UNICODE */

#ifndef ALT_TEXT
#define ALT_STRCMP strcmp
#define ALT_STRCPY strcpy
#define ALT_STRCAT strcat
#define ALT_STRNCMP strncmp
#define ALT_STRNCPY strncpy
#define ALT_STRLEN strlen
#define ALT_STRTOD strtod
#define ALT_STRTOL strtol
#define ALT_SSCANF sscanf
#define ALT_SPRINTF sprintf
#define ALT_ISSPACE isspace
#define ALT_ISPRINT isprint
#define ALT_ISASCII isascii
#define ALT_STRTOK  strtok
#define ALT_STRCHR  strchr
#define ALT_STRCHR_TYPE int
#define ALT_CHARMASK 0xffU
#define ALT_TEXT(x) x
#endif /* ALT_TEXT */

typedef char AltiaCharType;
typedef char AltiaNativeChar;
#define CharPtr char*

/* For API users, define additional macros */
#define API_STRCMP strcmp
#define API_STRNCMP strncmp
#define API_STRNCPY strncpy
#define API_STRCPY strcpy
#define API_STRCAT strcat
#define API_STRLEN strlen
#define API_SCANF sscanf
#define API_STRCHR strchr
#define API_STRCHR_TYPE int
#define API_STRTOK strtok
#define API_STRTOD strtod
#define API_STRTOL strtol
#define API_SSCANF sscanf
#define API_SPRINTF sprintf
#define API_ISSPACE isspace
#define API_ISPRINT isprint
#define API_ISASCII isascii
#define API_CHARMASK 0xffU
#define API_TEXT(x) x

#endif /* UNICODE */

/******************************************************************
 * For NULL definition
 ******************************************************************/
#ifndef NULL
#include <stdio.h>
#endif
 
/******************************************************************
 * Determine if function prototyping is enabled.
 ******************************************************************/
#ifndef AltiaFunctionPrototyping
#if defined(FUNCPROTO) || defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus) || defined(WIN32)
#define AltiaFunctionPrototyping 1
#else
#define AltiaFunctionPrototyping 0
#endif /* Check for function prototyping requirement */
#endif /* AltiaFunctionPrototyping */


/******************************************************************
 * Add timeval definitions if we are in windows
 ******************************************************************/
#ifdef WIN16
    
#ifndef WINSOCK16

struct timeval {
        long    tv_sec;         /* seconds */
        long    tv_usec;        /* and microseconds */
};

#endif

#endif

/******************************************************************
 * Determine if we have const capabilities
 ******************************************************************/
#if defined(__cplusplus) || defined(c_plusplus)
#undef CONST
#define CONST const
#else
#undef CONST
#define CONST
#endif /* __cplusplus */



/******************************************************************
 *
 *           ALTIA DEEPSCREEN INTERFACE FUNCTION DECLARATIONS
 *                      Declarations are for C/C++ 
 *
 *  If an application is going to be compiled as a standard Altia
 *  client (with a socket or DDE API) sometimes and as a DeepScreen
 *  application (with the DeepScreen version of the API) other times, you
 *  can conditionally compile these functions for the DeepScreen case
 *  with:
 *
 *      #ifdef DEEPSCREEN
 *      ...
 *      #endif DEEPSCREEN
 *
 *  Calling any of these special DeepScreen functions is NOT a
 *  requirement for programming with DeepScreen graphics code if you
 *  are using the DeepScreen version of the standard Altia API.  It
 *  opens the graphics into a new window when you call AtOpenConnection(),
 *  altiaConnect(), or AtStartInterface().  It updates the graphics when
 *  you call  AtSendEvent(), altiaSendEvent(), etc.  It returns stimulus
 *  and control events when you use AtNextEvent(), altiaNextEvent(),
 *  AtPollEvent(), altiaPollEvent(), etc.  It closes the graphics window
 *  when you call AtCloseConnection(), altiaDisconnect(), or
 *  AtStopInterface().
 * 
 ******************************************************************/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

    /*
     * int altiaLocalAddEvent(AltiaNativeChar *eventName, LocalEventType value)
     * int altiaLocalAddEventId(int nameId, LocalEventType value):
     * --------------------------------------------------------------
     * This function is only available with the DeepScreen standalone
     * version of the Altia API (i.e., no connection to an existing Altia
     * session is established to receive events).  In such an environment,
     * events come from Altia Stimulus or they can be injected from custom
     * code by calling the altiaLocalAddEvent() function.
     *
     * NOTE THAT THIS FUNCTION TAKES AN 8-BIT STRING ON NON-UNICODE
     * SYSTEMS OR A 16-BIT STRING ON UNICODE.  THIS IS INDEPENDENT
     * OF THE REST OF THE API FUNCTIONS.  AND value MUST BE THE
     * VALUE TYPE EXPECTED BY DEEPSCREEN CODE (LocalEventType) AND NOT
     * JUST AN int EVEN IF AN INTEGER API IS BEING USED.
     *
     * In any case, all events injected with altiaLocalAddEvent()
     * must first be selected with AtSelectEvent() or
     * altiaSelectEvent().  And then they can be received with
     * AtNextEvent(), altiaNextEvent(), AtCheckEvent() or
     * altiaCheckEvent().  See Section II of the Altia API manual
     * for complete details regarding these standard Altia API
     * functions and others.
     */
extern int altiaLocalAddEvent(
#if AltiaFunctionPrototyping
    AltiaNativeChar *eventName,
    LocalEventType value
#endif
);
extern int altiaLocalAddEventId(
#if AltiaFunctionPrototyping
    int nameId,
    LocalEventType value
#endif
);


    /*
     * int TargetAltiaInitialize(void):
     * --------------------------------------------------------------
     * This function does the work of creating a Window for the DeepScreen
     * graphics and initializing the DeepScreen code.  It determines whether
     * the Window has already been initialized and does nothing in such a
     * case.  This allows it to be called more than once by unrelated
     * sub-systems that may want to insure that everything is initialized.
     *
     * It is not necessary for a program to call this function if the
     * program is linking with the DeepScreen version of the Altia API.
     * It gets called automatically on API initialization via
     * AtOpenConnection(), altiaConnect(), or AtStartInterface().
     *
     * This function returns a value 1 on success, 0 on error.
     */
extern int TargetAltiaInitialize(
#if AltiaFunctionPrototyping
    void
#endif
);


    /*
     * int TargetAltiaAnimate(AltiaNativeChar *funcName, LocalEventType value):
     * --------------------------------------------------------------
     * This function is called to perform an Animate of the DeepScreen
     * graphics associated with a particular animation function name.
     * A non-zero value is returned if the animation requires an update
     * and TargetAltiaUpdate() can be called to perform the update.
     * A zero is returned if the animate request does not result in
     * any graphics changes.
     *
     * NOTE THAT THIS FUNCTION TAKES AN 8-BIT STRING ON NON-UNICODE
     * SYSTEMS OR A 16-BIT STRING ON UNICODE.  THIS IS INDEPENDENT
     * OF THE REST OF THE API FUNCTIONS.  AND value MUST BE THE
     * VALUE TYPE EXPECTED BY DEEPSCREEN CODE (LocalEventType) AND NOT
     * JUST AN int EVEN IF AN INTEGER API IS BEING USED.
     *
     * It is not necessary for a program to call this function if the
     * program is linking with the DeepScreen version of the Altia API.
     * It gets called automatically by AtSendEvent(), altiaSendEvent(),
     * etc., as needed.
     */
extern int TargetAltiaAnimate(
#if AltiaFunctionPrototyping
    AltiaNativeChar *funcName,
    LocalEventType value
#endif
);


    /*
     * void TargetAltiaUpdate(void):
     * --------------------------------------------------------------
     * This function is called to perform an update of the DeepScreen
     * graphics.  An update redraws any graphics that need changing since
     * the last call to TargetAltiaUpdate().  Drawing is only performed
     * during a TargetAltiaUpdate() call which gives application developers
     * full control of the redraw schedule.
     *
     * It is not necessary for a program to call this function if the
     * program is linking with the DeepScreen version of the Altia API.
     * If caching is disabled (the default), it gets called automatically
     * by AtSendEvent(), altiaSendEvent(), etc.  If caching is enabled,
     * it is called by AtFlushOutput(), AtPending(), etc.
     */
extern void TargetAltiaUpdate(
#if AltiaFunctionPrototyping
    void
#endif
);

    /*
     * int TargetAltiaLoop(AltiaNativeChar *portName):
     * --------------------------------------------------------------
     * This function provides a simple event processing loop that can
     * be used if there is no custom processing required.  It takes
     * a port name as an option if a full API is being used that
     * needs to connect to an existing Altia session.  Otherwise, just
     * pass NULL.  It returns a suggested program return value.
     *
     * NOTE THAT THIS FUNCTION TAKES AN 8-BIT STRING ON NON-UNICODE
     * SYSTEMS OR A 16-BIT STRING ON UNICODE.  THIS IS INDEPENDENT
     * OF THE REST OF THE API FUNCTIONS.
     */
extern int TargetAltiaLoop(
#if AltiaFunctionPrototyping
    AltiaNativeChar *portName
#endif
);


    /*
     * void TargetAltiaClose(void):
     * --------------------------------------------------------------
     * This function "closes" the DeepScreen code and cleans up the Window
     * It determines whether the Window has already been closed and does
     * nothing in such a case.  This allows it to be called more than once
     * by unrelated sub-systems that may want to insure that everything is
     * cleaned up.
     *
     * It is not necessary for a program to call this function if the
     * program is linking with the DeepScreen version of the Altia API.
     * It gets called automatically by AtCloseConnection(), altiaDisconnect(),
     * or AtStopInterface().
     */
extern void TargetAltiaClose(
#if AltiaFunctionPrototyping
    void
#endif
);


    /*
     * void TargetAltiaSemGet(int index):
     * --------------------------------------------------------------
     * This function gets a mutual exclusion type semaphore (if available
     * for the target operating system).  It is called by the DeepScreen
     * version of the Altia API to protect the DeepScreen code from being
     * entered by more than one task or thread at a time.  It takes an
     * index of 0, 1, or 2 depending on which semaphore should be used.
     * Semaphore 0 protects the typical API calls, semaphore 1 protects
     * other more complicated API calls like those to start/stop/open/close
     * the interface, and semaphore 2 protects the callback portion of the
     * API.
     *
     * It is not necessary for a program to call this function if the
     * program is linking with the DeepScreen version of the Altia API.
     * It gets called automatically by all API functions that are not
     * reentrant from multiple tasks or threads.
     */
extern void TargetAltiaSemGet(
#if AltiaFunctionPrototyping
    int index
#endif
);


    /*
     * void TargetAltiaSemRelease(int index):
     * --------------------------------------------------------------
     * This function releases a mutual exclusion type semaphore (if available
     * for the target operating system) that was presumably taken by
     * an earlier TargetAltiaSemGet() call.  It is called by the DeepScreen
     * version of the Altia API to release the protection of DeepScreen
     * code from being entered by more than one task or thread at a time.
     *
     * It is not necessary for a program to call this function if the
     * program is linking with the DeepScreen version of the Altia API.
     * It gets called automatically by all API functions that need protection
     * from reentrancy by multiple tasks or threads.
     */
extern void TargetAltiaSemRelease(
#if AltiaFunctionPrototyping
    int index
#endif
);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif



/******************************************************************
 *
 *                ALTIA BASE LIBRARY FUNCTION DECLARATIONS
 *
 *                      Declarations are for C/C++ 
 * 
 ******************************************************************/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if defined(WIN16)
    /*
     * int altiaMain(int argc, AltiaNativeChar *argv[]):
     * --------------------------------------------------------------
     * Under 16-bit Windows, a program's main function is redefined as
     * altiaMain by this header file (see #define main altiaMain above).
     * Then, the Altia library provides the WinMain function.
     * For Windows programs, WinMain is the entry point.  The Altia library's
     * WinMain will put parameters into the argc, argv form and call
     * the application's altiaMain.
     *    argc      is the number of arguments that were found on the command
     *              line.
     *    argv      is the array of character pointers that will be set to 
     *              point the the individual arguments.  This is static data
     *              do not free it.
     * Return value:
     *    Typically, a program returns an integer.
     *
     */
    extern int altiaMain(
#if AltiaFunctionPrototyping
    int argc,
    AltiaNativeChar *argv[]
#endif
);
#endif

    /*
     * int altiaSendEvent(const AltiaCharType *eventName,
     *                    AltiaEventType eventValue):
     * --------------------------------------------------------------
     * Initiate the transmission of an event to the Altia interface.
     * The event's name is given by eventName and its value
     * is given by eventValue.  If a communications failure
     * occurs, this function returns -1.  Otherwise, 0 is
     * returned.
     */
    extern int CALLP CALLTYPE altiaSendEvent(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *eventName,
    AltiaEventType eventValue
#endif
);

    /*
     * int altiaSendText(const AltiaCharType *eventName,
     *                   const AltiaCharType *text):
     * --------------------------------------------------------------
     * Initiate the transmission of a sequence of events whose
     * values are the characters pointed to by text and includes
     * the null character that terminates the string.  The event's
     * name is given by eventName.  Altia text i/o objects accept
     * strings in this manner and display them in the selected font
     * style on the screen.  A text i/o object taken from the
     * $ALTIAHOME/models/textio.dsn Models Library typically has an
     * animation named "text" which accepts character string events. The
     * GUI Models Library, $ALTIAHOME/models/gui.dsn, also has text i/o
     * objects that have been incorporated into higher level GUI
     * components.
     *
     * If a communications failure occurs, this function returns -1.
     * Otherwise, 0 is returned.
     */
    extern int CALLP CALLTYPE altiaSendText(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *eventName,
    CONST AltiaCharType *text
#endif
);


    /*
     * int altiaGetText(const AltiaCharType *textName,
     *                  AltiaCharType *buffer, int bufSize):
     * --------------------------------------------------------------
     * This function is used to retrieve the string currently being displayed
     * by a text i/o object.  An Altia text i/o object has a "text"
     * animation which it uses to send character events to an application
     * program. altiaGetText() will query the "text" animation whose name
     * is pointed to by textName and pass the retrieved text back to the
     * caller via the character array supplied by the caller and pointed to by
     * buffer.  The array's size is passed in bufSize so that this function
     * will not overrun the caller's character array buffer.  The returned
     * string will always be null terminated assuming bufSize is greater than
     * zero.
     *
     * To learn more about text i/o objects, open the Help menu available
     * from a Models Library View and select Textio Models. Actual text
     * input model examples are available in the $ALTIAHOME/models/textio.dsn
     * and $ALTIAHOME/models/gui.dsn Models Libraries.
     *
     * This function returns -1 if a communications failure occurs or the
     * "text" animation given does not respond to a query. In either case,
     * the first element of buffer is set to the null character '\0'.  A
     * 0 is returned if the operation was successful and buffer will point 
     * to a null terminated string.  The string could be only a null character
     * if the input object is not displaying any text.
     */
    extern int CALLP CALLTYPE altiaGetText(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *textName,
    AltiaCharType *buffer,
    int bufSize
#endif
);


    /*
     * int altiaGetTextExternalConn(const AltiaCharType *externalInputName,
     *                              const AltiaCharType *externalOutputName,
     *                              AltiaCharType *buffer, int bufSize):
     * --------------------------------------------------------------
     * This function is used to retrieve the string currently being displayed
     * by a text i/o object where the retrieval is being done through
     * external connections linked to object connections assigned to the
     * "text" animation for a text i/o object.  This requires a pair of
     * external connections.
     *
     * The first is an external INPUT connection (i.e., an input to
     * application code) that should be linked to an OUTPUT connection for
     * the text i/o object.  The OUTPUT connection for the text i/o object
     * must be assigned to the text i/o object's "text" animation.
     * For the Text Entry boxes from the standard inputs.dsn library, this
     * is the OUTPUT connection with the name "Text Entry String".
     *
     * The second is an external OUTPUT connection (i.e., an output from
     * application code) that should be linked to an INPUT connection for
     * the text i/o object.  The INPUT connection for the text i/o object
     * must be assigned to the text i/o object's "text" animation.
     * For the Text Entry boxes from the standard inputs.dsn library, this
     * is the INPUT connection with the name "Text Entry String".
     *
     * Note that the text i/o object is going to have both an INPUT and
     * OUTPUT connection that is assigned to the same "text" animation.
     * This is perfectly legal and necessary.
     *
     * On the other hand, the animation names assigned to each of the
     * external INPUT and OUTPUT connections MUST be different.  This is
     * why a special function (rather than just altiaGetText()) is required
     * for getting text from a text i/o object through external connections.
     *
     * The externalInputName parameter should point to a string containing
     * the animation name assigned to the external INPUT connection.
     * In Altia, this external INPUT connection must be linked to an object
     * OUTPUT connection that is assigned to the "text" animation for a
     * text i/o object.
     *
     * The externalOutputName parameter should point to a string containing
     * the animation name assigned to an external OUTPUT connection.
     * In Altia, this external OUTPUT connection must be linked to an object
     * INPUT connection that is assigned to the "text" animation for a
     * text i/o object.
     *
     * altiaGetTextExternalConn() will attempt to query the text i/o object's
     * "text" animation through the external connection animations pointed to
     * by externalInputName and externalOutputName.  If successful, the text
     * string from the text i/o objects is returned to the caller via the
     * character array supplied by the caller and pointed to by buffer.
     * The array's size is passed in bufSize so that this function will
     * not overrun the caller's character array buffer.  The returned string
     * will always be null terminated assuming bufSize is greater than
     * zero.
     *
     * This function returns -1 if a communications failure occurs or the
     * animation names given do not respond to a query. In either case,
     * the first element of buffer is set to the null character '\0'.  A
     * 0 is returned if the operation was successful and buffer will point 
     * to a null terminated string.  The string could be only a null character
     * if the text i/o object is not displaying any text.
     */
    extern int CALLP CALLTYPE altiaGetTextExternalConn(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *inName,
    CONST AltiaCharType *outName,
    AltiaCharType *buffer,
    int bufSize
#endif
);


    /*
     * int altiaCacheOutput(int onoff):
     * --------------------------------------------------------------
     * Normally, each altiaSendEvent or altiaSendText call immediately
     * transmits its data to the Altia interface and the interface
     * updates the display graphics. If altiaCacheOutput is called with
     * a non-zero (true) parameter value, events are cached (i.e. buffered)
     * in a local buffer.  They are transmitted to the Altia interface when
     * the buffer is filled, altiaFlushOutput is called, altiaCacheOutput(0)
     * is called, or another library routine is called other than
     * altiaSendEvent or altiaSendText that requires communication with the
     * Altia interface. When cached events are transmitted as a result of
     * a full buffer, the Altia interface accepts them and processes them,
     * but doesn't update the display graphics.  The program must call
     * altiaFlushOutput, altiaCacheOutput(0), or another library routine
     * other than altiaSendEvent or altiaSendText to force a display update.
     *
     * Caching events in this way significantly improves performance for
     * event intensive applications (e.g., plotting, charting or massive
     * screen text updating).
     *
     * Function returns 0 if successful or -1 if a communications failure
     * occurs.
     */
    extern int CALLP CALLTYPE altiaCacheOutput(
#if AltiaFunctionPrototyping
    int onoff
#endif
);
    /*
     * int altiaSyncClients(int onoff):
     * --------------------------------------------------------------
     * Normally, each client connected to altia independently sends
     * data to altia as fast as the client desires.  This can cause
     * data from one client to fill up the queue of another client.
     * This can cause delays in processing events from one client since
     * the queue is full of events from the other client.
     * To prevent this turn on the altiaSyncClients flag.
     * When this flag is turned on Altia will stop processing events 
     * from one client until the other clients can process those events.
     * This keeps the client programs "in sync" with each other and
     * prevents a single clients queue of data from growing much larger
     * then the other clients queues. This can be useful when a 
     * client is connected to 2 altias and one of these altia sessions
     * has another client that sends data at a fast rate. With this flag
     * set the two altia sessions will remain in sync with each other.
     *
     * Function returns 0 if successful or -1 if a communications failure
     * occurs.
     */
    extern int CALLP CALLTYPE altiaSyncClients(
#if AltiaFunctionPrototyping
    int onoff
#endif
);


    /*
     * int altiaFlushOutput(void):
     * --------------------------------------------------------------
     * If altiaCacheOutput was previously called with a non-zero (true)
     * parameter, then altiaFlushOutput flushes the outgoing event cache
     * and requests the Altia interface to update its display graphics as
     * needed. See altiaCacheOutput for more description of the event cache.
     *
     * Function returns 0 if successful or -1 if a communications failure
     * occurs.
     */
    extern int CALLP CALLTYPE altiaFlushOutput(
#if AltiaFunctionPrototyping
    void
#endif
);

    /*
     * int altiaPollEvent(const AltiaCharType *eventName, 
                          AltiaEventType *eventValueOut):
     * --------------------------------------------------------------
     * Request the current value for an event from the Altia interface.
     * The event's name is given by eventName and its value is
     * returned in the value pointed to by eventValueOut.
     * If an event of this type was previously routed to this client
     * and is awaiting receipt via altiaNextEvent(), it will be deleted
     * from the receiving queue.  A poll request gets the current
     * value for the event which makes all previous values obsolete.
     * If the event was not found or there was a communications
     * failure, -1 is the function's return value.  Otherwise,
     * 0 is returned.
     */
    extern int CALLP CALLTYPE altiaPollEvent(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *eventName,
    AltiaEventType *eventValueOut
#endif
);

    /*
     * int altiaLocalPollEvent(const AltiaCharType *eventName, 
     *                         AltiaEventType *eventValueOut):
     * --------------------------------------------------------------
     * Request the current value for an event from the local queue only.
     * The event's name is given by eventName and its value is
     * returned in the value pointed to by eventValueOut if and only if
     * a new value for the name was found in the local queue.  If more than
     * one event is awaiting receipt in the local queue, the value for the
     * newest instance is returned.  All of the events for the given name
     * are removed from the local queue upon return from the call.  In
     * summary, this function gets the best local value for the event which
     * makes all events in the queue obsolete and hence they are deleted.
     * Unlike altiaPollEvent(), this function only looks in the local queue
     * for events of the given name.  It will not force a transaction with
     * the Altia interface process.  If an event was found in the local
     * queue, 0 is returned and eventValueOut points to the value.  Otherwise,
     * -1 is returned and the integer pointed to by eventValueOut is not
     * modified in any way.
     */
    extern int CALLP CALLTYPE altiaLocalPollEvent(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *eventName,
    AltiaEventType *eventValueOut
#endif
);

    /*
     * int altiaNextEvent(AltiaCharType **eventNameOut,
     *                    AltiaEventType *eventValueOut):
     * --------------------------------------------------------------
     * Get the next available event that was routed to us by the
     * Altia interface.  This function blocks until an event is
     * available.  The only events that are routed are those
     * that have been selected by previously calling
     * altiaSelectEvent() or altiaSelectAllEvents().  This function
     * takes pointers to a character pointer and an AltiaEventType as
     * parameters.  A pointer to a string containing the name
     * of the next available event is returned via the first
     * parameter and its value is returned via the second.  The
     * string returned resides in memory that is private to this
     * library.  BE CAREFULL! It may be overwritten by the next
     * call to any function in this library.
     * Zero(0) is returned on successful completion.
     * If a communications failure occurs or the client has no 
     * events selected for routing, -1 is returned and the
     * character pointer returned points to an empty string because
     * that seems friendlier than pointing to NULL.
     */
    extern int CALLP CALLTYPE altiaNextEvent(
#if AltiaFunctionPrototyping
    AltiaCharType **eventNameOut,
    AltiaEventType *eventValueOut
#endif
);

    /*
     * int altiaCheckEvent(const AltiaCharType *eventName, 
     *                     AltiaEventType *eventValueOut):
     * --------------------------------------------------------------
     * Get the value for the next available event that was routed to us 
     * by the Altia interface and whose name matches eventName.  If there are
     * no events with this name available, this function returns immediately.
     * If one or more events with this name exist, the value for the oldest is 
     * returned and that instance of the event is removed from the queue.  All 
     * other events, including newer instances of the named event, remain in 
     * the queue in their appropriate order.  In order for this function to
     * work correctly, the named event must have been selected by previously
     * calling altiaSelectEvent() or altiaSelectAllEvents().
     *
     * This function takes a pointer to an eventName string.  If an instance
     * of the event is found, the function returns its value in the 
     * AltiaEventType variable pointed to by eventValueOut.
     *
     * If an event is found, 1 is returned and *eventValueOut is set.  If no
     * event is found, 0 is returned.  If a communications failure occurs,
     * -1 is returned. 
     */
    extern int CALLP CALLTYPE altiaCheckEvent(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *eventName,
    AltiaEventType *eventValueOut
#endif
);

    /*
     * int altiaPending(void):
     * --------------------------------------------------------------
     * Check for events that are waiting to be received. The
     * number of events available is returned where a return of
     * zero indicates no events are available. A -1 is returned
     * on a communications failure.  Note that altiaNextEvent() is called
     * to receive the oldest available event.
     */
    extern int CALLP CALLTYPE altiaPending(
#if AltiaFunctionPrototyping
    void
#endif
);

    /*
     * int altiaLocalPending(void):
     * --------------------------------------------------------------
     * Check for local events that are waiting to be received. If no
     * events are available locally, return 0.  In contrast, altiaPending()
     * will check the Altia interface process for additional events if
     * none are available locally.  The number of events available is
     * returned where a return of zero indicates no events are available.
     * A -1 is returned on a communications failure.  Note that
     * altiaNextEvent() is called to receive the oldest available event.
     */
    extern int CALLP CALLTYPE altiaLocalPending(
#if AltiaFunctionPrototyping
    void
#endif
);

    /*
     * int altiaSelectEvent(const AltiaCharType *eventName):
     * --------------------------------------------------------------
     * Select to have a specific event routed to this client by the Altia
     * interface.  Events that are selected can be received by calling
     * altiaNextEvent().
     */
    extern int CALLP CALLTYPE altiaSelectEvent(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *eventName
#endif
);

    /*
     * int altiaSelectAllEvents(void):
     * --------------------------------------------------------------
     * Select to have all events routed to this client by the Altia
     * interface.  Events can be received by calling altiaNextEvent().
     */
    extern int CALLP CALLTYPE altiaSelectAllEvents(
#if AltiaFunctionPrototyping
    void
#endif
);

    /*
     * int altiaSelectAllExternEvents(void):
     * --------------------------------------------------------------
     * Select to have all non-internal events routed to this client by the Altia
     * interface.  Events can be received by calling altiaNextEvent().
     */
    extern int CALLP CALLTYPE altiaSelectAllExternEvents(
#if AltiaFunctionPrototyping
    void
#endif
);

    /*
     * int altiaUnselectEvent(const AltiaCharType *eventName):
     * --------------------------------------------------------------
     * Turn off the routing of a specific event to this client.
     */
    extern int CALLP CALLTYPE altiaUnselectEvent(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *eventName
#endif
);

    /*
     * int altiaUnselectAllEvents(void):
     * --------------------------------------------------------------
     * Turn off the routing of all events to this client.  This is the
     * initial mode of operation for a new client connection.  Specific
     * calls must be made to altiaSelectEvent() or altiaSelectAllEvents()
     * before the Altia interface will route any events to a client.  
     */
    extern int CALLP CALLTYPE altiaUnselectAllEvents(
#if AltiaFunctionPrototyping
    void
#endif
);

    /*
     * int altiaUnselectAllExternEvents(void):
     * --------------------------------------------------------------
     * Turn off the routing of all external events to this client.  This is the
     * initial mode of operation for a new client connection.  Specific
     * calls must be made to altiaSelectAllExternEvents()
     * before the Altia interface will route all external events to a client.  
     */
    extern int CALLP CALLTYPE altiaUnselectAllExternEvents(
#if AltiaFunctionPrototyping
    void
#endif
);

    /*
     * int altiaEventSelected(const AltiaCharType *eventName):
     * --------------------------------------------------------------
     * Checks if events of the given name have been selected to be received
     * by the client.  Events are selected for receipt using
     * altiaSelectEvent() or altiaSelectAllEvents().
     *
     * If the event is selected, 1 is returned.  If the event is not selected,
     * 0 is returned.  If a communications failure occurs, -1 is returned.
     */
    extern int CALLP CALLTYPE altiaEventSelected(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *eventName
#endif
);

    /*
     * int altiaMoveObject(int objectNumber, int xOffset, int yOffset):
     * --------------------------------------------------------------------
     * Move the object identified by object number relative to its current
     * position.  The objectNumber parameter identifies the object that
     * should be moved.  An object's object number can be determined from
     * the Altia editor interface by selecting the object and then choosing
     * the "Rename Functions" item from the Animation Editor's or Stimulus
     * Editor's "Edit" menu. This will display the Function Rename Dialog.
     * The "Prepend" field of the Rename Dialog is loaded with the object's
     * object number.
     *
     * xOffset is the amount, in pixels, that the object should be
     * moved in the horizontal direction. yOffset is the amount, in pixels,
     * that the object should be moved in the vertical direction.  xOffset
     * or yOffset are positive to achieve a move to the right or up,
     * respectively.  xOffset or yOffset can be negative to achieve a move
     * to the left or down, respectively. All animation and stimulus for the 
     * object will now be performed relative to the object's new position.
     * Note that move animation can be used to move an object rather than
     * using this altia library call.  This call, however, allows
     * for more freedom of movement.
     *
     * If a communications failure occurs, this function returns -1.
     * Otherwise, 0 is returned.
     */
extern int CALLP CALLTYPE altiaMoveObject(
#if AltiaFunctionPrototyping
    int objectNumber,
    int xOffset,
    int yOffset
#endif
);

    /*
     * int altiaInputNumber(void):
     * --------------------------------------------------------------
     * Returns a number for the input connection from the Altia interface.
     * On a Unix system, this is the file descriptor for the input socket
     * or pipe.  This is useful for using select(2) calls to simultaneously
     * wait for stdio input or an event being routed from Altia.  For this
     * to work correctly, altiaPending() or altiaNextEvent() must be the last
     * library function called prior to the select(2) call.  The select(2)
     * call can then block until a new event is available for the client.
     * Note that the select(2) only unblocks when a new event is available.
     * It does not unblock if events are currently available, but haven't
     * been received by altiaNextEvent().  For best results, the count
     * returned from altiaPending() should be used as a loop count for calling
     * altiaNextEvent().  Then, select(2) can be called and it will unblock
     * when a new event is available.
     */
    extern int CALLP CALLTYPE altiaInputNumber(
#if AltiaFunctionPrototyping
    void
#endif
);

    /*
     * int altiaOutputNumber(void):
     * --------------------------------------------------------------
     * Returns a number for the output connection from the Altia interface.
     * On a Unix system, this is the file descriptor for the output socket
     * or pipe.
     */
    extern int CALLP CALLTYPE altiaOutputNumber(
#if AltiaFunctionPrototyping
    void
#endif
);

    /*
     * int altiaConnect(const AltiaCharType *portName):
     * --------------------------------------------------------------
     * Open a connection to an Altia interface.  This is not necessary
     * since the other library routines will attempt to open a default
     * connection if one is not already opened.  However, if you would
     * like to open a connection based on a unique name, you may do so
     * by passing a string pointer.  This allows multiple Altia sessions
     * to run on one workstation.  Passing NULL opens the default
     * connection if it is not already opened.  Using the default
     * connection only allows one Altia interface session on a workstation.
     *
     * If a connection is successfully opened, 0 is returned.  Otherwise,
     * -1 is returned and errno(2) should be set accordingly.
     *
     * For workstation-based clients, a domain or network socket serves as
     * the connection facility between an Altia interface and its client
     * programs.  A domain socket is specified by using a filename syntax for
     * the string pointed to by portName (e.g., "/tmp/screen").
     * A network socket is specified by using a "HOSTNAME:SOCKET" syntax
     * where SOCKET may be a number or it may be an alias name for a socket
     * number as specified in the /etc/services file.  If HOSTNAME is omitted,
     * than the name of the current host is assumed.  In any case, if
     * portName is non-NULL, the Altia session that the client is connecting
     * to must be started with the command line argument "-port PORTNAME"
     * where PORTNAME matches the string referenced by the portName parameter
     * for altiaConnect().  If portName is NULL, a default domain socket is
     * used and no special parameter is required when starting the Altia
     * interface session. The default domain socket name is created using the
     * DEFAULTBASENAME #define described later in this file.
     */
    extern int CALLP CALLTYPE altiaConnect(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *portName
#endif
);

    /* Define for maximum number of connections that can be supported from
     * a program using the Altia library.  See altiaSelectConnect() below
     * for more details.
     */
#ifdef WIN16
#define ALTIAMAXCONNECTS 3
#else
#ifndef ALTIAMAXCONNECTS
#define ALTIAMAXCONNECTS 10
#endif
#endif
    /*
     * void altiaSelectConnect(int index):
     * --------------------------------------------------------------
     * This version of the Altia library supports multiple Altia interface
     * connections from a single application program.  It does this without
     * changing the usage for the simple single connection approach.
     *
     * To establish and use multiple connections, a connection index must
     * be assigned to each connection.  The connection index for the first
     * connection would be 0.  This is also the default connection index
     * for the case where only 1 connection is established.  The second
     * connection would be assigned index 1, and so on, up to index 9.
     * That is, the library supports 10 simultaneous connections at a time.
     *
     * All library calls, including altiaConnect() and altiaDisconnect(),
     * operate on the currently selected connection.  A connection is
     * selected with a call to altiaSelectConnect(), passing the index 
     * for the connection you wish to select.  Until another call to
     * altiaSelectConnect(), all library calls will act on the selected
     * connection.
     *
     * If a connection index less than 0 or greater than 4 is passed to
     * altiaSelectConnect(), it is ignored and the previously selected
     * connection remains selected. To query for the current connection
     * index, use altiaGetConnect().
     */
    extern void CALLP CALLTYPE altiaSelectConnect(
#if AltiaFunctionPrototyping
    int index
#endif
);

    /*
     * int altiaGetConnect(void):
     * --------------------------------------------------------------
     * This version of the Altia library supports multiple Altia connections
     * from a single application program.  To select a connection for use,
     * call altiaSelectConnect(int index).  To find out which connection is
     * currently the selected connection, call altiaGetConnect().  Its return
     * value is the index number for the currently selected connection.
     */
    extern int CALLP CALLTYPE altiaGetConnect(
#if AltiaFunctionPrototyping
    void
#endif
);

    /*
     * void altiaDisconnect(void):
     * --------------------------------------------------------------
     * Close the connection to the Altia interface (if one is opened).
     * This can be useful if you want to restart the Altia interface
     * without restarting the application.  Simply issue a
     * altiaDisconnect(), kill/restart the interface, and then issue a
     * altiaConnect().
     */
    extern void CALLP CALLTYPE altiaDisconnect(
#if AltiaFunctionPrototyping
    void
#endif
);

    /*
     * void altiaStopInterface(void):
     * --------------------------------------------------------------
     * This function attempts to terminate the Altia Runtime interface
     * process by sending an "altiaQuit" event to the interface.
     * If the interface is configured in its standard mode, it will receive
     * this event and terminate as a result.  If AtStartInterface() was called
     * to originally start the interface, then AtStopInterface() should be
     * used instead of altiaStopInterface() because it is more rigorous
     * in its attempt to terminate the interface process.
     *
     * After the "altiaQuit" event is sent, the local connection is closed.
     *
     * If the Altia interface process is an editor interface, it cannot be
     * terminated programmatically.  This insures against accidental loss of
     * design changes.
     *
     * This function has no return value.
     */
    extern void CALLP CALLTYPE altiaStopInterface(
#if AltiaFunctionPrototyping
    void
#endif
);

    /*
     * void altiaSuppressErrors(int yes):
     * --------------------------------------------------------------
     * This function is used to suppress error messages during all
     * base level API calls.  Use AtSuppressErrors() to suppress all
     * error messages for Toolkit and base library calls.  Pass
     * non-zero to suppress messages and 0 to turn them on again.
     */
    extern void CALLP CALLTYPE altiaSuppressErrors(
#if AltiaFunctionPrototyping
    int yes
#endif
);

    /*
     * void altiaRetryCount(int count):
     * --------------------------------------------------------------
     * Normally, this Altia client library tries to reconnect to the
     * Altia interface on every function call after connection has
     * been lost.  Each function only tries once, but the library will
     * permit an infinite number of connection retries by default.  
     * The retry count can be changed to something finite through a call
     * to altiaRetryCount() or set back to infinite by passing 0 as the
     * count parameter.
     * If the retry count is hit, the Altia client library performs an
     * exit(1) which aborts the program.  A successful connection resets
     * the running count so an abort only occurs if the retry count is
     * hit during back-to-back failures.
     */
    extern void CALLP CALLTYPE altiaRetryCount(
#if AltiaFunctionPrototyping
    int count
#endif
);

    /*
     * void altiaSuppressExit(int yes):
     * --------------------------------------------------------------
     * This function allows the caller to suppress the program exit when
     * the API retry count is non-zero (see altiaRetryCount()) and something
     * goes wrong.  For a DeepScreen API, this is when the DeepScreen window
     * closes.  This gives the user more control over program execution in a
     * completely embedded application.  All API functions will return -1
     * after the window closes.  The caller can "reset" the API at this
     * point by doing an explicit altiaDisconnect().   A subsequent
     * altiaConnect(), AtOpenConnection(), or AtStartInterface() will
     * open a new version of the DeepScreen window and connect to it.
     */
    extern void CALLP CALLTYPE altiaSuppressExit(
#if AltiaFunctionPrototyping
    int yes
#endif
);

    /*
     * void altiaSleep(unsigned long millisecs):
     * --------------------------------------------------------------
     * This is a convenience routine that implements a millisecond
     * sleep.  It allows clients to execute a millisecond sleep independent
     * of actual operating system architecture.  While sleeping, all
     * client activity is suspended. The accuracy of this sleep is dependent
     * on system load and the resolution of the system's clock functions.
     */
    extern void CALLP CALLTYPE altiaSleep(
#if AltiaFunctionPrototyping
    unsigned long millisecs
#endif
);

    /*
     * CONST AltiaCharType *altiaNativeStr2AltiaStr(
     *                                        CONST AltiaNativeChar *str,
     *                                        AltiaCharType *buffer,
     *                                        int bufferLength)
     * --------------------------------------------------------------
     * Because of UNICODE system support, it may be necessary for user code to
     * convert a wide character string (coming from a main argv list for
     * example) into a string coming into an API function (like
     * altiaConnect()).  The return will be the address of the original
     * string if the native char type is the same as the character type
     * expected by the API.  Otherwise, the original string is converted into
     * the supplied buffer and the buffer's address is returned.
     */ 
    extern CONST AltiaCharType* CALLP CALLTYPE altiaNativeStr2AltiaStr(
#if AltiaFunctionPrototyping
    CONST AltiaNativeChar *str,
    AltiaCharType *buffer,
    int bufferLength
#endif
);

    /*
     * CONST AltiaNativeChar *altiaStr2NativeStr(CONST AltiaCharType *str,
     *                                           AltiaNativeChar *buffer,
     *                                           int bufferLength)
     * --------------------------------------------------------------
     * Because of UNICODE system support, it may be necessary for user code to
     * convert an 8-bit character string to a wide character string for
     * putting in an argv list for AtOpenConnection() or AtStartInterface().
     * The return will be the address of the original string if the native
     * char type is the same as the character type expected by the API.
     * Otherwise, the original string is converted into the supplied buffer
     * and the buffer's address is returned.
     */ 
    extern CONST AltiaNativeChar* CALLP CALLTYPE altiaStr2NativeStr(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *str,
    AltiaNativeChar *buffer,
    int bufferLength
#endif
);

    /*
     * int _altiaDoNotDisconnect(void):
     * --------------------------------------------------------------
     * If the application does not want the Altia editor to disconnect
     * it when the "Disconnect All Clients" option is chosen from the
     * editor's "Client" menu, the application can call this function.
     * This function should not be used with application code intended
     * to run with Altia Runtime or DeepScreen generated code.
     *
     * If a communications failure occurs, this function returns -1.
     * Otherwise, 0 is returned.
     */
    extern int CALLP CALLTYPE _altiaDoNotDisconnect(
#if AltiaFunctionPrototyping
    void
#endif
);

#if defined(WIN16)||defined(WIN32)
    /*
     * int altiaFetchArgcArgv(void *hInstance, AltiaNativeChar *lpCmdLIne,
     *                        int argsize, AltiaNativeChar **argv, int *argc);
     * --------------------------------------------------------------
     * This is a convenience routine that takes the command line that is
     * passed to the application via the WinMain function and returns an
     * argument array and count that would have been passed to the application
     * via the Main function call.  The argv and argc values can then be
     * passed to the altia functions that require them.
     * Parameters:
     *    hInstance is the first parameter in the WinMain function it is
     *              used to get the executable name (argv[0]).
     *    lpCmdLine is the command line parameters passed to the application
     *              at startup.  Single or double quotes can be used to 
     *              put multiple tokens into a single argument.
     *    argsize   is the size of the array of character pointers that was
     *              passed in argv.
     *    argv      is the array of character pointers that will be set to 
     *              point the the individual arguments.  This is static data
     *              do not free it.
     *    argc      is the number of arguments that were found on the command
     *              line.
     * Return value:
     *    This function returns the actual number of arguments placed in 
     *    the argv array.  If this value and argc differ then not all of the
     *    arguments fit into the argv array.  
     *
     * Side Effects:
     *    This function replaces spaces with null characters in the string
     *    passed into lpCmdLine.  If you want to use lpCmdLine again make
     *    a copy and pass that copy to this function, but be sure that
     *    the copy will be around afterward since argv will point into it.
     */
    extern int CALLP CALLTYPE altiaFetchArgcArgv(
#if AltiaFunctionPrototyping
    void *hInstance,
    AltiaNativeChar *lpCmdLIne,
    int argsize,
    AltiaNativeChar **argv,
    int *argc
#endif
);

    /*
     * AtConnectId altiaStartInterface(const AltiaCharType *dsnFile,
     *                                 int editMode, 
     *                                 const AltiaNativeChar *cmdLine);
     * --------------------------------------------------------------
     * This is a convenience routine that starts Altia Design and connects
     * to it without having to worry about c style arguments.  
     * Parameters:
     *    dsnFile   is the name of the Altia Design file that you want Altia
     *              to open when it starts up.
     *    editMode  is a 1 if you want to start Altia Design in edit mode else
     *              is should be a 0 if you want it to start in run mode.
     *    cmdLine   is the command line parameters passed to Altia
     *              at startup.  Single or double quotes can be used to 
     *              put multiple tokens into a single argument.
     *              line.
     * altiaStartInterface() returns a non-negative connection id if it is
     * successful.  It returns -1 if it fails and messages are sent to
     * stderr to help diagnose the problem.
     */
    extern int CALLP CALLTYPE altiaStartInterface(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *dsnFile,
    int editMode,
    CONST AltiaNativeChar *cmdLine
#endif
);
#endif

    /*
     * void altiaRemoveConnect(void):
     * --------------------------------------------------------------
     * NOT NORMALLY FOR USE BY APPLICATION PROGRAMS.  This function is
     * provided for use by Toolkit routines.  It removes the domain socket
     * file or pipe files associated with the selected connection if the
     * connection has been closed with a previous call to altiaDisconnect().
     * This function must be called before an altiaClearConnect().
     */
    extern void altiaRemoveConnect(
#if AltiaFunctionPrototyping
    void
#endif
);

    /*
     * void altiaClearConnect(void):
     * --------------------------------------------------------------
     * NOT NORMALLY FOR USE BY APPLICATION PROGRAMS.  This function is
     * provided for use by Toolkit routines.  It clears internal data
     * structures for the selected connection if the connection has been
     * closed with a previous call to altiaDisconnect().
     */
    extern void altiaClearConnect(
#if AltiaFunctionPrototyping
    void
#endif
);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif


/******************************************************************
 * Below is the default base for the pathname used to create pipes and
 * domain sockets used to connect Altia to an application program.  For
 * host-based applications, the connection is socket based.  For emulator-
 * based applications, pipes are used.  The domain socket name created
 * for host-to-host communications is built by appending the host's
 * name (as returned by gethostname(2)) to this default basename.
 * The pipe names created for host-to-emulator communications are
 * built by appending the suffixes described immediately below
 * to this default basename.
 ******************************************************************/

#define DEFAULTBASENAME "/usr/tmp/vSe."
#define MAXBASELENGTH   20

/* For Win32, Domain sockets not supported so define a default
 * socket number.
 */
#undef DEFAULT_PORT_STRING
#define DEFAULT_PORT_STRING "5100"

/* For Win32, DDE connection we need to define a default Service, Topic,
 * and Item names
 */
#undef DEFAULT_DDESERVICE 
#define DEFAULT_DDESERVICE "AltDDE"

#undef DEFAULT_DDETOPIC
#define DEFAULT_DDETOPIC "ClientData"

#undef DEFAULT_DDEITEM
#define DEFAULT_DDEITEM "Binary"

#define ALTIA_DOUBLE_TEST_VALUE 0.1234567

/******************************************************************
 * The following suffixes are added to the basename (default or user
 * provided) to create the named pipes for emulation-based application
 * programs.  One takes data from the host to the application program.
 * The other takes data from the application program to the host.
 ******************************************************************/
#define PIPEFROMHOSTSUFFIX "fRoMhOsT"
#define PIPETOHOSTSUFFIX "tOhOsT"
#define MAXSUFFIXLENGTH  30


/******************************************************************
 * Below are the message types/flags/masks for transactions between
 * Altia and an application program.  It is not important that application
 * writers be familiar with these.  They are here as a reference for
 * internal use.
 ******************************************************************/

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
#define LastEventMsg         0x11
#define UndefinedMsg         0x7f

/* Flags for the type field in a client/Altia message */
#define ErrorInMsg      0x80

/* Masks for the type field in a client/Altia message */
#define MsgMask         0x7f
#define FlagsMask       0x80
#define ErrorInMsgMask  0x80

/* Flags used in protecting API calls for multi-threaded apps */
#ifndef ALTIA_SEM_BASE
#define ALTIA_SEM_BASE 0
#define ALTIA_SEM_KIT 1
#define ALTIA_SEM_CBK 2
#define ALTIA_SEM_START 3
#define ALTIA_SEM_COUNT 4
#endif


/******************************************************************
 *
 *      ALTIA TOOLKIT DATATYPES/FUNCTIONS/PROCEDURES DECLARATIONS
 *
 *                     Declarations are for C/C++ 
 *
 *  Toolkit procedures and functions extend the capabilities of the
 *  base library to support callback style programming.  That is to
 *  say, you can register a function with the toolkit that should be
 *  called when some event occurs such as the receipt of an event from
 *  an Altia interface or maybe a timeout. This library also offers
 *  built in support for multiple Altia interface connections.
 * 
 ******************************************************************/

/* The following are data types or constants consumed or returned by
 * toolkit functions and procedures.
 */
typedef void*              AtPointer;
typedef int                AtConnectId;
typedef int                AtInputId;
typedef int                AtTimerId;

typedef unsigned long      AtEventMask;
#define AtEventAltia       1
#define AtEventTimer       2
#define AtEventInput       4
#define AtEventAll         (AtEventAltia | AtEventTimer | AtEventInput)

typedef unsigned long      AtInputType;
#define AtInputNone        0L
#define AtInputRead        (1L<<0)
#define AtInputWrite       (1L<<1)
#define AtInputExcept      (1L<<2)

/*
 * To manage multiple connections within the Toolkit library, the
 * different toolkit subsystems, which reside in different sources,
 * need access to the connection information.  It is stored in an
 * array of structures of type AtConnectStruct.  This information
 * is not relevant to users of the Toolkit.  It's declared here to
 * keep its definition centralized.
 */
typedef struct _AtConnectStruct
{
    char *name;
    int  connect;
    int  pid;
} AtConnectStruct;

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

    /*
     * typedef void (*AtCallbackProc)(AtConnectId connection,
     *                                const AltiaCharType *eventName,
     *                                AltiaEventType eventValue,
     *                                AtPointer clientData):
     * --------------------------------------------------------------
     * Description of the syntax for Altia interface event callbacks.
     * If you want to write a callback routine to be notified when a
     * particular Altia event is received, it must take parameters
     * as shown and return nothing.  It will be passed the id of the
     * Altia interface connection from which the event was received,
     * the name of the event, its value, and a piece of custom data
     * that may be specified at the time the callback is added to
     * the callback list (see AtAddCallback()).
     */
typedef void (CALLP CALLTYPE *AtCallbackProc)(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    CONST AltiaCharType *eventName,
    AltiaEventType eventValue,
    AtPointer clientData
#endif
);

    /*
     * typedef void (*AtTextProc)(AtConnectId connection,
     *                                const AltiaCharType *eventName,
     *                                const AltiaCharType *string,
     *                                AtPointer clientData):
     * --------------------------------------------------------------
     * Description of the syntax for Altia interface text event callbacks.
     * If you want to write a callback routine to be notified when a
     * a complete string is received from a particular Altia event,
     * it must take parameters as shown and return nothing.  It will be
     * passed the id of the Altia interface connection from which events
     * were received, the name of the event, the null-terminated string 
     * containing the most recent event values, casted into characters,
     * since a prior null value was received, and a piece of custom data
     * that may be specified at the time the callback is added to the
     * callback list (see AtAddTextCallback()).
     *
     * Normally, text callbacks are set up to receive strings from Altia
     * text i/o objects. An alternative method is to simply get the text
     * with AtGetText() (see description below) when it is necessary to do so.
     *
     * To learn more about text i/o objects, open the Help menu available
     * from a Models Library View and select Textio Models. Actual text
     * input model examples are available in the $ALTIAHOME/models/textio.dsn
     * and $ALTIAHOME/models/gui.dsn Models Libraries.
     */
typedef void (CALLP CALLTYPE *AtTextProc)(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    CONST AltiaCharType *eventName,
    CONST AltiaCharType *string,
    AtPointer clientData
#endif
);

    /*
     * typedef void (*AtInputProc)(AtPointer clientData,
     *                             int source, AtInputId inputId):
     * --------------------------------------------------------------
     * Description of the syntax for I/O descriptor callback routines.
     * If you want to be notified of the availability of data from an
     * interprocess communications source such as a pipe, stdin, or a
     * socket, you can set up an I/O callback routine on the given source.
     * You could also set up a callback routine on a writable I/O descriptor
     * which usually means you want to be notified when the descriptor
     * can be written to.  See AtAddInput() for more details.  When
     * called, an I/O callback receives a custom data parameter, the
     * I/O descriptor that triggered the callback, and the input id
     * that is used to reference the callback for removing it from the
     * callback list, if desired (See AtRemoveInput()).
     */
typedef void (CALLP CALLTYPE *AtInputProc)(
#if AltiaFunctionPrototyping
    AtPointer clientData,
    int source,
    AtInputId inputId
#endif
);

    /*
     * typedef void (*AtTimerProc)(AtPointer clientData, unsigned long msecs,
     *                             AtTimerId timerId):
     * --------------------------------------------------------------
     * Description of the syntax for timer callback routines.  If you
     * want to be notified at timed intervals, you can set up a callback
     * routine to accomplish this.  The callback routine is registered
     * using AtAddTimer().  When the designated interval has elapsed,
     * the callback routine is invoked.  It is passed a custom data
     * parameter, the interval in milliseconds associated with the
     * timeout, and the timer id associated with the callback.  The
     * callback is removed from the timeout queue prior to execution
     * of the callback.  It can be reinstated with a call to AtAddTimer().
     */
typedef void (CALLP CALLTYPE *AtTimerProc)(
#if AltiaFunctionPrototyping
    AtPointer clientData,
    unsigned long msecs,
    AtTimerId timerId
#endif
);


    /*
     * int AtSendEvent(AtConnectId connection, const AltiaCharType *eventName, 
                       AltiaEventType eventValue):
     * --------------------------------------------------------------------
     * Toolkit function to send an event to a particular Altia interface
     * session.  THIS ROUTINE MIMICS altiaSendEvent() except that it takes
     * the additional connection id parameter as returned by
     * AtOpenConnection(). It is a convenience routine provided for those
     * who are using the toolkit capabilities of the library.
     */
extern int CALLP CALLTYPE AtSendEvent(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    CONST AltiaCharType *eventName,
    AltiaEventType eventValue
#endif
);

    /*
     * int AtSendText(AtConnectId connection, const AltiaCharType *eventName,
     *                const AltiaCharType *text):
     * --------------------------------------------------------------------
     * Toolkit function to send a string of character events to a particular
     * Altia interface session.  Typically, the event name is the "putchar"
     * function for an Altia text output object.  THIS ROUTINE MIMICS
     * altiaSendText() except that it takes the additional connection id
     * parameter as returned by AtOpenConnection(). It is a convenience
     * routine provided for those who are using the toolkit capabilities
     * of the library.
     */
extern int CALLP CALLTYPE AtSendText(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    CONST AltiaCharType *eventName,
    CONST AltiaCharType *text
#endif
);

    /*
     * int AtGetText(AtConnectId connection, const AltiaCharType *textName,
     *               AltiaCharType *buffer, int bufSize):
     * --------------------------------------------------------------------
     * Toolkit function to get a character string from a text i/o object
     * within an Altia interface session.  THIS ROUTINE MIMICS altiaGetText()
     * except that it takes the additional connection id parameter as returned
     * by AtOpenConnection(). It is a convenience routine provided for those
     * who are using the toolkit capabilities of the library.
     */
extern int CALLP CALLTYPE AtGetText(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    CONST AltiaCharType *textName,
    AltiaCharType *buffer,
    int bufSize
#endif
);

    /*
     * int AtGetTextExternalConn(AtConnectId connection,
     *                           const AltiaCharType *externalInputName,
     *                           const AltiaCharType *externalOutputName,
     *                           AltiaCharType *buffer, int bufSize):
     * --------------------------------------------------------------------
     * Toolkit function to get a character string from a text i/o object
     * within an Altia interface session.  THIS ROUTINE MIMICS
     * altiaGetTextExternalConn() except that it takes the additional
     * connection id parameter as returned by AtOpenConnection(). It is a
     * convenience routine provided for those who are using the toolkit
     * capabilities of the library.
     */
extern int CALLP CALLTYPE AtGetTextExternalConn(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    CONST AltiaCharType *inName,
    CONST AltiaCharType *outName,
    AltiaCharType *buffer,
    int bufSize
#endif
);

    /*
     * int AtCacheOutput(AtConnectId connection, int onoff):
     * --------------------------------------------------------------------
     * Toolkit function to turn event caching on (onoff=1) or off (onoff=0).
     * THIS ROUTINE MIMICS altiaCacheOutput() except that it takes the
     * additional connection id parameter as returned by AtOpenConnection().
     * It is a convenience routine provided for those who are using the
     * toolkit capabilities of the library.
     */
extern int CALLP CALLTYPE AtCacheOutput(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    int onoff
#endif
);

    /*
     * int AtSyncClients(AtConnectId connection, int onoff):
     * --------------------------------------------------------------------
     * Toolkit function to turn client syncing on (onoff=1) or off (onoff=0).
     * THIS ROUTINE MIMICS altiaSyncClients() except that it takes the
     * additional connection id parameter as returned by AtOpenConnection().
     * It is a convenience routine provided for those who are using the
     * toolkit capabilities of the library.
     */
extern int CALLP CALLTYPE AtSyncClients(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    int onoff
#endif
);
    /*
     * int AtFlushOutput(AtConnectId connection):
     * --------------------------------------------------------------------
     * Toolkit function to flush output event cache.
     * THIS ROUTINE MIMICS altiaFlushOutput() except that it takes the
     * additional connection id parameter as returned by AtOpenConnection().
     * It is a convenience routine provided for those who are using the
     * toolkit capabilities of the library.
     */
extern int CALLP CALLTYPE AtFlushOutput(
#if AltiaFunctionPrototyping
    AtConnectId connection
#endif
);

    /*
     * int AtPollEvent(AtConnectId connection, const AltiaCharType *eventName, 
     *                 AltiaEventType *eventValueOut):
     * --------------------------------------------------------------------
     * Toolkit function to poll the current value of an event within a
     * particular Altia interface session.  THIS ROUTINE MIMICS
     * altiaPollEvent() except that it takes the additional connection id
     * parameter as returned by AtOpenConnection(). It is a convenience
     * routine provided for those who are using the toolkit
     * capabilities of the library.
     */
extern int CALLP CALLTYPE AtPollEvent(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    CONST AltiaCharType *eventName,
    AltiaEventType *eventValueOut
#endif
);

    /*
     * int AtLocalPollEvent(AtConnectId connection,
     *                      const AltiaCharType *eventName, 
     *                      AltiaEventType *eventValueOut):
     * --------------------------------------------------------------------
     * Toolkit function to poll the local queue for the current value of an
     * event within a particular Altia interface session.  THIS ROUTINE MIMICS
     * altiaLocalPollEvent() except that it takes the additional connection id
     * parameter as returned by AtOpenConnection(). It is a convenience
     * routine provided for those who are using the toolkit
     * capabilities of the library.
     */
extern int CALLP CALLTYPE AtLocalPollEvent(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    CONST AltiaCharType *eventName,
    AltiaEventType *eventValueOut
#endif
);

    /*
     * int AtNextEvent(AtConnectId connection,
     *                 const AltiaCharType **eventNameOut, 
     *                 AltiaEventType *eventValueOut):
     * --------------------------------------------------------------------
     * Toolkit function gets the next available event that was routed to us
     * by the selected Altia interface.  THIS ROUTINE MIMICS
     * altiaNextEvent() except that it takes the additional connection id
     * parameter as returned by AtOpenConnection(). It is a convenience
     * routine provided for those who are using the toolkit
     * capabilities of the library.
     */
extern int CALLP CALLTYPE AtNextEvent(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    CONST AltiaCharType **eventNameOut,
    AltiaEventType *eventValueOut
#endif
);

    /*
     * int AtCheckEvent(AtConnectId connection, const AltiaCharType *eventName, 
     *                  AltiaEventType *eventValueOut):
     * --------------------------------------------------------------------
     * Toolkit function to get a particular event if one is available.
     * THIS ROUTINE MIMICS  altiaCheckEvent() except that it takes the
     * additional connection id parameter as returned by AtOpenConnection().
     * It is a convenience routine provided for those who are using the
     * toolkit capabilities of the library.
     */
extern int CALLP CALLTYPE AtCheckEvent(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    CONST AltiaCharType *eventName,
    AltiaEventType *eventValueOut
#endif
);

    /*
     * int AtPending(AtConnectId connection):
     * --------------------------------------------------------------------
     * Toolkit function gets a count of next available events that were routed
     * to us by the selected Altia interface.  THIS ROUTINE MIMICS
     * altiaPending() except that it takes the additional connection id
     * parameter as returned by AtOpenConnection(). It is a convenience
     * routine provided for those who are using the toolkit
     * capabilities of the library.
     */
extern int CALLP CALLTYPE AtPending(
#if AltiaFunctionPrototyping
    AtConnectId connection
#endif
);

    /*
     * int AtLocalPending(AtConnectId connection):
     * --------------------------------------------------------------------
     * Toolkit function gets a count of locally available events that were
     * previously routed to us by the selected Altia interface.  THIS ROUTINE
     * MIMICS altiaLocalPending() except that it takes the additional
     * connection id parameter as returned by AtOpenConnection(). It is a
     * convenience routine provided for those who are using the toolkit
     * capabilities of the library.
     */
extern int CALLP CALLTYPE AtLocalPending(
#if AltiaFunctionPrototyping
    AtConnectId connection
#endif
);

    /*
     * int AtSelectEvent(AtConnectId connection, const AltiaCharType *eventName):
     * --------------------------------------------------------------------
     * Toolkit function to select to receive the named event from the
     * given Altia interface.  THIS ROUTINE MIMICS altiaSelectEvent()
     * except that it takes the additional connection id
     * parameter as returned by AtOpenConnection(). It is a convenience
     * routine provided for those who are using the toolkit
     * capabilities of the library.
     */
extern int CALLP CALLTYPE AtSelectEvent(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    CONST AltiaCharType *eventName
#endif
);

    /*
     * int AtSelectAllEvents(AtConnectId connection):
     * --------------------------------------------------------------------
     * Toolkit function to select to receive all events from the given
     * Altia interface.  THIS ROUTINE MIMICS altiaSelectAllEvents()
     * except that it takes the additional connection id
     * parameter as returned by AtOpenConnection(). It is a convenience
     * routine provided for those who are using the toolkit
     * capabilities of the library.
     */
extern int CALLP CALLTYPE AtSelectAllEvents(
#if AltiaFunctionPrototyping
    AtConnectId connection
#endif
);

    /*
     * int AtSelectAllExternEvents(AtConnectId connection):
     * --------------------------------------------------------------------
     * Toolkit function to select to receive all external events from the given
     * Altia interface.  THIS ROUTINE MIMICS altiaSelectAllExternEvents()
     * except that it takes the additional connection id
     * parameter as returned by AtOpenConnection(). It is a convenience
     * routine provided for those who are using the toolkit
     * capabilities of the library.
     */
extern int CALLP CALLTYPE AtSelectAllExternEvents(
#if AltiaFunctionPrototyping
    AtConnectId connection
#endif
);

    /*
     * int AtUnselectEvent(AtConnectId connection,
     *                     const AltiaCharType *eventName):
     * --------------------------------------------------------------------
     * Toolkit function to unselect receipt of the named event from the
     * given Altia interface.  THIS ROUTINE MIMICS altiaUnselectEvent()
     * except that it takes the additional connection id
     * parameter as returned by AtOpenConnection(). It is a convenience
     * routine provided for those who are using the toolkit
     * capabilities of the library.
     */
extern int CALLP CALLTYPE AtUnselectEvent(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    CONST AltiaCharType *eventName
#endif
);

    /*
     * int AtUnselectAllEvents(AtConnectId connection):
     * --------------------------------------------------------------------
     * Toolkit function to unselect receipt of all events from the given
     * Altia interface.  THIS ROUTINE MIMICS altiaUnselectAllEvents()
     * except that it takes the additional connection id
     * parameter as returned by AtOpenConnection(). It is a convenience
     * routine provided for those who are using the toolkit
     * capabilities of the library.
     */
extern int CALLP CALLTYPE AtUnselectAllEvents(
#if AltiaFunctionPrototyping
    AtConnectId connection
#endif
);

    /*
     * int AtUnselectAllExternEvents(AtConnectId connection):
     * --------------------------------------------------------------------
     * Toolkit function to unselect receipt of all external events from given
     * Altia interface.  THIS ROUTINE MIMICS altiaUnselectAllExternEvents()
     * except that it takes the additional connection id
     * parameter as returned by AtOpenConnection(). It is a convenience
     * routine provided for those who are using the toolkit
     * capabilities of the library.
     */
extern int CALLP CALLTYPE AtUnselectAllExternEvents(
#if AltiaFunctionPrototyping
    AtConnectId connection
#endif
);

    /*
     * int AtEventSelected(AtConnectId connection, const AltiaCharType *name):
     * --------------------------------------------------------------------
     * Toolkit function to check if the named event is selected to be received
     * from the given Altia interface.  THIS ROUTINE MIMICS
     * altiaEventSelected() except that it takes the additional connection id
     * parameter as returned by AtOpenConnection(). It is a convenience
     * routine provided for those who are using the toolkit
     * capabilities of the library.
     */
extern int CALLP CALLTYPE AtEventSelected(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    CONST AltiaCharType *name
#endif
);

    /*
     * int AtMoveObject(AtConnectId connectId, int objectNumber,
     *                  int xOffset, int yOffset):
     * --------------------------------------------------------------------
     * Toolkit function to move an object in the Altia interface relative to
     * its current position.  THIS ROUTINE MIMICS altiaMoveObject()
     * except that it takes the additional connection id
     * parameter as returned by AtOpenConnection(). It is a convenience
     * routine provided for those who are using the toolkit
     * capabilities of the library.
     */
extern int CALLP CALLTYPE AtMoveObject(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int objectNumber,
    int xOffset,
    int yOffset
#endif
);

    /*
     * int AtInputNumber(AtConnectId connection):
     * --------------------------------------------------------------------
     * Toolkit function to get the input file descriptor number for the 
     * given Altia interface connection.  THIS ROUTINE MIMICS
     * altiaInputNumber() except that it takes the additional connection id
     * parameter as returned by AtOpenConnection(). It is a convenience
     * routine provided for those who are using the toolkit
     * capabilities of the library.
     */
extern int CALLP CALLTYPE AtInputNumber(
#if AltiaFunctionPrototyping
    AtConnectId connection
#endif
);

    /*
     * int AtOutputNumber(AtConnectId connection):
     * --------------------------------------------------------------------
     * Toolkit function to get the output file descriptor number for the 
     * given Altia interface connection.  THIS ROUTINE MIMICS
     * altiaOutputNumber() except that it takes the additional connection id
     * parameter as returned by AtOpenConnection(). It is a convenience
     * routine provided for those who are using the toolkit
     * capabilities of the library.
     */
extern int CALLP CALLTYPE AtOutputNumber(
#if AltiaFunctionPrototyping
    AtConnectId connection
#endif
);

    /*
     * void AtSuppressErrors(int yes):
     * --------------------------------------------------------------
     * This function is used to suppress error messages during all
     * Toolkit and base level API calls.  Use altiaSuppressErrors() to
     * only suppress error messages for base library calls.  Pass
     * non-zero to suppress messages and 0 to turn them on again.
     * This function does not take a connection id.  It suppresses
     * errors for all connections until it is called with 0.
     */
extern void CALLP CALLTYPE AtSuppressErrors(
#if AltiaFunctionPrototyping
    int yes
#endif
);

    /*
     * void AtRetryCount(AtConnectId connection, int count):
     * --------------------------------------------------------------------
     * Toolkit function to set retry count for given Altia interface
     * connection.  THIS ROUTINE MIMICS altiaRetryCount() except that it
     * takes the additional connection id parameter as returned by
     * AtOpenConnection(). It is a convenience routine provided for
     * those who are using the toolkit capabilities of the library.
     */
extern void CALLP CALLTYPE AtRetryCount(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    int count
#endif
);

    /*
     * void AtSuppressExit(AtConnectId connection, int yes):
     * --------------------------------------------------------------
     * Toolkit function to suppress exits when retry count is non-zero.
     * THIS ROUTINE MIMICS altiaSuppressExit() except that it takes the
     * additional connection id parameter as returned by AtOpenConnection().
     * It is a convenience routine provided for those who are using the
     * toolkit capabilities of the library.
     */
extern void CALLP CALLTYPE AtSuppressExit(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    int yes
#endif
);

    /*
     * void AtDispatchEvent(AtConnectId connection,
     *                      const AltiaCharType *eventName, 
     *                      AltiaEventType eventValue):
     * --------------------------------------------------------------------
     * Toolkit function to invoke callbacks for an Altia interface event that
     * was received by an explicit altiaNextEvent() or AtNextEvent() call.
     * In almost all cases, this routine would be called if altiaNextEvent()
     * or AtNextEvent() fetches an event that the calling procedure doesn't
     * know how to handle.  AtDispatchEvent() could then be called to
     * dispatch the event and its value to any handlers that are registered
     * for it (see AtAddCallback() for registering callbacks).
     */
extern void CALLP CALLTYPE AtDispatchEvent(
#if AltiaFunctionPrototyping
    AtConnectId connection, CONST AltiaCharType *eventName, AltiaEventType eventValue
#endif
);

    /*
     * AtConnectId AtOpenConnection(const AltiaCharType *portName,
     *                              const AltiaCharType *optionName,
     *                              int argc, const AltiaNativeChar *argv[]):
     * --------------------------------------------------------------------
     * Toolkit function to open a connection to an Altia interface.  This
     * function returns a connection id that serves as a parameter to most
     * other toolkit routines.  The portName parameter is analogous to the
     * portName parameter for altiaConnect().  Passing NULL opens the
     * default connection if it is not already opened.  Only one Altia
     * interface can operate on the default connection at any given time on
     * a single workstation.  To open a connection to an alternative Altia
     * interface, one must specify a port name for the connection.
     *
     * In addition, AtOpenConnection can be passed the command line argument
     * list for the program and it will search the list for a connection port
     * name if portName is NULL.  In this situation, if optionName is also
     * NULL, it will look for a command line sequence of "-port PORTNAME".
     * Otherwise, it will use the string pointed to by optionName as the
     * option differentiator instead of "-port". For example, if optionName
     * points to the string "-screen", then a command line sequence of
     * "-screen /tmp/screen" would set the connection port name to
     * "/tmp/screen".
     *
     * For workstation-based clients, a domain or network socket serves as
     * the connection facility between an Altia interface and its client
     * programs.  A domain socket is specified by using a filename syntax for
     * the port name (e.g., "/tmp/screen").  A network socket is specified
     * by using a "HOSTNAME:SOCKET" syntax where SOCKET may be a number or
     * it may be an alias name for a socket number as specified in the
     * /etc/services file.  If HOSTNAME is omitted, than the name of the
     * current host is assumed.  In all cases, the Altia session that the
     * client is connecting to must be started with the command line
     * argument "-port PORTNAME" where PORTNAME matches the connection port
     * name parsed by AtOpenConnection().  If AtOpenConnection() opens the
     * default connection because portName is NULL and no command line
     * arguments are matched, then the "-port PORTNAME" argument is
     * unnecessary for the Altia interface start-up.
     *
     * AtOpenConnection() will also parse the command line arguments list
     * for an occurence of "-retry VALUE" to set the count for retrying a
     * failed connection.  If no retry argument is found, the retry count
     * is set to 1 which means exit() will be called if a connection is
     * still down after 1 retry. Call AtRetryCount() to set the retry count
     * to something other than 1.
     */
extern AtConnectId CALLP CALLTYPE AtOpenConnection(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *portName,
    CONST AltiaCharType *optionName,
    int argc,
    CONST AltiaNativeChar *argv[]
#endif
);

    /*
     * void AtCloseConnection(AtConnectId connection):
     * --------------------------------------------------------------------
     * Toolkit routine to close an open Altia interface connection.  This is
     * usually not necessary.  The connection will close automatically when
     * the client program exits.  However, there may be circumstances that
     * require closing a connection.
     */
extern void CALLP CALLTYPE AtCloseConnection(
#if AltiaFunctionPrototyping
    AtConnectId connection
#endif
);

    /*
     * #ifndef DEEPSCREEN
     * AtConnectId AtStartInterface(const char *dsnFile, 
     *                            const char *rtmFile,
     *                            int editMode, int argc,
     *                            const char *argv[]):
     * #else
     * AtConnectId AtStartInterface(const AltiaCharType *dsnFile, 
     *                            const AltiaCharType *rtmFile,
     *                            int editMode, int argc,
     *                            const AltiaNativeChar *argv[]):
     * #endif
     * --------------------------------------------------------------------
     * This toolkit function will start a copy of the Altia Run-time interface
     * executable, load it with the design file whose path is given by
     * the string pointed to by dsnFile, configure it with information from
     * the Run-time configuration file whose path is given by the string
     * pointed to by rtmFile, and configure any additional attributes from
     * the argument list described by argc and argv.
     *
     * After the interface is started, a connection to it will be established
     * and the id for this connection is the return value of this function.
     *
     * The dsnFile parameter must be non-NULL and point to a valid Altia
     * Interface design file.  If the file doesn't exist or isn't a valid
     * design file, this function may fail in mysterious ways or a subsequent
     * call to send or receive events from the interface will fail.
     *
     * The rtmFile parameter may be NULL.  If it is, a Run-time configuration
     * file name will be constructed from the design file name by replacing
     * the last suffix of the design file with ".rtm" or by simply adding 
     * ".rtm" if no suffix is found.  A non-existant Run-time configuration 
     * file will not cause a fatal error.  The Altia Run-time interface will
     * simply configure itself to a default specification.
     *
     * Additional parameters may be specified with an array of string pointers
     * and a count giving the number of valid entries in the array.  This
     * array and count may be the standard argc and argv parameters passed to
     * the main() function or they can be constructed from within the program
     * prior to the call to AtStartInterface().  Each entry will be passed
     * as a positional parameter to the Altia Run-time executable.  See
     * other documentation on the parameters accepted by an Altia interface.
     *
     * AtStartInterface() will construct a unique file name for the domain
     * socket to be used for the Altia interface connection.  If a "-port"
     * entry is found in the supplied argv array, the next entry in the
     * argv array will be used for constructing a port name; however, the
     * name may get customized in an attempt to guarantee its uniqueness.
     * After an AtStartInterface() call successfully completes, the port
     * name actually used can be retrieved with a call to AtGetPortName().
     * That function is described later in this file.
     *
     * AtStartInterface() will also parse the argv list for the entries
     * "-retry" followed by "VALUE" to set the count for retrying a
     * failed connection.  If no retry argument is found, the retry count
     * is set to 0 which means an unlimited number of transmission failures
     * are allowed.  If desired, AtRetryCount() can be called to set the
     * retry count to something other than 0 after AtStartInterface()
     * completes.
     *
     * AtStartInterface() actually attempts to fork and execute the Altia
     * Run-time executable, altiart.out.  If the environment variable
     * ALTIAHOME is set, it uses it to construct a path to altiart.out
     * of the form $ALTIAHOME/bin/altiart.out.  If ALTIAHOME is not set,
     * altiart.out is executed with no path information in the hopes that
     * the environment's path information specifies the directory containing
     * altiart.out.
     *
     * An interface and connection started with AtStartInterface() can be
     * terminated with a call to AtStopInterface().  AtStopInterface() will
     * close the connection and then attempt to kill the interface.  If
     * AtCloseConnection() is called, the interface will be left running and
     * only the connection will be closed.
     * 
     * AtStartInterface() returns a non-negative connection id if it is
     * successful.  It returns -1 if it fails and messages are sent to
     * stderr to help diagnose the problem.
     *
     * AtStartInterface() only works properly for socket-based interface
     * connections.  This is the type of connection that is established
     * when a program is linked with liblan.a.  AtStartInterface() will
     * fail for programs linked with libpipe.a - it attempts to establish an
     * Altia interface connection with pipes instead of a socket.
     *
     * -----------------------------
     * SPECIAL DEEPSCREEN USAGE NOTE
     * -----------------------------
     * The DeepScreen version of AtStartInterface() takes arguments
     * that reference wide character strings if the UNICODE version
     * of the Altia API is requested for a UNICODE target.  To compile
     * DeepScreen generated code as well as application code for this
     * situation, the preprocessor macros UNICODE, ALTIAUNICODEAPI
     * and DEEPSCREEN must be defined at compile time.
     *
     * The AtStartInterface() in a TCP/IP socket or DDE version of the
     * Altia API (which communicates with Altia Design, Altia Runtime
     * or Altia FacePlate) always takes arguments that reference regular
     * 8-bit character strings even for a UNICODE version of the API
     * library.  To compile application code for this situation, the
     * preprocessor macros UNICODE and ALTIAUNICODEAPI must be defined
     * at compile time.  The preprocessor macro DEEPSCREEN should NOT
     * be defined at compile time.
     */
extern AtConnectId CALLP CALLTYPE AtStartInterface(
#if AltiaFunctionPrototyping
#ifndef DEEPSCREEN
    CONST char *dsnFile,
    CONST char *rtmFile,
    int editMode,
    int argc,
    CONST char *argv[]
#else
    CONST AltiaCharType *dsnFile,
    CONST AltiaCharType *rtmFile,
    int editMode,
    int argc,
    CONST AltiaNativeChar *argv[]
#endif
#endif
);

    /*
     * const AltiaCharType *AtGetPortName(AtConnectId connection):
     * --------------------------------------------------------------------
     * Toolkit routine that returns a pointer to the port name for the given
     * Altia interface connection.  The port name is the basis for the
     * socket or pipe name used to establish the connection.  It could be
     * used by another program as the portName parameter to an altiaConnect()
     * or AtOpenConnection() call if the program wished to connect to the
     * same Altia interface session.
     * 
     * The string returned by AtGetPortName() is the internal copy kept by
     * the toolkit library.  It should NOT be modified in any way.  If
     * modification is necessary, a copy of it should be made by the
     * calling routine.
     */
extern CONST AltiaCharType* CALLP CALLTYPE AtGetPortName(
#if AltiaFunctionPrototyping
    AtConnectId connection
#endif
);

    /*
     * void AtStopInterface(AtConnectId connection):
     * --------------------------------------------------------------------
     * Toolkit routine to close an opened Altia interface connection and kill
     * the associated Altia Run-time interface if it was originally started
     * with a call to AtStartInterface().  If the connection was originally
     * started by a call to AtOpenConnection(), this function makes its best
     * attempt to kill the associated Altia interface by sending it an
     * "altiaQuit" event.  If the interface is configured in its standard
     * mode, it will receive this event and terminate as a result.
     *
     * If the Altia interface process is an editor interface, it cannot be
     * terminated programmatically.  This insures against accidental loss of
     * design changes.
     */
extern void CALLP CALLTYPE AtStopInterface(
#if AltiaFunctionPrototyping
    AtConnectId connection
#endif
);

    /*
     * void AtAddCallback(AtConnectId connection,
     *                    const AltiaCharType *eventName,
     *                    AtCallbackProc proc, AtPointer clientData):
     * --------------------------------------------------------------------
     * Toolkit routine to add a procedure to the Altia interface event
     * callback list.  When an Altia interface event is received from the
     * specified connection, and the event's name matches eventName,  the
     * callback routine given by proc will be invoked.  AtAddCallback()
     * makes the appropriate call to altiaSelectEvent() to insure that the
     * client program receives eventName events from the specified connection.
     * Special data may be passed to the callback procedure through the
     * clientData parameter, or it can be set to NULL.
     *
     * The syntax for the AtCallbackProc procedure type was described earlier.
     */
extern void CALLP CALLTYPE AtAddCallback(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    CONST AltiaCharType *eventName,
    AtCallbackProc proc,
    AtPointer clientData
#endif
);

    /*
     * void AtRemoveCallback(AtConnectId connection,
     *                    const AltiaCharType *eventName,
     *                    AtCallbackProc proc, AtPointer clientData):
     * --------------------------------------------------------------------
     * Toolkit routine to remove a procedure from the Altia interface event
     * callback list if it is currently in the list.  A procedure is only
     * removed if it was added to the list with exactly the same attributes
     * specified by connection, eventName, proc, and clientData.
     *
     * If there are no other callbacks in the list for this event,
     * altiaUnselectEvent() will be called for the given connection and
     * event name.
     */
extern void CALLP CALLTYPE AtRemoveCallback(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    CONST AltiaCharType *eventName,
    AtCallbackProc proc,
    AtPointer clientData
#endif
);

    /*
     * void AtAddTextCallback(AtConnectId connection,
     *                    const AltiaCharType *eventName,
     *                    AtTextProc proc, AtPointer clientData):
     * --------------------------------------------------------------------
     * Toolkit routine to add a procedure to the Altia interface event
     * callback list for receiving a string of events.  Instead of invoking
     * the associated callback for each individual event, the toolkit casts
     * the events into characters and buffers them internally until a null
     * character (i.e., end of string identifier) is received.  It then passes
     * the entire set of events as a string to the desired callback routine.
     * AtAddTextCallback() makes the appropriate call to altiaSelectEvent()
     * to insure that the client program receives eventName events from the
     * specified connection.  Special data may be passed to the callback
     * procedure through the clientData parameter, or it can be set to NULL.
     *
     * The syntax for the AtTextProc procedure type was described earlier.
     *
     * Normally, text callbacks are set up to receive strings from Altia
     * text i/o objects. An alternative method is to simply get the text
     * with AtGetText() (see description above) when it is necessary to do so.
     *
     * To learn more about text i/o objects, open the Help menu available
     * from a Models Library View and select Textio Models. Actual text
     * input model examples are available in the $ALTIAHOME/models/textio.dsn
     * and $ALTIAHOME/models/gui.dsn Models Libraries.
     */
extern void CALLP CALLTYPE AtAddTextCallback(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    CONST AltiaCharType *eventName,
    AtTextProc proc,
    AtPointer clientData
#endif
);

    /*
     * void AtRemoveTextCallback(AtConnectId connection,
     *                    const AltiaCharType *eventName,
     *                    AtTextProc proc, AtPointer clientData):
     * --------------------------------------------------------------------
     * Toolkit routine to remove a string callback procedure from the Altia
     * interface event callback list if it is currently in the list.
     * A procedure is only removed if it was added to the list with exactly
     * the same attributes specified by connection, eventName, proc,
     * and clientData.
     *
     * If there are no other callbacks in the list for this event,
     * altiaUnselectEvent() will be called for the given connection and
     * event name.
     */
extern void CALLP CALLTYPE AtRemoveTextCallback(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    CONST AltiaCharType *eventName,
    AtTextProc proc,
    AtPointer clientData
#endif
);

    /*
     * void AtRemoveAllCallbacks(AtConnectId connection,
     *                           const AltiaCharType *eventName):
     * --------------------------------------------------------------------
     * Toolkit routine to remove all procedures from the Altia interface event
     * callback list that are for the specified event name and connection.
     * altiaUnselectEvent() will be called for the given connection and
     * event name. This routine will remove all normal callbacks,
     * text callbacks, and clone callbacks that have been registered for
     * the given event and connection.
     */
extern void CALLP CALLTYPE AtRemoveAllCallbacks(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    CONST AltiaCharType *eventName
#endif
);

    /*
     * AtInputId AtAddInput(int source, AtInputType inputType,
     *                      AtInputProc proc, AtPointer clientData):
     * --------------------------------------------------------------------
     * Toolkit routine to add a procedure to the I/O event callback list.
     * The procedure is executed when one of three possible events occur:
     *
     * If inputType is AtInputRead, the procedure is executed when data is
     * available for reading from the file descriptor specified by source.
     * In most cases, source would reference a named pipe, stdin, a socket,
     * or some special device that can be monitored by the select(2) operating
     * system call or equivalent.
     *
     * If inputType is AtInputWrite, the procedure is executed when data
     * can be written to the file descriptor specified by source.  In most
     * cases, source would reference a named pipe, stdout, stderr, a socket,
     * or some special device that can be monitored by the select(2) operating
     * system call or equivalent.
     *
     * If inputType is AtInputExcept, the procedure is executed when an
     * exception occurs on the file descriptor specified by source.  In most
     * cases, source would reference some special device that supports the
     * exception monitoring capabilities specified by the select(2) operating
     * system call or equivalent.
     *
     * The procedure to call when the event occurs is specified by
     * proc.  Special data may be sent to the procedure through the
     * clientData parameter, or it can be set to NULL.
     *
     * An identification number is returned that identifies the callback
     * registration.  It can be used to remove the callback from the I/O
     * event callback list at a future time via AtRemoveInput().  If the 
     * return id is less than 0, the callback could not be added to the list.
     *
     * At this time, source must be a file descriptor with a value ranging
     * from 0 thru 31.
     */
extern AtInputId CALLP CALLTYPE AtAddInput(
#if AltiaFunctionPrototyping
    int source,
    AtInputType inputType,
    AtInputProc proc,
    AtPointer clientData
#endif
);

    /*
     * void AtRemoveInput(AtInputId id):
     * --------------------------------------------------------------------
     * Toolkit routine to remove the I/O event callback procedure, identified
     * by id, from the I/O event callback list.
     */
extern void CALLP CALLTYPE AtRemoveInput(
#if AltiaFunctionPrototyping
    AtInputId id
#endif
);

    /*
     * AtTimerId AtAddTimer(unsigned long interval,
     *                      AtTimerProc proc, AtPointer clientData):
     * --------------------------------------------------------------------
     * Toolkit routine to add a procedure to the timer event callback list.
     * The procedure, specified by proc, is executed when interval
     * milliseconds have expired since AtAddTimer() was called. 
     *
     * When the callback is invoked, it is removed from the timer event
     * callback list.  In other words, it is a one-shot deal.  The callback
     * can be reinstated into the list with an identical call to AtAddTimer()
     * from the callback itself, or from some other procedure.
     *
     * The procedure to call when the time expires is specified by
     * proc.  Special data may be sent to the procedure through the
     * clientData parameter, or it can be set to NULL.
     *
     * An identification number is returned that identifies the callback
     * registration.  It can be used to remove the callback from the timer
     * event callback list at a future time via AtRemoveTimer().  If the 
     * return id is less than 0, the callback could not be added to the list.
     *
     * The toolkit makes its best attempt at calling the procedure within
     * the specified interval, but accuracy is dependent on system load and
     * the time expended by other procedures executed by the program.
     */
extern AtTimerId CALLP CALLTYPE AtAddTimer(
#if AltiaFunctionPrototyping
    unsigned long interval,
    AtTimerProc proc,
    AtPointer clientData
#endif
);

    /*
     * void AtRemoveTimer(AtTimerId id):
     * --------------------------------------------------------------------
     * Toolkit routine to remove a timer event callback procedure, identified
     * by id, from the timer event callback list.  Since the callback
     * is removed from the list when its time interval expires, it is only
     * necessary to call AtRemoveTimer() if there is a need to remove a
     * timer event callback before its interval expires.
     */
extern void CALLP CALLTYPE AtRemoveTimer(
#if AltiaFunctionPrototyping
    AtTimerId id
#endif
);

    /*
     * void AtMainLoop(void):
     * --------------------------------------------------------------------
     * This toolkit function will loop indefinitely, processing pending
     * Altia interface events, timer events, and I/O events.  It uses
     * the select(2) or equivalent system call to suspend program execution
     * until some event is ready for processing.  This greatly reduces the
     * CPU usage of the program.  Event processing entails identifying the
     * pending events and invoking the appropriate callback functions that
     * have been registered with calls to AtAddCallback(), AtAddTimer(), and
     * AtAddInput().
     *
     * AtMainLoop() will typically never return control to its calling
     * procedure unless a fatal select(2) system call error occurs.
     *
     * Typically, AtMainLoop() is called from main() after all callbacks
     * have been registered and variables, file descriptors, etc. have been
     * initialized.
     */
extern void CALLP CALLTYPE AtMainLoop(
#if AltiaFunctionPrototyping
    void
#endif
);

    /*
     * AtEventMask AtAllPending(AtConnectId connection):
     * --------------------------------------------------------------------
     * Toolkit function to return a mask that describes the types of toolkit
     * events waiting for service, including any Altia interface events
     * pending on the connection specified by connection.  The mask returned
     * is an OR'ing of AtEventAltia (Altia interface events are pending),
     * AtEventTimer (timer events are pending), or AtEventInput (I/O events
     * are pending).  If all mask bits are set, the mask value is equivalent
     * to AtEventAll. If no events are awaiting service, 0 is returned.  No
     * events are actually processed by this call.
     *
     * NOTE:  As of version 1.2 of Altia , this function is not implemented.
     */
extern AtEventMask CALLP CALLTYPE AtAllPending(
#if AltiaFunctionPrototyping
    AtConnectId connection
#endif
);

    /*
     * void AtProcessEvent(AtConnectId connection, AtEventMask eventTypes):
     * --------------------------------------------------------------------
     * Toolkit function to request processing of pending events of one or more
     * type.  The eventTypes parameter is an OR'ing of AtEventAltia (process
     * Altia interface events for the connection specified by connection),
     * AtEventTimer (process any pending timer events), or AtEventInput
     * (process any pending I/O events).  If eventTypes is set to AtEventAll,
     * all pending events of any type will be processed;  however, only
     * pending Altia interface events from the connection will be
     * processed.  Pending events from any other Altia interface connections
     * will not be processed unless AtProcessEvent is called with the
     * appropriate connection id.
     *
     * NOTE:  As of version 1.2 of Altia , this function is not implemented.
     */
extern void CALLP CALLTYPE AtProcessEvent(
#if AltiaFunctionPrototyping
    AtConnectId connection,
    AtEventMask mask
#endif
);



/******************************************************************
 *
 *    ALTIA DEEPSCREEN ANIMATION ID INTERFACE FUNCTION DECLARATIONS
 *                  Declarations are for C/C++ 
 *
 *  These functions are only supported in the DeepScreen version of
 *  the Altia API.  These functions are not supported in the client
 *  (socket or DDE) version of the Altia API.
 *
 *  If an application is going to be compiled as an Altia client
 *  (with a socket or DDE API) sometimes and as a DeepScreen application
 *  (with the DeepScreen version of the API) other times, you can
 *  conditionally compile these functions for the DeepScreen case with:
 *
 *      #ifdef DEEPSCREEN
 *      ...
 *      #endif DEEPSCREEN
 *
 *  For creating the best common application code to compile with either
 *  a client Altia API library or DeepScreen Altia API library, do something
 *  like the following in application source code or an application
 *  common header file:
 *
 *    #include "altia.h"
 *    #ifdef DEEPSCREEN
 *        #include "altia/altiaAnimationIdTable.h"
 *        #define altiaSendEvent altiaSendEventId
 *        #define altiaSendText altiaSendTextId
 *    #else
 *        #define ALT_ANIM(x) API_TEXT(#x)
 *    #endif
 *
 *  After these macro are defined or a header file is included that
 *  defines them, use calls like the following to send events or
 *  strings to the client or DeepScreen version of the Altia API.
 *  The 1st example sends value 50 to a "speed" animation.
 *  The 2nd example sends string "Hello World" to a "text" animation.
 *
 *      altiaSendEvent(ALT_ANIM(speed), (AltiaEventType) 50);
 *      altiaSendText(ALT_ANIM(text), API_TEXT("Hello World"));
 *
 *  Please see the DeepScreen User's Guide for more information about
 *  using these functions.
 * 
 ******************************************************************/

    /*
     * int altiaGetAnimationId(const AltiaCharType *name):
     * OR:
     * int AtGetAnimationId(AtConnectId connectId, const AltiaCharType *name):
     * --------------------------------------------------------------------
     * Returns a unique numerical ID for the animation specified.  The
     * numerical ID can be used in the other ID APIs such as
     * altiaSendEventId() and altiaSendTextId().
     *
     * Zero and positive integers are valid retrun values.
     * If the specified animation does not exist a value of -1 is returned.
     */
    extern int CALLP CALLTYPE altiaGetAnimationId(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *name
#endif
);

    extern int CALLP CALLTYPE AtGetAnimationId(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    CONST AltiaCharType *name
#endif
);

    /*
     * int altiaSendEventId(int nameId, AltiaEventType eventValue):
     * OR:
     * int AtSendEventId(AtConnectId connectId, int nameId,
     *                   AltiaEventType value):
     * --------------------------------------------------------------------
     * Same as altiaSendEvent() but using a numerical animation ID instead
     * of an animation character string.
     *
     * If the specified animation does not exist a value of -1 is returned.
     * Otherwise zero is returned.
     */
    extern int CALLP CALLTYPE altiaSendEventId(
#if AltiaFunctionPrototyping
    int nameId, 
    AltiaEventType eventValue
#endif
);
    
    extern int CALLP CALLTYPE AtSendEventId(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int nameId, 
    AltiaEventType value
#endif
);
    
    /*
     * int altiaSendTextId(int nameId, CONST AltiaCharType *text):
     * OR:
     * int AtSendTextId(AtConnectId connectId, int nameId,
     *                  CONST AltiaCharType *text):
     * --------------------------------------------------------------------
     * Same as altiaSendEvent() but using a numerical animation ID instead
     * of an animation character string.
     *
     * If the specified animation does not exist a value of -1 is returned.
     * Otherwise zero is returned.
     */
    extern int CALLP CALLTYPE altiaSendTextId(
#if AltiaFunctionPrototyping
    int nameId, 
    CONST AltiaCharType *text
#endif
);
    
    extern int CALLP CALLTYPE AtSendTextId(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int nameId, 
    CONST AltiaCharType *text
#endif
);

extern int CALLP CALLTYPE TargetAltiaAnimateId(
#if AltiaFunctionPrototyping
    int nameId,
    LocalEventType value
#endif
);



/******************************************************************
 *
 *    ALTIA DEEPSCREEN TASKING INTERFACE FUNCTION DECLARATIONS
 *                  Declarations are for C/C++ 
 *
 *  These functions are only supported in the DeepScreen version of
 *  the Altia API.  These functions are not supported in the client
 *  (socket or DDE) version of the Altia API.
 *
 *  If an application is going to be compiled as an Altia client
 *  (with a socket or DDE API) sometimes and as a DeepScreen application
 *  (with the DeepScreen version of the API) other times, you can
 *  conditionally compile these functions for the DeepScreen case with:
 *
 *      #ifdef DEEPSCREEN
 *      ...
 *      #endif DEEPSCREEN
 *
 *  Please see the DeepScreen User's Guide for more information about
 *  using these functions.
 * 
 ******************************************************************/

/* Altia DeepScreen Tasking data structure declarations. */
typedef struct
{
    int task_size;
    int task_current_used;
    int task_peak_used;
    int task_overflow;
    int object_size;
    int object_current_used;
    int object_peak_used;
    int object_overflow;
    int object_max_time;
    int object_max_id;
} AltiaTaskStatusType;

    /*
     * void altiaTaskRun(unsigned long milliSeconds):
     * --------------------------------------------------------------
     * This function executes Altia as a task.  The function will return
     * when any of the following conditions are true:
     *
     * 1. The Altia Task Queue is empty
     * 2. The execution time exceeds the specified time (milliSeconds)
     *
     * In case 2, calling altiaTaskRun() again will resume where the 
     * previous invokation stopped.
     *
     * Returns -1 if ALTIA_TASKING is not defined, 0 otherwise.
     */
    extern int CALLP CALLTYPE altiaTaskRun(
#if AltiaFunctionPrototyping
    unsigned long milliSeconds
#endif
);

    /*
     * void altiaTaskStatus(AltiaTaskStatusType * status):
     * --------------------------------------------------------------
     * Obtains the status of the Altia Task.  The status is loaded
     * into the status parameter.
     *
     * Returns -1 if ALTIA_TASKING is not defined or if the status
     * pointer is NULL.  Returns 0 otherwise.
     */
    extern int CALLP CALLTYPE altiaTaskStatus(
#if AltiaFunctionPrototyping
    AltiaTaskStatusType * status
#endif
);


/******************************************************************
 *
 *    ALTIA DEEPSCREEN FONT INTERFACE FUNCTION DECLARATIONS
 *                  Declarations are for C/C++ 
 *
 *  These functions are only supported in the DeepScreen version of
 *  the Altia API.  These functions are not supported in the client
 *  (socket or DDE) version of the Altia API.
 *
 *  If an application is going to be compiled as an Altia client
 *  (with a socket or DDE API) sometimes and as a DeepScreen application
 *  (with the DeepScreen version of the API) other times, you can
 *  conditionally compile these functions for the DeepScreen case with:
 *
 *      #ifdef DEEPSCREEN
 *      ...
 *      #endif DEEPSCREEN
 *
 *  Please see the DeepScreen User's Guide for more information about
 *  using these functions.
 * 
 ******************************************************************/
 
    /*
     * int altiaGetTextWidth(const AltiaCharType *name, AltiaCharType *text):
     * --------------------------------------------------------------
     * This function calculates the pixel width of the specified text
     * using the characteristics of the object with the specified 
     * animation name.
     *
     * Returns -1 if text is not valid or if there is not a textio object
     * associated with the animation name, 0 otherwise.
     */
    extern int CALLP CALLTYPE altiaGetTextWidth(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *name, 
    AltiaCharType *text
#endif
);
 
    /*
     * int altiaGetTextWidthId(int nameId, AltiaCharType *text):
     * --------------------------------------------------------------
     * This function calculates the pixel width of the specified text
     * using the characteristics of the object with the specified 
     * animation id.
     *
     * Returns -1 if text is not valid or if there is not a textio object
     * associated with the animation id, 0 otherwise.
     */
    extern int CALLP CALLTYPE altiaGetTextWidthId(
#if AltiaFunctionPrototyping
    int nameId, 
    AltiaCharType *text
#endif
);
 
    /*
     * int altiaGetFont(const AltiaCharType *name):
     * --------------------------------------------------------------
     * This function finds the enumerated font ID assigned to the object
     * with the specified animation name.
     *
     * Returns -1 if there is not a textio object associated with the
     * animation name, otherwise a non-negative font id.
     */
    extern int CALLP CALLTYPE altiaGetFont(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *name
#endif
);
 
    /*
     * int altiaGetFontId(int nameId):
     * --------------------------------------------------------------
     * This function finds the enumerated font ID assigned to the object
     * with the specified animation id.
     *
     * Returns -1 if there is not a textio object associated with the
     * animation id, otherwise a non-negative font id.
     */
    extern int CALLP CALLTYPE altiaGetFontId(
#if AltiaFunctionPrototyping
    int nameId
#endif
);
 
    /*
     * int altiaSetFont(const AltiaCharType *name, int fontId):
     * --------------------------------------------------------------
     * This function assigns the enumerated font ID to the object
     * with the specified animation name.
     *
     * Returns -1 if there is not a textio object associated with the
     * animation name, 0 otherwise.
     */
    extern int CALLP CALLTYPE altiaSetFont(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *name, 
    int fontId
#endif
);
 
    /*
     * int altiaSetFontId(int nameId, int fontId):
     * --------------------------------------------------------------
     * This function assigns the enumerated font ID to the object
     * with the specified animation id.
     *
     * Returns -1 if there is not a textio object associated with the
     * animation name, 0 otherwise.
     */
    extern int CALLP CALLTYPE altiaSetFontId(
#if AltiaFunctionPrototyping
    int nameId, 
    int fontId
#endif
);

/******************************************************************
 *
 *      ALTIA VIEW MANIPULATION DATATYPES/FUNCTIONS/PROCEDURES DECLARATIONS
 *
 *                     Declarations are for C/C++ 
 *
 *  Altia View manipulation procedures and functions allow developers
 *  to manipulate altia view instances from a program.  Views can be
 *  opened, sized, and positioned within the windowing system.  The portion
 *  of the Altia design that a view is displaying can be set as well as the
 *  magnification of the view.
 * 
 ******************************************************************/

#define UserPlaceView -2
#define RootWindowPlace -1
#define MainViewPlace 0
#define MinViewNum 1
#define DefaultViewX -32666
#define DefaultViewY -32666
#define CenterViewX -32666
#define CenterViewY -32666
#define DefaultViewWidth 0
#define DefaultViewHeight 0
#define DefaultViewMag 0.0


    /*
     * int altiaOpenSimpleView(int newViewId,
     *                         CONST AltiaCharType *instanceName):
     * OR:
     * int AtOpenSimpleView(AtConnectId connectId, int newViewId,
     *                      CONST AltiaCharType *instanceName):
     * --------------------------------------------------------------------
     * This is a request to open a new view.  The caller passes a view id
     * that they can then use as a parameter to other view manipulation
     * routines.  The id must be unique with respect to all programmatically
     * created views. For example, if a view was already opened with an id of
     * 1, then a second view could be opened with an id of 2.  View id's must
     * be values greater than or equal to 1.
     *
     * Currently, instanceName is unused.  In the future, it will be possible
     * to give the new view a unique instance name and thereby specify some
     * of its attributes (e.g., height, width) from an X resource file or
     * the X resource database.
     *
     * AtOpenSimpleView() is the Toolkit version of altiaOpenSimpleView().
     * Both functions return 0 if they were successful.  If a parameter
     * is illegal, -2 is returned.  If a communications failure occurred, -1
     * is return. Not all illegal parameters can be detected prior to sending
     * the open request to the Altia interface.  The Altia interface does not
     * return an error to the program if it receives an illegal open request.
     * It simply will not open the new view.
     */
    extern int CALLP CALLTYPE altiaOpenSimpleView(
#if AltiaFunctionPrototyping
    int newViewId,
    CONST AltiaCharType *instanceName
#endif
);
    extern int CALLP CALLTYPE AtOpenSimpleView(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int newViewId,
    CONST AltiaCharType *instanceName
#endif
);


    /*
     * int altiaOpenView(int newViewId, CONST AltiaCharType *instanceName,
     *                   int universeX, int universeY, int width, int height,
     *                   double mag):
     * OR:
     * int AtOpenView(AtConnectId connectId,
     *                int newViewId, CONST AltiaCharType *instanceName,
     *                int universeX, int universeY, int width, int height,
     *                double mag):
     * --------------------------------------------------------------------
     * This is a request to open a new view.  The caller passes a view id
     * that they can then use as a parameter to other view manipulation
     * routines.  The id must be unique with respect to all programmatically
     * created views. For example, if a view was already opened with an id of
     * 1, then a second view could be opened with an id of 2.  View id's must
     * be values greater than or equal to 1.
     *
     * Currently, instanceName is unused.  In the future, it will be possible
     * to give the new view a unique instance name and thereby specify some
     * of its attributes (e.g., height, width) from an X resource file or
     * the X resource database.
     *
     * The caller may also specify the area of the design that should appear
     * within the view.  The parameters universeX and universeY specify the
     * design universe location that should appear in the lower left corner
     * of the new view.  The width and height of the view window as well as
     * the magnification factor that should be applied to the design being
     * shown in the view are also specified.  The design positioning, sizing
     * and magnification of the view can be left up to the system by specifying
     * DefaultViewX, DefaultViewY, DefaultViewHeight, DefaultViewWidth, or
     * DefaultViewMag for universeX, universeY, width, height, and mag,
     * respectively. For more information on the mag parameter, see the
     * description for altiaMagnifyView().
     *
     * AtOpenView() is the Toolkit version of altiaOpenView().
     * Both functions return 0 if they are successful.  If a parameter is
     * illegal, -2 is returned.  If a communications failure occurred, -1
     * is return. Not all illegal parameters can be detected prior to sending
     * the open request to the Altia interface.  The Altia interface does not
     * return an error to the program if it receives an illegal open request.
     * It simply will not open the new view.
     */
    extern int CALLP CALLTYPE altiaOpenView(
#if AltiaFunctionPrototyping
    int newViewId,
    CONST AltiaCharType *instanceName,
    int univX, int univY,
    int width, int height,
    ALT_DOUBLE mag
#endif
);
    extern int CALLP CALLTYPE AtOpenView(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int newViewId,
    CONST AltiaCharType *instanceName,
    int univX, int univY,
    int width, int height,
    ALT_DOUBLE mag
#endif
);


    /*
     * int altiaOpenViewPlaced(int newViewId,
     *                   CONST AltiaCharType *instanceName,
     *                   int referenceViewId, int referenceX, int referenceY,
     *                   int universeX, int universeY, int width, int height,
     *                   double mag):
     * OR:
     * int AtOpenViewPlaced(AtConnectId connectId,
     *                      int newViewId, CONST AltiaCharType *instanceName,
     *                      int referenceViewId, int referenceX, int referenceY,
     *                      int universeX, int universeY, int width, int height,
     *                      double mag):
     * --------------------------------------------------------------------
     * The operation of this function is identical to that of altiaOpenView()
     * except that you can additionally specify how you want the new view
     * placed within the windowing system.
     *
     * The new view is placed relative to the existing view specified by
     * the referenceViewId parameter.  If this parameter is set to the
     * constant RootWindowPlace, then placement is made relative to the
     * root window of the windowing system (i.e., relative to the full
     * display screen in most cases). If this parameter is set to the constant
     * MainViewPlace, the new view is placed relative to the main Altia
     * interface view.  This would be the first view opened by the interface.
     * All other values are taken to be the id's of views previously opened
     * using altiaOpenSimpleView(), altiaOpenView(), or altiaOpenViewPlaced().
     *
     * The referenceX and referenceY parameters specify the pixel position of
     * the new view's lower left corner relative to the reference view's lower
     * left corner.  For instance, passing a referenceX of 20 and referenceY 
     * of 20 would position the new view's lower left corner 20 pixels above
     * and 20 pixels to the right of the reference view's lower left corner.
     * The new view can be centered within the reference view by passing the
     * the constants CenterViewX and CenterViewY for referenceX and referenceY.
     *
     * AtOpenViewPlaced() is the Toolkit version of altiaOpenViewPlaced().
     * See the description for altiaOpenView() for information on the
     * remaining parameters and the return values.
     */
    extern int CALLP CALLTYPE altiaOpenViewPlaced(
#if AltiaFunctionPrototyping
    int newViewId,
    CONST AltiaCharType *instanceName,
    int referenceViewId, int referenceX, int referenceY,
    int univX, int univY, int width, int height,
    ALT_DOUBLE mag
#endif
);
    extern int CALLP CALLTYPE AtOpenViewPlaced(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int newViewId,
    CONST AltiaCharType *instanceName,
    int referenceViewId, int referenceX, int referenceY,
    int univX, int univY, int width, int height,
    ALT_DOUBLE mag
#endif
);


    /*
     * int altiaOpenWindowView(int windowId,
     *                   CONST AltiaCharType *instanceName,
     *                   int universeX, int universeY, double mag):
     * OR:
     * int AtOpenWindowView(AtConnectId connectId,
     *                int windowId, CONST AltiaCharType *instanceName,
     *                int universeX, int universeY, double mag):
     * --------------------------------------------------------------------
     * This is a request to open a new view, but open it into an existing window
     * created previously by another application.  The caller passes the
     * window id of the existing window.  This id is also used as a parameter
     * to other view manipulation routines.  The id must be for a window that
     * is associated with the same display that the Altia interface is associated
     * with.  On displays running the X windowing system, windowId is a window
     * id returned from a XCreateWindow() or similar call. The Altia interface
     * will draw and refresh its portion of the graphics being directed to the
     * window.  If drawing or refreshing of other graphics is necessary, it
     * must be handled by the application that originally created the window.
     *
     * Currently, instanceName is unused.  In the future, it will be possible
     * to give the new view a unique instance name and thereby specify some
     * of its attributes (e.g., universeX, universeY, mag) from an X resource
     * file or the X resource database.
     *
     * The caller may also specify the area of the design that should appear
     * within the view.  The parameters universeX and universeY specify the
     * design universe location that should appear in the lower left corner
     * of the new view.  The magnification factor that should be applied to
     * the design being shown in the view is also specified.  The design
     * positioning and magnification of the view can be left up to the system
     * by specifying DefaultViewX, DefaultViewY, or DefaultViewMag for
     * universeX, universeY, and mag, respectively. For more information on
     * the mag parameter, see the description for altiaMagnifyView().
     *
     * AtOpenWindowView() is the Toolkit version of altiaOpenWindowView().
     * Both functions return 0 if they are successful.  If a parameter is
     * illegal, -2 is returned.  If a communications failure occurred, -1
     * is return. Not all illegal parameters can be detected prior to sending
     * the open request to the Altia interface.  The Altia interface does not
     * return an error to the program if it receives an illegal open request.
     * It simply will not open a view into the existing window.
     */
    extern int CALLP CALLTYPE altiaOpenWindowView(
#if AltiaFunctionPrototyping
    int windowId,
    CONST AltiaCharType *instanceName,
    int univX, int univY,
    ALT_DOUBLE mag
#endif
);
    extern int CALLP CALLTYPE AtOpenWindowView(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int windowId,
    CONST AltiaCharType *instanceName,
    int univX, int univY,
    ALT_DOUBLE mag
#endif
);


    /*
     * int altiaCloseView(int newViewId):
     * OR:
     * int AtCloseView(AtConnectId connectId, int newViewId):
     * --------------------------------------------------------------------
     * Close a view previously opened with a call to altiaOpenSimpleView(),
     * altiaOpenView(), or altiaOpenViewPlace().
     *
     * AtCloseView() is the Toolkit version of altiaCloseView().
     * Both functions return 0 if they are successful.  If the view id
     * is illegal, -2 is returned.  If a communications failure occurs, -1 is
     * returned. Not all illegal view id's can be detected prior to sending
     * the close request to the Altia interface.  The Altia interface does not
     * return an error to the program if it receives an illegal view id.
     * It simply does not close a view.
     */
    int CALLP CALLTYPE altiaCloseView(
#if AltiaFunctionPrototyping
    int viewId
#endif
);
    int CALLP CALLTYPE AtCloseView(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int viewId
#endif
);


    /*
     * int altiaMoveView(int viewId, int x, int y):
     * OR:
     * int AtMoveView(AtConnectId connectId, int viewId, int x, int y):
     * --------------------------------------------------------------------
     * Change the portion of the design that is shown within the given view.
     * The x and y parameters specify the design universe location that should
     * appear in the lower left corner of the given view.
     *
     * AtMoveView() is the Toolkit version of altiaMoveView().
     * Both functions return 0 if they are successful.  If a communications
     * failure occurs, -1 is returned.
     *
     * An illegal view id or inappropriate x or y parameter cannot be detected
     * prior to sending the move request to the Altia interface.  In such a
     * case, the return value will be 0 and the move operation will simply
     * not happen.
     */
    int CALLP CALLTYPE altiaMoveView(
#if AltiaFunctionPrototyping
    int viewId, int x, int y
#endif
);
    int CALLP CALLTYPE AtMoveView(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int viewId, int x, int y
#endif
);


    /*
     * int altiaSizeView(int viewId, int width, int height):
     * OR:
     * int AtSizeView(AtConnectId connectId, int viewId, int width, int height):
     * --------------------------------------------------------------------
     * Change the size, in screen pixels, of the view given by viewId.
     *
     * AtSizeView() is the Toolkit version of altiaSizeView().
     * Both functions return 0 if they are successful.  If width or height
     * are illegal (i.e., less than or equal to 0), -2 is returned.  If a
     * communications failure occurs, -1 is returned.
     *
     * An illegal view id cannot be detected prior to sending the size
     * request to the Altia interface.  In such a case, the return value will
     * be 0 and the size operation will simply not happen.
     */
    int CALLP CALLTYPE altiaSizeView(
#if AltiaFunctionPrototyping
    int viewId, int width, int height
#endif
);
    int CALLP CALLTYPE AtSizeView(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int viewId, int width, int height
#endif
);


    /*
     * int altiaGetViewSize(int viewId, int *widthOut, int *heightOut):
     * OR:
     * int AtGetViewSize(AtConnectId connectId, int viewId,
     *                   int *widthOut, int *heightOut):
     * --------------------------------------------------------------------
     * Get the current size, in screen pixels, of the view given by viewId.
     * The caller supplies pointers to integer variables.  When the function
     * returns, these variables will be set to the current width and height
     * for the view.
     *
     * AtGetViewSize() is the Toolkit version of altiaGetViewSize().
     * Both functions return 0 if they are successful.  If widthOut or
     * heightOut are illegal (i.e., nil pointers), -2 is returned.  If a
     * communications failure occurs, -1 is returned.
     *
     * An illegal view id cannot be detected prior to sending the size
     * request to the Altia interface.  In such a case, the return value will
     * be 0 and the values returned in the integers pointed to by widthOut
     * and heightOut will be invalid.
     */
    int CALLP CALLTYPE altiaGetViewSize(
#if AltiaFunctionPrototyping
    int viewId, int *widthOut, int *heightOut
#endif
);
    int CALLP CALLTYPE AtGetViewSize(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int viewId, int *widthOut, int *heightOut
#endif
);


    /*
     * int altiaGetViewWindowId(int viewId, int *windowIdOut):
     * OR:
     * int AtGetViewWindowId(AtConnectId connectId, int viewId,
     *                       int *windowIdOut):
     * --------------------------------------------------------------------
     * Get the Windowing System's window id for the view given by viewId.
     * The caller supplies a pointer to an integer variable.  When the function
     * returns, this variable will be set to the window Id for the view.
     *
     * AtGetViewWindowId() is the Toolkit version of altiaGetViewWindowId().
     * Both functions return 0 if they are successful.  If windowIdOut is
     * illegal (i.e., a nil pointer), -2 is returned.  If a communications
     * failure occurs, -1 is returned.
     *
     * An illegal view id cannot be detected prior to sending the id
     * request to the Altia interface.  In such a case, the return value will
     * be 0 and the value returned in the integer pointed to by windowIdOut
     * will probably be 0.
     */
    int CALLP CALLTYPE altiaGetViewWindowId(
#if AltiaFunctionPrototyping
    int viewId, int *windowIdOut
#endif
);
    int CALLP CALLTYPE AtGetViewWindowId(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int viewId, int *windowIdOut
#endif
);


    /*
     * int altiaMagnifyView(int viewId, double mag):
     * OR:
     * int AtMagnifyView(AtConnectId connectId, int viewId, double mag):
     * --------------------------------------------------------------------
     * Change the magnification factor of the area being displayed in the view
     * given by viewId.  The factor must be a positive value greater than or
     * equal to .01.  As examples, a factor of .5 would cause objects in the
     * view to be shown at one-half their normal size. With a factor of 2.0,
     * objects in a view would appear twice their normal size.
     *
     * AtMagnifyView() is the Toolkit version of altiaMagnifyView().
     * Both functions return 0 if they are successful.  If mag is less than
     * .01, -2 is returned.  If a communications failure occurs, -1 is returned.
     *
     * An illegal view id cannot be detected prior to sending the magnify
     * request to the Altia interface.  In such a case, the return value will
     * be 0 and the magnify operation will simply not happen.
     *
     * The Altia system may impose limits on the magnification range (it
     * currently limits it to 1/16 (.0625) through 16.0).  If a request
     * outside of this range is received, it will be truncated to the minimum
     * or maximum of the range, whichever is appropriate.
     */
    int CALLP CALLTYPE altiaMagnifyView(
#if AltiaFunctionPrototyping
    int viewId, ALT_DOUBLE mag
#endif
);
    int CALLP CALLTYPE AtMagnifyView(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int viewId, ALT_DOUBLE mag
#endif
);


    /*
     * int altiaOpenDesignFile(int designId, AltiaCharType *fileName):
     * OR:
     * int AtOpenDesignFile(AtConnectId connectId, int designId, 
                            AltiaCharType *fileName):
     * --------------------------------------------------------------------
     * This function opens a new Altia Design file named fileName and assigns
     * the id passed in designId to the new design.  It also sets the 
     * current DesignId to that design (see altiaSetDesignId).  If designId
     * is an id of an already opened file, then that file gets replaced by
     * the new file.  A designId of zero will change the original design that
     * was opened. If designId is not associated with any existing design
     * file, then a new design is opened.  The open view functions must then
     * be called to open one or more views into the new design.
     *
     * AtOpenDesignFile() is the Toolkit version of altiaOpenDesign().
     * Both functions return 0 if they are successful.  If designid < 0 or
     * the fileName is NULL or empty, -2 is returned.
     * If a communications failure occurs, -1 is returned.
     *
     * A non-existent or invalid file error is not detected by this function.
     * Altia will just ignore the request but the return value from this
     * function will be 0.
     *
     */
    int CALLP CALLTYPE altiaOpenDesignFile(
#if AltiaFunctionPrototyping
    int designId, AltiaCharType *fileName
#endif
);
    int CALLP CALLTYPE AtOpenDesignFile(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int designId, AltiaCharType *fileName
#endif
);

    /*
     * int altiaSetDesignId(int designId):
     * OR:
     * int AtSetDesignId(AtConnectId connectId, int designId):
     * --------------------------------------------------------------------
     * This function sets the current DesignId for Altia to the value given
     * by designId. The current DesignId must be correctly set when using
     * routines that require an object id (like the clone and object move
     * routines) because object id's are only unique within a design file.
     * The current DesignId must also be correctly set for the various
     * view routines so that views are opened and changed for the correct
     * design file.  The initial value of the DesignId is 0 which implies
     * all cloning, object moving, and view opens/changes will be directed
     * to the original design file.
     *
     * AtSetDesignId() is the Toolkit version of altiaSetDesignId().
     * Both functions return 0 if they are successful.  If designid is < 0,
     * -2 is returned.
     * If a communications failure occurs, -1 is returned.
     *
     */
    int CALLP CALLTYPE altiaSetDesignId(
#if AltiaFunctionPrototyping
    int designId
#endif
);
    int CALLP CALLTYPE AtSetDesignId(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int designId
#endif
);

    /*
     * int altiaCloseDesignId(int designId):
     * OR:
     * int AtCloseDesignId(AtConnectId connectId, int designId): 
     * --------------------------------------------------------------------
     * This function closes the design file associated with the designId.
     * All views opened for that designId are closed.  You cannot close
     * designId = 0 since that is the original design.  If you want to close
     * designId = 0, you must send an "altiaQuit" event to Altia or call
     * altiaStopInterface()/AtStopInterface() to terminate the Altia
     * interface process (if Altia is in run mode).
     *
     * AtCloseDesignId() is the Toolkit version of altiaCloseDesignId().
     * Both functions return 0 if they are successful.  If designid is <= 0, 
     * -2 is returned.
     * If a communications failure occurs, -1 is returned.
     *
     */
    int CALLP CALLTYPE altiaCloseDesignId(
#if AltiaFunctionPrototyping
    int designId
#endif
);
    int CALLP CALLTYPE AtCloseDesignId(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int designId
#endif
);


/******************************************************************
 *
 *      ALTIA CLONE MANIPULATION FUNCTIONS/PROCEDURES DECLARATIONS
 *
 *                     Declarations are for C/C++ 
 *
 *  Altia clone manipulation procedures and functions allow developers
 *  to create and manipulate clone objects.  That is, a user can create
 *  a duplicate of an existing Altia interface object.  The duplicate, or
 *  clone as we call it, is completely independent of the original.  Its
 *  animation and stimulus function names have a "#:" prepended to them to
 *  uniquely identify them from the original objects function names.  The
 *  '#' in "#:" is the ascii representation of the number designated for the
 *  clone object when it was created with altiaCreateClone() or
 *  AtCreateClone(). All of the routines supplied by this library that deal
 *  with clone function names take or receive the clone number separated from
 *  the base animation or stimulus function name.  As such, it is actually
 *  unnecessary for the application developer to know that the clone's
 *  animation and stimulus functions are derived using the "#:name" syntax.
 ******************************************************************/

    /*
     *    int altiaCreateClone(int objectNumber, int cloneNumber):
     * OR:
     *    int AtCreateClone(AtConnectId connectId, int objectNumber,
     *                      int cloneNumber):
     * --------------------------------------------------------------------
     * Toolkit or base library routine to create a clone object from an
     * already existing normal object.  The normal object is referred to by
     * its object number. An object's object number can be determined from
     * the Altia editor interface by selecting the object and then choosing the 
     * "Rename Functions" item from the Animation Editor's or Stimulus
     * Editor's "Edit" menu. This will display the Function Rename Dialog.
     * The "Prepend" field of the Rename Dialog is loaded with the object's
     * object number.
     *
     * The clone object is created with the object number given by
     * cloneNumber.  Note that cloneNumber can be any integer value of your
     * choosing.  As a good practice, Altia recommends that negative integer
     * values be used for clone object numbers.  For instance, start with
     * -1 and decrement the value each time a new clone is created.  Use of
     * negative values for clone objects is not a requirement, but it insures
     * that clone object numbers are unique from standard object numbers.
     * Conflicts can only occur if you are creating/manipulating clone objects
     * and then manipulating normal objects with the altiaMoveObject() or
     * AtMoveObject() routines.
     *
     * The clone object is positioned identically to the original
     * object and on top of it.  If the original object is in a group, the
     * clone is created within the same group. The original object can be
     * a primitive object (e.g, rectangle, square, circle, etc.) or a group
     * object.
     *
     * If the original object has animation or stimulus functions, those 
     * functions are automatically renamed with the format "#:function_name"
     * where # is the ascii equivalent of the clone number and function_name
     * is the original function name.  This allows programs to send or
     * receive events to/from the clone object. The functions
     * altiaSendCloneEvent() or AtSendCloneEvent() can be used to send events
     * to clone objects.  The Toolkit routine AtAddCloneCallback() can be
     * used to register a callback to receive a clone event.
     * 
     * AtCreateClone() is the Toolkit version of altiaCreateClone().  Both
     * functions return 0 on success or -1 if a communications failure occurs.
     */
extern int CALLP CALLTYPE altiaCreateClone(
#if AltiaFunctionPrototyping
    int objectNumber, int cloneNumber
#endif
);
extern int CALLP CALLTYPE AtCreateClone(
#if AltiaFunctionPrototyping
    AtConnectId connectId, int objectNumber, int cloneNumber
#endif
);

    /*
     *    int altiaMoveClone(int cloneNumber, int xOffset, int yOffset):
     * OR:
     *    int AtMoveClone(AtConnectId connectId, int cloneNumber,
     *                    int xOffset, int yOffset):
     * --------------------------------------------------------------------
     * Toolkit or base library routine to move a clone object relative to
     * its current position.  The cloneNumber parameter identifies the clone
     * that should be moved.  It was originally supplied to the 
     * altiaCreateClone() or AtCreateClone() function when the clone was
     * created. xOffset is the amount, in pixels, that the object should be
     * moved in the horizontal direction. yOffset is the amount, in pixels,
     * that the object should be moved in the vertical direction.  xOffset
     * or yOffset are positive to achieve a move to the right or up,
     * respectively.  xOffset or yOffset can be negative to achieve a move
     * to the left or down, respectively.
     * 
     * AtMoveClone() is the Toolkit version of altiaMoveClone().  Both
     * functions return 0 on success or -1 if a communications failure occurs.
     */
extern int CALLP CALLTYPE altiaMoveClone(
#if AltiaFunctionPrototyping
    int cloneNumber, int xOffset, int yOffset
#endif
);
extern int CALLP CALLTYPE AtMoveClone(
#if AltiaFunctionPrototyping
    AtConnectId connectId, int cloneNumber, int xOffset, int yOffset
#endif
);

    /*
     *    int altiaDeleteClone(int cloneNumber):
     * OR:
     *    int AtDeleteClone(AtConnectId connectId, int cloneNumber):
     * --------------------------------------------------------------------
     * Toolkit or base library routine to delete a clone object from the
     * Altia interface.  The cloneNumber parameter identifies the clone
     * that should be deleted.  It was originally supplied to the 
     * altiaCreateClone() or AtCreateClone() function when the clone was
     * created. After a clone is deleted, its clone number may be reused to
     * create a new clone of any normal object in the Altia interface.
     * 
     * AtDeleteClone() is the Toolkit version of altiaDeleteClone().  Both
     * functions return 0 on success or -1 if a communications failure occurs.
     */
extern int CALLP CALLTYPE altiaDeleteClone(
#if AltiaFunctionPrototyping
    int cloneNumber
#endif
);
extern int CALLP CALLTYPE AtDeleteClone(
#if AltiaFunctionPrototyping
    AtConnectId connectId, int cloneNumber
#endif
);

    /*
     *    int altiaDeleteAllClones():
     * OR:
     *    int AtDeleteAllClones(AtConnectId connectId):
     * --------------------------------------------------------------------
     * Toolkit or base library routine to delete ALL clone objects from the
     * Altia interface.  After all clones are deleted, their clone numbers may
     * be reused to create new clones of any normal object in the Altia
     * interface.
     * 
     * AtDeleteAllClones() is the Toolkit version of altiaDeleteAllClones().
     * Both functions return 0 on success or -1 if a communications failure
     * occurs.
     */
extern int CALLP CALLTYPE altiaDeleteAllClones(
#if AltiaFunctionPrototyping
    void
#endif
);
extern int CALLP CALLTYPE AtDeleteAllClones(
#if AltiaFunctionPrototyping
    AtConnectId connectId
#endif
);

    /*
     *    int altiaSendCloneEvent(int cloneNumber, const AltiaCharType *name,
     *                            AltiaEventType value):
     * OR:
     *    int AtSendCloneEvent(AtConnectId connectId, int cloneNumber,
     *                         const AltiaCharType* name, AltiaEventType value):
     * --------------------------------------------------------------------
     * Toolkit or base library routine to send an event to a clone object.
     * The cloneNumber parameter identifies the clone that the event should
     * be sent to. The name parameter points to a string containing the event
     * name and the value parameter gives the value for the event.
     * altiaSendCloneEvent() and AtSendCloneEvent() pack the clone number and
     * event name into a single string of the the form "#:name" and send this,
     * along with the event value,  to the Altia interface.  The "#:name"
     * format is the format for all clone animation or stimulus names (See
     * the above description for altiaCreateClone()).
     *
     * AtSendCloneEvent() is the Toolkit version of altiaSendCloneEvent().
     * Both functions return 0 on success or -1 if a communications failure
     * occurs.
     */
extern int CALLP CALLTYPE altiaSendCloneEvent(
#if AltiaFunctionPrototyping
    int cloneNumber, CONST AltiaCharType *name, AltiaEventType value
#endif
);
extern int CALLP CALLTYPE AtSendCloneEvent(
#if AltiaFunctionPrototyping
    AtConnectId connectId, int cloneNumber, CONST AltiaCharType *name, AltiaEventType value
#endif
);

    /*
     *   int altiaPollCloneEvent(int cloneNumber, const AltiaCharType *name, 
                                 AltiaEventType *value):
     * OR:
     *   int AtPollCloneEvent(AtConnectId connectId, int cloneNumber,
     *                        const AltiaCharType *name, AltiaEventType *value):
     * --------------------------------------------------------------------
     * Toolkit or base library routine to poll an event for a clone object.
     * The cloneNumber parameter identifies the clone that the event should
     * be polled for. The name parameter points to a string containing the
     * event name and the value parameter is a pointer to a variable to hold
     * the polled value for the event.
     * altiaPollCloneEvent() and AtPollCloneEvent() pack the clone number and
     * event name into a single string of the the form "#:name" and send this
     * to the Altia interface.  The "#:name" * format is the format for all
     * clone animation or stimulus names (See the above description for
     * altiaCreateClone()).
     *
     * If an event with the name "#:name" was previously routed to this
     * client and is awaiting receipt via altiaNextEvent() or AtMainLoop(),
     * it will be deleted from the receiving queue.  A poll request gets the
     * current value for the event which makes all previous values obsolete.
     *
     * AtPollCloneEvent() is the Toolkit version of altiaPollCloneEvent().
     * Both functions return 0 on success or -1 if a communications failure
     * occurs.
     */
extern int CALLP CALLTYPE altiaPollCloneEvent(
#if AltiaFunctionPrototyping
    int cloneNumber, CONST AltiaCharType *name, AltiaEventType *value
#endif
);
extern int CALLP CALLTYPE AtPollCloneEvent(
#if AltiaFunctionPrototyping
    AtConnectId connectId, int cloneNumber, CONST AltiaCharType *name, AltiaEventType *value
#endif
);

    /*
     *    int altiaSendCloneText(int cloneNumber, const AltiaCharType *name,
     *                           const AltiaCharType *text):
     * OR:
     *    int AtSendCloneText(AtConnectId connectId, int cloneNumber,
     *                         const AltiaCharType *name,
     *                         const AltiaCharType *text):
     * --------------------------------------------------------------------
     * Toolkit or base library routine to send a string of character events
     * to a clone object.  The cloneNumber parameter identifies the clone
     * that the events should be sent to - normally this would be a cloned
     * Altia text output or input object. The name parameter would typically
     * point to the "putchar" animation name for the output or input object.
     * See altiaSendText() for more details on the "putchar" animation.
     * altiaSendCloneText() and AtSendCloneText() pack the clone number and
     * event name into a single string of the the form "#:name" and send this,
     * along with the character value pointed to by text,  to the Altia
     * interface.  The "#:name" format is the format for all clone animation
     * or stimulus names (See the above description for altiaCreateClone()).
     *
     * AtSendCloneText() is the Toolkit version of altiaSendCloneText().
     * Both functions return 0 on success or -1 if a communications failure
     * occurs.
     */
extern int CALLP CALLTYPE altiaSendCloneText(
#if AltiaFunctionPrototyping
    int cloneNumber, CONST AltiaCharType *name, CONST AltiaCharType *string
#endif
);
extern int CALLP CALLTYPE AtSendCloneText(
#if AltiaFunctionPrototyping
    AtConnectId connectId, int cloneNumber, CONST AltiaCharType *name, CONST AltiaCharType *string
#endif
);

    /*
     *    int altiaGetCloneText(int cloneNumber,
     *                           const AltiaCharType *getCharName,
     *                           AltiaCharType *buffer, int bufSize):
     * OR:
     *    int AtGetCloneText(AtConnectId connectId, int cloneNumber,
     *                     const AltiaCharType *getCharName,
     *                     AltiaCharType *buffer, int bufSize):
     * --------------------------------------------------------------------
     * These are clone versions of the altiaGetText() and AtGetText()
     * functions. Parameters are identical accept for the additional
     * cloneNumber to specify the clone which is the target of this call.
     * Please see altiaGetText() and AtGetText() for usage and capabilities.
     */
extern int CALLP CALLTYPE altiaGetCloneText(
#if AltiaFunctionPrototyping
    int cloneNumber, CONST AltiaCharType *getCharName, AltiaCharType *strOut, int strSize
#endif
);
extern int CALLP CALLTYPE AtGetCloneText(
#if AltiaFunctionPrototyping
    AtConnectId connectId, int cloneNumber, CONST AltiaCharType *getCharName, AltiaCharType *strOut, int strSize
#endif
);

    /*
     *    int altiaCheckCloneEvent(int cloneNumber, const AltiaCharType *name,
     *                             AltiaEventType *valueOut):
     * OR:
     *    int AtCheckCloneEvent(AtConnectId connectId, int cloneNumber,
     *                     const AltiaCharType *name, AltiaEventType *valueOut);
     * --------------------------------------------------------------------
     * These are clone versions of the altiaCheckEvent() and AtCheckEvent()
     * functions. Parameters are identical accept for the additional
     * cloneNumber to specify the clone which is the target of this call.
     * Please see altiaCheckEvent() and AtCheckEvent() for usage
     * and capabilities information.
     */
extern int CALLP CALLTYPE altiaCheckCloneEvent(
#if AltiaFunctionPrototyping
    int cloneNumber, CONST AltiaCharType *name, AltiaEventType *value
#endif
);
extern int CALLP CALLTYPE AtCheckCloneEvent(
#if AltiaFunctionPrototyping
    AtConnectId connectId, int cloneNumber, CONST AltiaCharType *name, AltiaEventType *value
#endif
);

    /*
     *    int altiaCloneEventSelected(int cloneNumber,
     *                                const AltiaCharType *name):
     * OR:
     *    int AtCloneEventSelected(AtConnectId connectId, int cloneNumber,
     *                     const AltiaCharType *name):
     * --------------------------------------------------------------------
     * These are clone versions of the altiaEventSelected() and
     * AtEventSelected() functions. Parameters are identical accept for the
     * additional cloneNumber to specify the clone which is the target of
     * this call.  Please see altiaEventSelected() and AtEventSelected()
     * for usage and capabilities information.
     */
extern int CALLP CALLTYPE altiaCloneEventSelected(
#if AltiaFunctionPrototyping
    int cloneNumber, CONST AltiaCharType *name
#endif
);
extern int CALLP CALLTYPE AtCloneEventSelected(
#if AltiaFunctionPrototyping
    AtConnectId connectId, int cloneNumber, CONST AltiaCharType *name
#endif
);

    /*
     * typedef void (*AtCloneProc)(AtConnectId connectId,
     *                                int cloneNumber,
     *                                const AltiaCharType *eventName,
     *                                AltiaEventType value,
     *                                AtPointer clientData):
     * --------------------------------------------------------------
     * Description of the syntax for Altia interface cloned object event
     * callbacks.  If you want to write a callback routine to be notified
     * when an event from a cloned object is received, it must take the
     * parameters as shown and it returns nothing.  It will be passed
     * the id of the Altia interface connection from which the event was
     * received, the number of the clone initiating the event, the name of
     * the event , and a piece of custom data that may be specified at the
     * time the callback is added to the callback list (see
     * AtAddCloneCallback()).
     */
typedef void (CALLP CALLTYPE *AtCloneProc)(
#if AltiaFunctionPrototyping
    AtConnectId connectId, int cloneNumber, CONST AltiaCharType *eventName, AltiaEventType value, AtPointer clientData
#endif
);

    /*
     * void AtAddCloneCallback(AtConnectId connectId, int cloneNumber,
     *                    const AltiaCharType *eventName,
     *                    AtCloneProc callbackProc,
     *                    AtPointer clientData):
     * --------------------------------------------------------------------
     * Toolkit routine to add a procedure to the Altia interface event
     * callback list for an event that is being generated by a clone object.
     * In addition to the usual connection id parameter, the number of the
     * clone that will be generating the event, as well as the event name,
     * is specified.  When an event from this clone is received and it has
     * the specified name, the callback routine given by callbackProc will
     * be invoked.  AtAddCloneCallback() makes the appropriate call to
     * altiaSelectEvent() to insure that the client program is set up to
     * receive eventName events from the specified connection for the given
     * clone object.  Special data may be passed to the callback procedure
     * through the clientData parameter, or it can be set to NULL.
     *
     * The syntax for the AtCloneProc procedure type was described earlier.
     */
extern void CALLP CALLTYPE AtAddCloneCallback(
#if AltiaFunctionPrototyping
    AtConnectId connection, int cloneNumber, CONST AltiaCharType *eventName, AtCloneProc proc, AtPointer clientData
#endif
);

    /*
     * void AtRemoveCloneCallback(AtConnectId connectId, int cloneNumber,
     *                    const AltiaCharType *eventName,
     *                    AtCloneProc callbackProc, AtPointer clientData):
     * --------------------------------------------------------------------
     * Toolkit routine to remove a procedure from the Altia interface event
     * callback list if it is currently in the list.  A procedure is only
     * removed if it was added to the list with exactly the same attributes
     * specified by connectId, eventName, callbackProc, and clientData.
     *
     * If there are no other callbacks in the list for this event,
     * altiaUnselectEvent() will be called for the given connection and
     * event name.
     */
extern void CALLP CALLTYPE AtRemoveCloneCallback(
#if AltiaFunctionPrototyping
    AtConnectId connection, int cloneNumber, CONST AltiaCharType *eventName, AtCloneProc proc, AtPointer clientData
#endif
);

    /*
     * typedef void (*AtCloneTextProc)(AtConnectId connectId,
     *                                int cloneNumber,
     *                                const AltiaCharType *eventName,
     *                                const AltiaCharType *text,
     *                                AtPointer clientData):
     * --------------------------------------------------------------
     * Description of the syntax for Altia interface text callback for
     * cloned objects.  If you want to write a callback routine to be notified
     * when a null terminated string of text events has been received from a
     * cloned object, the callback must take the parameters as shown and it
     * returns nothing.  It will be passed the id of the Altia interface
     * connection from which the string was received, the number of the
     * clone initiating the transmission, the name of the event , a pointer
     * to the string of characters actually received, and a piece of custom
     * data that may be specified at the time the callback is added to
     * the callback list (see AtAddCloneTextCallback()).
     *
     * Normally, clone text callbacks are set up to receive strings from
     * clones which are Altia text i/o objects. An alternative method is
     * to simply get the text with AtGetCloneText() (see description above)
     * when it is necessary to do so.
     *
     * To learn more about text i/o objects, open the Help menu available
     * from a Models Library View and select Textio Models. Actual text
     * input model examples are available in the $ALTIAHOME/models/textio.dsn
     * and $ALTIAHOME/models/gui.dsn Models Libraries.
     */
typedef void (CALLP CALLTYPE *AtCloneTextProc)(
#if AltiaFunctionPrototyping
    AtConnectId connectId, int cloneNumber, CONST AltiaCharType *eventName, CONST AltiaCharType *text, AtPointer clientData
#endif
);

    /*
     * void AtAddCloneTextCallback(AtConnectId connectId, int cloneNumber,
     *                    const AltiaCharType *eventName,
     *                    AtCloneTextProc callbackProc,
     *                    AtPointer clientData):
     * --------------------------------------------------------------------
     * Toolkit routine to add a procedure to the Altia interface event
     * callback list for text events that are being generated by a clone
     * object.  That object would normally be an Altia text i/o object and
     * the eventName parameter would point to the "text" animation name
     * for the object.
     *
     * To learn more about text i/o objects, open the Help menu available
     * from a Models Library View and select Textio Models. Actual text
     * input model examples are available in the $ALTIAHOME/models/textio.dsn
     * and $ALTIAHOME/models/gui.dsn Models Libraries.
     *
     * In addition to the usual connection id parameter, the number of the
     * clone that will be generating the events, as well as the event name,
     * is specified.  When a null-terminated string of events of the given
     * name are received from this clone, they are packed into a character
     * array and the callback routine given by callbackProc is invoked.
     * AtAddCloneTextCallback() makes the appropriate call to
     * altiaSelectEvent() to insure that the client program is set up to
     * receive eventName events from the specified connection for the given
     * clone object.  Special data may be passed to the callback procedure
     * through the clientData parameter, or it can be set to NULL.
     *
     * The syntax for the AtCloneTextProc procedure type was described earlier.
     *
     * An alternative method to setting up a text callback for getting
     * text i/o object data is to simply get the text with AtGetCloneText()
     * (see description above) when it is necessary to do so.
     */
extern void CALLP CALLTYPE AtAddCloneTextCallback(
#if AltiaFunctionPrototyping
    AtConnectId connection, int cloneNumber, CONST AltiaCharType *eventName, AtCloneTextProc proc, AtPointer clientData
#endif
);

    /*
     * void AtRemoveCloneTextCallback(AtConnectId connectId, int cloneNumber,
     *                    const AltiaCharType *eventName,
     *                    AtCloneTextProc callbackProc, AtPointer clientData):
     * --------------------------------------------------------------------
     * Toolkit routine to remove a procedure from the Altia interface event
     * callback list if it is currently in the list.  A procedure is only
     * removed if it was added to the list as a clone text callback with
     * exactly the same attributes specified by connectId, eventName,
     * callbackProc, and clientData.
     *
     * If there are no other callbacks in the list for this event,
     * altiaUnselectEvent() will be called for the given connection and
     * event name.
     */
extern void CALLP CALLTYPE AtRemoveCloneTextCallback(
#if AltiaFunctionPrototyping
    AtConnectId connection, int cloneNumber, CONST AltiaCharType *eventName, AtCloneTextProc proc, AtPointer clientData
#endif
);

/******************************************************************
 *
 *      ALTIA PLAY/RECORD INTERFACE FUNCTIONS/PROCEDURES DECLARATIONS
 *
 *                     Declarations are for C/C++ 
 *
 *  Altia Play/Record procedures and functions allow developers
 *  to control the event record and playback facilities available in Altia.
 *  This includes selecting a file to record to, starting and stopping
 *  recording, and selecting a file to play from, starting, stopping, stepping,
 *  and rewinding the playback.
 * 
 ******************************************************************/

    /*
     * int altiaSetPlayFile(CONST AltiaCharType *fileName):
     * OR:
     * int AtSetPlayFile(AtConnectId connectId, CONST AltiaCharType *fileName):
     * --------------------------------------------------------------------
     * This is a request to select a file to play from.  The file must be
     * a properly formatted Altia event file (usually a product of a previous
     * record session) and it must be readable.
     *
     * AtSetPlayFile() is the Toolkit version of altiaSetPlayFile()
     * and it takes the standard AtConnectId as a additional parameter.
     *
     * Both functions return 0 if they were successful.  If the file name is
     * nil or empty, -2 is returned. If a communications failure occurred, -1
     * is return. Not all illegal parameters can be detected prior to sending
     * the selection request to the Altia interface.  The Altia interface does
     * not return an error to the program if it receives an illegal file name
     * or the file is not readable.  It simply will not select the file for
     * playback.  If the file is not a properly formatted file, it will still
     * be opened, but playback will most likely be uneventful as no lines can
     * be correctly parsed.
     */
    extern int CALLP CALLTYPE altiaSetPlayFile(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *fileName
#endif
);
    extern int CALLP CALLTYPE AtSetPlayFile(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    CONST AltiaCharType *fileName
#endif
);

    /*
     * int altiaSetPlayMacro(int macroNum):
     * OR:
     * int AtSetPlayMacro(AtConnectId connectId, int macroNum):
     * --------------------------------------------------------------------
     * This is a request to select a macro file to play from.  A number is
     * passed to identify the macro file.  Given the number, a file name is
     * created using the convention "macro%d.rec" where %d is the value of
     * macroNum converted to text.  The macro file associated with macroNum
     * must be a properly formatted Altia event file (usually a product of a
     * previous record session) and it must be readable.  This function is
     * a convenience routine so as not to require the caller to create
     * a complete file name.
     *
     * AtSetPlayMacro() is the Toolkit version of altiaSetPlayMacro()
     * and it takes the standard AtConnectId as a additional parameter.
     *
     * Both functions return 0 if they were successful.  If the macro number is
     * less than 0 or greater than 999, -2 is returned.  If a communications
     * failure occurred, -1 is return. Not all illegal parameters can be
     * detected prior to sending the selection request to the Altia interface.
     * The Altia interface does not return an error to the program if it
     * receives a macro number for a file that does not exist or
     * the file is not readable.  It simply will not select the file for
     * playback.  If the file is not a properly formatted file, it will still
     * be opened, but playback will most likely be uneventful as no lines can
     * be correctly parsed.
     */
    extern int CALLP CALLTYPE altiaSetPlayMacro(
#if AltiaFunctionPrototyping
    int macroNum
#endif
);
    extern int CALLP CALLTYPE AtSetPlayMacro(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int macroNum
#endif
);

    /*
     * int altiaStartPlay(double speed):
     * OR:
     * int AtStartPlay(AtConnectId connectId, double speed):
     * --------------------------------------------------------------------
     * This is a request to start playback.  The file to play from must have
     * already been chosen with a previous call to
     * altiaSetPlayFile()/AtSetPlayFile() or
     * altiaSetPlayMacro()/AtSetPlayMacro().
     *
     * The playback speed is selected by passing a floating point value where
     * 1.0 represents a playback speed equal to the recording speed, 2.0
     * representings a speed 2 times faster, etc.  Values in the range of
     * .01 to 100.0 are accepted.
     *
     * Playback can also be restarted with these functions after being stopped.
     * Or, if playback is already on, the playback speed can be changed by also
     * using one of these two functions. 
     *
     * If playback previously finished, a request to start it again forces
     * a repositioning back to the beginning of the playback file and then
     * playback is resumed.
     *
     * AtStartPlay() is the Toolkit version of altiaStartPlay()
     * and it takes the standard AtConnectId as an additional parameter.
     *
     * Both functions return 0 if they were successful.  If the speed is less
     * than .01, it is set to 1.0. If a communications failure occurred, -1
     * is returned. Not all illegal parameters can be detected prior to sending
     * the play request to the Altia interface.  The Altia interface does
     * not return an error to the program if it encounters a problem.  For
     * example, if no file was previously selected for playback.  If the file
     * selected for playback is not properly formatted, playback will most
     * likely be uneventful as no lines can be correctly parsed.
     */
    extern int CALLP CALLTYPE altiaStartPlay(
#if AltiaFunctionPrototyping
    ALT_DOUBLE speed
#endif
);
    extern int CALLP CALLTYPE AtStartPlay(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    ALT_DOUBLE speed
#endif
);

    /*
     * int altiaPlayMacro(int macroNum):
     * OR:
     * int AtPlayMacro(AtConnectId connectId, int macroNum):
     * --------------------------------------------------------------------
     * This is a request to select a macro file to play from and begin
     * playback immediately.  A number is passed to identify the macro file.
     * Given the number, a file name is created using the convention
     * "macro%d.rec" where %d is the value of macroNum converted to text.
     * The macro file associated with macroNum must be a properly formatted
     * Altia event file (usually a product of a previous record session) and
     * it must be readable.  This function is a convenience routine so as not
     * to require the caller to create a complete file name and then start
     * playback.
     *
     * These functions force playback to start at the beginning of the
     * designated macro file despite the current playback state.
     *
     * AtPlayMacro() is the Toolkit version of altiaPlayMacro()
     * and it takes the standard AtConnectId as a additional parameter.
     *
     * Both functions return 0 if they were successful.  If the macro number is
     * less than 0 or greater than 999, -2 is returned.  If a communications
     * failure occurred, -1 is return. Not all illegal parameters can be
     * detected prior to sending the select&play request to the Altia interface.
     * The Altia interface does not return an error to the program if it
     * receives a macro number for a file that does not exist or
     * the file is not readable.  It simply will not select the file for
     * playback.  If the file is not a properly formatted file, it will still
     * be opened, but playback will most likely be uneventful as no lines can
     * be correctly parsed.
     */
    extern int CALLP CALLTYPE altiaPlayMacro(
#if AltiaFunctionPrototyping
    int macroNum
#endif
);
    extern int CALLP CALLTYPE AtPlayMacro(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int macroNum
#endif
);

    /*
     * int altiaStopPlay(void):
     * OR:
     * int AtStopPlay(AtConnectId connectId):
     * --------------------------------------------------------------------
     * This is a request to stop playback.  If playback is on, it is stopped
     * and the playback position is preserved to allow a restart from a
     * altiaStartPlay()/AtStartPlay() or
     * altiaStepPlay()/AtStepPlay() call.  In addition, the playback position
     * can be moved to the beginning of the current playback file with a
     * call to altiaRewindPlay()/AtRewindPlay().
     *
     * AtStopPlay() is the Toolkit version of altiaStopPlay()
     * and it takes the standard AtConnectId as an additional parameter.
     *
     * Both functions return 0 if they were successful.
     * If a communications failure occurred, -1 is returned.
     * If playback is already off, this function has no affect and returns 0.
     */
    extern int CALLP CALLTYPE altiaStopPlay(
#if AltiaFunctionPrototyping
    void
#endif
);
    extern int CALLP CALLTYPE AtStopPlay(
#if AltiaFunctionPrototyping
    AtConnectId connectId
#endif
);

    /*
     * int altiaRewindPlay(void):
     * OR:
     * int AtRewindPlay(AtConnectId connectId):
     * --------------------------------------------------------------------
     * This is a request to rewind the current playback file if one is
     * selected.  If playback is currently on, it will continue from the
     * beginning of the file after the rewind is performed.
     *
     * AtRewindPlay() is the Toolkit version of altiaRewindPlay()
     * and it takes the standard AtConnectId as an additional parameter.
     *
     * Both functions return 0 if they were successful.
     * If a communications failure occurred, -1 is returned.
     * If no playback file is actually selected, this function has no affect
     * and 0 is returned.
     */
    extern int CALLP CALLTYPE altiaRewindPlay(
#if AltiaFunctionPrototyping
    void
#endif
);
    extern int CALLP CALLTYPE AtRewindPlay(
#if AltiaFunctionPrototyping
    AtConnectId connectId
#endif
);

    /*
     * int altiaStepPlay(int stepCount):
     * OR:
     * int AtStepPlay(AtConnectId connectId, int stepCount):
     * --------------------------------------------------------------------
     * This is a request to take a limited number of steps through the
     * currently selected playback file. The file to step from must have
     * already been chosen with a previous call to
     * altiaSetPlayFile()/AtSetPlayFile() or
     * altiaSetPlayMacro()/AtSetPlayMacro().
     *
     * The number of events to step through is chosen by stepCount.
     * If it is less than or equal to zero (0), then a step count of 1 is
     * chosen.
     *
     * If playback is currently stopped at a position other than the
     * start of the file or playback is in progress, stepping will occur
     * from the current playback point.
     *
     * After the desired number of steps are taken, playback is stopped and
     * can be resumed with another step request or by starting continuous
     * playback with a altiaStartPlay()/AtStartPlay() call.
     *
     * AtStepPlay() is the Toolkit version of altiaStepPlay()
     * and it takes the standard AtConnectId as an additional parameter.
     *
     * Both functions return 0 if they were successful.  If the step count
     * is less than or equal to 0, a step count of 1 is chosen.  If a
     * communications failure occurred, -1 is returned.
     * If no file is currently selected for playback or the end of the file
     * is reached, these functions have no affect and 0 is returned.
     */
    extern int CALLP CALLTYPE altiaStepPlay(
#if AltiaFunctionPrototyping
    int stepCount
#endif
);
    extern int CALLP CALLTYPE AtStepPlay(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int stepCount
#endif
);

    /*
     * int altiaSetRecordFile(CONST AltiaCharType *fileName):
     * OR:
     * int AtSetRecordFile(AtConnectId connectId,
     *                     CONST AltiaCharType *fileName):
     * --------------------------------------------------------------------
     * This is a request to select a file to record to.  The file must be
     * writable if it exists or the directory must be writable if the file
     * needs to be created.  This function creates a file if one does not
     * exist.  This implies that an empty file will be left over if this
     * function is called but recording is never started.
     *
     * This function prepares for recording but does not actually start
     * the recording process.  Recording is started with a call to
     * altiaStartRecord()/AtStartRecord()
     * or altiaAppendRecord()AtAppendRecord().
     *
     * AtSetRecordFile() is the Toolkit version of altiaSetRecordFile()
     * and it takes the standard AtConnectId as a additional parameter.
     *
     * Both functions return 0 if they were successful.  If the file name is
     * nil or empty, -2 is returned. If a communications failure occurred, -1
     * is return. Not all illegal parameters can be detected prior to sending
     * the selection request to the Altia interface.  The Altia interface does
     * not return an error to the program if it receives an illegal file name
     * or the file is not writable.  It simply will not select the file for
     * record. 
     */
    extern int CALLP CALLTYPE altiaSetRecordFile(
#if AltiaFunctionPrototyping
    CONST AltiaCharType *fileName
#endif
);
    extern int CALLP CALLTYPE AtSetRecordFile(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    CONST AltiaCharType *fileName
#endif
);

    /*
     * int altiaSetRecordMacro(int macroNum):
     * OR:
     * int AtSetRecordMacro(AtConnectId connectId, int macroNum):
     * --------------------------------------------------------------------
     * This is a request to select a macro file to record to.  A number is
     * passed to identify the macro file.  Given the number, a file name is
     * created using the convention "macro%d.rec" where %d is the value of
     * macroNum converted to text.  The macro file associated with macroNum
     * must be writable if it exists or the directory must be writable if the
     * file needs to be created.  This function creates a file if one does not
     * exist.  This implies that an empty file will be left over if this
     * function is called but recording is never started.
     *
     * This function prepares for recording but does not actually start
     * the recording process.  Recording is started with a call to
     * altiaStartRecord()/AtStartRecord()
     * or altiaAppendRecord()AtAppendRecord().
     *
     * This function is a convenience routine so as not to require the caller
     * to create a complete file name.
     *
     * AtSetRecordMacro() is the Toolkit version of altiaSetRecordMacro()
     * and it takes the standard AtConnectId as a additional parameter.
     *
     * Both functions return 0 if they were successful.  If the macro number is
     * less than 0 or greater than 999, -2 is returned.  If a communications
     * failure occurred, -1 is return. Not all illegal parameters can be
     * detected prior to sending the selection request to the Altia interface.
     * The Altia interface does not return an error to the program if it
     * receives a macro number for a file that cannot be opened or created.
     * It simply will not select the file for recording.
     */
    extern int CALLP CALLTYPE altiaSetRecordMacro(
#if AltiaFunctionPrototyping
    int macroNum
#endif
);
    extern int CALLP CALLTYPE AtSetRecordMacro(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int macroNum
#endif
);

    /*
     * int altiaStartRecord(int confirmOperation):
     * OR:
     * int AtStartRecord(AtConnectId connectId, int confirmOperation):
     * --------------------------------------------------------------------
     * This is a request to start recording from the current record file
     * position.  The record file must have been previously selected using
     * altiaSetRecordFile()/AtSetRecordFile()
     * or altiaSetRecordMacro()/AtSetRecordMacro().
     *
     * If the record file selection was the last record operation, then
     * recording will begin at the start of the current file.  If recording
     * was previously in progress and then stopped, then it will start at
     * file position following the last recorded position.
     *
     * If the record file already exists, the caller has the option of allowing
     * a confirmation dialog to appear which allows the user to terminate the
     * record session before the file is modified.  Pass a non-zero value
     * for confirmOperation to allow the dialog or a zero (0) to suppress it.
     *
     * AtStartRecord() is the Toolkit version of altiaStartRecord()
     * and it takes the standard AtConnectId as a additional parameter.
     *
     * Both functions return 0 if they were successful.  If a communications
     * failure occurred, -1 is return. If no file is currently selected for
     * recording, this function has no affect and 0 is returned.
     */
    extern int CALLP CALLTYPE altiaStartRecord(
#if AltiaFunctionPrototyping
    int confirmOperation
#endif
);
    extern int CALLP CALLTYPE AtStartRecord(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int confirmOperation
#endif
);

    /*
     * int altiaAppendRecord(int confirmOperation):
     * OR:
     * int AtAppendRecord(AtConnectId connectId, int confirmOperation):
     * --------------------------------------------------------------------
     * This is a request to start recording at the end of the currently
     * selected record file (i.e., recorded events will be appended to the
     * current contents of the selected file).  A record file is selected
     * using * altiaSetRecordFile()/AtSetRecordFile()
     * or altiaSetRecordMacro()/AtSetRecordMacro().
     *
     * 
     * If the record file already exists, the caller has the option of allowing
     * a confirmation dialog to appear which allows the user to terminate the
     * record session before the file is modified.  Pass a non-zero value
     * for confirmOperation to allow the dialog or a zero (0) to suppress it.
     *
     * AtAppendRecord() is the Toolkit version of altiaAppendRecord()
     * and it takes the standard AtConnectId as a additional parameter.
     *
     * Both functions return 0 if they were successful.  If a communications
     * failure occurred, -1 is return. If no file is currently selected for
     * recording, this function has no affect and 0 is returned.
     */
    extern int CALLP CALLTYPE altiaAppendRecord(
#if AltiaFunctionPrototyping
    int confirmOperation
#endif
);
    extern int CALLP CALLTYPE AtAppendRecord(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int confirmOperation
#endif
);

    /*
     * int altiaRecordMacro(int macroNum, int confirmOperation):
     * OR:
     * int AtRecordMacro(AtConnectId connectId, int macroNum,
     *                   int confirmOperation):
     * --------------------------------------------------------------------
     * This is a request to select a macro file to record to and start
     * recording immediately.  A number is passed to identify the macro file.
     * Given the number, a file name is created using the convention
     * "macro%d.rec" where %d is the value of macroNum converted to text.
     * The macro file associated with macroNum must be writable if it exists
     * or the directory must be writable if the file needs to be created.
     * This function creates a file if one does not exist.  This implies that
     * an empty file will be left over if this function is called but
     * recording is immediately stopped.
     *
     * If the macro file already exists, the caller has the option of allowing
     * a confirmation dialog to appear which allows the user to terminate the
     * record session before the file is modified.  Pass a non-zero value
     * for confirmOperation to allow the dialog or a zero (0) to suppress it.
     *
     * Recording is started at the beginning of the file regardless of the
     * existence of data in the file or the current state of recording.
     *
     * This function is a convenience routine so as not to require the caller
     * to create a complete file name.
     *
     * AtRecordMacro() is the Toolkit version of altiaRecordMacro()
     * and it takes the standard AtConnectId as a additional parameter.
     *
     * Both functions return 0 if they were successful.  If the macro number is
     * less than 0 or greater than 999, -2 is returned.  If a communications
     * failure occurred, -1 is return. Not all illegal parameters can be
     * detected prior to sending the selection request to the Altia interface.
     * The Altia interface does not return an error to the program if it
     * receives a macro number for a file that cannot be opened or created.
     * It simply will not select the file for recording.  The user can also
     * decide to abort the recording via the confirmation dialog if it appears.
     */
    extern int CALLP CALLTYPE altiaRecordMacro(
#if AltiaFunctionPrototyping
    int macroNum,
    int confirmOperation
#endif
);
    extern int CALLP CALLTYPE AtRecordMacro(
#if AltiaFunctionPrototyping
    AtConnectId connectId,
    int macroNum,
    int confirmOperation
#endif
);

    /*
     * int altiaStopRecord(void):
     * OR:
     * int AtStopRecord(AtConnectId connectId):
     * --------------------------------------------------------------------
     * This is a request to stop recording.  If recording is on, it is stopped
     * and the record position is preserved to allow a restart from a
     * altiaStartRecord()/AtStartRecord() call.
     *
     * AtStopRecord() is the Toolkit version of altiaStopRecord()
     * and it takes the standard AtConnectId as an additional parameter.
     *
     * Both functions return 0 if they were successful.
     * If a communications failure occurred, -1 is returned.
     * If recording is already off, this function has no affect and returns 0.
     */
    extern int CALLP CALLTYPE altiaStopRecord(
#if AltiaFunctionPrototyping
    void
#endif
);
    extern int CALLP CALLTYPE AtStopRecord(
#if AltiaFunctionPrototyping
    AtConnectId connectId
#endif
);



#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#undef CONST

#endif /* AltiaTools_h */
