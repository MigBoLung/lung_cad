#include "mig_config.h"
#include "mig_defs.h"

#include "mig_scp_store.h"
#include "mig_scp_param_keys.h"
#include "mig_scp_error_codes.h"

/***************************************************/
#include "log4cplus/logger.h"
#include "log4cplus/configurator.h"
#include "log4cplus/helpers/loglog.h"
#include "log4cplus/helpers/stringhelper.h"

using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;

/***************************************************/
#define APP_NAME        "dcmstore"
#define APP_DESC        "DICOM Store SCP"

/***************************************************/
/* private variables */
static char *params_ini_f_name = NULL;
static mig_dic_t *params = NULL;

static char *logger_ini_f_name = NULL;
static Logger logger = Logger::getInstance ( SCP_LOGGER_NAME );

/***************************************************/
/* private function prototypes */

MIG_C_LINKAGE_START

static int
_initialize_server ( char **argv );

static void
_cleanup_server ();

static void
_usage ();

static void
_dump_parameters ( mig_dic_t *d );

MIG_C_LINKAGE_END

/***************************************************/
int
main ( int argc , char **argv )
{
        int ret = MIG_SCP_OK;
        
        if ( argc <= 2 )
        {
                _usage ();
                exit ( EXIT_FAILURE );
        }
        
        /* initialize server */
        ret = _initialize_server ( argv );
        if ( ret != MIG_SCP_OK )
        {
                exit ( EXIT_FAILURE );
        }

        LOG4CPLUS_INFO ( logger , "Server parameters file : " <<  params_ini_f_name );
        LOG4CPLUS_INFO ( logger , "Logging parameters file : " << logger_ini_f_name );
        
        _dump_parameters ( params );
        
        /* start DICOM Storage SCP */
        ret = mig_scp_store ( params );
        if ( ret != MIG_SCP_OK )
        {
                LOG4CPLUS_FATAL ( logger , "storescp exited with code : " << ret );
                exit ( EXIT_FAILURE );
        }
        
        /* if we get here it means we are exiting,
           so cleanup */
        
        _cleanup_server ();
        
        exit ( EXIT_SUCCESS );
}

/***************************************************/
static void
_usage ()
{
        printf ( "\n%s" , APP_DESC );
        printf ( "\nUsage : %s scp_ini log_ini..." , APP_NAME );
        printf ( "\n" );
}

/***************************************************/
static int
_initialize_server ( char **argv )
{
        /* first setup server configuration file */
        params_ini_f_name = (char*)
                malloc ( sizeof(char) * ( strlen( argv[1] ) + 1 ) );
        if ( params_ini_f_name == NULL )
        {
                fprintf ( stderr , 
                "\nError memory..." );
                return MIG_SCP_ERROR_ENOMEM;
        }
        
        strcpy ( params_ini_f_name , argv[1] );

        /* load parameters from file */
        params = mig_ut_ini_new ( params_ini_f_name );
        if ( params == NULL )
        {
                fprintf ( stderr , 
                "\nError loading parameters from : %s..." ,
                params_ini_f_name );
                return MIG_SCP_ERROR_LOAD_PARAMS;
        }

        /* second setup logging system */
        logger_ini_f_name = (char*)
                malloc ( sizeof(char) * ( strlen( argv[2] ) + 1 ) );
        if ( logger_ini_f_name == NULL )
        {
                fprintf ( stderr , 
                "\nError memory..." );
                return MIG_SCP_ERROR_ENOMEM;
        }
                
        strcpy ( logger_ini_f_name , argv[2] );
        
        try 
        {
                PropertyConfigurator::doConfigure ( logger_ini_f_name );
        }
        catch ( ... )
        {
                fprintf ( stderr , 
                "\nError loading log parameters from : %s..." ,
                logger_ini_f_name );
                return MIG_SCP_ERROR_LOAD_LOG;        
        }

        return MIG_SCP_OK;
}

/***************************************************/
static void
_cleanup_server ()
{
        mig_ut_ini_free ( params );
        Logger::shutdown ();
        
        free( params_ini_f_name );
        free( logger_ini_f_name );
}

/***************************************************/
static void
_dump_parameters ( mig_dic_t *d )
{
        char buffer[MAX_PATH];
        
        mig_ut_ini_dump_buffer ( params , buffer , MAX_PATH );
        LOG4CPLUS_INFO ( logger , 
                "Parameters file contents : " << endl << buffer );
        
}
