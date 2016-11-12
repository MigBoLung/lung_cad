/*
******************************************************************************
*
* Author      : Todor Petkov
* Filename    : mig_timer.c
* Created     : 2007/06/21
* Description : Various timing functions
*
******************************************************************************
*/

#include "mig_ut_time.h"

/*
******************************************************************************
*                               GLOBAL FUNCTIONS
******************************************************************************
*/

/*
******************************************************************************
*                               TIME - TICKS
******************************************************************************
*/

#if defined(WIN32)

ticks
getticks ()
{
	uint32_t ts1 , ts2;

	__asm {
      rdtsc
      mov ts1 , eax
      mov ts2 , edx
   }

	return ( ( (ticks) ts2 << 32 ) |
	         ( (ticks) ts1 ) );
}

#else

ticks
getticks ()
{
   return 0;
}

#endif

/****************************************************************************/

double
elapsed_ticks ( ticks t1 , ticks t0 )
{
	return ( ( (double)t1 ) - ( (double)t0 ) );
}

/*
******************************************************************************
*                               TIME - CPU
******************************************************************************
*/

clock_t
getticks_cpu ()
{
   return clock ();
}

/****************************************************************************/

double
elapsed_cpu ( clock_t t1 , clock_t t0 )
{
	return ( ( (double)t1/(double)CLOCKS_PER_SEC) -
	         ( (double)t0/(double)CLOCKS_PER_SEC) );

}

/*
******************************************************************************
*                               TIME - SYS
******************************************************************************
*/

time_t
getticks_sys ()
{
   return time(NULL);
}

/****************************************************************************/

double
elapsed_sys ( time_t t1 , time_t t0 )
{
	return ( (double)t1 - (double)t0 );
}

/*
******************************************************************************
*                               GET CURRENT DATE AND TIME
******************************************************************************
*/

int
mig_ut_date_time ( char *curr_date , char *curr_time )
{
   struct tm *t;
   time_t curr;
   
   if ( time ( &curr ) == -1 ) {
      return -1;
   }
   
   t = localtime ( &curr );
   strftime ( curr_time , TIME_LEN , TIME_FMT , t );  
   strftime ( curr_date , DATE_LEN , DATE_FMT , t );
   return 0;
}
