#include "mig_scp_store.h"
#include "mig_scp_param_keys.h"
#include "mig_scp_error_codes.h"

#include "mig_data_dicom.h"
#include "mig_error_codes.h"

#include "libmigut.h"
#include "libmigio.h"
#include "libmigst.h"
#include "libmigdb.h"

/*AAAA for this ask Todor, we add it just to get the thing compiled */
#define MIG_FUNCTION_NAME "mig_scp_store.cpp"

/***********************************************************/
/* PRIVATE VARS */
/***********************************************************/

/* logger for the whole scp */
static Logger _log = Logger::getInstance ( SCP_LOGGER_NAME );

/* ini file parameters */
static mig_dic_t *_params = NULL;

/* database data */
static mig_db_t _db_data;

/* name of base directory for storage */
static char *_dir_base = NULL;

/* network - listening port number */
static int _port = DEFAULT_SCP_PORT;

/* network - maximum packet data unit */
static int _max_pdu = DEFAULT_SCP_MAX_PDU;

/* network - acse timeout */
static int _acse_timeout = DEFAULT_SCP_ACSE_TIMEOUT;

/* network - dimse timeout */
static int _dimse_timeout = DEFAULT_SCP_DIMSE_TIMEOUT;

/* association - calling ae title */
static char *_calling_aetitle = DEFAULT_SCP_AETITLE_CALLING;

/* association - called ae title ( my title ) */
static char *_called_aetitle =  DEFAULT_SCP_AETITLE_CALLED;

/* full path where current study is going to be stored */
static char _full_path[MAX_PATH];

static int
_is_first = 1;

static Float64 
_image_position_patient_start[] = 
        { 0.0 , 0.0 , 0.0 };

static Float64
_image_position_patient_end[] = 
        { 0.0 , 0.0 , 0.0 };

/* dicom info about data being received */ 
static mig_dcm_data_t _dicom_data;

/* size info about data being received */
static mig_size_t _size_data =
{
        0 , 0 , 0 , 0 , 
        0 , 0 , 0 ,
        0.0 , 0.0 , 0.0 , 0.0
};

#include <string>
#include <list>

static list<string> _instance_uids;

/***********************************************************/
/* storage callback data */
typedef struct _cbdata_t
{
        DcmFileFormat           *dcmff;
        T_ASC_Association       *assoc;

} cbdata_t;

/***********************************************************/
/* private function declarations */
/***********************************************************/
static void
_init_params ();

static int
_init_storage ();

static int
_init_network ( T_ASC_Network **net );

static int
_init_dcm_asc_config ( DcmAssociationConfiguration& asccfg );

static void
_cleanup ( T_ASC_Network *net );

static OFCondition
_accept_association ( T_ASC_Network *net ,
                      DcmAssociationConfiguration& asccfg );

static void
_dump_association_info ( T_ASC_Association *assoc );

static int
_parse_meta_info ( DcmDataset *dataset , 
                   mig_dcm_data_t *dicom_data ,
                   mig_size_t *size_data );

static OFCondition
_process_commands ( T_ASC_Association *assoc );

static OFCondition
_echo_scp ( T_ASC_Association *assoc ,
            T_DIMSE_Message *msg ,
            T_ASC_PresentationContextID presID );

static OFCondition
_store_scp ( T_ASC_Association *assoc ,
             T_DIMSE_Message *msg ,
             T_ASC_PresentationContextID presID );

static void
_execute_on_eos ();

static void
_store_cb ( void *data ,
            T_DIMSE_StoreProgress *progress ,
            T_DIMSE_C_StoreRQ *req ,
            char *image_f_name ,
            DcmDataset **dataset ,
            T_DIMSE_C_StoreRSP *rsp ,
            DcmDataset **status );

