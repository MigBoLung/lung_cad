% MetaImage is composed of 2 file : a header file ( extension .mdh ) and a 
% binary file containing pixel/voxel data ( extension .raw ).
%
% Neccessary MetaImage tags 
% ObjectType = string { ex. "Image" , ... } : type of object
% NDims = int { ex. 3 } : number of dimensions
% DimSize = array[NDims] of int { ex. 512 512 500 } : x y z... dimensions
% ElementType = string { ex. MET_USHORT , MET_UCHAR } : type of pixels
% ElementDataFile = string { ex. image.raw ] : name of binary file
%
% Additional MetaImage tags 
% HeaderSize = X int : when loading binary image file skip X bytes at the
% beginning of file. When HeaderSize = -1 loader calculates auto magically
% the number of bytes to skip at the beginning of binary file
% ElementSpacing = X Y Z... : distance between pixel centers in mm
% ElementSize = X Y Z... : size of voxel in mm in each direction
% ElementByteOrderMSB = boolean { ex. True, False }
%
% For a more detailed description see :
% http://www.itk.org/Wiki/MetaIO/Documentation
%

function err = mat_2_metaimage ( src_file_name , dst_file_name )

err = 0;

% nargin
% varargin

if ( nargin < 1 )
    err = -1;
    return;
end

% if dst_file_name is not given we 
% build it stripping .mat extension from src_file_name
if ( nargin == 1 )
   pos = strfind( src_file_name ,  '.mat' );
   if ( pos == [] )
       err = -2;
       return;
   end
   
   dst_file_name = src_file_name(1:pos-1);
end

% try loading file
try
    in = load( src_file_name );
catch
    err = -3;
    return;
end

% check wether loaded data contains SUPP field
if ( ~isfield( in , 'SUPP' ) )
    err = -4;
    return;
else
    supp = in.SUPP;
end

% check wether loaded data contains MASKS or CT field
if ( isfield( in , 'CT' ) )
    data = in.CT;
    data_type_out = 'MET_USHORT';
elseif ( isfield( in , 'MASKS' ) )
    data = in.MASKS;
    data = uint8( data * 255 );
    data_type_out = 'MET_UCHAR';
else
    err = -4;
    return;
end

% output consists of two files :
% 1. header file      : dst_file_name.mhd
% 2. binary data file : dst_file_name_suffix.raw

out_hdr_name = [ dst_file_name , '.mhd' ];
out_bin_name = [ dst_file_name , '.raw' ];

% build file name of binary file to write inside header file
% search for last occurence of file separator for current platform
pos = strfind( dst_file_name , filesep );
if ( isempty(pos) )    % only file name without absolute path
    out_bin_hdr_name = [ dst_file_name , '.raw' ];
else
    out_bin_hdr_name = [ dst_file_name( max(pos) + 1 : end ) , '.raw' ];
end
    
% write header 
[ fid , msg ] = fopen( out_hdr_name , 'wt' );
if ( fid == -1 )
    err = -5;
    fprintf( 2 , '\n%s\n' , msg );
    return;
end

fprintf( fid , 'ObjectType = %s\n' , 'Image' );
fprintf( fid , 'NDims = %d\n' , 3 );
fprintf( fid , 'ElementType = %s\n' , data_type_out );
fprintf( fid , 'ElementDataByteOrderMSB = %s\n' , 'False' );
fprintf( fid , 'DimSize = %d %d %d\n' , supp.height, supp.width , supp.n );
fprintf( fid , 'ElementSpacing = %3.2f %3.2f %3.2f\n' , supp.PixelSpacing(1) , supp.PixelSpacing(2), supp.SliceSpacing );
fprintf( fid , 'ElementDataFile = %s\n' , out_bin_hdr_name );
fclose( fid );

% write binary file
[ fid , msg ] = fopen( out_bin_name , 'w' );
if ( fid == -1 )
    err = -6;
    fprintf( 2 , '\n%s\n' , msg );
    return;
end

cnt = fwrite( fid , data );
fclose( fid );

if ( cnt ~= supp.width * supp.height * supp.n )
    err = -7;
end
