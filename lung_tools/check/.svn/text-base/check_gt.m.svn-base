function [ checked_nodules , err_msg , err_id ] = check_gt( mat_dir_name , gt_dir_name )

% clear error status
err_msg = [];
err_id = 0;

global MASKS_L;
global MASKS_R;
global BB_L;
global BB_R;
global SUPP;

% clear output
checked_nodules = [];

% file extensions
gt_file_ext  = '.txt';
mat_file_ext = '.mat';

% check input parameters
if ( isempty( mat_dir_name ) | isempty( gt_dir_name ) )
    err_msg = 'ERROR. Empty input...';
    err_id = -1;
    return;
end

% parse gt directory
parse_dir = fullfile( gt_dir_name , ['*' gt_file_ext ] );
gt_dir = dir( parse_dir );
gt_dir_size = size( gt_dir , 1 );
if ( gt_dir_size == 0 )
    err_msg = ['ERROR. Gt directory ' gt_dir_name ' empty...'];
    err_id = -2;
    return;
end

% loop through gt directory
for i = 1 : gt_dir_size
    
    gt_file_name = gt_dir(i).name;
    [ pathstr , fname , ext , ver ] = fileparts(gt_file_name);
    patient_id = str2num( fname );
    
    % load ground truth file for patient patient_id
    [ patient_nodules , err_id , err_msg ] = load_gt( fullfile( gt_dir_name , gt_file_name ) , patient_id );
    if ( err_id ~= 0 )
        return;
    end
    
    % try parsing mat file directory to get mat files corresponding
    % to patient's gt file
    parse_dir = fullfile( mat_dir_name , [ fname '_*_*.mat' ] );
    mat_dir = dir( parse_dir );
    mat_dir_size = size( mat_dir , 1 );
    
    if ( ( mat_dir_size == 0 ) || ...
         ( mat_dir_size == 1 ) || ...
         ( mat_dir_size > 2 ) )
        
        err_msg = ['ERROR. Mat files directory ' mat_dir_name ' does not contain files for patient ' fname '...'];
        err_id = -3;
        return;
        
    else
        
        % if there are two file suppose these are for separate left/right lung
        
        % left lung mat file
        mat_file_name = fullfile( mat_dir_name , mat_dir(1).name );
        try 
            
            data = load( mat_file_name );
            
            % first get data
            if ( isfield( data , 'CT' ) )
                MASKS_L = data.CT > 0;
            elseif ( isfield( data , 'MASKS' ) )
                MASKS_L = data.MASKS;
            else
                err_msg = 'ERROR. Mat file should contain CT or MASKS variables...';
                err_id = -4;
                return;
            end
                
            % get BB
            if ( isfield( data , 'BB' ) )
                BB_L = data.BB;
            else
                err_msg = 'ERROR. Mat file should contain BB variable...';
                err_id = -4;
                return;
            end
            
            % get SUPP - loaded just once
            if ( isfield( data , 'SUPP' ) )
                SUPP = data.SUPP;
            else
                err_msg = 'ERROR. Mat file should contain SUPP variable...';
                err_id = -4;
                return;
            end

        catch
       
            err_msg = ['ERROR. Could not load mat file ' mat_file_name '...'];
            err_id = -5;
            return;
        
        end
        
        clear data;
        
        % right lung mat file
        mat_file_name = fullfile( mat_dir_name , mat_dir(2).name );
        try 
            
            data = load( mat_file_name );
            
            % first get data
            if ( isfield( data , 'CT' ) )
                MASKS_R = data.CT > 0;
            elseif ( isfield( data , 'MASKS' ) )
                MASKS_R = data.MASKS;
            else
                err_msg = 'ERROR. Mat file should contain CT or MASKS variables...';
                err_id = -4;
                return;
            end
                
            % get BB
            if ( isfield( data , 'BB' ) )
                BB_R = data.BB;
            else
                err_msg = 'ERROR. Mat file should contain BB variable...';
                err_id = -4;
                return;
            end

        catch
       
            err_msg = ['ERROR. Could not load mat file ' mat_file_name '...'];
            err_id = -5;
            return;
        
        end
        
        clear data;

    end
    
    % by this time we should have all gt data for current patient loaded
    % into structure patient_nodules.
    % Also global variables MASKS_L, BB_L, MASKS_R, BB_R have been loaded.
    fprintf( 1 , '\nLoaded GT and MAT files for patient : %d' ,  str2num( SUPP.PatientID ) );
    
    % Now perform check    
    [ nodules , err_msg , err_id ] = check_patient_gt( patient_nodules );
    
    % save in-plane resolution, z thickness and z spacing for current
    % patient
    
    patient_checked_nodules.patient_id  = str2num( SUPP.PatientID );
    patient_checked_nodules.plane_res   = SUPP.PixelSpacing(1);
    patient_checked_nodules.z_res       = SUPP.SliceSpacing;
    patient_checked_nodules.z_thickness = SUPP.SliceThickness;
    patient_checked_nodules.nodules     = nodules;
    
    % clear all unused data
    clear MASKS_L BB_L MASKS_R BB_R SUPP
    if ( err_id ~= 0 )
        
        % if error abort
        return;
        
    end
    
    % save this patient results to list
    checked_nodules = cat( 2 , checked_nodules , patient_checked_nodules );
        
end % end i loop - gt_dir entries