/***********************************************************/
/* EXPORTS */
/***********************************************************/
int
mig_scp_store ( mig_dic_t *parameters )
{
        DcmAssociationConfiguration asccfg;
        T_ASC_Network *net = NULL;

        int status;
        OFCondition cond = EC_Normal;

        if ( parameters == NULL )
                return MIG_SCP_ERROR_EINVAL;

	/* STARTUP CONFIGURATION */

	/* save ini parameters to local var */
	_params = parameters;

        /* check wether dicom dictionary is loaded */
        if ( !dcmDataDict.isDictionaryLoaded() )
        {
                LOG4CPLUS_ERROR ( (_log) , MIG_FUNCTION_NAME << " Loading dicom dictionary " );
		return MIG_SCP_ERROR_DCM_DICTIONARY;
        }

	/* initialize parameters */
	_init_params ();

	/* initialize storage */
	status = _init_storage ();
	if ( status != MIG_SCP_OK )
		goto get_out;

	/* initilize network */
	status = _init_network ( &net );
	if ( status != MIG_SCP_OK )
		goto get_out;

	/* initialize association configuration from file */
	status = _init_dcm_asc_config ( asccfg );
	if ( status != MIG_SCP_OK )
		goto get_out;

        /* START LISTENING + RECEIVING DATA */
        while ( cond == EC_Normal )
        {
		cond = _accept_association ( net , asccfg );
        }

        LOG4CPLUS_ERROR ( _log , 
                MIG_FUNCTION_NAME << " Accepting association : " << cond.text() );

get_out :

	_cleanup ( net );
	return status;
}

/***********************************************************/
/* private function definitions */
/***********************************************************/
static void
_init_params ()
{
	/* network - listening port number */
	_port = mig_ut_ini_getint ( _params ,
                                    PARAM_SCP_PORT ,
                                    DEFAULT_SCP_PORT );

	/* network - acse timeout */
	_acse_timeout = mig_ut_ini_getint ( _params ,
                                            PARAM_SCP_ACSE_TIMEOUT ,
                                            DEFAULT_SCP_ACSE_TIMEOUT );

	/* network - maximum packet data unit */
	_max_pdu = mig_ut_ini_getint ( _params ,
                                       PARAM_SCP_MAX_PDU ,
                                       DEFAULT_SCP_MAX_PDU );

	/* network - dimse timeout */
	_dimse_timeout = mig_ut_ini_getint ( _params ,
                                             PARAM_SCP_DIMSE_TIMEOUT ,
                                             DEFAULT_SCP_DIMSE_TIMEOUT );

	/* association - calling ae title */
	_calling_aetitle = mig_ut_ini_getstring ( _params ,
					          PARAM_SCP_CALLING_AE ,
					          DEFAULT_SCP_AETITLE_CALLING );

	/* association - called ae title ( my title ) */
	_called_aetitle = mig_ut_ini_getstring ( _params ,
					         PARAM_SCP_CALLED_AE ,
					         DEFAULT_SCP_AETITLE_CALLED );
}

/***********************************************************/
static int
_init_storage ()
{
	int status;
	int flag = 0;

        /* setup base directory for file storage */
        _dir_base = mig_ut_ini_getstr ( _params , PARAM_SCP_DIR_BASE );
        if ( _dir_base == NULL )
        {
                LOG4CPLUS_ERROR ( _log , MIG_FUNCTION_NAME << " Base directory parameter is null" );
                return MIG_SCP_ERROR_PARAMS_MISSING;
        }

        /* check if base directory for storage exists */
        status = mig_ut_fs_isdir ( _dir_base , &flag );
        if ( status != MIG_OK ) /* error io */
        {
                LOG4CPLUS_ERROR ( _log , MIG_FUNCTION_NAME << " Cannot open directory for reading " << _dir_base );
                return MIG_SCP_ERROR_IO;
        }

        if ( flag == 0 ) /* no error but directory does not exist */
        {
                LOG4CPLUS_ERROR ( _log , MIG_FUNCTION_NAME << " Directory missing " << _dir_base );
                return MIG_SCP_ERROR_IO;
        }

        /* get database file name */
        char *db_fname = 
                mig_ut_ini_getstr ( _params , PARAM_SCP_DB_FILE );
        if ( db_fname == NULL )
        {
                LOG4CPLUS_ERROR ( _log , 
                        MIG_FUNCTION_NAME << " Database file name parameter is null" );
                        return MIG_SCP_ERROR_PARAMS_MISSING;
        }

        /* try initializing database connection */
        status = mig_db_init ( &_db_data , db_fname );
        if ( status != MIG_OK )
        {
                LOG4CPLUS_ERROR ( _log , 
                        MIG_FUNCTION_NAME << " " << _db_data.err );
                        return MIG_SCP_ERROR_DCM_OPEN_DB;
        }

        /* cleanup receiving data structure */
        memset ( &_dicom_data , 
                0x00 , 
                sizeof( mig_dcm_data_t ) );
	
        return  MIG_SCP_OK;
}

