/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_timer.h
* Created     : 2007/06/21
* Description : Various timing functions
*
******************************************************************************
*/

#ifndef __MIG_TIMER_H__
#define __MIG_TIMER_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "mig_data_types.h"

MIG_C_LINKAGE_START

/*
******************************************************************************
*                               DATA TYPES
******************************************************************************
*/

typedef unsigned int uint32_t;
#if defined(WIN32)
  typedef long long int64_t;
#endif
typedef int64_t ticks;

/*
******************************************************************************
*                               PROTOTYPES
******************************************************************************
*/

/*
******************************************************************************
*                                GET CURRENT TICKS
*
* Description : This function returns current number of system ticks
*               (since last reboot)
*
* Arguments   : none
*
* Returns     : current number of ticks
*
* Notes       : ticks is defined as 64 bit integer
*
******************************************************************************
*/

ticks getticks ();

/*
******************************************************************************
*                                CALCULATE ELAPSED TICKS
*
* Description : This function calculates elapsed ticks
*
* Arguments   : t1 end ticks number
*               t0 start ticks number
*
* Returns     : elapsed ticks as double
*
* Notes       :
*
******************************************************************************
*/

double
elapsed_ticks ( ticks t1 , ticks t0 );

/*
******************************************************************************
*                                GET CURRENT CPU TIME
*
* Description : This function returns current cpu time using POSIX clock()
*               function
*
* Arguments   : none
*
* Returns     : current clock time
*
* Notes       : on 32bit machines there is wrap around after 36 minutes
*
******************************************************************************
*/

clock_t
getticks_cpu ();

/*
******************************************************************************
*                                CALCULATE ELAPSED CPU TIME
*
* Description : This function calculates elapsed cpu time
*
* Arguments   : t1 end cpu time
*               t0 start cpu time
*
* Returns     : elapsed cpu time in seconds
*
* Notes       :
*
******************************************************************************
*/

double
elapsed_cpu ( clock_t t1 , clock_t t0 );

/*
******************************************************************************
*                                GET CURRENT SYSTEM TIME ( IO + CPU )
*
* Description : This function returns current system time elapsed since Epoche
*               using POSIX time() function
*
* Arguments   : none
*
* Returns     : current time value since Epoch
*
* Notes       :
*
******************************************************************************
*/

time_t
getticks_sys ();

/*
******************************************************************************
*                                CALCULATE ELAPSED SYSTEM TIME
*
* Description : This function calculates elapsed system time ( io + cpu )
*
* Arguments   : t1 end system time
*               t0 start system time
*
* Returns     : elapsed system time in seconds
*
* Notes       :
*
******************************************************************************
*/

double
elapsed_sys ( time_t t1 , time_t t0 );


/**********************************************************/
#define DATE_FMT        "%Y%m%d"
#define TIME_FMT        "%H%M%S"
#define DATE_LEN        (8+1)
#define TIME_LEN        (6+1)

extern int
mig_ut_date_time ( char *curr_date , char *curr_time );

MIG_C_LINKAGE_END

#endif /* __MIG_TIMER_H__ */
