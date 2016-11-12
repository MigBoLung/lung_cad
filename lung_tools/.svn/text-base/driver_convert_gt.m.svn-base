close all
clear all
clc

dip_initialise

% input directory
dir_in = '/scratch/todor/gt_bin/';

% output directory
dir_out = '/scratch/todor/gt_txt/';

% read input directory
[ names , err ] = sprintf( '%s*.mat' , dir_in );
dir_mat = dir( names );
dir_mat_size = size( dir_mat , 1 );

if ( dir_mat_size <= 0 )
        error ( 'No .mat files found...' );
end

% run through all file found in directory
for i = 1 : dir_mat_size

    % get filename
    name = dir_mat( i ).name;
    [ name_mat , err ] = sprintf( '%s%s' , dir_in , name );

    base = name( 1 : strfind( name , '.mat' ) - 1 );
    [ name_txt , err ] = sprintf( '%s%s.txt' , dir_out , base(1:end-3) );

    fprintf( 1 , '\n%s' , base );

    tmp = load( name_mat );
    GT = tmp.GT;
    SUPP = tmp.SUPP;
    clear tmp;

    reg_data = get_gt_data ( GT , SUPP );
    err = put_gt_nodules( name_txt , reg_data , SUPP );
    if ( err )
        error ( 'Erroro in put_gt_nodules...' );
    end

end
