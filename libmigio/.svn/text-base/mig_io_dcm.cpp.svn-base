#include "mig_io_dcm.h"
#include "mig_st_lst.h"
#include "mig_ut_mem.h"
#include "mig_ut_bit.h"
#include "mig_error_codes.h"

/*********************************************************************/
/* Type of DICOM file we know how to read */
#define ALLOWED_BITS_ALLOCATED                  16
#define ALLOWED_SAMPLES_PER_PIXEL               1
#define ALLOWED_PHOTOMETRIC_INTERPRETATION      "MONOCHROME2\0"

/*********************************************************************/
/* directory entry */
typedef struct
{
        char    *fname;
        char    *instance_uid;
        int     instance_number;

} _dir_entry;

/************************************************************************/
/* dicom internal data */
typedef struct __info_i
{
    char photometric_interpretation[MIG_DCM_FIELD_LEN];
    Float64 pixel_spacing[2];
    Float64 slice_thickness;
    Uint16 samples_per_pixel;
    Uint16 rows;
    Uint16 cols;
	Uint16 bits_allocated;
    Uint16 bits_stored;
    mig_dcm_data_t *out;

} _info_i;

/*********************************************************************/
/* parse files in directory "path". Return filename list in d.
   d is made of _mig_dicom_dir_entry structures. Entries are ordered by
   dicom instance number of each image */
static int
_parse_dir ( char *path , mig_lst_t *d );

/* directory entries comparaison function */
static int
_dir_cmp_f ( const void *a , const void *b );

/* directory entries free function */
static void
_dir_free_f ( void *data );

/* read DICOM tags from given dataset */
static int
_data_info_get ( DcmDataset *dataset,
                 _info_i *info );

/* check weather current file is DICOM part 10 complient */
static int
_data_check ( char *fname );

/* check weather we know how to read this kind of DICOM files */
static int
_data_check_info ( _info_i *info );

