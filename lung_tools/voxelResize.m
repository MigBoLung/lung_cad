function resizedVoxel = voxelResize(originalVoxel,resLinDim)
% voxelResize -- Perform the resizing of a 3D voxel
%
% Usage:
%
% resizedVoxel = voxelResize(originalVoxel,resLinDim)
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
% EXAMPLE:
%
% vo = Make3DDataEmiro(32,32,8,0);
% figure, slice(1:32,1:32,1:8,vo,16,16,4), colormap(gray), title('Original voxel')
% figure, isosurface(vo), title('Original voxel')
% vr = voxelResize(vo,16);
% figure, slice(1:16,1:16,1:16,vr,8,8,8), colormap(gray), title('Resized voxel')
% figure, isosurface(vr), title('Resized voxel')

% Matteo Masotti (matteo.masotti@bo.infn.it)
% Last Revision: 14 Apr 2005



% Checking the number of input arguments (min = 2)
if nargin < 2
    disp('Error: Incorrect number of input arguments');
    help voxelResize;
    return
end

% Linear dimensions of the original voxel
[xDim,yDim,zDim] = size(originalVoxel);

% 3D RESIZING
%
% The built-in function imresize is applied along each axis of the voxel (z, x and y), 
% while only one of the remaining dimensions is resized
%
%     Z
%     |
%     |
%     |
%     + - - - - X
%    /
%   /
%  /
% Y

% Consider slices along the z-axis, while resizing the x-axis dimension
for z = 1:zDim
    
    % tmpX is the array containing the voxel with the x-axis resized
    tmpX(:,:,z) = imresize(originalVoxel(:,:,z),[yDim resLinDim],'bilinear');
    
end

% Consider slices along the x-axis, while resizing the y-axis dimension
for x = 1:resLinDim
    
    % tmpXY is the array containing the voxel with the x-axis and y-axis resized    
    tmpXY(:,x,:) = imresize(squeeze(tmpX(:,x,:)),[resLinDim zDim],'bilinear');

end

% Consider slices along the y-axis, while resizing the z-axis dimension
for y = 1:resLinDim
    
    % resizedVoxel is the array containing the voxel with the x-axis, y-axis and z-axis resized        
    resizedVoxel(y,:,:) = imresize(squeeze(tmpXY(y,:,:)),[resLinDim resLinDim],'bilinear');
    
end