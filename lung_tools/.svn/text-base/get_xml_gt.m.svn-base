function [ gt_info ] = get_xml_gt( fname , log_fid )

gt_info.nodules = [];

% load xml tree
t = xmltree( fname );
if ( isempty( t ) )
	error ('Reading xml file...' );
end

% convert tree to matlab structure
m = convert( t );

ResponseHeader = m.ResponseHeader;

for i = 1 : size( m.readingSession , 2 )
	
	readingSession = m.readingSession{i};
	
	% fprintf ( log_fid , '\nReading Session : %d' , i );
	
	if ( isfield( readingSession , 'unblindedReadNodule' ) )
		
		nodules = readingSession.unblindedReadNodule;
		nodule_num = size ( nodules , 2 );
		
		fprintf ( log_fid , '\nNumber of nodules : %d' , nodule_num );
		
		% for all nodules
		for j = 1 : nodule_num
			
			if ( nodule_num == 1 )
				nodule = nodules;
			else
				nodule = nodules{j};
			end
			
			% some nodules have characteristics !
			if ( ~isfield( nodule , 'characteristics' ) )
				nodule.characteristics = [];
			end
			
			gt_info.nodules = cat( 2 , gt_info.nodules , nodule );
		
			nodule_id = nodule.noduleID;
		
			% nodule can be on multiple slices
			nodule_num_rois = size( nodule.roi , 2 );
			nodule_num_slices = 0;
			
			fprintf ( log_fid , '\nNodule...' );
			fprintf ( log_fid , '\n\t id     : %s' , nodule_id );
			fprintf ( log_fid , '\n\t rois   : %d' , nodule_num_rois );
		
			% for all slices of a nodule
			for k = 1 : nodule_num_rois
			
				if ( nodule_num_rois == 1 )
					roi = nodule.roi;
				else
					roi = nodule.roi{k};
				end
				
				%if ( strcmp( roi.inclusion , 'TRUE') )

				nodule_image_id = roi.imageSOP_UID;
				fprintf ( log_fid , '\n\t image : %s' , nodule_image_id );

				nodule_edge_map = roi.edgeMap;
				nodule_num_coord = size( nodule_edge_map , 2 );
				fprintf ( log_fid , '\n\t num coord : %d' , nodule_num_coord );

				nodule_num_slices = nodule_num_slices + 1;
				
				%end

			end % for all rois
		
		end % for all nodules 
	
	end % unblindedReadNodule

end % for all reading sessions

if ( isempty( gt_info.nodules ) )
	gt_info.num_nodules = 0;
else
	gt_info.num_nodules = size( gt_info.nodules , 2 );
end

