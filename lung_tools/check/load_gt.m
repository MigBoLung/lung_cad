% LOAD_GT Load all nodules from GT text file
%
% SYNOPSIS :
%
% [ nodules , err_id , err_msg ] = load_gt( gt_file_name , patient_id )
%
% gt_file_name  : string. Gt file name.
%
% nodules       : [1 x 1] strct made of two fields :
%                   1. patient_id - string. Patinet id.
%                   2. num        - integer. number of nodules in this gt
%                   3. data       - [1 x num] struct.
%                       Each data entry is made of three fields :
%                           1. id         - integer ( startts at 1 ). Id of nodule.
%                           2. num_slices - integer. Number of slices where nodule is visible.
%                           3. slices     - [5 x num_slices] array. The
%                           five fields are :
%                               1. integer. z ( slice ) coordinate.
%                               2. integer. x ( horizontal ) coordinate.
%                               3. integer. y ( vertical ) coordinate.
%                               4. integer. min nodule diameter in pixels.
%                               5. integer. max nodule diameter in pixels.
%                       
% err_id        : integer. Error id can be :
%                   0 - no error
%                  -1 - Gt txt file could not be opened.
%                  -2 - Gt txt file missing number of nodules field.
%                  -3 - Gt txt file missing nodule id field.
%                  -4 - Gt txt file missing number of nodule slices field.
%                  -5 - Gt txt file missing a slice field.
%
% err_msg       : string. Error message.
%
% For structure of a text GT have a look at GT_FILE_STRUCTURE.TXT
% in this directory.

function [ nodules , err_id , err_msg ] = load_gt( gt_file_name , patient_id )

err_id = 0;
err_msg = [];
nodules = [];

% open text file for reading
fid = fopen( gt_file_name , 'r' );
if ( fid == -1 )
    [ err_msg , err_id ] = lasterr;
    return;
end

% get number of nodules in file
[ tmp , cnt ] = fscanf( fid , '%d\n' , 1 );
if ( cnt ~= 1 )
	err_msg = 'WRONG FORMAT. Txt file missing number of nodules field';
    err_id = -2;
    fclose( fid );
    return;
end

nodules.num = tmp;
nodules.data = [];

for i = 1 : nodules.num
    [ nodule , err_id , err_msg ] = load_gt_nodule( fid );
    if ( err_id ~= 0 )
        fclose( fid );
        return;
    end
    nodules.data = [ nodules.data , nodule ];
end

nodules.patient_id = patient_id;
fclose( fid );