/*************************************************************/
/* EXPORTS */
/*************************************************************/
int
mig_dcm_get_info ( mig_dcm_data_t *dicom_data ,
                   mig_size_t *size_data )
{
    int ret , i;
    char FullName[MAX_PATH];
    char Storage[MAX_PATH];

    OFCondition cond;
        
    DcmFileFormat df;
	DcmDataset *dataset;
        
    Float64 image_position_patient_start[3];
    Float64 image_position_patient_end[3];

    mig_lst_t dir_contents = { NULL , NULL , 0  , free };
    _dir_entry *dir_entry = NULL;
    _info_i info;

    /* make sure data dictionary is loaded */
    if ( !dcmDataDict.isDictionaryLoaded() )
        return MIG_ERROR_INTERNAL;

    /* zero output */
    dicom_data->file_names = NULL;

    /* copy storage location to local 
       variable making sure that storage
       path end with a slash */
    if ( dicom_data->storage[strlen( dicom_data->storage )] == MIG_PATH_SEPARATOR )
        snprintf ( (char*) Storage , MAX_PATH , "%s" , dicom_data->storage ); 
    else
        snprintf ( (char*) Storage , MAX_PATH , "%s%c" , dicom_data->storage , MIG_PATH_SEPARATOR ); 

    /* get directory contents */
    ret = _parse_dir ( Storage , &dir_contents );
    if ( ret != MIG_OK )
        goto error;

    /* get first file in directory */
    dir_entry = (_dir_entry*) mig_lst_peek_head ( &dir_contents );
    if ( dir_entry == NULL )
    {
        ret = MIG_ERROR_INTERNAL;
        goto error;
    }

    /* load first dicom file found */
    snprintf ( (char*) FullName , MAX_PATH , "%s%s" , Storage , dir_entry->fname );
    cond = df.loadFile ( FullName );
    if ( cond.bad() )
    {
        ret = MIG_ERROR_IO;
        goto error;
    }

    dataset = df.getDataset ();
    if ( !dataset )
    {
        ret = MIG_ERROR_UNSUPPORTED;
        goto error;
    }

    /* read DICOM tags from first file */
	info.out = dicom_data; 
    _data_info_get ( dataset , &info );

    /* check wether we know how to read this kind of dicom files */
    ret = _data_check_info ( &info );
    if ( ret != MIG_OK )
        goto error;

    /* copy size data from internal structure to output structure */
    size_data->w = info.rows;
    size_data->h = info.cols;
    size_data->slices = dir_contents.num;
    size_data->dim = info.rows * info.cols;
    size_data->dim_stack = size_data->dim * size_data->slices;
    size_data->size = size_data->dim * sizeof(Mig16u);
    size_data->size_stack = size_data->dim_stack * sizeof(Mig16u);
    size_data->h_res = info.pixel_spacing[0];
    size_data->v_res = info.pixel_spacing[1];
    size_data->z_res = 0.0;
    size_data->thickness = info.slice_thickness;

    /* get patient position for first slice */
    cond = dataset->findAndGetFloat64 ( DCM_ImagePositionPatient , image_position_patient_start[0] );
    if ( cond.bad() )
        image_position_patient_start[0] = 0.0;

    cond = dataset->findAndGetFloat64 ( DCM_ImagePositionPatient , image_position_patient_start[1] , 1 );
    if ( cond.bad() )
        image_position_patient_start[1] = 0.0;

    cond = dataset->findAndGetFloat64 ( DCM_ImagePositionPatient , image_position_patient_start[2] , 2 );
    if ( cond.bad() )
        image_position_patient_start[2] = 0.0;

    /* make space for file names */
    dicom_data->file_names = (char**) calloc ( size_data->slices + 1 , sizeof(char*) );
    if ( dicom_data->file_names == NULL )
    {
        ret = MIG_ERROR_MEMORY;
        goto error;
    }

    /* 1. copy instance uids 
       2. copy file names 
       3. determine z resolution of stack */
    mig_lst_iter iter;
    mig_lst_iter_get ( &iter , &dir_contents );

    i = 0;
    while ( ( dir_entry = (_dir_entry*) mig_lst_iter_next ( &iter ) ) != NULL )
    {
        /* copy file name */
        dicom_data->file_names[i] = (char*) malloc ( ( strlen( dir_entry->fname ) + 1 ) * sizeof(char) );
        if ( dicom_data->file_names[i] == NULL )
        {
            ret = MIG_ERROR_MEMORY;
            goto error;        
        }
                
        snprintf ( dicom_data->file_names[i] , MIG_DCM_FIELD_LEN , "%s" , dir_entry->fname );
        ++ i;
    }
                         
    /* get patient position of last slice so we can determine slice spacing */
    dir_entry = (_dir_entry*) mig_lst_peek_tail ( &dir_contents );
    if ( dir_entry == NULL )
    {
        ret = MIG_ERROR_INTERNAL;
        goto error;
    }

    snprintf ( (char*) FullName , MAX_PATH , "%s%s" , Storage , dir_entry->fname );
    cond = df.loadFile ( FullName );
    if ( cond.bad() )
    {
        ret = MIG_ERROR_IO;
        goto error;
    }

    dataset = df.getDataset ();
    if ( dataset == NULL )
    {
        ret = MIG_ERROR_UNSUPPORTED;
        goto error;
    }
                                
    /* get patient position for last slice */
    cond = dataset->findAndGetFloat64 ( DCM_ImagePositionPatient , image_position_patient_end[0] );
    if ( cond.bad() )
        image_position_patient_end[0] = 0.0;

    cond = dataset->findAndGetFloat64 ( DCM_ImagePositionPatient , image_position_patient_end[1] , 1 );
    if ( cond.bad() )
        image_position_patient_end[1] = 0.0;

    cond = dataset->findAndGetFloat64 ( DCM_ImagePositionPatient , image_position_patient_end[2] , 2 );
    if ( cond.bad() )
        image_position_patient_end[2] = 0.0;
        
    /* slice spacing is calculated by taking the distance between
       first and last slice, devided by number of slices */
    size_data->z_res = sqrt( 
        MIG_POW2( image_position_patient_end[0] -  
                  image_position_patient_start[0] ) +
        MIG_POW2( image_position_patient_end[1] - 
                  image_position_patient_start[1] ) +
        MIG_POW2( image_position_patient_end[2] - 
                  image_position_patient_start[2] ) ) /
        ( size_data->slices - 1 );

    /* truncate z resolution to 2 decimal places after . */
    size_data->z_res = ( floorf( size_data->z_res * 100.0f ) ) / 100.0f;

    /* free directory contents */
    mig_lst_free_custom_static ( &dir_contents , &_dir_free_f );
        
    return MIG_OK;

error :

    /* free directory contents */
    mig_lst_free_custom_static ( &dir_contents , &_dir_free_f );

    /* free file names */
    if ( dicom_data->file_names != NULL )
    {
        i = 0;
        while ( dicom_data->file_names[i] != NULL )
        {
            free ( dicom_data->file_names[i] );
            ++ i;
        }
        
        free ( dicom_data->file_names );
        dicom_data->file_names = NULL;
    }

    return ret;
}

