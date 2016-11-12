#include <pthread.h>
#include "mig_ut_cpu.h"

/***************************************************************/
/* PRIVATE */
/***************************************************************/
#define CPUID_MMX       0x00800000
#define CPUID_SSE       0x02000000
#define CPUID_SSE2      0x04000000
#define CPUID_SSE3      0x00000001

static unsigned int regs[4];

MIG_C_LINKAGE_START

static void
_intel_cache ( cpuinfo_t *cpu );

static void
_amd_cache ( cpuinfo_t *cpu );

#if defined(WIN32)

extern void
mig_cpuid ( unsigned int level ,
            unsigned int regs[4] );

#else /* LINUX */

extern void
_mig_cpuid ( unsigned int level ,
             unsigned int regs[4] );

#define mig_cpuid(a,b)  _mig_cpuid(a,b)

#endif

MIG_C_LINKAGE_END

/***************************************************************/
/* EXPORTS */
/***************************************************************/

/***************************************************************/
#if defined(WIN32)
/***************************************************************/

int
mig_ut_cpu_thread_id ()
{
        return ( GetCurrentThreadId () >> 2 );
}

int
mig_ut_cpu_proc_id ()
{
        return ( (int) GetCurrentProcessId() );
}

/***************************************************************/
#else   // LINUX
/***************************************************************/

int
mig_ut_cpu_thread_id ()
{
        return ( (int) pthread_self() );
}

int
mig_ut_cpu_proc_id ()
{
        return ( (int) getpid() );
}

/***************************************************************/
#endif // LINUX
/***************************************************************/

void
mig_ut_cpu_info ( cpuinfo_t *cpu )
{
        memset ( cpu , 0 , sizeof(cpuinfo_t));

        /* number of processors */
#if defined(WIN32)
        cpu->num = pthread_num_processors_np();
#else
        cpu->num = (int) sysconf( _SC_NPROCESSORS_CONF );
#endif

        /* get supported instruction sets */
        mig_cpuid ( 0x00000001 , regs );

        cpu->flags  = ( regs[3] & CPUID_MMX  ) >> 20;
        cpu->flags |= ( regs[3] & CPUID_SSE  ) >> 23;
        cpu->flags |= ( regs[3] & CPUID_SSE2 ) >> 25;
        cpu->flags |= ( regs[2] & CPUID_SSE3 );

        /* setup cache data */
        mig_cpuid ( 0x00000002 , regs );

        if ( regs[0] & 0x00000000FF )
                _intel_cache( cpu );
        else
                _amd_cache ( cpu );
}

/***************************************************************/
/* PRIVATE */
/***************************************************************/

