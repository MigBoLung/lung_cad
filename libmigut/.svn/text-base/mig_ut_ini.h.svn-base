#ifndef __MIG_UT_INI_H__
#define __MIG_UT_INI_H__

#include "mig_config.h"
#include "mig_defs.h"

MIG_C_LINKAGE_START

typedef struct _mig_dic_t
{
	/** Number of entries in dictionary */
	int n;

	/** Storage size */
	int size;

	/** List of string values */
	char **val;

	/** List of string keys */
	char **key;

	/** List of hash values for keys */
	unsigned *hash;

} mig_dic_t;

extern mig_dic_t*
mig_ut_ini_new ( char *ininame );

extern void
mig_ut_ini_free ( mig_dic_t *d );

extern int
mig_ut_ini_getnsec ( mig_dic_t *d );

extern char*
mig_ut_ini_getsecname (mig_dic_t *d , int n );

extern void
mig_ut_ini_dump ( mig_dic_t *d , FILE *f );

extern int
mig_ut_ini_dump_buffer ( mig_dic_t *d , char *buffer , int len );

extern void
mig_ut_ini_dump_ini ( mig_dic_t *d , FILE *f );

extern char*
mig_ut_ini_getkey ( mig_dic_t *d , char *section , char *key );

extern char*
mig_ut_ini_getstr ( mig_dic_t *d , char *key );

extern char*
mig_ut_ini_getstring ( mig_dic_t *d , char *key , char *def );

extern int
mig_ut_ini_getint ( mig_dic_t *d , char *key , int notfound );

extern int*
mig_ut_ini_getintarray ( mig_dic_t *d , char *key , int *len );

extern float
mig_ut_ini_getfloat ( mig_dic_t *d , char *key , float notfound );

extern float*
mig_ut_ini_getfloatarray ( mig_dic_t *d , char *key , int *len );

extern double
mig_ut_ini_getdouble ( mig_dic_t *d , char *key , double notfound );

extern double*
mig_ut_ini_getdoublearray ( mig_dic_t *d , char *key , int *len );

extern int
mig_ut_ini_getboolean ( mig_dic_t *d , char *key , int notfound );

extern int
mig_ut_ini_find_entry ( mig_dic_t *ini , char *entry );

extern int
mig_ut_ini_setstr ( mig_dic_t *ini , char *entry , char *val );

extern void
mig_ut_ini_unset ( mig_dic_t *ini , char *entry );

MIG_C_LINKAGE_END

#endif /* __MIG_UT_INI_H__ */