/***********************************************************/
static int
_init_network ( T_ASC_Network **net )
{
#if defined(WIN32)

	/* load windows sockets library */

	WSADATA wsaData;
	if ( WSAStartup( MAKEWORD( 2 , 2 ) , &wsaData ) != NO_ERROR )
        {
                LOG4CPLUS_ERROR ( _log , MIG_FUNCTION_NAME << " Loading windows sockets " );
                return  MIG_SCP_ERROR_DCM_NETWORK_INIT;

        }
#endif

	/* initialize DCMTK network */
	OFCondition cond = ASC_initializeNetwork (  NET_ACCEPTOR ,
                                                    _port ,
                                                    _acse_timeout ,
                                                    net );
        if ( cond.bad() )
        {
                LOG4CPLUS_ERROR ( _log , MIG_FUNCTION_NAME << " Initializing network : " << cond.text() );
                return MIG_SCP_ERROR_DCM_NETWORK_INIT;
        }

        LOG4CPLUS_INFO( _log , \
        MIG_FUNCTION_NAME << " Listening on port : " << _port << ", with ACSE timeout : " << _acse_timeout );

	return MIG_SCP_OK;
}

/***********************************************************/
static int
_init_dcm_asc_config ( DcmAssociationConfiguration& asccfg )
{
	OFCondition cond;

	/* load association configuration file */
        char *asccfg_file = mig_ut_ini_getstr ( _params , PARAM_SCP_AS_CONFIG );
        if ( asccfg_file == NULL )
        {
                LOG4CPLUS_ERROR ( _log , MIG_FUNCTION_NAME << " Initializing association configuration " );
                return MIG_SCP_ERROR_DCM_ASC_CONFIG;
        }

        cond = DcmAssociationConfigurationFile::initialize ( asccfg , asccfg_file );
        if ( cond.bad() )
        {
                LOG4CPLUS_ERROR ( _log , MIG_FUNCTION_NAME << " Initializing association configuration : " << cond.text() );
                return MIG_SCP_ERROR_DCM_ASC_CONFIG;
        }

        if ( !asccfg.isKnownProfile ( PARAM_SCP_PROFILE ) )
        {
                LOG4CPLUS_ERROR ( _log , MIG_FUNCTION_NAME << " Unknown profile : " << PARAM_SCP_PROFILE );
                return MIG_SCP_ERROR_DCM_ASC_CONFIG;
        }

        if ( ! asccfg.isValidSCPProfile ( PARAM_SCP_PROFILE ) )
        {
                LOG4CPLUS_ERROR ( _log , MIG_FUNCTION_NAME << " Profile : " << PARAM_SCP_PROFILE << \
                        " not valid for SCP use. Duplicate abstract syntaxes found "  );
                return MIG_SCP_ERROR_DCM_ASC_CONFIG;
        }

	return MIG_SCP_OK;
}

/***********************************************************/
static void
_cleanup (  T_ASC_Network *net )
{
	if ( net )
		ASC_dropNetwork ( &net );

#if defined(WIN32)
        WSACleanup ();
#endif

	if ( mig_db_close ( &_db_data ) != MIG_OK )
                LOG4CPLUS_ERROR ( _log , MIG_FUNCTION_NAME << " mig_db_close " );

	_params = NULL;
}

