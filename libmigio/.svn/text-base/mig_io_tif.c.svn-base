#include "mig_io_tif.h"
#include "mig_ut_mem.h"
#include "mig_ut_bit.h"

#include "mig_error_codes.h"

#include "tiff.h"
#include "tiffio.h"

typedef struct
{
        Mig32u w;               /* width */
        Mig32u h;               /* height */
        Mig16u bps;             /* bits per sample */
        Mig16u spp;             /* samples per pixel */
        Mig32f hres;            /* horizontal resolution */
        Mig32f vres;            /* vertical resolution */
        Mig16u unitsres;        /* resolution units of measure */
        Mig16u comp;            /* compression type */
        char dsc[50+1];         /* image description */

        Mig16u photo;           /* photometric interpretation */
        Mig16u planar;          /* planar configuration */
        Mig16u fillorder;       /* data fill order */

} _MigTiffInfo;

/****************************************************************************/
#define _MIG_SET_TIFF_TAGS(tiff_f,info) \
do { \
        TIFFSetField( (tiff_f), TIFFTAG_IMAGEWIDTH              , (info)->w             );      \
        TIFFSetField( (tiff_f), TIFFTAG_IMAGELENGTH             , (info)->h             );      \
        TIFFSetField( (tiff_f), TIFFTAG_BITSPERSAMPLE           , (info)->bps           );      \
        TIFFSetField( (tiff_f), TIFFTAG_SAMPLESPERPIXEL         , (info)->spp           );      \
        TIFFSetField( (tiff_f), TIFFTAG_XRESOLUTION             , (info)->hres          );      \
        TIFFSetField( (tiff_f), TIFFTAG_YRESOLUTION             , (info)->vres          );      \
        TIFFSetField( (tiff_f), TIFFTAG_RESOLUTIONUNIT          , (info)->unitsres      );      \
        TIFFSetField( (tiff_f), TIFFTAG_COMPRESSION             , (info)->comp          );      \
        TIFFSetField( (tiff_f), TIFFTAG_IMAGEDESCRIPTION        , (info)->dsc           );      \
        TIFFSetField( (tiff_f), TIFFTAG_PHOTOMETRIC             , (info)->photo         );      \
        TIFFSetField( (tiff_f), TIFFTAG_PLANARCONFIG            , (info)->planar        );      \
        TIFFSetField( (tiff_f), TIFFTAG_FILLORDER               , (info)->fillorder     );      \
} while(0)


/***************************************************************************************************/
#define _MIG_GET_TIFF_TAGS(tiff_f,info) \
do { \
        TIFFGetField( (tiff_f), TIFFTAG_IMAGEWIDTH              , &((info)->w)          );      \
        TIFFGetField( (tiff_f), TIFFTAG_IMAGELENGTH             , &((info)->h)          );      \
        TIFFGetField( (tiff_f), TIFFTAG_BITSPERSAMPLE           , &((info)->bps)        );      \
        TIFFGetField( (tiff_f), TIFFTAG_SAMPLESPERPIXEL         , &((info)->spp)        );      \
        TIFFGetField( (tiff_f), TIFFTAG_XRESOLUTION             , &((info)->hres)       );      \
        TIFFGetField( (tiff_f), TIFFTAG_YRESOLUTION             , &((info)->vres)       );      \
        TIFFGetField( (tiff_f), TIFFTAG_RESOLUTIONUNIT          , &((info)->unitsres)   );      \
        TIFFGetField( (tiff_f), TIFFTAG_COMPRESSION             , &((info)->comp)       );      \
        TIFFGetField( (tiff_f), TIFFTAG_IMAGEDESCRIPTION        , &((info)->dsc)        );      \
        TIFFGetField( (tiff_f), TIFFTAG_PHOTOMETRIC             , &((info)->photo)      );      \
        TIFFGetField( (tiff_f), TIFFTAG_PLANARCONFIG            , &((info)->planar)     );      \
        TIFFGetField( (tiff_f), TIFFTAG_FILLORDER               , &((info)->fillorder)  );      \
} while(0)

