function [ GT , SUPP ] = encode_gt( patient_info )

% various useful information about patient
SUPP.n = patient_info.slices;
SUPP.width = 512;
SUPP.height = 512;
SUPP.SliceThickness = patient_info.SliceThickness;
SUPP.SliceSpacing = patient_info.SliceSpacing;
SUPP.PixelSpacing = patient_info.PixelSpacing;
SUPP.PatientID = patient_info.PatientID;
SUPP.StudyInstanceUID = patient_info.StudyInstanceUID;
SUPP.SeriesInstanceUID = patient_info.SeriesInstanceUID;
SUPP.gt = patient_info.fname_gt;

% this will hold uint8 stack
GT = repmat( uint8(0) , [ SUPP.height SUPP.width SUPP.n ] );

% get instance numbers
instance_numbers = [ patient_info.files.InstanceNumber ];

% start processing gt xml info
nod_info = patient_info.gt_info_xml.nodules;

fprintf ( 1 , '\nNumber of signed nodules : %d' ,  size( nod_info , 2 ) );

for n = 1 : size( nod_info , 2 )
	
	nodule = nod_info(n);
	rois = nodule.roi(:);

	fprintf ( 1 , '\nnodule id   : %s' ,  nodule.noduleID );
	fprintf ( 1 , '\nnodule rois : %d' ,  size( rois , 1 ) );
	
	for j = 1 : size( rois , 1 );
		
		roi = nodule.roi(j);
		
		if ( isa ( roi , 'cell' ) )
			roi = roi{:};
		end
		
		% inclusion == FALSE means that the following
		% roi should be excluded from the nodule ( maybe
		% it is an internal area ). We do not consider
		% rois with inclusion == FALSE
		if ( strcmp( roi.inclusion , 'FALSE' ) )
			continue;
		end

		% image unique identifier
		im_uid = roi.imageSOP_UID;
		
		% look trough all slices and
		% find the one that has the corresponding UID
		for i = 1 : SUPP.n
			
			if ( strcmp( patient_info.files(i).SOPInstanceUID , ...
				     im_uid ) )
				instance_number = patient_info.files(i).InstanceNumber;
				file_name = patient_info.files(i).fname;
				break;
			end
			
		end
		
		slice_number = instance_number;
		
		% get list of points
		edge_map = roi.edgeMap(:);
		
		fprintf( '\n\troi id              : %d' , j );
		fprintf( '\n\troi slice number    : %d' , slice_number );
		fprintf( '\n\troi file name       : %s' , file_name );
		fprintf( '\n\troi num points      : %d' , size( edge_map , 1 ) );
		
		% tmp will hold binary slice : we will
		% mark inside all points of the nodule
		% contour
		tmp = repmat( logical(0) , [ SUPP.width SUPP.height ] );
		
		for i = 1 : size( edge_map , 1 )
		
			p = edge_map(i);
			
			if ( isa ( p , 'cell' ) )
				p = p{:};
			end
			
			x = str2double( p.xCoord );
			y = str2double( p.yCoord );
			
			tmp( y , x ) = 1; 
		end
		
		% if nodule is given as only one coordinate ( i.e. its
		% diameter is below 3mm that we store just the coordinate.
		% if on the other hand we have contour points of a nodule
		% >= 3mm than we have to perform some operations to get the
		% nodule area...
		
		tmp = bwfill( tmp , 'holes' );
		GT( : , : , slice_number ) = ...
			imadd ( GT( : , : , slice_number ) , uint8(tmp) );

		% 		if ( size( edge_map , 1 ) == 1 ) % only center pixel
		% 			GT( : , : , slice_number ) = ...
		% 			imadd ( GT( : , : , slice_number ) , uint8(tmp) );
		% 		else
		% 			tmp1 = bwfill( tmp , 'holes' );
		% 			tmp = xor( tmp , tmp1 );
		% 			GT( : , : , slice_number ) = ...
		% 				imadd ( GT( : , : , slice_number ) , uint8(tmp) );
		% 		end
	end
end
