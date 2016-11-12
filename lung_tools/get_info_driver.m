close all
clear all
clc;

% dir list file name
fname_list = 'lidc.lst';

a = textread( fname_list , '%s' , -1 );
patients_data = [];

% for all input directories
for n = 1 : size( a , 1 )

	name = a{n};

	% name = [a{n} , '\'];
	
	fprintf( 1 , '\nProcessing : %s' , name );
	
	% read and process dicom and xml data from disk
	try
		patient_info = get_patient_info( name , 1 );
		patients_data = cat( 1 , patients_data , patient_info );
		patient_info = [];
		
	catch
		lasterr
		error('File processing...');
	end
end

save lidc_patients_data patients_data



