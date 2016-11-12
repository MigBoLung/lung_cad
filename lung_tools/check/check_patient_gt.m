function [ nodules , err_msg , err_id ] = check_patient_gt( patient_nodules )

global MASKS_L;
global MASKS_R;
global BB_L;
global BB_R;

nodules = [];
err_msg = [];
err_id = 0;

% left / right median line in horizonal direction
% left for us is the opposite of what we see on the left as we watch the patient images on
% the screen... 
LEFT_RIGHT_SEP = 512 / 2;


% process each nodule for current patient
for i = 1 : patient_nodules.num

    % current nodule
    nodule = patient_nodules.data(i);
    
    % for each slice making up nodule
    for  j = 1 : nodule.num_slices
        
        slice_data = nodule.slices( : , j );
        z_coord  = slice_data(1);
        x_coord  = slice_data(2);
        y_coord  = slice_data(3);
        nod_diam = slice_data(4);
        nod_r    = floor( nod_diam / 2 );
            
        if ( x_coord > LEFT_RIGHT_SEP ) % we are in left lung
            
            side = 'LEFT';
            
            % adjust crop coordinates to bounding box
            x_coord_crop = x_coord - BB_L.x0 + 1;
            y_coord_crop = y_coord - BB_L.y0 + 1;
            z_coord_crop = z_coord - BB_L.z0 + 1;

            x0 = x_coord_crop - nod_r;
	        x1 = x_coord_crop + nod_r;

            y0 = y_coord_crop - nod_r;
            y1 = y_coord_crop + nod_r;
            
            fprintf( 1 , '\nLEFT Crop coordinates : %02d - %02d , %02d - %02d , %02d' , ...
                         x0 , x1 , y0 , y1 , z_coord_crop );
            
            if ( ( x0 <= 0 ) || ( x1 > size( MASKS_L , 2 ) ) || ...
                 ( y0 <= 0 ) || ( y1 > size( MASKS_L , 1 ) ) || ...
		 ( z_coord_crop <= 0 ) )
                data_crop = zeros( nod_diam , nod_diam );
            else
                data_crop = MASKS_L(  y0 : y1 , x0 : x1 , z_coord_crop );
	    end
            
        else % we are in right lung
            
            side = 'RIGHT';
            
            % adjust crop coordinates to bounding box
            x_coord_crop = x_coord - BB_R.x0 + 1;
            y_coord_crop = y_coord - BB_R.y0 + 1;
            z_coord_crop = z_coord - BB_R.z0 + 1;

            x0 = x_coord_crop - nod_r;
	        x1 = x_coord_crop + nod_r;

            y0 = y_coord_crop - nod_r;
            y1 = y_coord_crop + nod_r;
            
            fprintf( 1 , '\nLEFT Crop coordinates : %02d - %02d , %02d - %02d , %02d' , ...
                         x0 , x1 , y0 , y1 , z_coord_crop );
            
            if ( ( x0 <= 0 ) || ( x1 > size( MASKS_L , 2 ) ) || ...
                 ( y0 <= 0 ) || ( y1 > size( MASKS_L , 1 )  ) || ...
		 ( z_coord_crop <= 0 ) )
                data_crop = zeros( nod_diam , nod_diam );
            else
                data_crop = MASKS_R(  y0 : y1 , x0 : x1 , z_coord_crop );
            end
        
        end

        % cut crop area
        data_crop_area = sum( data_crop(:) );
        
        % build a verification bw crop :
        % verification crop is a bw circle image of the same dimensions as
        % the crop and radius <= nod_r
        gt_crop = zeros( size(data_crop) );
        gt_crop_center = round( size(gt_crop) / 2 );
        gt_crop( gt_crop_center(1), gt_crop_center(2) ) = 1;
        gt_crop = bwdist( gt_crop , 'euclidean' ) <= nod_r;
        gt_crop_area = sum( gt_crop(:) );
        
        % final crop is given by intersection of crop coming from patient
        % data and our dummy crop constructed from ground truth data
        int_crop = data_crop & gt_crop;
        int_crop_area = sum( int_crop(:) );
        
        % save crops
        nodule.data(j).data_crop = data_crop;
        nodule.data(j).gt_crop   = gt_crop;
        nodule.data(j).int_crop  = int_crop;
        
        % save crop area
        nodule.data(j).data_crop_area = data_crop_area;
        nodule.data(j).gt_crop_area = gt_crop_area;
        nodule.data(j).int_crop_area = int_crop_area;
        
        % save overlap info
        nodule.data(j).overlap = nodule.data(j).int_crop_area / nodule.data(j).gt_crop_area;
        nodule.data(j).overlap_perc = nodule.data(j).overlap * 100;

    end % for each slice of current nodule
    
    nodule.side = side;
    nodules = cat( 2 , nodules , nodule );
    
end % for each nodule for current patient
