clear all
close all
clc

% if PROCESS is set to 1 we do the whole processing
% if PROCESS is set to 0 we just load the results file from disk
% and display some statistics
PROCESS = 0;

% define nodule size limit ( diameter in mm )
NODULE_MIN_SIZE_LIMIT = 4; 
NODULE_MAX_SIZE_LIMIT = 11;

gt_file_dir = '/scratch/todor/check/GT/';
mat_file_dir = '/scratch/todor/check/SEG_600_1200_NO_FILTER/';
results_file = 'F:\work\campanini\docs_reports\2008_12_10_lung\check_600_1600_1_2008_12_10_11_40.mat';

if ( PROCESS == 1 ) % perform full processing

    fprintf( 1 , '\nPerforming full processing...' );
    
    tic;
    [ gt_check , err_msg , err_id ] = check_gt( mat_file_dir , gt_file_dir );
    total_time = toc;

    if ( err_id ~= 0 )

        fprintf( 1 , '\nERROR : %d' , err_id );
        fprintf( 1 , '\nERROR MSG : %s' , err_msg );
        [id,msg] = lasterr;
        fprintf( 1 , '\nERROR AS FROM LASTERR : %d : %s' , id , msg );
        return;
    end

    % save results
    now = clock;
    [ results_name ] = sprintf( '%s_%04d_%02d_%02d_%02d_%02d.mat' , ...
                                results_file , now(1) , now(2) , now(3) , now(4) , now(5) );
    
    save( results_name , 'gt_check' );
    
    fprintf( 1 , '\n-------------------------------------------------------' );
    fprintf( 1 , '\nProcessed MAT directory : %s' , mat_file_dir );
    fprintf( 1 , '\nProcessed GT directory  : %s' , gt_file_dir );
    fprintf( 1 , '\nResults saved to        : %s' , results_name );
    fprintf( 1 , '\nProcessing total time   : %f sec' , total_time );
    fprintf( 1 , '\n-------------------------------------------------------' );

else % just load results from disk
    
        fprintf( 1 , '\nLoading results from disk file : %s...\n' , results_file );
        load( results_file );

end % PROCESS == 1

% Now parse checked_nodules structure and print some statistics
num_nodules = 0;
num_isl_nodules = 0;
num_osl_nodules = 0;
num_detected = 0;
num_totally_lost = 0;
num_partially_lost = 0;
partially_lost = [];

for i = 1 : size( gt_check , 2 )    % for each patient
    
    patient = gt_check(i);
    nodules = patient.nodules;
    
    num_nodules = num_nodules + size( nodules , 2 );
    
    for j = 1 : size( nodules , 2 ) % for each patient nodule
       
        nodule = nodules(j);
        
        % check if nodule is ok for size limit
        min_nodule_diam = nodule.slices( 4 , 1 ) * patient.plane_res;
        max_nodule_diam = nodule.slices( 5 , 1 ) * patient.plane_res;
        
        if ( ( min_nodule_diam >= NODULE_MIN_SIZE_LIMIT ) && ...
             ( max_nodule_diam <= NODULE_MAX_SIZE_LIMIT )  )
            
            % fprintf( '\n%d - %d - %d' , i , patient.patient_id , nodule.id );
         
            % nodule is ok for size limit
            num_isl_nodules = num_isl_nodules + 1;
            
            totally_lost_slices     = 0;
            totally_detected_slices = 0;
            partially_lost_slices   = 0;
            
            for k = 1 : nodule.num_slices % for each nodule slice
                
                data = nodule.data(k);
                
                if ( data.overlap == 0 )
                    totally_lost_slices = totally_lost_slices + 1;
                elseif ( data.overlap < 1 )
                    partially_lost_slices = partially_lost_slices + 1;
                else
                    totally_detected_slices = totally_detected_slices + 1; 
                end

            end % for each nodule slice
            
            if ( totally_lost_slices == nodule.num_slices )
                num_totally_lost = num_totally_lost + 1;
                
                % fprintf( ' - TL\n' );
                
            end
            
            if ( totally_detected_slices == nodule.num_slices )
                num_detected = num_detected + 1;
                
                % fprintf( ' - TD\n' );
                
            end
            
            if ( ( partially_lost_slices > 0 ) || ...
                 ( totally_lost_slices > 0 ) && ...
                 ( totally_detected_slices > 0 ) )
                
                % fprintf( ' - PL\n' );
                 
                 num_partially_lost = num_partially_lost + 1;
                 
                 lost.patient_id = patient.patient_id;
                 lost.nodule_id = nodule.id;
                 lost.num_slices = nodule.num_slices;
                 lost.totally_lost = totally_lost_slices;
                 lost.partially_lost = partially_lost_slices;
                 lost.totally_detected = totally_detected_slices;
                 
                 partially_lost = cat( 1 , partially_lost , lost );
                 
            end   
            
        else

            % nodule is not ok for size limit
            num_osl_nodules = num_osl_nodules + 1;
        end
        
        
    end % for each nodule

end % for each patient

fprintf( 1 , '\nPatients in gt files : %d'  , size( gt_check , 2 ) );
fprintf( 1 , '\nNodules in gt files  : %d'  , num_nodules );
fprintf( 1 , '\nDiameter size limits (mm) : min %d , max %d' , NODULE_MIN_SIZE_LIMIT , NODULE_MAX_SIZE_LIMIT );
fprintf( 1 , '\nNodules outside size limitations : %d'  , num_osl_nodules );
fprintf( 1 , '\nNodules inside size limitations : %d'  , num_isl_nodules );
fprintf( 1 , '\nTotally detected nodules  : %d'  , num_detected );
fprintf( 1 , '\nTotally lost nodules  : %d'  , num_totally_lost );
fprintf( 1 , '\nPartially lost nodules  : %d'  , num_partially_lost );
fprintf( 1 , '\nPartially lost nodules  : \n' );

for i = 1 : size( partially_lost , 1 ) % for all partially lost nodules
   
    lost = partially_lost(i);
    
    fprintf( 1 , '\n' );
    fprintf( 1 , '\n Patient id : %d, Nodule id : %d' , lost.patient_id , lost.nodule_id );
    fprintf( 1 , '\n num slices : %d, totally lost : %d , partially lost : %d , totally_detected : %d' , ...
                 lost.num_slices , lost.totally_lost , lost.partially_lost , lost.totally_detected );
end

fprintf( 1 , '\n' );


