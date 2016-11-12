close all
clear all
clc

dir_in = 'E:\TODOR\trabajo\data\maggiore\results_00_no_filter\';
dir_out = 'E:\TODOR\trabajo\data\maggiore\results_00_no_filter\merged\';
fname_left = '897741_1.3.76.8.94.92334.221103_875000_l_seg.mat';
fname_right = '897741_1.3.76.8.94.92334.221103_875000_r_seg.mat';
fname_out = '897741_1.3.76.8.94.92334.221103_875000_seg.mat';

IN = load( [dir_in,fname_left ] );
CT_LEFT = IN.CT;
BB_LEFT = IN.BB;
SUPP_ORIG = IN.SUPP_ORIG;
SUPP_SEG = IN.SUPP;

IN = load( [dir_in,fname_right ] );
CT_RIGHT = IN.CT;
BB_RIGHT = IN.BB;

clear IN;

[CT,SUPP] = merge_l_r_16u( CT_RIGHT , BB_RIGHT , CT_LEFT  , BB_LEFT , SUPP_ORIG , SUPP_SEG );

save( [ dir_out , fname_out ] , 'CT' , 'SUPP' );