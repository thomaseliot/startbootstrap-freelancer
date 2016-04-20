/**
 * @brief   Target OS abstraction layer
 *
 * @copyright (c) 2015 Altia, Inc.
 *
 * Permission to use, copy, and modify this software for any purpose is
 * hereby granted, provided that this copyright notice appears in all
 * copies and that the entity using, copying, or modifying this software
 * has purchased an Altia Deep Screen license.  Permission to sell or
 * distribute this source code is denied.
 *
 * Altia makes no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied warranty.
 *
 * ALTIA INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL ALTIA INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _OS_WRAPPER_H_
#define _OS_WRAPPER_H_

/******************************************************************************
 * Includes
 ******************************************************************************/


/******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef NULL
#define NULL 0
#endif

#if !defined(FALSE) || (FALSE!=0)
#define FALSE 0
#endif

#if !defined(TRUE) || (TRUE!=1)
#define TRUE 1
#endif


#define OS_CLOCK_REALTIME   0

#define OS_WAIT_FOREVER     (-1)
#define OS_SEM_Q_PRIORITY   1
#define OS_SEM_FULL         1

#define NBBY                8                           /* number of bits per byte */
#define NFDBITS             (sizeof(fd_mask) * NBBY)    /* bits per mask */
#define FD_SETSIZE          1024
#define howmany(x, y)       (((x)+((y)-1))/(y))

#define OS_FD_SET(n, p)     FD_SET(n, p)
#define OS_FD_CLR(n, p)     FD_CLR(n, p)
#define OS_FD_ISSET(n, p)   FD_ISSET(n, p)
#define OS_FD_ZERO(p)       FD_ZERO(p)


/******************************************************************************
 * Types
 ******************************************************************************/
typedef struct timeval
{
    unsigned long tv_sec;
    unsigned long tv_usec;
} timeval;

typedef struct timeval OS_timeval;


typedef struct timespec
{
    unsigned long tv_sec;
    unsigned long tv_nsec;
} timespec;

typedef struct timespec OS_timespec;


typedef long fd_mask;

struct fd_set
{
    fd_mask fds_bits[howmany(FD_SETSIZE, NFDBITS)];
};

typedef struct fd_set OS_fd_set;

typedef void * OS_SEM_ID;


/******************************************************************************
 * Configuration
 ******************************************************************************/


/******************************************************************************
 * APIs
 ******************************************************************************/
extern int os_open(void);
extern int os_close(void);
extern int os_clock_gettime(int clock_id, OS_timespec *tp);
extern int os_select(int width, OS_fd_set *readfds, OS_fd_set *writefds, OS_fd_set *exceptfds, OS_timeval *timeout);
extern OS_SEM_ID os_semBCreate(int idx, int options, int initialState);
extern int os_semTake(OS_SEM_ID semId, int timeout);
extern int os_semGive(OS_SEM_ID semId);

#endif  // #ifndef _OS_WRAPPER_H_

