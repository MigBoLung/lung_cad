#ifndef __STORESCP_H__
#define __STORESCP_H__

#include "mig_config.h"
#include "mig_defs.h"
#include "libmigut.h"

/* dcmtk */
#include "dcmtk/config/osconfig.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#define INCLUDE_CCTYPE
#define INCLUDE_CSIGNAL

#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdebug.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofdatime.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmnet/dcasccfg.h"
#include "dcmtk/dcmnet/dcasccff.h"

/* log4cplus */

#include "log4cplus/logger.h"
#include "log4cplus/configurator.h"
#include "log4cplus/helpers/loglog.h"
#include "log4cplus/helpers/stringhelper.h"

using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;

MIG_C_LINKAGE_START

extern int
mig_scp_store ( mig_dic_t *parameters );

MIG_C_LINKAGE_END

#endif /* __STORESCP_H__ */