/***********************************************************/
static OFCondition
_accept_association ( T_ASC_Network *net ,
                      DcmAssociationConfiguration& asccfg )
{
        T_ASC_Association *assoc;
        OFCondition cond = EC_Normal;

        cond = ASC_receiveAssociation ( net ,
                                        &assoc ,
                                        _max_pdu ,
                                        NULL ,
                                        NULL ,
                                        OFFalse );

        if ( cond.bad() )
        {
                LOG4CPLUS_ERROR ( _log , MIG_FUNCTION_NAME << " Receiving association : " << cond.text() );
                cond = EC_Normal;
                goto cleanup;
        }

        /* decide if to accept association based
           on ae calling and ae called titles */

        /* does the request come from a known ae title */
        if ( strcmp ( assoc->params->DULparams.callingAPTitle ,
		      _calling_aetitle ) != 0 )
        {
                T_ASC_RejectParameters rej =
                {
                        ASC_RESULT_REJECTEDPERMANENT,
                        ASC_SOURCE_SERVICEUSER,
                        ASC_REASON_SU_CALLINGAETITLENOTRECOGNIZED
                };

                cond = ASC_rejectAssociation ( assoc , &rej );
                cond = EC_Normal;
                LOG4CPLUS_INFO( _log , MIG_FUNCTION_NAME << " Rejecting because of calling title : " << assoc->params->DULparams.calledAPTitle );
                goto cleanup;
        }

        /* is the request for us  */
        if ( strcmp ( assoc->params->DULparams.calledAPTitle ,
		      _called_aetitle ) != 0 )
        {
                T_ASC_RejectParameters rej =
                {
                        ASC_RESULT_REJECTEDPERMANENT,
                        ASC_SOURCE_SERVICEUSER,
                        ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED
                };

                cond = ASC_rejectAssociation ( assoc , &rej );
                cond = EC_Normal;
                LOG4CPLUS_INFO( _log , MIG_FUNCTION_NAME << " Rejecting because of called title : " << assoc->params->DULparams.callingAPTitle );
                goto cleanup;
        }

        /* set presentation contexts as defined in config file */
        cond = asccfg.evaluateAssociationParameters ( PARAM_SCP_PROFILE , *assoc );
        if ( cond.bad() )
        {
                T_ASC_RejectParameters rej =
                {
                        ASC_RESULT_REJECTEDPERMANENT,
                        ASC_SOURCE_SERVICEUSER,
                        ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED
                };

                ASC_rejectAssociation ( assoc , &rej );
                cond = EC_Normal;
                 LOG4CPLUS_INFO( _log , MIG_FUNCTION_NAME << " Rejecting because of application contexts mismatch " );
                goto cleanup;
        }
        else
        {
                ASC_acknowledgeAssociation ( assoc );
                LOG4CPLUS_INFO( _log , MIG_FUNCTION_NAME << " Association accepted " );

        }

        _dump_association_info ( assoc );

        /* handle C-ECHO-RQ and C-STORE-RQ */
        cond = _process_commands ( assoc );

        if ( cond == DUL_PEERREQUESTEDRELEASE )
        {
                ASC_acknowledgeRelease ( assoc );
                cond = EC_Normal;
        }
        else
        {
                ASC_abortAssociation ( assoc );
                cond = EC_Normal;
        }

cleanup :

        ASC_dropSCPAssociation ( assoc );
        ASC_destroyAssociation ( &assoc );
        return cond;
}

