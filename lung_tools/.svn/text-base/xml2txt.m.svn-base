% function [] = xml2txt( fname , log_fid )

function [] = xml2txt( fname , log_fid )

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

	fprintf ( log_fid , '\nReading Session : %d' , i );

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

			nodule_id = nodule.noduleID;

			% nodule can be on multiple slices
			nodule_num_slices = size( nodule.roi , 2 );

			fprintf ( log_fid , '\nNodule...' );
			fprintf ( log_fid , '\n\t id     : %s' , nodule_id );
			fprintf ( log_fid , '\n\t slices : %d' , ...
					nodule_num_slices );

			% for all slices of a nodule
			for k = 1 : nodule_num_slices

				if ( nodule_num_slices == 1 )
					roi = nodule.roi;
				else
					roi = nodule.roi{k};
				end

				nodule_image_id = roi.imageSOP_UID;
				fprintf ( log_fid , '\n\t image : %s' , ...
						nodule_image_id );

				nodule_edge_map = roi.edgeMap;
				nodule_num_coord = size( nodule_edge_map , 2 );
				fprintf ( log_fid , '\n\t num coord : %d' , ...
						nodule_num_coord );

			end % for all slices

		end % for all nodules

	end % unblindedReadNodule

end % for all reading sessions
