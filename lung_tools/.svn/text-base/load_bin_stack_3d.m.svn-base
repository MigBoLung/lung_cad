function [ Data, Rc ] = load_bin_stack_3d ( filename )
%
% [ Data, Rc ] = load_bin_stack_3d ( filename )
%
% Load stack of data from binary file. The binary file is supposed
% to have been encoded as follows :
% 4 bytes (int) -> stack width
% 4 bytes (int) -> stack height
% 4 bytes (int) -> stack z
% 4 bytes (float) * width * height * z -> pixel data
%
% Loaded stack is returned in Data ( double array )
% Return code Rc is :  0 on success
%                     -1 could not open binary file for reading
%                     -2 could not read width / height / z
%                     -3 could not read pixel data
%

% zero output
Data = [];
Rc = 0;

% open file for rading 
fid = fopen( filename , 'r' );
if ( fid == -1 )
    Rc = -1;
    return;
end

% read stack width height depth
[ dim , cnt] = fread( fid , 3 , 'int32'); 
if ( cnt ~= 3 )
    fclose ( fid );
	Rc = -2;
	return;
end

% read all stack data
[ tmp , cnt ] = fread( fid , prod(dim) , 'float32'); 
if ( cnt ~= prod(dim) )
    fclose ( fid );
	Rc = -3;
	return;
end

% reshape data to 3D array
tmp = reshape( tmp , dim' );

% transform to column major order
Data = permute( tmp , [2 1 3] );

% close input file
fclose( fid );



