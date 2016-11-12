#ifndef __MIG_UT_CPU_H__
#define __MIG_UT_CPU_H__

#include "mig_config.h"
#include "mig_defs.h"

MIG_C_LINKAGE_START

#define MIG_HAS_MMX(flags)  ( ( (flags) & 0x08 ) >> 3 )
#define MIG_HAS_SSE(flags)  ( ( (flags) & 0x04 ) >> 2 )
#define MIG_HAS_SSE2(flags) ( ( (flags) & 0x02 ) >> 1 )
#define MIG_HAS_SSE3(flags) ( ( (flags) & 0x01 ) )

typedef struct _cpuinfo_t
{
        /* Siported Extensions */

        /* MMX , SSE , SSE2 , SSE3 */
        unsigned char flags;

        /* L1 data  cache */

        /* L1 line size -> bytes */
        int l1_line;

        /* L1 associativity */
        int l1_assoc;

        /* L1 size -> KBytes */
        int l1_size;

        /* L2 cache */

        /* L2 line size -> bytes */
        int l2_line;

        /* L2 associativity */
        int l2_assoc;

        /* L2 size -> KBytes */
        int l2_size;

        /* number of processors */
        int num;

} cpuinfo_t;

extern void
mig_ut_cpu_info ( cpuinfo_t *cpu );

extern int
mig_ut_cpu_thread_id ();

extern int
mig_ut_cpu_proc_id ();

MIG_C_LINKAGE_END

#endif // __MIG_UT_CPU_H__