/****************************************************************************/
static void
_mig_io_tif_info_dump ( _MigTiffInfo *info );

/****************************************************************************/
static int
_mig_io_tif_check_tags ( _MigTiffInfo *info );

/****************************************************************************/
static int
_mig_io_tif_w_1u ( TIFF *tiff_file , Mig8u *src , _MigTiffInfo *info );

static int
_mig_io_tif_w_8u ( TIFF *tiff_file , Mig8u *src , _MigTiffInfo *info );

static int
_mig_io_tif_w_16u ( TIFF *tiff_file , Mig8u *src ,  _MigTiffInfo *info );

/****************************************************************************/
static int
_mig_io_tif_r_1u ( TIFF *tiff_file , Mig8u *dst , _MigTiffInfo *info );

static int
_mig_io_tif_r_8u ( TIFF *tiff_file , Mig8u *dst , _MigTiffInfo *info );

static int
_mig_io_tif_r_16u ( TIFF *tiff_file , Mig8u *dst , _MigTiffInfo *info );

/****************************************************************************/
int
mig_io_tif_w ( char *name , Mig8u  *src , mig_tiff_info_t *info )
{
        TIFF *tiff_f = NULL;
        _MigTiffInfo tmp_info;
        int ret;
	
        /* open tiff file for writing */
        tiff_f = TIFFOpen ( name , "w" );
        if ( !tiff_f )
        {
                ret = MIG_ERROR_IO;
                goto error;
        }

        /* copy all tiff info into intermediate structure */
        tmp_info.w              = info->w;
        tmp_info.h              = info->h;
        tmp_info.bps            = info->bps;
        tmp_info.spp            = info->spp;
        tmp_info.hres           = info->hres;
        tmp_info.vres           = info->vres;
        tmp_info.unitsres       = info->unitsres;
        tmp_info.comp           = info->comp;

        strncpy( (char*)tmp_info.dsc , info->dsc , 50 );

        if ( info->bps == 1 )
                tmp_info.photo = PHOTOMETRIC_MINISWHITE;
        else
                tmp_info.photo = PHOTOMETRIC_MINISBLACK;

        tmp_info.planar         = PLANARCONFIG_CONTIG;
        tmp_info.fillorder      = FILLORDER_MSB2LSB;

        /* if debugging dump tiff info */
#if defined(DEBUG_TIFF)
        _mig_io_tif_info_dump ( &tmp_info );
#endif

        /* check weather we know how to write this kind of tiff file */
        ret = _mig_io_tif_check_tags ( &tmp_info );
        if ( ret != MIG_OK )
                goto error;

        /* put tags to tiff file */
        _MIG_SET_TIFF_TAGS( tiff_f , ( &tmp_info ) );

        /* write data to tiff file */
        switch ( info->bps )
        {
                case 1 :
                        ret = _mig_io_tif_w_1u (tiff_f, src, &tmp_info);
                        break;
                case 8 :
                        ret = _mig_io_tif_w_8u (tiff_f, src, &tmp_info);
                        break;
                case 16 :
                        ret = _mig_io_tif_w_16u (tiff_f, src, &tmp_info);
                        break;
                default :
                        /* should never happen */
                        ret = MIG_ERROR_INTERNAL;
                        break;
        }

        if ( ret != MIG_OK )
                goto error;

        TIFFClose ( tiff_f );
        return MIG_OK;

error:
        if ( tiff_f )
                TIFFClose ( tiff_f );
        return ret;
}

/****************************************************************************/
int
mig_io_tif_w_stack ( char *name , Mig8u *src , int slices , mig_tiff_info_t *info )
{
        int rc = MIG_OK;
        char full_name[MAX_PATH];
        int dim , k;

        dim = ( info->bps == 1 ) ? 1 : ( info->bps ) >> 3;
        dim *= ( info->w * info->h );

        for ( k = 0 ; k < slices ; ++k , src += dim )
        {
                snprintf( full_name , MAX_PATH , "%s_%04d.tif", name , k );
                rc = mig_io_tif_w ( (char*) &full_name , src, info );
                if ( rc != MIG_OK )
                        break;
        }

        return rc;
}

