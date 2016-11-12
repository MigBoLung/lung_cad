close all
clear all
clc;

% log file 
log_name = 'xml2txt.log';
%log_fid  = 1;
log_fid  = fopen( log_name , 'wt+' );

% xml gt file can be found here
dir_in  = 'gt_xml\';

% txt gt file are going to be stored here
dir_out = 'gt_txt\';

% read xml gt input directory
[ names_in , err ] = sprintf( '%s*.xml' , dir_in );
dir_xml = dir( names_in );
dir_xml_size = size( dir_xml , 1 );

clear names_in err

% for all xml files
for n = 1 : dir_xml_size
	
	% get filenames
	name = dir_xml( n ).name;
	base = name( 1 : strfind( name , '.xml' ) - 1 );
	[ name_xml , err ] = sprintf( '%s%s' , dir_in , name );
	
	fprintf ( log_fid , '\nProcessing : %s\n' , name_xml );

	% read and process xml from disk
	try
		xml2txt( name_xml , log_fid );
	catch
		error('File could not be read...');
	end	
end

fclose ( log_fid );
