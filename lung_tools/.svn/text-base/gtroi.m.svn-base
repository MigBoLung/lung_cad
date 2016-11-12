function [ centroid , diams ] = gtroi ( slice , SUPP )

bw = roipoly( slice );
[lab,n] = bwlabel( bw );

figure, imshow( bw , [] );
pause;
close all;

s = regionprops( lab , 'Centroid', 'MinorAxisLength' , 'MajorAxisLength' );

min_r = s.MinorAxisLength;
max_r = s.MajorAxisLength;
centroid = round( cat( 1 , s.Centroid ) );

min_d = min_r * 2.0 * SUPP.PixelSpacing(1);
max_d = max_r * 2.0 * SUPP.PixelSpacing(1);

diams = round( [ min_d , max_d ] );