/****************************************************************************/
int
mig_io_tif_w_stack_1 ( char *name , Mig8u *src ,
                       int w , int h , int slices )
{
        mig_tiff_info_t info;

        info.w =w;
        info.h = h;
        info.bps = 1;
        info.spp = 1;
        info.hres = 1.0;
        info.vres = 1.0;
        info.unitsres = MIG_UNITS_NONE;
        info.comp = MIG_CMP_CCITTFAX4;
        sprintf( info.dsc , "NONE" );

        return mig_io_tif_w_stack ( name , src , slices , &info );
}

/****************************************************************************/
int
mig_io_tif_w_stack_8 ( char *name , Mig8u *src ,
                       int w , int h , int slices )
{
        mig_tiff_info_t info;

        info.w =w;
        info.h = h;
        info.bps = 8;
        info.spp = 1;
        info.hres = 1.0;
        info.vres = 1.0;
        info.unitsres = MIG_UNITS_NONE;
        info.comp = MIG_CMP_LZW;
        sprintf( info.dsc , "NONE" );

        return mig_io_tif_w_stack ( name , src , slices , &info );
}

/****************************************************************************/
int
mig_io_tif_w_stack_16 ( char *name , Mig8u *src ,
                        int w , int h , int slices )
{
        mig_tiff_info_t info;

        info.w = w;
        info.h = h;
        info.bps = 16;
        info.spp = 1;
        info.hres = 1.0;
        info.vres = 1.0;
        info.unitsres = MIG_UNITS_NONE;
        info.comp = MIG_CMP_LZW;
        sprintf( info.dsc , "NONE" );

        return mig_io_tif_w_stack ( name , (Mig8u*)src , slices , &info );
}

/****************************************************************************/
int
mig_io_tif_w_rgb ( char *fname ,
		   unsigned char *im , int w , int h )
{
        _MigTiffInfo info;
        TIFF *tiff_f = NULL;
        int i;
        unsigned char *RPlane , *GPlane , *BPlane;
	
        /* open tiff file for writing */
        tiff_f = TIFFOpen ( fname , "w" );
        if ( tiff_f == NULL )
                return MIG_ERROR_IO;

        /* fill in tiff tags data */
        info.w = w;
        info.h = h;
        info.bps = 8;
        info.spp = 3;
        info.hres = 1.0;
        info.vres = 1.0;
        info.unitsres = MIG_UNITS_NONE;
        info.comp = MIG_CMP_NONE;
        sprintf( info.dsc , "NONE" );
        info. photo = PHOTOMETRIC_RGB;
        info.planar = PLANARCONFIG_SEPARATE;
        info.fillorder = FILLORDER_MSB2LSB;

        /* write tiff tags to file */
        _MIG_SET_TIFF_TAGS( tiff_f , ( &info ) );

        RPlane = im;
        GPlane = im + w * h;
        BPlane = im + 2 * w * h;

        /* write image data to file */
        for ( i = 0 ; i < h ; ++i )
                TIFFWriteScanline ( tiff_f , RPlane + i * w , i , 0 );

        for ( i = 0 ; i < h ; ++i )
                TIFFWriteScanline ( tiff_f , GPlane + i * w , i , 1 );
                
        for ( i = 0 ; i < h ; ++i )
                TIFFWriteScanline ( tiff_f , BPlane + i * w , i , 2 );

        /* clean up */
        TIFFClose ( tiff_f );
        return MIG_OK;
}

