%
% [out] = load_tiff( DirName , type )
%
% Load a sequence of tif files as stack from directory
%
% INPUT :
%	DirName - slash terminated name of directory where
%                 tif files are stored.
%       type    - 1 -> tif files are 1 bpp (binary).
%                 2 -> tif files are 16 bpp (unsigned short).
%
%  OUTPUT :
%       out     - stack of loaded images (empty if error).
%
%  NOTES : Tif files should have been stored on disk in order (i.e.
%          im_00.tif , im_01.tif, etc. ). Tif files should have the
%          extension .tif.
%

function [out] = load_tiff( DirName , type )

out = [];

% get all .tif names
WildCardName = [ DirName , '\*.tif' ];

d = dir( WildCardName );
if ( isempty( d ) )
	warning( 'Could not read directory...' );
	return 
end

NumberFiles = size( d , 1 );

% read one file and deterime size
CurrentName = d(1).name;
[ FullName , err ] = sprintf( '%s\\%s' , DirName , CurrentName );
s = size( imread( FullName ) );

% build output array
switch type

	case 1
		out = repmat( logical(0) , [s NumberFiles] );
	case 2
		out = repmat( uint16(0) ,  [s NumberFiles] );
	otherwise
		warning('Unknown type...');
		return
end

for i = 1 : NumberFiles
	
	CurrentName = d(i).name;
	[ FullName , err ] = sprintf( '%s\\%s' , DirName , CurrentName );
	
	fprintf( 1 , '\nTrying to load : %s' , FullName );
	
	switch type
	
		case 1
			im = logical( imread( FullName ) );
		case 2
			im = uint16( imread( FullName ) );
		otherwise
			warning('Unknown type...');
			return
        end
	
	if ( isempty( im ) )
		warning( 'Error loading file...' );
		return
	end
	
	out(:,:,i) = im;
end
