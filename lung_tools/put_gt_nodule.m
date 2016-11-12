function put_gt_nodule( fid , nodule , SUPP )

if ( isempty( nodule ) )
	error ('Input is empty');
end

% calculate nodule diameter
if ( nodule.Size == 1 )

    	% for nodules that are officialy < 3mm
    	% decide that actual diameter is 2mm
	% old -> diameter = 3.0 / SUPP.PixelSpacing(1);

	diameter = 2.0 / SUPP.PixelSpacing(1);
else

    	% for nodules that are >= 3mm calculate approximate diameter
    	% if approximate diameter is smaller than 3mm decide diameter
    	% is 3.5mm

	diameter = sqrt( ( nodule.Minimum(1) - nodule.Maximum(1) ).^2 + ...
                         ( nodule.Minimum(2) - nodule.Maximum(2) ).^2 );

	if ( diameter <= ( 3.0 / SUPP.PixelSpacing(1) ) )

		diameter = 3.5 / SUPP.PixelSpacing(1);

	end

end

fprintf( fid , '%d\n' , nodule.id );
fprintf( fid , '%d\n' , nodule.Dimensions(3) );

for k = nodule.Minimum(3) : nodule.Maximum(3)

	% write nodule data :
        % corrections are made for MATLAB.
        % Center coordinates + slice number
        % are shifted by 1

    fprintf( fid , '%d %d %d %4.2f %4.2f\n' , ...
             k + 1 , ...
             round(nodule.Center(1)) + 1 , ...
             round(nodule.Center(2)) + 1 , ...
             diameter , diameter );

end