/****************************************************************************/
int
mig_io_tif_r ( char *name , Mig8u **dst , mig_tiff_info_t *info )
{
        TIFF *tiff_f = NULL;
        _MigTiffInfo tmp_info;
        Mig8u *buffer = NULL;
        unsigned int s;
        int ret;

        /* open tiff file for reading */
        tiff_f = TIFFOpen (name, "r");
        if (!tiff_f)
        {
                ret = MIG_ERROR_IO;
                goto error;
        }

        /* read tiff tags out of file */
        _MIG_GET_TIFF_TAGS(tiff_f, &tmp_info);

        /* if debugging dump tiff info */
#if defined(DEBUG_TIFF)
        _mig_io_tif_info_dump (&tmp_info);
#endif

        /* check weather we know how to read this kind of tiff file */
        ret = _mig_io_tif_check_tags (&tmp_info);
        if (ret != MIG_OK)
        {
                goto error;
        }

        /* copy all tiff info into our output struct */
        info->w         = tmp_info.w;
        info->h         = tmp_info.h;
        info->bps       = tmp_info.bps;
        info->spp       = tmp_info.spp;
        info->hres      = tmp_info.hres;
        info->vres      = tmp_info.vres;
        info->unitsres  = tmp_info.unitsres;
        info->comp      = tmp_info.comp;

        strncpy( info->dsc , ( const char* )&(tmp_info.dsc) , 50 );

        /* prepare output buffer to hold image data */
        s = TIFFScanlineSize (tiff_f) * (info->h);
        if ( info->bps == 1 )
                s <<= 3;

        buffer = (Mig8u*) mig_malloc (s);
        if ( !buffer )
        {
                ret = MIG_ERROR_MEMORY;
                goto error;
        }

        /* read data from tiff file */
        switch (info->bps)
        {
                case 1 :
                        ret = _mig_io_tif_r_1u (tiff_f, buffer, &tmp_info);
                        break;
                case 8 :
                        ret = _mig_io_tif_r_8u (tiff_f, buffer, &tmp_info);
                        break;
                case 16 :
                        ret = _mig_io_tif_r_16u (tiff_f, buffer, &tmp_info);
                        break;
                default :
                        /* should never happen */
                        ret = MIG_ERROR_INTERNAL;
                        break;
        }

        if (ret != MIG_OK)
                goto error;

        /* clean up */
        TIFFClose ( tiff_f );
        *dst = buffer;
        return MIG_OK;

error:
        if ( buffer )
                mig_free ( buffer );
        if ( tiff_f )
                TIFFClose ( tiff_f );

        *dst = NULL;

        return ret;
}

/****************************************************************************/
int
mig_io_tif_w_1 ( char *name , Mig8u *src , int w , int h )
{
        mig_tiff_info_t info;

        info.w =w;
        info.h = h;
        info.bps = 1;
        info.spp = 1;
        info.hres = 1.0;
        info.vres = 1.0;
        info.unitsres = MIG_UNITS_NONE;
        info.comp = MIG_CMP_CCITTFAX4;
        sprintf( info.dsc , "NONE" );

        return mig_io_tif_w ( name , src , &info );
}

/****************************************************************************/
int
mig_io_tif_w_8 ( char *name , Mig8u *src , int w , int h )
{
        mig_tiff_info_t info;

        info.w =w;
        info.h = h;
        info.bps = 8;
        info.spp = 1;
        info.hres = 1.0;
        info.vres = 1.0;
        info.unitsres = MIG_UNITS_NONE;
        info.comp = MIG_CMP_LZW;
        sprintf( info.dsc , "NONE" );

        return mig_io_tif_w ( name , src , &info );
}

/****************************************************************************/
int
mig_io_tif_w_16 ( char *name , Mig16u *src , int w , int h )
{
        mig_tiff_info_t info;

        info.w =w;
        info.h = h;
        info.bps = 16;
        info.spp = 1;
        info.hres = 1.0;
        info.vres = 1.0;
        info.unitsres = MIG_UNITS_NONE;
        info.comp = MIG_CMP_LZW;
        sprintf( info.dsc , "NONE" );

        return mig_io_tif_w ( name , (Mig8u*)src , &info );
}