/*********************************************************************/
extern void
mig_dcm_dump_info ( mig_dcm_data_t *dicom_data , 
                    mig_size_t *size_data ,
                    std::stringstream& os )
{
        os << "Dicom directory properties : ";
        os << "\n\t Patient ID                 : " << dicom_data->patient_id;
        os << "\n\t Patient Name               : " << dicom_data->patient_name;
        os << "\n\t Study UID                  : " << dicom_data->study_uid;
        os << "\n\t Study Date                 : " << dicom_data->study_date;
        os << "\n\t Study Time                 : " << dicom_data->study_time;
        os << "\n\t Series UID                 : " << dicom_data->series_uid;
        os << "\n\t Storage directory          : " << dicom_data->storage;
        
        os << "\n\t Width x Height x Slices    : " << size_data->w << " x " \
                                                   << size_data->h << " x " \
                                                   << size_data->slices;
      
        os << "\n\t Dim slice x Dim stack      : " << size_data->dim << " x " \
                                                   << size_data->dim_stack;
      
        os << "\n\t Size slice x Size stack    : " << size_data->size << " x " \
                                                   << size_data->size_stack;
      
        os << "\n\t H res x V res x Z res      : " << size_data->h_res << " x " \
                                                   << size_data->v_res << " x " \
                                                   << size_data->z_res;
      
        os << "\n\t Slice thickness            : " << size_data->thickness;
}

/******************************************************************************
   The following function does not read DICOMDIR. 
   It just tries to parse one by one all dicom
   files stored inside a directory on disk and load them
   as unsigned short, applying gray level scaling defined
   by window center wc and window width ww
*/

//#define MIG_MT

#if !defined(MIG_MT)

int
mig_dcm_rdir_16u ( Mig16u **dst , 
                   mig_dcm_data_t *dicom_data , 
                   mig_size_t *size_data ,
                   int voi_lut_type , 
                   int wc , 
                   int ww ,
                   int max_slice_num )
{
    DcmFileFormat df;
    DcmDataset *dataset;
    DicomImage *di = NULL;
        
    OFCondition cond;
    int ret , i;
        
    char CurrFileName[MAX_PATH];
    char Storage[MAX_PATH];

    Mig8u *buff = NULL;
    Mig16u *tmp  = NULL;    /* holds the whole stack */
    Mig16u *idx  = NULL;    /* index for single slice */
        
    /* copy storage location to local variable making sure that storage path end with a slash */
    if ( dicom_data->storage[strlen( dicom_data->storage )] == MIG_PATH_SEPARATOR )
        snprintf ( (char*) Storage , MAX_PATH , "%s" , dicom_data->storage ); 
    else
        snprintf ( (char*) Storage , MAX_PATH , "%s%c" , dicom_data->storage , MIG_PATH_SEPARATOR ); 

    /* tmp will hold the whole stack */
    tmp = (Mig16u*)
    mig_malloc ( size_data->size_stack );
    if ( tmp == NULL )
    {
        ret = MIG_ERROR_MEMORY;
        goto error;
    }

    idx = tmp;
    i = 0;
    while ( ( dicom_data->file_names[i] != NULL ) && ( i < max_slice_num ) )
    {
        /* form full path to file name */
        snprintf ( CurrFileName , MAX_PATH , "%s%s" , Storage , dicom_data->file_names[i] );
        DcmFileFormat dfile;
        cond = dfile.loadFile ( CurrFileName );
        if ( cond.bad() )
        {
            ret = MIG_ERROR_IO;
            goto error;
        }

        dataset = dfile.getDataset ();
        if ( dataset == NULL )
        {
            ret = MIG_ERROR_UNSUPPORTED;
            goto error;
        }

        di = new DicomImage ( &dfile , dataset->getOriginalXfer() );
        if ( di == NULL )
        {
            ret =  MIG_ERROR_MEMORY;
            goto error;
        }

        int win_cnt = di->getWindowCount();

        /* select voi lut transformation */
        switch ( voi_lut_type ) 
        {
            case MIG_VOI_LUT_STORED :
                
                /* if there is a stored window then use it */
                if ( win_cnt > 0 )
                {
                    if ( !di->setWindow( win_cnt - 1 ) )
                    {
                        ret = MIG_ERROR_INTERNAL;
                        goto error;
                    }
                    break;
                }
                
                /* ATTENTION : if we cannot get a stored window
                   we jump directley to the next case statment, that
                   is we try to force our own window. */

            case MIG_VOI_LUT_FORCE :

                if ( !di->setWindow( wc , ww ) )
                {
                    ret = MIG_ERROR_INTERNAL;
                    goto error;
                }
                
                break;

            case MIG_VOI_LUT_WIN_MIN_MAX :
                
                if ( !di->setMinMaxWindow(0) )
                {
                    ret = MIG_ERROR_INTERNAL;
                    goto error;
                }
                
                break;

            case MIG_VOI_LUT_WIN_MIN_MAX_NO_EXTREMES :
                
                if ( !di->setMinMaxWindow(1) )
                {
                    ret = MIG_ERROR_INTERNAL;
                    goto error;
                }
                break;

            /*
                case VOI_LUT_HIST :
                
                if ( !di->setHistogramWindow( OFstatic_cast( double , opt_windowParameter ) / 100.0 ) )
                {
                    ret = MIG_ERROR_INTERNAL;
                    goto error;
                }
                break;
            */

            default :
                
                ret = MIG_ERROR_INTERNAL;
                goto error;
        }

        
        
        /* get hold of pixel data and copy it into out buffer */
        buff = (unsigned char*) di->getOutputData ( 16 );
        if ( buff == NULL )
        {
            ret =  MIG_ERROR_MEMORY;
            goto error;
        }

        mig_memcpy ( (void*) buff , idx , size_data->size );
        delete di;

        idx += size_data->dim;
        ++ i;
    }

    if ( size_data->slices > max_slice_num )
    {
        size_data->slices     = max_slice_num;
        size_data->dim_stack  = max_slice_num * ( size_data->w ) * ( size_data->h );
        size_data->size_stack = ( size_data->dim_stack ) * sizeof( unsigned short );
    }
        
    /* assign output */
    *dst = tmp;
        
    return MIG_OK;

error:

    if ( tmp != NULL )
        mig_free ( tmp );

    if ( di != NULL )
        delete di;

    return ret;
}