/***********************************************************/
static OFCondition
_process_commands ( T_ASC_Association *assoc )
{
        OFCondition cond = EC_Normal;
        T_DIMSE_Message msg;
        T_ASC_PresentationContextID presID = 0;
        DcmDataset *statusDetail = NULL;

	/* this is the loop that takes care of
	   multiple requests per association, i.e. multiple
	   images from some source
	*/
        while ( cond == EC_Normal ||
                cond == DIMSE_NODATAAVAILABLE ||
                cond == DIMSE_OUTOFRESOURCES )
        {
                cond = DIMSE_receiveCommand ( assoc ,
                                              DIMSE_BLOCKING ,
                                              0 ,
                                              &presID ,
                                              &msg ,
                                              &statusDetail );
                if ( cond == EC_Normal )
                {
                        switch ( msg.CommandField )
                        {
                                case DIMSE_C_ECHO_RQ :

                                        cond = _echo_scp ( assoc , &msg , presID );
                                        break;

                                case DIMSE_C_STORE_RQ :

                                        /* store a single image */
                                        cond = _store_scp ( assoc , &msg , presID );

                                        /* increment slice number counter */
                                        _size_data.slices ++;
                                        break;

                                default :

                                        cond = DIMSE_BADCOMMANDTYPE;

                                        LOG4CPLUS_ERROR ( _log , MIG_FUNCTION_NAME <<
                                                        " Bad command : " << OFstatic_cast( unsigned ,
                                                          msg.CommandField ) );
                                        break;
                        }
                }
        }

	/* end of series */
	if ( ( _size_data.slices > 0 ) &&
	     ( cond == DUL_PEERREQUESTEDRELEASE ) )
	{
		LOG4CPLUS_INFO ( _log , 
                        MIG_FUNCTION_NAME << " Received : " << \
                        _size_data.slices << " images" );
		_execute_on_eos ();
	}

        return cond;
}

/***********************************************************/
static OFCondition
_echo_scp ( T_ASC_Association *assoc ,
            T_DIMSE_Message *msg ,
            T_ASC_PresentationContextID presID )
{
        LOG4CPLUS_DEBUG( _log , MIG_FUNCTION_NAME << " Processing an echo request " );

        OFCondition cond = DIMSE_sendEchoResponse ( assoc ,
                                                    presID ,
                                                    &msg->msg.CEchoRQ ,
                                                    STATUS_Success ,
                                                    NULL );
        return cond;
}

/***********************************************************/
static OFCondition
_store_scp ( T_ASC_Association *assoc ,
             T_DIMSE_Message *msg ,
             T_ASC_PresentationContextID presID )
{
        LOG4CPLUS_DEBUG( _log , 
                MIG_FUNCTION_NAME << " Processing a store request " );

        OFCondition cond = EC_Normal;
        T_DIMSE_C_StoreRQ *req = &msg->msg.CStoreRQ;

        cbdata_t data;
        data.assoc = assoc;

        DcmFileFormat dcmff;
        data.dcmff = &dcmff;
        DcmDataset *dset = dcmff.getDataset();

        if ( _dimse_timeout == 0 )
        {
                cond = DIMSE_storeProvider ( assoc , presID , req , NULL , 
                                             OFTrue , &dset , _store_cb ,
                                             &data , DIMSE_BLOCKING , 0 );
        }
        else
        {
                cond = DIMSE_storeProvider ( assoc , presID , req , NULL , 
                                             OFTrue , &dset , _store_cb ,
					     &data , DIMSE_NONBLOCKING , 
                                             _dimse_timeout );
        }

        if ( cond.bad() )
        {
                LOG4CPLUS_ERROR ( _log , 
                        MIG_FUNCTION_NAME << " Store failed : " << cond.text() );
        }

        return cond;
}

