function [ reg_data ] = get_gt_data ( GT , SUPP )

if ( isempty ( find( GT > 0 ) ) )
	reg_data = [];
	return
end

% label GT
labs = dip_label( GT , 3 , '' , 0 , 0 , '' );

% find maximum label
max_lab = max( labs );

% for all labels
for lab = 1 : max_lab

	% find pixels corresponding to current label
	idx = find( labs == lab );

	% find maximum value of current region inside GT
	max_val = max( GT( idx + 1 ) );

	% mask off pixels below region maximum
	labs( idx ) = ( GT( idx + 1 ) == max_val ) * lab;
end

% measure data
reg_data = dip_measure( labs , GT , ...
	                { 'Dimensions' , 'Size' , 'Center' , ...
		          'Minimum' , 'Maximum' } , [] , 3 );

reg_data = struct( reg_data );
if ( size( reg_data,1) == 1 )
    return;
end

reg_data_check = reg_data( find( [reg_data.Size] <= 3 ));
reg_data_ok    = reg_data( find( [reg_data.Size] > 3 ));

DMAX = 5;
n = size( reg_data_check , 1 );
i = 1;
while ( i <= n - 1 )

	reg_curr = reg_data_check(i);

    j = i + 1;
	while ( j <= n )

		reg_next = reg_data_check(j);
		d = sqrt( sum( ( reg_next.Center - reg_curr.Center ).^2 ) );

		% if distance is less than DMAX pixels
		% we are dealing with the same region
		if ( d < DMAX )
			reg_data_check = cat( 1 , ...
				          reg_data_check(1:j-1) , ...
					      reg_data_check(j+1:end));
			n = n - 1;
            continue;
        end
        j = j + 1;
    end

    i = i + 1;
end

reg_data = cat( 1 , reg_data_ok , reg_data_check );
for i = 1 : size( reg_data , 1 );
   reg_data(i).id = i;
end