#else /* MIG_MT is defined */

/******************************************************************************
   The following function does the same job as the function
   above but using pthreads. MIG_MT should be defined when
   compiling this code.
*/

#include "pthread.h"

typedef struct _mig_work_t
{
        char **files;
        int  num;
        char *Storage;
        mig_size_t *s;
        Mig16u *dst;
        int wc , ww;
        int status;

} mig_work_t;

void*
worker_routine ( void *arg )
{
        mig_work_t *work 
                = (mig_work_t*) arg;

        DcmFileFormat df;
    DcmDataset *dataset;
        DicomImage *di = NULL;
        OFCondition cond;
        int i;
        char CurrFileName[MAX_PATH];
        Mig8u *buff = NULL;

        for ( i = 0 ; i < work->num ; ++i , 
              work->dst += work->s->dim )
        {
                /* form full path to file */
                snprintf ( CurrFileName , MAX_PATH ,
                           "%s%s" , 
                           work->Storage ,
                           work->files[i] );
                
                DcmFileFormat dfile;
                cond = dfile.loadFile ( CurrFileName );
                if ( cond.bad() )
                {
                        work->status = MIG_ERROR_IO;
                        return NULL;
                }

                dataset = dfile.getDataset ();
                if ( dataset == NULL )
                {
                        work->status = MIG_ERROR_UNSUPPORTED;
                        return NULL;
                }

                di = new DicomImage ( &dfile , 
                        dataset->getOriginalXfer() );
                if ( di == NULL )
                {
                        work->status =  MIG_ERROR_MEMORY;
                        return NULL;
                }

                if ( !di->setWindow( work->wc , work->ww ) )
                {
                        work->status = MIG_ERROR_INTERNAL;
                        delete di;
                        return NULL;
                }

                /* get hold of pixel data and copy it into out buffer */
                buff = (unsigned char*) 
                        di->getOutputData ( 16 );
                if ( buff == NULL )
                {
                        work->status =  MIG_ERROR_MEMORY;
                        delete di;
                        return NULL;
                }

                mig_memcpy ( (void*) buff , 
                        work->dst , work->s->size );
                
                delete di;
        }

        return NULL;
}