/***********************************************************/
static void
_store_cb ( void *data ,
            T_DIMSE_StoreProgress *progress ,
            T_DIMSE_C_StoreRQ *req ,
            char *image_f_name ,
            DcmDataset **dataset ,
            T_DIMSE_C_StoreRSP *rsp ,
            DcmDataset **status )
{
        static char out[MAX_PATH];
        const char *instance_uid = NULL;
        OFCondition cond;

        if ( progress->state == DIMSE_StoreEnd )
        {
                *status = NULL;
                cbdata_t *cbdata = OFstatic_cast( cbdata_t* , data );

                /* if this is the first slice in a stack */
                if ( _is_first == 1 )
                {                
                        /* parse dicom tags */
                        if ( _parse_meta_info ( *dataset , 
                                                &_dicom_data ,
                                                &_size_data ) != MIG_OK )
                        {
                                rsp->DimseStatus =
                                        STATUS_STORE_Error_CannotUnderstand;

                                LOG4CPLUS_ERROR ( _log , 
                                        MIG_FUNCTION_NAME << " Missing data fields " );
                                return;
                        }

		        /* save path to current series in global
                           variable so it is available to end of
                           series function */
                        snprintf ( _full_path , 
                                   MAX_PATH ,
                                   "%s%c%s%c%s%c" ,
                                   _dicom_data.patient_id , PATH_SEPARATOR ,
                                   _dicom_data.study_uid  , PATH_SEPARATOR ,
                                   _dicom_data.series_uid , PATH_SEPARATOR );
                        
                        /* create path for storage */
                        snprintf ( out , MAX_PATH , "%s%c%s" ,
                                   _dir_base , PATH_SEPARATOR , _full_path );

                        if ( mig_ut_fs_mkdir ( out ) != 0 )
                        {
                                rsp->DimseStatus =
                                        STATUS_STORE_Refused_OutOfResources;
                                LOG4CPLUS_ERROR ( _log , 
                                        MIG_FUNCTION_NAME << " Creating path : " << \
                                        out );
                                return;
                        }

                        /* save full path in databse structure */
                        /*
                        snprintf ( _dicom_data.storage , 
                                   MAX_PATH , 
                                   _full_path );
                        */
                        snprintf ( _dicom_data.storage , 
                                   MAX_PATH , out );
                        
                        _is_first = 0;
                }
                        
                E_TransferSyntax xfer
                        = (*dataset)->getOriginalXfer();
                
                cond = (*dataset)->findAndGetString( 
                        DCM_SOPInstanceUID , instance_uid );
                if ( cond != EC_Normal )
                        return;
	        
                snprintf ( out , MAX_PATH ,
                           "%s%s%s" , _dir_base , _full_path , instance_uid );

                cond = cbdata->dcmff->saveFile ( out , xfer );
                if ( cond != EC_Normal )
                {
                        LOG4CPLUS_ERROR ( _log , MIG_FUNCTION_NAME << " Writing data : " << \
                                out << endl << cond.text() );
                        rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
                }

                _instance_uids.push_back( string( instance_uid ) );

                /* get patient position for all slices */
                cond = (*dataset)->findAndGetFloat64 ( DCM_ImagePositionPatient ,
                        _image_position_patient_end[0] );
                cond = (*dataset)->findAndGetFloat64 ( DCM_ImagePositionPatient ,
                        _image_position_patient_end[1] , 1 );
                cond = (*dataset)->findAndGetFloat64 ( DCM_ImagePositionPatient ,
                        _image_position_patient_end[2] , 2 );
        }
}

