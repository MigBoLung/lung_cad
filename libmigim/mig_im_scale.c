#include "mig_im_scale.h"

void
mig_im_scale_whitening ( float *Src , float *Dst , float nsigma , 
						int len , float *mean , float *std ) {

	int i = 0;
	
	//float invstd = 0;

	
	for ( i = 0 ; i != len ; i++ ) {
		Dst[i] = ( Src[i] - mean[i] ) / ( std[i] * nsigma ) ;
	}

	/*old version, maybe just a little faster but doesn't handle inplace
	for ( i = 0 ; i != len ; i++ ) {
		*Dst++ = ( *Src++ - *mean++ ) / ( *std++ * nsigma ) ;
	}
	*/
}

/********************************************************
C99 has inline, this a good candidate for inline,
	but doesn't compile AS IS on VC. (not tried elsewhere)
*********************************************************/
	
void
mig_im_scale_whitening_inplace ( float *Src , float nsigma , 
						int len , float *mean , float *std )
{
	/* we just call the other version, with same src and dst */
	mig_im_scale_whitening ( Src , Src , nsigma , 
						 len , mean , std );
	
}