int
mig_dcm_rdir_16u ( Mig16u **dst ,
                   mig_dcm_data_t *dicom_data ,
                   mig_size_t *size_data ,
                   int wc , int ww )
{
        char Storage[MAX_PATH];        
        Mig16u *tmp  = NULL;    /* holds the whole stack */
        int ret;

#define CREW_SIZE 2
                
        pthread_t  crew[CREW_SIZE];
        mig_work_t work[CREW_SIZE];

        /* copy storage location to local 
           variable making sure that storage
           path end with a slash */
        if ( dicom_data->storage[strlen( dicom_data->storage )] ==
                MIG_PATH_SEPARATOR )
                snprintf ( (char*) Storage , MAX_PATH , 
                        "%s" , dicom_data->storage ); 
        else
                snprintf ( (char*) Storage , MAX_PATH , 
                        "%s%c" , dicom_data->storage , MIG_PATH_SEPARATOR ); 

        /* tmp will hold the whole stack */
        tmp = (Mig16u*)
		mig_malloc ( size_data->size_stack );
        if ( tmp == NULL )
        {
                ret = MIG_ERROR_MEMORY;
                goto error;
        }

        /* prepare data for threads and spawn
           threads */
        
        /* thread 1 */
        work[0].files = dicom_data->file_names;
        work[0].num  = ( size_data->slices >> 1 );
        work[0].Storage = (char*)Storage;
        work[0].s = size_data;
        work[0].dst = tmp;
        work[0].wc = wc;
        work[0].ww = ww;
        work[0].status = MIG_OK;

        /* thread 2 */
        work[1].files = 
                dicom_data->file_names + ( size_data->slices >> 1 );
        work[1].num  = size_data->slices - ( size_data->slices >> 1 );
        work[1].Storage = (char*)Storage;
        work[1].s = size_data;
        work[1].dst = tmp + work[0].num * size_data->dim;
        work[1].wc = wc;
        work[1].ww = ww;
        work[1].status = MIG_OK;
       
        /* spawn threads */
        if ( pthread_create ( &crew[0] , NULL , 
                &worker_routine , &work[0] ) != 0 )
        {
                ret = MIG_ERROR_MEMORY;
                goto error;
        }
        
        if ( pthread_create ( &crew[1] , NULL , 
                &worker_routine , &work[1] ) != 0 )
        {
                ret = MIG_ERROR_MEMORY;
                goto error;
        }

        /* wait for threads to finish */
        pthread_join ( crew[0] , NULL );
        pthread_join ( crew[1] , NULL );

        /* check return codes */
        if ( work[0].status != MIG_OK )
        {
                ret = work[0].status;
                goto error;
        }

        if ( work[1].status != MIG_OK )
        {
                ret = work[1].status;
                goto error;
        }

        /* assign output */
        *dst = tmp;
        
        return MIG_OK;

error:

        if ( tmp != NULL )
                mig_free ( tmp );

        return ret;
}

#endif /* MIG_MT defined */

/**************************************************************************/
/* PRIVATE */
/**************************************************************************/
/* check for presence of DICOM file preambule
   in binary file */
static int
_data_check ( char *fname )
{
        int status = MIG_OK;
        FILE *f = NULL;
        char signature[4];

        f = fopen( fname , "rb" );
        if ( f == NULL )
            return MIG_ERROR_IO;

        /* look for sequence of bytes in binary file */
        if ( ( fseek ( f , DCM_PreambleLen , SEEK_SET ) < 0 ) ||
             ( fread ( signature , 1 , DCM_MagicLen , f ) != DCM_MagicLen ) )
        {
                status = MIG_ERROR_UNSUPPORTED;
        } 
        else
        {
                if ( strncmp( signature , DCM_Magic , DCM_MagicLen ) != 0 )
                {
                        status = MIG_ERROR_UNSUPPORTED;
                }
        }

        fclose ( f );
        return status;
}

