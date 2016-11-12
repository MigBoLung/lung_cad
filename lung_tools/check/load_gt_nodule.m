% LOAD_GT_NODULE Load a single nodule from GT text file
%
% SYNOPSIS :
%
% [ nodule , err_id , err_msg ] = load_gt_nodule( fid )
%
% fid  : file descriptor as obtained from fopen.
%
% nodule        : [1 x 1] struct. Single nodule structure made of :
%                           1. id         - integer ( startts at 1 ). Id of nodule.
%                           2. num_slices - integer. Number of slices where nodule is visible.
%                           3. slices     - [5 x num_slices] array. The
%                           five fields are :
%                               1. integer. z ( slice ) coordinate.
%                               2. integer. x ( horizontal ) coordinate.
%                               3. integer. y ( vertical ) coordinate.
%                               4. float. min nodule diameter in pixels.
%                               5. float. max nodule diameter in pixels.
%                       
% err_id        : integer. Error id can be :
%                   0 - no error
%                  -2 - Gt txt file missing number of nodules field.
%                  -3 - Gt txt file missing nodule id field.
%                  -4 - Gt txt file missing number of nodule slices field.
%                  -5 - Gt txt file missing a slice field.
%
% err_msg       : string. Error message.
%
% For structure of a text GT have a look at GT_FILE_STRUCTURE.TXT
% in this directory.

function [ nodule , err_id , err_msg ] = load_gt_nodule( fid );

nodule = [];
err_id = 0;
err_msg = [];

% get id of current nodule
[tmp , cnt] = fscanf( fid , '%d\n' , 1 );
if ( cnt ~= 1 )
	err_msg = 'WRONG FORMAT. Txt file missing nodule id field';
    err_id = -3;
    return;
end

nodule.id = tmp;

% get number of slices in current nodule
[tmp , cnt] = fscanf( fid , '%d\n' , 1 );
if ( cnt ~= 1 )
	err_msg = 'WRONG FORMAT. Txt file missing number of nodule slices field';
    err_id = -4;
    return;
end

nodule.num_slices = tmp;
nodule.slices = [];

for i = 1 : nodule.num_slices
    [tmp,cnt] = fscanf( fid , '%d %d %d %f %f\n' , 5 );
    if ( cnt ~= 5 )
        err_msg = 'WRONG FORMAT. Txt file missing a slice field';
        err_id = -5;
        return;
    end
    
    nodule.slices = [nodule.slices , tmp];
end

