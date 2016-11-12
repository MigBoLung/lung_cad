function [ Data, Rc ] = load_bin_stack_2d ( dirname , filename )
%
% [ Data, Rc ] = load_bin_stack_2d ( dirname , filename )
%
% Load stack of data from binary files inside directory. 
% dirname should contain the base directory where files are stored.
% Filename should be a regular expression for the file names.
% The binary files are supposed to have been encoded as follows :
% 4 bytes (int) -> image width
% 4 bytes (int) -> image height
% 4 bytes (int) -> image z ( should be 1 )
% 4 bytes (float) * width * height -> pixel data
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

% read directory contents
dir_contents = dir( [dirname,filename] );
if ( isempty( dir_contents ) )
    Rc = -1;
    return;
end

% total image number in directory
im_number = size( dir_contents , 1 );


for  i = 1 : im_number
    
    dir_entry = dir_contents(i);
    if ( dir_entry.isdir == 1 )
        continue;
    end
    
    % open file for rading 
    fid = fopen( [ dirname , dir_entry.name ] , 'r' );
    if ( fid == -1 )
        Rc = -1;
        return;
    end

    % read image width height z
    [ dim , cnt ] = fread( fid , 3 , 'int32' ); 
    if ( cnt ~= 3 )
        fclose ( fid );
    	Rc = -2;
        return;
    end

    % read image data
    [ tmp , cnt ] = fread( fid , prod(dim) , 'float32' ); 
    if ( cnt ~= prod(dim) )
        fclose ( fid );
        Rc = -3;
    	return;
    end

    % reshape data to 2D array
    tmp = reshape( tmp , dim' );

    % transform to column major order
    tmp = permute( tmp , [2 1 3] );

    % close input file
    fclose( fid );

    % add image contents to stack
    Data = cat( 3 , Data , tmp );

end


