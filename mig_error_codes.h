#ifndef __MIG_ERROR_CODES_H__
#define __MIG_ERROR_CODES_H__

/* error codes */
typedef enum
{
        MIG_OK                          =  0,
        MIG_ERROR_MEMORY                =  -1,
        MIG_ERROR_INTERNAL              =  -2,
        MIG_ERROR_UNSUPPORTED           =  -3,
        MIG_ERROR_PARAM                 =  -4,
        MIG_ERROR_IO                    =  -5,
        MIG_ERROR_EOF                   =  -6,
        MIG_ERROR_THREAD                =  -7,
        MIG_ERROR_INVALID_HANDLE        =  -8,
        MIG_ERROR_BUSY                  =  -9,
        MIG_ERROR_LOCK                  = -10,
        MIG_ERROR_UNLOCK                = -11,
        MIG_ERROR_DB                    = -12,
        MIG_ERROR_DIR_CONTENTS_EXCEEDED = -13
} MIG_STATUS;

#endif /* __MIG_ERROR_CODES_H__ */

/*******************************************************************/
/* DIXYGEN DOCUMENTATION */
/*******************************************************************/

/** \file mig_error_codes.h
    \brief Error codes for whole system.
*/

/** \enum MIG_STATUS
    \brief Return codes.
*/