/*****************************************************************************/
/* read DICOM tags from file */
static int
_data_info_get ( DcmDataset *dataset ,
                 _info_i *info )
{
        OFCondition cond;
        const char *str = NULL;

        /* photometric interpretation */
        cond = dataset->findAndGetString (
                DCM_PhotometricInterpretation , str );
        if ( ( cond.bad() ) || ( str == NULL ) )
                strcpy ( info->photometric_interpretation , 
                         MIG_DCM_UNKNOWN_TAG );
        else
                strncpy ( info->photometric_interpretation , 
                          str , MIG_DCM_FIELD_LEN );

        /* pixel spacing */
        cond = dataset->findAndGetFloat64 ( DCM_PixelSpacing ,
                                            info->pixel_spacing[0] );
        if ( cond.bad() )
                info->pixel_spacing[0] = 0.0;

        cond = dataset->findAndGetFloat64 ( DCM_PixelSpacing ,
                                            info->pixel_spacing[1] , 1 );
        if ( cond.bad() )
                info->pixel_spacing[1] = 0.0;

        /* slice thickness */
        cond = dataset->findAndGetFloat64 ( DCM_SliceThickness,
                                            info->slice_thickness );
        if ( cond.bad() )
                info->slice_thickness = 0.0;

        /* samples per pixel */
        cond = dataset->findAndGetUint16 ( DCM_SamplesPerPixel ,
                                           info->samples_per_pixel );
        if ( cond.bad() )
                info->samples_per_pixel = 0;

        /* rows */
        cond = dataset->findAndGetUint16 ( DCM_Rows , info->rows );
        if ( cond.bad() )
                info->rows = 0;

        /* columns */
        cond = dataset->findAndGetUint16 ( DCM_Columns , info->cols );
        if ( cond.bad() )
                info->cols = 0;
        
        /* bits allocated */
        cond = dataset->findAndGetUint16 ( DCM_BitsAllocated ,
                                           info->bits_allocated );
        if ( cond.bad() )
                info->bits_allocated = 0;

        /* bits stored */
        cond = dataset->findAndGetUint16 ( DCM_BitsStored ,
                                           info->bits_stored );
        if ( cond.bad() )
                info->bits_stored = 0;
                        
        /* patient ID */
        cond = dataset->findAndGetString( DCM_PatientID , str );
        if ( cond != EC_Normal )
                return MIG_ERROR_UNSUPPORTED;
	      snprintf ( info->out->patient_id , 
		   MIG_DCM_FIELD_LEN , "%s" , str );

        /* patients name */
        cond = dataset->findAndGetString( DCM_PatientsName , str );
        if ( cond != EC_Normal )
                return MIG_ERROR_UNSUPPORTED;
	      snprintf ( info->out->patient_name , 
		   MIG_DCM_FIELD_LEN , "%s" , str );

        /* study UID */
        cond = dataset->findAndGetString( DCM_StudyInstanceUID , str );
        if ( cond != EC_Normal )
                return MIG_ERROR_UNSUPPORTED;
	      snprintf ( info->out->study_uid , 
		   MIG_DCM_FIELD_LEN , "%s" , str );
        
        /* series UID */
        cond = dataset->findAndGetString( DCM_SeriesInstanceUID , str );
        if ( cond != EC_Normal )
                return MIG_ERROR_UNSUPPORTED;
	      snprintf ( info->out->series_uid , 
		   MIG_DCM_FIELD_LEN , "%s" , str );

        /* study date */
        cond = dataset->findAndGetString( DCM_StudyDate , str );
        if ( cond != EC_Normal )
                return MIG_ERROR_UNSUPPORTED;
	      snprintf ( info->out->study_date , 
		   MIG_DCM_FIELD_LEN , "%s" , str );
        
        /* study time */
        cond = dataset->findAndGetString( DCM_StudyTime , str );
        if ( cond != EC_Normal )
                return MIG_ERROR_UNSUPPORTED;
	      snprintf ( info->out->study_time , 
		   MIG_DCM_FIELD_LEN , "%s" , str );
        
	return MIG_OK;
}

/**************************************************************************/
/* basic check of read tags -> can we load
   this kind of DICOM file ? */
static int
_data_check_info ( _info_i *info )
{
        if ( info->bits_allocated !=
	     ALLOWED_BITS_ALLOCATED )
                return MIG_ERROR_UNSUPPORTED;

        if ( strcmp( info->photometric_interpretation ,
             ALLOWED_PHOTOMETRIC_INTERPRETATION) != 0 )
		return MIG_ERROR_UNSUPPORTED;

        if ( info->samples_per_pixel !=
	        ALLOWED_SAMPLES_PER_PIXEL)
		return MIG_ERROR_UNSUPPORTED;

        return MIG_OK;
}

/**************************************************************************/
/* directory entries comparaison function used
   to sort directory contents base on 
   DICOM instance number */
static int
_dir_cmp_f ( const void *a , 
             const void *b )
{
        _dir_entry *tmp_1 , *tmp_2;

        tmp_1 = (_dir_entry*)a;
        tmp_2 = (_dir_entry*)b;

        if ( tmp_1->instance_number < 
             tmp_2->instance_number )
                return -1;

        if ( tmp_1->instance_number > 
             tmp_2->instance_number )
                return 1;

        return 0;
}

/**************************************************************************/
/* function used to free one directory
   entry from directory contents list */
static void
_dir_free_f ( void *data )
{
        _dir_entry *tmp = 
                (_dir_entry*)data;

        if ( tmp == NULL )
                return;

        if ( tmp->fname != NULL )
                free ( tmp->fname );

        if ( tmp->instance_uid != NULL )
                free ( tmp->instance_uid );

        free ( data );
}

/***************************************************************************/
#if defined(WIN32)

/* parse directory for DICOM files ordering
   them by dicom attribute Instance Number */