static void
_intel_cache ( cpuinfo_t *cpu )
{
        int i , j;
        unsigned char val;

        /* for all regs */
        for ( i = 0 ; i < 4 ; ++i )
        {
                /* if bit 31 is not 0 the reg does
                   not contain valid data */
                if ( regs[i] & 0x80000000 )
                        continue;

                /* for all bytes in reg */
                for ( j = 0 ; j < 4 ; ++j , regs[i] >>= 8 )
                {
                        val = regs[i] & 0x000000FF;

                        switch ( val )
                        {
                                // L1 data
                                case 0x0A :

                                        cpu->l1_line  = 32;
                                        cpu->l1_assoc = 2;
                                        cpu->l1_size  = 8;
                                        break;

                                case 0x0C :

                                        cpu->l1_line  = 32;
                                        cpu->l1_assoc = 4;
                                        cpu->l1_size  = 16;
                                        break;

                                case 0x2C :

                                        cpu->l1_line  = 64;
                                        cpu->l1_assoc = 8;
                                        cpu->l1_size  = 32;
                                        break;

                                case 0x60 :

                                        cpu->l1_line  = 64;
                                        cpu->l1_assoc = 8;
                                        cpu->l1_size  = 16;
                                        break;

                                case 0x66 :

                                        cpu->l1_line  = 64;
                                        cpu->l1_assoc = 4;
                                        cpu->l1_size  = 8;
                                        break;

                                case 0x67 :

                                        cpu->l1_line  = 64;
                                        cpu->l1_assoc = 4;
                                        cpu->l1_size  = 16;
                                        break;

                                case 0x68 :

                                        cpu->l1_line  = 64;
                                        cpu->l1_assoc = 4;
                                        cpu->l1_size  = 32;
                                        break;

                                // L2
                                case 0x39 :

                                        cpu->l2_line  = 64;
                                        cpu->l2_assoc = 4;
                                        cpu->l2_size  = 128;
                                        break;

                                case 0x3A :

                                        cpu->l2_line  = 64;
                                        cpu->l2_assoc = 6;
                                        cpu->l2_size  = 192;
                                        break;

                                case 0x3B :

                                        cpu->l2_line  = 64;
                                        cpu->l2_assoc = 2;
                                        cpu->l2_size  = 128;
                                        break;

                                case 0x3C :

                                        cpu->l2_line  = 64;
                                        cpu->l2_assoc = 4;
                                        cpu->l2_size  = 256;
                                        break;

                                case 0x3D :

                                        cpu->l2_line  = 64;
                                        cpu->l2_assoc = 6;
                                        cpu->l2_size  = 384;
                                        break;

                                case 0x3E :

                                        cpu->l2_line  = 64;
                                        cpu->l2_assoc = 4;
                                        cpu->l2_size  = 512;
                                        break;

                                case 0x41 :

                                        cpu->l2_line  = 32;
                                        cpu->l2_assoc = 4;
                                        cpu->l2_size  = 128;
                                        break;

                                case 0x42 :

                                        cpu->l2_line  = 32;
                                        cpu->l2_assoc = 4;
                                        cpu->l2_size  = 256;
                                        break;

                                case 0x43 :

                                        cpu->l2_line  = 32;
                                        cpu->l2_assoc = 4;
                                        cpu->l2_size  = 512;
                                        break;

                                case 0x44 :

                                        cpu->l2_line  = 32;
                                        cpu->l2_assoc = 4;
                                        cpu->l2_size  = 1024;
                                        break;

                                case 0x45 :

                                        cpu->l2_line  = 32;
                                        cpu->l2_assoc = 4;
                                        cpu->l2_size  = 2048;
                                        break;

                                case 0x49 :

                                        cpu->l2_line  = 64;
                                        cpu->l2_assoc = 16;
                                        cpu->l2_size  = 4096;
                                        break;


                                case 0x78 :

                                        cpu->l2_line  = 64;
                                        cpu->l2_assoc = 4;
                                        cpu->l2_size  = 1024;
                                        break;

                                case 0x79 :

                                        cpu->l2_line  = 64;
                                        cpu->l2_assoc = 8;
                                        cpu->l2_size  = 128;
                                        break;

                                case 0x7A :

                                        cpu->l2_line  = 64;
                                        cpu->l2_assoc = 8;
                                        cpu->l2_size  = 256;
                                        break;

                                case 0x7B :

                                        cpu->l2_line  = 64;
                                        cpu->l2_assoc = 8;
                                        cpu->l2_size  = 512;
                                        break;

                                case 0x7C :

                                        cpu->l2_line  = 64;
                                        cpu->l2_assoc = 8;
                                        cpu->l2_size  = 1024;
                                        break;

                                case 0x7D :

                                        cpu->l2_line  = 64;
                                        cpu->l2_assoc = 8;
                                        cpu->l2_size  = 2048;
                                        break;

                                case 0x7F :

                                        cpu->l2_line  = 64;
                                        cpu->l2_assoc = 2;
                                        cpu->l2_size  = 512;
                                        break;

                                case 0x82 :

                                        cpu->l2_line  = 32;
                                        cpu->l2_assoc = 8;
                                        cpu->l2_size  = 256;
                                        break;

                                case 0x83 :

                                        cpu->l2_line  = 32;
                                        cpu->l2_assoc = 8;
                                        cpu->l2_size  = 512;
                                        break;

                                case 0x84 :
                                                                                                                        cpu->l2_line  = 32;
                                        cpu->l2_assoc = 8;
                                        cpu->l2_size  = 1024;
                                        break;

                                case 0x85 :

                                        cpu->l2_line  = 32;
                                        cpu->l2_assoc = 8;
                                        cpu->l2_size  = 2048;
                                        break;

                                case 0x86 :

                                        cpu->l2_line  = 64;
                                        cpu->l2_assoc = 4;
                                        cpu->l2_size  = 512;
                                        break;

                                case 0x87 :
                                                                                                                        cpu->l2_line  = 64;
                                        cpu->l2_assoc = 8;
                                        cpu->l2_size  = 1024;
                                        break;
                        }
                }
        }
}

/***************************************************************/
static void
_amd_cache ( cpuinfo_t *cpu )
{
        mig_cpuid ( 0x80000005 , regs );

        cpu->l1_line  = ( regs[3] & 0x000000FF );
        cpu->l1_assoc = ( regs[3] >> 16 ) & 0x000000FF;
        cpu->l1_size  = ( regs[3] >> 24 ) & 0x000000FF;

        mig_cpuid ( 0x80000006 , regs );


        cpu->l2_line  = ( regs[3] & 0x000000FF );
        cpu->l2_assoc = ( regs[3] >> 12 ) & 0x0000000F;
        cpu->l2_size  = ( regs[3] >> 16 ) & 0x0000FFFF;

        switch ( cpu->l2_assoc )
        {
                case 6 :
                        cpu->l2_assoc = 8;
                        break;
                case  8 :
                        cpu->l2_assoc = 16;
                        break;
                case 255 :
                        cpu->l2_assoc = 0;
                        break;
        }
}

