#ifndef __LIBMIGFPR_2_H__
#define __LIBMIGFPR_2_H__

#include "mig_config.h"
#include "mig_defs.h"

#include "mig_data_types.h"
#include "mig_data_image.h"
#include "mig_data_dicom.h"
#include "mig_data_cad.h"

#include "mig_params_cad.h"
#include "mig_error_codes.h"

#include "libmigut.h"
#include "libmigim.h"
#include "libmigst.h"
#include "libmigio.h"
#include "libmigsvm.h"
#include "libmigtag.h"

/* log4cplus */

#include "log4cplus/logger.h"
#include "log4cplus/configurator.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/helpers/loglog.h"
#include "log4cplus/helpers/stringhelper.h"

using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;

MIG_C_LINKAGE_START

extern DLLEXPORT int
mig_init ( mig_dic_t *params ,
           mig_cad_data_t *data );

extern DLLEXPORT int
mig_run ();

extern DLLEXPORT void
mig_cleanup ( void* );

extern DLLEXPORT void
mig_info ( mig_dll_info_t *info );

MIG_C_LINKAGE_END


#endif /* __LIBMIGFPR_2_H__ */