/****************************************************************************/
static int
_mig_io_tif_check_tags ( _MigTiffInfo *info )
{
        if ( (info->bps != 1)   &&
             (info->bps != 8)   &&
             (info->bps != 16) )
                return MIG_ERROR_UNSUPPORTED;

        if (info->spp != 1)
                return MIG_ERROR_UNSUPPORTED;

        if ( (info->comp != MIG_CMP_NONE)               &&
             (info->comp != MIG_CMP_CCITTRLE)           &&
             (info->comp != MIG_CMP_CCITTFAX3)          &&
             (info->comp != MIG_CMP_CCITTFAX4)          &&
             (info->comp != MIG_CMP_LZW)                &&
             (info->comp != MIG_CMP_DEFLATE_ADOBE)      &&
             (info->comp != MIG_CMP_DEFLATE)            &&
             (info->comp != MIG_CMP_PACKBITS) )
                return MIG_ERROR_UNSUPPORTED;

        if (info->bps == 8)
        {
                if ( (info->comp == MIG_CMP_CCITTFAX3) ||
                     (info->comp == MIG_CMP_CCITTFAX4) )
                        return MIG_ERROR_UNSUPPORTED;
        }

        if ( ( (info->bps == 1) && (info->photo  !=  PHOTOMETRIC_MINISWHITE) ) ||
             ( (info->bps != 1) && (info->photo  !=  PHOTOMETRIC_MINISBLACK ) ) )
                return MIG_ERROR_UNSUPPORTED;

        if (info->planar != PLANARCONFIG_CONTIG)
                return MIG_ERROR_UNSUPPORTED;

        return MIG_OK;
}

/****************************************************************************/
static void
_mig_io_tif_info_dump ( _MigTiffInfo *info )
{
        fprintf ( stdout, "\n----------------------------------------------     ");
                fprintf (stdout, "\nTiff file information :             ");
                fprintf (stdout, "\nwidth               -       %d", info->w);
                fprintf (stdout, "\nheight              -       %d", info->h);
                fprintf (stdout, "\nbits / sample       -       %d", info->bps);
                fprintf (stdout, "\nsamples / pixel     -       %d", info->spp);
                fprintf (stdout, "\nh resolution        -       %f", info->hres);
                fprintf (stdout, "\nv resolution        -       %f", info->vres);
        fprintf (stdout, "\nresolution units            -       ");
                switch (info->unitsres)
                {
                        case MIG_UNITS_NONE :
                                fprintf (stdout, "none");
                                break;
                        case MIG_UNITS_CM :
                                fprintf (stdout, "centimeters");
                                break;
                        default:
                                fprintf (stdout, "unknown: %d", info->unitsres);
                }

                fprintf (stdout, "\ncompression         -       ");
                switch (info->comp)
                {
                        case MIG_CMP_NONE :
                                fprintf (stdout, "NONE");
                                break;
                        case MIG_CMP_CCITTFAX3 :
                                fprintf (stdout, "CCITTFAX3");
                                break;
                        case MIG_CMP_CCITTFAX4 :
                                fprintf (stdout, "CCITTFAX4");
                                break;
                        case MIG_CMP_LZW :
                                fprintf (stdout, "LZW");
                                break;
                        case MIG_CMP_DEFLATE_ADOBE :
                                fprintf (stdout, "DEFLATE ADOBE");
                                break;
                        case MIG_CMP_DEFLATE :
                                fprintf (stdout, "DEFLATE");
                                break;
                        case MIG_CMP_PACKBITS :
                                fprintf (stdout, "PACKBITS");
                                break;
                        default:
                                fprintf (stdout, "UNKNOWN : %d", info->comp);
                }

                fprintf (stdout, "\nphotometric -       ");
                switch (info->photo)
                {
                        case PHOTOMETRIC_MINISWHITE:
                                fprintf (stdout, "min is white");
                                break;
                        case PHOTOMETRIC_MINISBLACK :
                                fprintf (stdout, "min is black");
                                break;
                        case PHOTOMETRIC_RGB :
                                fprintf (stdout, "rgb");
                                break;
                        default:
                                fprintf (stdout, "unknown: %d", info->photo);
                }

                fprintf (stdout, "\nplanar      -       ");
                switch (info->planar)
                {
                        case PLANARCONFIG_CONTIG :
                                fprintf (stdout, "single image plane");
                                break;
                        case PLANARCONFIG_SEPARATE :
                                fprintf (stdout, "separate planes");
                                break;
                        default:
                                fprintf (stdout, "unknown : %d", info->planar);
                }

                fprintf (stdout, "\ndescription -       %s", info->dsc );
                fprintf (stdout, "\n----------------------------------------------\n");
}

