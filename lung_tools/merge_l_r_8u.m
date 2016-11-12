function [MASKS,SUPP] = merge_l_r_8u( MASKS_RIGHT , BB_RIGHT , ...
                                      MASKS_LEFT  , BB_LEFT , ...
                                      SUPP_ORIG , SUPP_SEG )


% get dimensions from original
w = SUPP_ORIG.width;
h = SUPP_ORIG.height;
slices = SUPP_ORIG.n;

% allocate output stack
MASKS = repmat( logical(0) , [h w slices] );

% copy dimension data to output SUPP

SUPP        = SUPP_SEG;
SUPP.width  = SUPP_ORIG.width;
SUPP.height = SUPP_ORIG.height;
SUPP.n      = SUPP_ORIG.n;

% copy right lung to MERGED
MASKS( BB_RIGHT.y0 + 1 : BB_RIGHT.y1 + 1 , ...
	   BB_RIGHT.x0 + 1 : BB_RIGHT.x1 + 1 , ...
	   BB_RIGHT.z0 + 1 : BB_RIGHT.z1 + 1 ) = MASKS_RIGHT;

% copy left lung to MERGED
MASKS( BB_LEFT.y0 + 1 : BB_LEFT.y1 + 1 , ...
	   BB_LEFT.x0 + 1 : BB_LEFT.x1 + 1 , ...
	   BB_LEFT.z0 + 1 : BB_LEFT.z1 + 1 ) = MASKS_LEFT;




