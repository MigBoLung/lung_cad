close all
clear all
clc;

% patient info file name
fname_patients_data = 'lidc_patients_data.mat';
dir_out = '/scratch/todor/lab/';

pdata = load( fname_patients_data );
pdata = pdata.patients_data;

% for all patients in list
for p = 1 : size( pdata , 1 ) 
	
	pat = pdata(p);

	fprintf ( 1 , '\n------------------------------------' );
	fprintf ( 1 , '\nPatient : %s' , pat.PatientID );
	fprintf ( 1 , '\nStudy   : %s' , pat.StudyInstanceUID );
	fprintf ( 1 , '\nSeries  : %s' , pat.SeriesInstanceUID );
     
	try
	
		[ GT , SUPP ] = encode_gt( pat );
		[ name_out , err ] = sprintf ( '%s%s_%s_%s_gt.mat' , ...
			dir_out , ...
			pat.PatientID , ...
			pat.StudyInstanceUID , ...
			pat.SeriesInstanceUID );
		
		save( name_out , 'GT' , 'SUPP' );
		
		clear GT SUPP

	catch
		lasterr
		continue;
	end
end