/***************************************************************************************************/
static int
_mig_io_tif_w_1u ( TIFF *tiff_file , Mig8u *src , _MigTiffInfo *info )
{
        int i, j, w;
        Mig8u *buffer = NULL, *idx;
        int ret;

        w = TIFFScanlineSize (tiff_file);

        buffer = (Mig8u*) malloc(w * (info->h) * sizeof(Mig8u));
        if (!buffer)
        {
                ret = MIG_ERROR_MEMORY;
                goto error;
        }

        /*
             1. swap black / white for PHOTOMETRIC MINISWHITE
             2. pack
             3 write buffer out.
        */
        idx = buffer;
        for (j=0;j<info->h;++j, idx+=w)
        {
                for (i=0;i<info->w;++i,++src)
                {
                        if (*src == 0x00)
                        {
                                mig_set_8_1(idx, i);
                        }
                        else
                        {
                                mig_clear_8_1(idx, i);
                        }
                }
                TIFFWriteScanline (tiff_file, idx, j, 0);
        }

        free (buffer);
        return MIG_OK;

error:

        if (buffer)
                free (buffer);
        return ret;
}

/****************************************************************************/
static int
_mig_io_tif_w_8u ( TIFF *tiff_file , Mig8u *src , _MigTiffInfo *info )
{
        int i;

        for ( i = 0 ; i < info->h ; ++i , src += info->w )
                TIFFWriteScanline ( tiff_file , (tdata_t)src , i , 0 );

        return MIG_OK;
}

/****************************************************************************/
static int
_mig_io_tif_w_16u ( TIFF *tiff_file , Mig8u *src , _MigTiffInfo *info )
{
        int i, w;
	
        w = (info->w) << 1;

        for (i=0;i<info->h;++i, src+= w)
                TIFFWriteScanline (tiff_file, (void*)src, i, 0);

        return MIG_OK;
}

/****************************************************************************/
static int
_mig_io_tif_r_1u ( TIFF *tiff_file , Mig8u *dst , _MigTiffInfo *info )
{
        int i, j, w;
        Mig8u *buffer = NULL, *idx, val;
        int ret;

        w = TIFFScanlineSize(tiff_file);
        buffer = (Mig8u*) malloc (w * (info->h));
        if (!buffer)
        {
                ret = MIG_ERROR_MEMORY;
                goto error;
        }

        /*
             1 read buffer in
             2.unpack
             3. swap black / white for PHOTOMETRIC MINISWHITE
        */
        idx = buffer;
                for (j=0;j<info->h;++j, idx+= w)
                {
                        TIFFReadScanline (tiff_file, idx, j, 0);
                for (i=0;i<info->w;++i, ++dst)
                        {
                                val = mig_get_8_1(idx, i);
                                *dst = (val == 0x00)?0xFF:0x00;
                        }
        }

        free (buffer);
        return MIG_OK;

error:
        if (buffer)
                free (buffer);

        return ret;
}

/****************************************************************************/
static int
_mig_io_tif_r_8u ( TIFF *tiff_file , Mig8u *dst , _MigTiffInfo *info )
{
        int i;

        for (i=0;i<info->h;++i, dst+= info->w)
        {
                TIFFReadScanline (tiff_file, dst, i, 0);
        }

        return MIG_OK;
}

/****************************************************************************/
static int
_mig_io_tif_r_16u ( TIFF *tiff_file , Mig8u *dst , _MigTiffInfo *info )
{
        int i, w;

        w = (info->w) << 1;

        for (i=0;i<info->h;++i, dst+= w)
                TIFFReadScanline (tiff_file, dst, i, 0);

        return MIG_OK;
}