static int
_parse_dir ( char *path , 
             mig_lst_t *d )
{
        BOOL                    finished;
        HANDLE                  list = NULL;
        TCHAR                   dir[MAX_PATH+1];
        char                    FileFullName[MAX_PATH];
        WIN32_FIND_DATA         file_data;
        int                     len_base;
        int			ret = MIG_OK;
        int                     instance_number;
        const char              *instance_uid = NULL;
        char                    *path_new = NULL;
        int                     trailing_char;
        _dir_entry              *CurrDirEntry = NULL;
        OFCondition             cond;
        DcmDataset              *dataset = NULL;

	/* setup trailing char which should be a slash */
        len_base = strlen( path ) + 1;
        trailing_char = (int)*( path + len_base - 2 );

        if ( ( trailing_char != 0x5C ) && 
             ( trailing_char != 0x2F ) )
                len_base ++;

        path_new = (char*) 
                calloc ( len_base , sizeof( char ) );
        if ( !path_new )
        {
                ret = MIG_ERROR_MEMORY;
                goto error;
        }

        /* setup slash direction */
        if ( ( trailing_char != 0x5C ) && 
             ( trailing_char != 0x2F ) )
                sprintf ( path_new , "%s\\" , path );
        else
                sprintf ( path_new , "%s", path );

        if ( sprintf ( dir , "%s*" , path_new ) <= 0 )
	{
		ret = MIG_ERROR_INTERNAL;
		goto error;
	}
	
        list = FindFirstFile ( dir , &file_data );
        if ( list == INVALID_HANDLE_VALUE )
        {
		//DWORD err = GetLastError();
                ret = MIG_ERROR_IO;
                goto error;
        }

        finished = FALSE;
        while ( !finished )
        {
                if ( ! ( file_data.dwFileAttributes & 
                         FILE_ATTRIBUTE_DIRECTORY ) )
                {                      
                        /* form full file name from
                           base directory and directory
                           entry */
                        snprintf ( FileFullName , 
                                   MAX_PATH , "%s%s" ,
                                   path_new ,
                                   file_data.cFileName );
                        
                        /* check weather current dir entry is
                           DICOM part 10 complient file
                         */
                        if ( _data_check ( FileFullName ) != MIG_OK )
                        {
                                /* if file not complient 
                                   jump to next file */
                                goto next;
                        }
                        
                        /* start loading DICOM file to get
                           instance number tag and 
                           instance uid tag */
                        DcmFileFormat dfile;
                        cond = dfile.loadFile( FileFullName );
                        if ( cond.bad() )
                        {
                                ret = MIG_ERROR_IO;
                                goto error;
                        }

                        dataset = dfile.getDataset();
                        if ( cond.bad() )
                        {
                                ret = MIG_ERROR_UNSUPPORTED;
                                goto error;
                        }

                        /* Get Instance Number */
                        cond = dataset->findAndGetSint32( 
                                DCM_InstanceNumber ,
                                (Sint32&) instance_number );
                        if ( cond.bad() )
                        {
                                ret = MIG_ERROR_UNSUPPORTED;
                                goto error;
                        }

                        /* Get SOP Instance Uid */
                        cond = dataset->findAndGetString( 
                                DCM_SOPInstanceUID ,
                                instance_uid );
                        if ( cond != EC_Normal )
                        {
                                ret = MIG_ERROR_UNSUPPORTED;
                                goto error;
                        }

                        /* prepare a new directory entry */
                        CurrDirEntry = (_dir_entry*)
                                calloc ( 1 , sizeof(_dir_entry) );
                        if ( CurrDirEntry == NULL )
                        {
                                ret = MIG_ERROR_MEMORY;
                                goto error;
                        }

                        CurrDirEntry->instance_number = 
                                instance_number;
                        
                        CurrDirEntry->fname = 
                                strdup ( file_data.cFileName );
                        if ( CurrDirEntry->fname == NULL )
                        {
                                ret = MIG_ERROR_MEMORY;
                                goto error;
                        }
                        
                        CurrDirEntry->instance_uid = 
                                strdup ( instance_uid );
                        if ( CurrDirEntry->instance_uid == NULL )
                        {
                                ret = MIG_ERROR_MEMORY;
                                goto error;
                        }
	              
                        /* insert new dir entry into list ordering by
                           ascending instance number */
                        ret = mig_lst_ins_sort ( 
                                d , CurrDirEntry , &_dir_cmp_f );
                        if ( ret )
                        {
                                ret = MIG_ERROR_MEMORY;
                                goto error;
                        }
                }
next :
                if ( !FindNextFile( list , &file_data ) )
                        if ( GetLastError() == ERROR_NO_MORE_FILES )
                                finished = TRUE;
        }

        free ( path_new );
        FindClose( list );
        return ret;

error:

        FindClose( list );
        if ( path_new != NULL )
                free ( path_new );

        return ret;
}

