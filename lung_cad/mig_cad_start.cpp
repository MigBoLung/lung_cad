#include "mig_config.h"
#include "mig_defs.h"

#include "mig_cad.h"
#include "mig_error_codes.h"

/***************************************************/
#include "log4cplus/logger.h"
#include "log4cplus/configurator.h"
#include "log4cplus/helpers/loglog.h"
#include "log4cplus/helpers/stringhelper.h"

using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;

/***************************************************/
#define APP_NAME        "lungcad"
#define APP_DESC        "Lung CAD"

/***************************************************/
/* private variables */
static char *_params_ini_f_name = NULL;
static mig_dic_t *_CadParams = NULL;

static Logger _CadLogger = Logger::getInstance ( CAD_LOGGER_NAME );

/***************************************************/
/* private function prototypes */

MIG_C_LINKAGE_START

/*
******************************************************************************
*                       INITIALIZE CAD SERVER
*
* Description : This function performs the following tasks :
*                       1. Read cad parameters from ini file fiven on command line
*                       2. Set up logging system
*
* Arguments   : 
*               argv - command line
*
* Returns     : MIG_OK on success
*               error code on failure
*
******************************************************************************
*/

static int
_initialize_server ( char **argv );

/*
******************************************************************************
*                       PRINT USAGE INSTRUCTIONS
*
* Description : This function prints cad usage instructions on stdout
*
******************************************************************************
*/

static void
_usage ();

/*
******************************************************************************
*                       DUMP CAD PARAMETERS TO LOG
*
* Description : This function writes all cad parameters to log file
*
******************************************************************************
*/

static void
_dump_parameters ();

MIG_C_LINKAGE_END

/***************************************************/
int
main ( int argc , char **argv )
{
    int ret = MIG_OK;

    if ( argc < 2 )
    {
        _usage ();
        exit ( EXIT_FAILURE );
    }

    /* initialize server */
    ret = _initialize_server ( argv );
    if ( ret != MIG_OK )
    {
        exit ( EXIT_FAILURE );
    }

    LOG4CPLUS_INFO ( _CadLogger , "Cad parameters file : " <<  _params_ini_f_name );
    
    /* dump parameters to log */
    _dump_parameters ();

    /* initializet CAD */
    ret = mig_cad_init ( _CadParams );
    if ( ret != MIG_OK )
    {
        LOG4CPLUS_FATAL ( _CadLogger , "cad init exited with code : " << ret );
        exit ( EXIT_FAILURE );
    }

    /* run CAD */
    ret = mig_cad_run ();
    if ( ret != MIG_OK )
    {
        LOG4CPLUS_FATAL ( _CadLogger , "cad exited with code : " << ret );
        exit ( EXIT_FAILURE );
    }

    exit ( EXIT_SUCCESS );
}

/***************************************************/
static void
_usage ()
{
    printf ( "\n%s" , APP_DESC );
    printf ( "\nUsage : %s cad_ini ..." , APP_NAME );
    printf ( "\n" );
}

/***************************************************/
static int
_initialize_server ( char **argv )
{
    /* read from general ini file */
    char *_logger_ini_f_name = NULL; 

    /* first setup server configuration file */
    _params_ini_f_name = (char*) malloc ( sizeof(char) * ( strlen( argv[1] ) + 1 ) );
    if ( _params_ini_f_name == NULL )
    {
        fprintf ( stderr , "\nError memory..." );
        return MIG_ERROR_MEMORY;
    }

    strcpy ( _params_ini_f_name , argv[1] );

    /* load parameters from file */
    _CadParams = mig_ut_ini_new ( _params_ini_f_name );
    if ( _CadParams == NULL )
    {
        fprintf ( stderr , "\nError loading parameters from : %s..." , _params_ini_f_name );
        return MIG_ERROR_IO;
    }

    /* setup logging system */
    _logger_ini_f_name =  
        mig_ut_ini_getstring ( _CadParams , PARAM_LOG_INI , DEFAULT_PARAM_LOG_INI );
    if ( _logger_ini_f_name == NULL )
    {
        fprintf ( stderr , "\nError loading log parameters..." );
        return MIG_ERROR_IO;
    }

    try
    {
        PropertyConfigurator::doConfigure ( _logger_ini_f_name );
    }
    catch ( ... )
    {
        fprintf ( stderr , "\nError loading log parameters from : %s..." , _logger_ini_f_name );
        return MIG_ERROR_IO;
    }

    return MIG_OK;
}

/***************************************************/
#define BUFF_SIZE 4096

static void
_dump_parameters ()
{
    char buffer[BUFF_SIZE];

    mig_ut_ini_dump_buffer ( _CadParams , buffer , BUFF_SIZE );
    LOG4CPLUS_DEBUG ( _CadLogger , "Parameters file contents : " << endl << buffer );
}

#undef BUFF_SIZE