/***********************************************************/
static void
_execute_on_eos ()
{
        int rc;
        char **idx;

	LOG4CPLUS_INFO ( _log , MIG_FUNCTION_NAME << " EOS " );
        
        /* get current date and time */
        rc = mig_ut_date_time ( (char*)&(_dicom_data.received_date) , 
                                (char*)&(_dicom_data.received_time) );
        if ( rc != MIG_OK )
        {
                LOG4CPLUS_ERROR ( _log ,
		MIG_FUNCTION_NAME << " Getting date and time " );
        }

        _size_data.dim_stack = _size_data.dim * _size_data.slices;
        _size_data.size_stack = _size_data.dim_stack * sizeof(Mig16u);

        _size_data.z_res = sqrt( 
                MIG_POW2( _image_position_patient_end[0] -  
                          _image_position_patient_start[0] ) +
                MIG_POW2( _image_position_patient_end[1] - 
                          _image_position_patient_start[1] ) +
                MIG_POW2( _image_position_patient_end[2] - 
                          _image_position_patient_start[2] ) ) /
                         ( _size_data.slices - 1 );

        /* instance uids */
        _dicom_data.instance_uids = (char**)
                calloc ( _size_data.slices + 1 , sizeof(char*) );
        if ( _dicom_data.instance_uids == NULL )
                return;
        
        idx = _dicom_data.instance_uids;
        list<string>::iterator it;
        
        for ( it = _instance_uids.begin() ; 
              it != _instance_uids.end() ; ++it , ++idx )
        {
                *idx = (char*) (*it).c_str();
        }
        
        /* try writing data to database */
        rc = mig_db_put_series ( &_db_data ,
                                 &_dicom_data ,
                                 &_size_data );
        if ( rc != MIG_OK )
        {
                LOG4CPLUS_ERROR ( _log ,
		MIG_FUNCTION_NAME << " Database put info..." );
        }


	    /* set receive status */
        rc = mig_db_set_status ( &_db_data , 
                _dicom_data.storage ,
                MIG_RECEIVE , MIG_PROC_STATUS_DONE );
        if ( rc != MIG_OK )
        {
				LOG4CPLUS_ERROR ( _log ,
				MIG_FUNCTION_NAME << " Set receive status " << rc << "..." );
                LOG4CPLUS_ERROR ( _log ,
				MIG_FUNCTION_NAME << "Db Error message " << _db_data.err << "..." );
        }
        
        /* set receive date */
        rc = mig_db_set_date ( &_db_data , 
                _dicom_data.storage ,
                MIG_RECEIVE , 
				_dicom_data.received_date ,
                _dicom_data.received_time );
        if ( rc != MIG_OK )
        {
                LOG4CPLUS_ERROR ( _log ,
				MIG_FUNCTION_NAME << " Set receive date-time " << rc << "..." );
				LOG4CPLUS_ERROR ( _log ,
				MIG_FUNCTION_NAME << "Db Error message " << _db_data.err << "..." );
        }

        /* set process status */
        rc = mig_db_set_status ( &_db_data , 
                _dicom_data.storage ,
                MIG_PROCESS , 
                MIG_PROC_STATUS_READY );
        if ( rc != MIG_OK )
        {
                LOG4CPLUS_ERROR ( _log ,
				MIG_FUNCTION_NAME << " Set process status " << rc << "..." );
				LOG4CPLUS_ERROR ( _log ,
				MIG_FUNCTION_NAME << "Db Error message " << _db_data.err << "..." );
        }


        _instance_uids.clear ();
        _is_first = 1;
        _full_path[0] = '\0';

        free ( _dicom_data.instance_uids );
        bzero ( &_dicom_data , sizeof(mig_dcm_data_t) );
        bzero ( &_size_data  , sizeof(mig_size_t) );
}

/***********************************************************/
static void
_dump_association_info ( T_ASC_Association *assoc )
{
        std::stringstream os;
        ASC_dumpParameters ( assoc->params , os );
        LOG4CPLUS_INFO ( _log , os.str() );
}

