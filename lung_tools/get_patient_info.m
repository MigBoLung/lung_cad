function [ patient_info ] = get_patient_info( dir_name , log_fid )

patient_info.fname_dir = dir_name;

% read dicom input directory
[ names , err ] = sprintf( '%s*.dcm' , dir_name );
dir_dcm = dir( names );
dir_dcm_size = size( dir_dcm , 1 );

if ( dir_dcm_size <= 0 )
	error ( 'No .dcm files found...' );
end

% holds one dcm file entry
file = struct( 'fname'  , '' , ...
               'SOPInstanceUID' , '' , ...
	           'InstanceNumber' , '' );

% make space for dicom data
patient_info.files = repmat( file , [ 1 dir_dcm_size ] );
patient_info.slices = dir_dcm_size;

% read xml gt filename
[ names , err ] = sprintf( '%s*.xml' , dir_name );
dir_xml = dir( names );
dir_xml_size = size( dir_xml , 1 );

% there should be only one xml gt file
if ( dir_xml_size ~= 1 )
	error ( 'There should be one and only one xml gt file...' );
end

patient_info.fname_gt = dir_xml( 1 ).name;

% first dcm file gets special treatment

% get filename
name = dir_dcm( 1 ).name;
base = name( 1 : strfind( name , '.dcm' ) - 1 );
[ name_dcm , err ] = sprintf( '%s%s' , dir_name , name );
info = dicominfo( name_dcm );

if ( isempty( info ) )
	error( 'Wrong .dcm file format ?!...' );
end

% save some patient information
patient_info.PatientID = info.PatientID;
patient_info.StudyInstanceUID = info.StudyInstanceUID;
patient_info.SeriesInstanceUID = info.SeriesInstanceUID;
patient_info.SliceThickness = info.SliceThickness;
patient_info.PixelSpacing = info.PixelSpacing;
patient_info.ConvolutionKernel = info.ConvolutionKernel;

patient_info.files(1).fname = name;
patient_info.files(1).SOPInstanceUID = info.SOPInstanceUID;
patient_info.files(1).InstanceNumber = info.InstanceNumber;

% for all other dcm files
for n = 2 : dir_dcm_size

	name = dir_dcm( n ).name;
	base = name( 1 : strfind( name , '.dcm' ) - 1 );
	[ name_dcm , err ] = sprintf( '%s%s' , dir_name , name );
	info = dicominfo( name_dcm );

	if ( isempty( info ) )
		error( 'Wrong .dcm file format ?!...' );
	end

	patient_info.files(n).fname = name;
	patient_info.files(n).SOPInstanceUID = info.SOPInstanceUID;
	patient_info.files(n).InstanceNumber = info.InstanceNumber;

end

in = [patient_info.files.InstanceNumber];
idx0 = find( in == min( in(:) ) );
idx1 = find( in == max( in(:) ) );

name = dir_dcm( idx0 ).name;
base = name( 1 : strfind( name , '.dcm' ) - 1 );
[ name_dcm , err ] = sprintf( '%s%s' , dir_name , name );
info = dicominfo( name_dcm );

p0 = info.ImagePositionPatient;

name = dir_dcm( idx1 ).name;
base = name( 1 : strfind( name , '.dcm' ) - 1 );
[ name_dcm , err ] = sprintf( '%s%s' , dir_name , name );
info = dicominfo( name_dcm );

p1 = info.ImagePositionPatient;

patient_info.SliceSpacing =  sqrt (  ( p1(1) - p0(1) )^2 + ...
			             ( p1(2) - p0(2) )^2 + ...
	                             ( p1(3) - p0(3) )^2 ) / ...
				     ( patient_info.slices - 1 );


% now get xml ground truth
[ name_xml , err ] = sprintf( '%s%s' , dir_name , patient_info.fname_gt );
patient_info.gt_info_xml = get_xml_gt( name_xml , 1 );
