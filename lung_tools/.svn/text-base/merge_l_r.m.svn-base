function [MERGED,SUPP] = merge_l_r( CT_RIGHT , BB_RIGHT , ...
	                                CT_LEFT  , BB_LEFT , ...
				                    SUPP_ORIGINAL )

%
% [MERGED,SUPP] = merge_l_r( CT_RIGHT , SUPP_RIGHT , ...
% 	                         CT_LEFT  , SUPP_LEFT , ...
% 			                 SUPP_ORIGINAL )
%
% Merge left and right segmented stacks into a full stack
%
% INPUT:
%
%   originalVoxel   :   the original voxel to resize, i.e. a (32,32,8) matrix
%
% OUTPUT:
%
%   resizedVoxel    :   the (resLinDim,resLinDim,resLinDim) resized voxel, i.e. a (16,16,16) matrix
%
%   resLinDim       :   the linear dimensions of the resized voxel
%

% get dimensions from original
w = SUPP_ORIGINAL.width;
h = SUPP_ORIGINAL.height;
slices = SUPP_ORIGINAL.n;

% allocate output stack
MERGED = repmat( uint16(0) , [h w slices] );

% copy dimension data to output SUPP
SUPP = SUPP_ORIGINAL;

% copy right lung to MERGED
MERGED( BB_RIGHT.y0 + 1 : BB_RIGHT.y1 + 1 , ...
	BB_RIGHT.x0 + 1 : BB_RIGHT.x1 + 1 , ...
	BB_RIGHT.z0 + 1 : BB_RIGHT.z1 + 1 ) = CT_RIGHT;

% copy left lung to MERGED
MERGED( BB_LEFT.y0 + 1 : BB_LEFT.y1 + 1 , ...
	BB_LEFT.x0 + 1 : BB_LEFT.x1 + 1 , ...
	BB_LEFT.z0 + 1 : BB_LEFT.z1 + 1 ) = CT_LEFT;