/***********************************************************/
static int
_parse_meta_info ( DcmDataset *dataset , 
                   mig_dcm_data_t *dicom_data ,
                   mig_size_t *size_data )
{        
        OFCondition cond;
        Uint16 rows;
        Uint16 cols;
        Float64 pixel_spacing[2];
        Float64 slice_thickness;
	
        const char *str = NULL;

        /* patient ID */
        cond = dataset->findAndGetString( DCM_PatientID , str );
        if ( cond != EC_Normal )
                return MIG_ERROR_UNSUPPORTED;
	snprintf ( dicom_data->patient_id , 
		   MIG_DCM_FIELD_LEN , "%s" , str );

        /* patients name */
        cond = dataset->findAndGetString( DCM_PatientsName , str );
        if ( cond != EC_Normal )
                return MIG_ERROR_UNSUPPORTED;
	snprintf ( dicom_data->patient_name , 
		   MIG_DCM_FIELD_LEN , "%s" , str );

        /* study UID */
        cond = dataset->findAndGetString( DCM_StudyInstanceUID , str );
        if ( cond != EC_Normal )
                return MIG_ERROR_UNSUPPORTED;
	snprintf ( dicom_data->study_uid , 
		   MIG_DCM_FIELD_LEN , "%s" , str );
        
        /* series UID */
        cond = dataset->findAndGetString( DCM_SeriesInstanceUID , str );
        if ( cond != EC_Normal )
                return MIG_ERROR_UNSUPPORTED;
	snprintf ( dicom_data->series_uid , 
		   MIG_DCM_FIELD_LEN , "%s" , str );

        /* study date */
        cond = dataset->findAndGetString( DCM_StudyDate , str );
        if ( cond != EC_Normal )
                return MIG_ERROR_UNSUPPORTED;
	snprintf ( dicom_data->study_date , 
		   MIG_DCM_FIELD_LEN , "%s" , str );
        
        /* study time */
        cond = dataset->findAndGetString( DCM_StudyTime , str );
        if ( cond != EC_Normal )
                return MIG_ERROR_UNSUPPORTED;
	snprintf ( dicom_data->study_time , 
		   MIG_DCM_FIELD_LEN , "%s" , str );
        
        /* received date */
        sprintf ( dicom_data->received_date , 
                  MIG_DCM_UNKNOWN_DATE_TAG );
        
        /* received time */
        sprintf ( dicom_data->received_time , 
                  MIG_DCM_UNKNOWN_TIME_TAG );
        
        /* processed date */
        sprintf ( dicom_data->processed_date , 
                  MIG_DCM_UNKNOWN_DATE_TAG );
        
        /* processed time */
        sprintf ( dicom_data->processed_time , 
                  MIG_DCM_UNKNOWN_TIME_TAG );
        
        /* sent date */
        sprintf ( dicom_data->sent_date , 
                  MIG_DCM_UNKNOWN_DATE_TAG );
        
        /* sent time */
        sprintf ( dicom_data->sent_time , 
                  MIG_DCM_UNKNOWN_TIME_TAG );

        /* convolution kernel */
        cond = dataset->findAndGetString ( DCM_ConvolutionKernel , str );
        if ( cond != EC_Normal )
		snprintf ( dicom_data->convolution_kernel , 
			   MIG_DCM_FIELD_LEN , MIG_DCM_UNKNOWN_TAG );
        else
                snprintf ( dicom_data->convolution_kernel , 
		           MIG_DCM_FIELD_LEN , "%s" , str );

        /* height */
        cond = dataset->findAndGetUint16 ( DCM_Rows , rows );
        if ( cond.bad() )
                return MIG_ERROR_UNSUPPORTED;
        size_data->h = (int) rows;

        /* width */
        cond = dataset->findAndGetUint16 ( DCM_Columns , cols );
        if ( cond.bad() )
                return MIG_ERROR_UNSUPPORTED;
        size_data->w = (int) cols;
              
        /* pixel spacing */
        cond = dataset->findAndGetFloat64 ( DCM_PixelSpacing ,
                                            pixel_spacing[0] );                            
        if ( cond.bad() )
                return MIG_ERROR_UNSUPPORTED;
        size_data->h_res = (float) pixel_spacing[0];

        size_data->dim = size_data->w * size_data->h;
        size_data->size = size_data->dim * sizeof(Mig16u);
                
        cond = dataset->findAndGetFloat64 ( DCM_PixelSpacing ,
                                            pixel_spacing[1] , 1 );
        if ( cond.bad() )
                return MIG_ERROR_UNSUPPORTED;
        size_data->v_res = (float) pixel_spacing[1];

        /* slice thickness */
        cond = dataset->findAndGetFloat64 ( DCM_SliceThickness,
                                            slice_thickness );
        if ( cond.bad() )
                return MIG_ERROR_UNSUPPORTED;
        size_data->thickness = (float) slice_thickness;

        /* get patient position for first slice */
        cond = dataset->findAndGetFloat64 ( DCM_ImagePositionPatient ,
                                            _image_position_patient_start[0] );
        if ( cond.bad() )
                return MIG_ERROR_UNSUPPORTED;

        cond = dataset->findAndGetFloat64 ( DCM_ImagePositionPatient ,
                                             _image_position_patient_start[1] , 1 );
        if ( cond.bad() )
                return MIG_ERROR_UNSUPPORTED;

        cond = dataset->findAndGetFloat64 ( DCM_ImagePositionPatient ,
                                            _image_position_patient_start[2] , 2 );
        if ( cond.bad() )
                return MIG_ERROR_UNSUPPORTED;

        return MIG_OK;
}