/***************************************************************************/
#else   /* LINUX */

/* parse directory for DICOM files ordering
   them by dicom attribute Instance Number */
static int
_parse_dir ( char *path , 
             mig_lst_t *d )
{
        struct dirent           *entry;
        DIR                     *dp;
        int                     len_base;
        int                     ret = MIG_OK;
        int                     instance_number;
        const char              *instance_uid;
        char                    FileFullName[MAX_PATH];
        char                    *path_new = NULL;
        int                     trailing_char;
        _dir_entry              *CurrDirEntry = NULL;
        OFCondition             cond;
        DcmDataset              *dataset = NULL;

        if ( ( dp = opendir( path ) ) == NULL )
        {
                ret = MIG_ERROR_IO;
                goto error;
        }

        /* setup trailing char */
        len_base = strlen ( path ) + 1;
        trailing_char = (int)*( path + len_base - 2 );

        if ( ( trailing_char != 0x5C ) && 
             ( trailing_char != 0x2F ) )
                len_base ++;

        path_new = (char*) 
                malloc ( len_base * sizeof( char ) );
        if ( path_new == NULL )
        {
                ret = MIG_ERROR_MEMORY;
                goto error;
        }

        /* setup slash direction */
        if ( ( trailing_char != 0x5C ) && ( trailing_char != 0x2F ) )
                sprintf ( path_new , "%s/" , path );
        else
                sprintf ( path_new , "%s", path );

        /* for each directory entry */
        while ( ( entry = readdir ( dp ) ) != NULL )
        {
                if ( ( strcmp( entry->d_name , "." ) == 0 ) ||
                     ( strcmp( entry->d_name , ".." ) == 0 ) )
                        continue;

                snprintf ( FileFullName , MAX_PATH ,
                           "%s%s" ,
                          path_new ,
                          entry->d_name );

                /* check weather current dir entry is
                   DICOM part 10 complient file
                 */
                if ( _data_check ( FileFullName ) != MIG_OK )
                        continue;

                /* load dicom file */
                DcmFileFormat dfile;
                cond = dfile.loadFile ( FileFullName );
                if ( cond.bad() )
                {
                        ret = MIG_ERROR_IO;
                        goto error;
                }

                dataset = dfile.getDataset ();
                if ( cond.bad() )
                {
                        ret = MIG_ERROR_UNSUPPORTED;
                        goto error;
                }
                
                /* instance number */
                cond = dataset->findAndGetSint32( 
                        DCM_InstanceNumber ,
                        (Sint32 &) instance_number );
                if ( cond.bad() )
                {
                        ret = MIG_ERROR_UNSUPPORTED;
                        goto error;
                }

                /* instance uid */
                cond = dataset->findAndGetString( 
                        DCM_SOPInstanceUID , instance_uid );
                if ( cond != EC_Normal )
                {
                        ret = MIG_ERROR_UNSUPPORTED;
                        goto error;
                }

                /* copy info to our structure */
                CurrDirEntry = (_dir_entry*)
                        calloc ( 1 , sizeof(_dir_entry) );
                if ( CurrDirEntry == NULL )
                {
                        ret = MIG_ERROR_MEMORY;
                        goto error;
                }
                
                CurrDirEntry->instance_number = 
                        instance_number;

                CurrDirEntry->instance_uid = 
                        strdup( instance_uid );
                if ( CurrDirEntry->instance_uid == NULL )
                {
                        ret = MIG_ERROR_MEMORY;
                        goto error;
                }

                CurrDirEntry->fname = 
                        strdup( entry->d_name );
                if ( CurrDirEntry->fname == NULL )
                {
                        ret = MIG_ERROR_MEMORY;
                        goto error;
                }

                /* insert new directory entry into
                   directory list ordering by ascending
                   instance number */
                ret = mig_lst_ins_sort (
                        d , CurrDirEntry , &_dir_cmp_f );
                if ( ret )
                {
                        ret = MIG_ERROR_MEMORY;
                        goto error;
                }
        }

        free ( path_new );
        closedir ( dp );

        return ret;

error:

        closedir ( dp );

        if ( path_new != NULL )
                free ( path_new );

        return ret;
}

#endif  /* PLATFORM SPECIFIC READDIR */
