function err = put_gt_nodules( fname , nodules , SUPP )

err = 0;

% if there are no nodules
% do not write gt file
if ( isempty( nodules ) )
	return;
end

fid = fopen( fname , 'w+' );
if ( fid == -1 )
    err = -1;
    return;
end

num_nodules = size( nodules , 1 );
fprintf( fid , '%d\n' , num_nodules );

for k = 1 : num_nodules
    put_gt_nodule( fid , nodules(k) , SUPP );
end

fclose( fid );
