function [result_model, result_coeffs] = save_svm_data ( file_name , var_model , var_coeffs )
%save_svm_data ( file_name, var_model, var_coeffs )
%
%Saves libsvm model (var_model) and normalization coefficients (var_coeffs),
%respectively in "FILE_NAME.model" and "FILE_NAME.norm". With FILE_NAME
%being the character array passed to file_name.
%
%
%returns [results_model, results_coeffs] which gives errors codes for the
%two save operations. A value of 0 stands for correct writing.

    ERR_NUM_MISMATCH = -10;
    ERR_PARAMS = -20;
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % PARAMETER CHECKING
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %file_name
    if ~ischar(file_name)
        disp('ERROR: file_name should be a character vector');
        result_model = ERR_PARAMS;
        result_coeffs = ERR_PARAMS;
        return
    end
    
    %var_model is checked by svmsavemodel
    
    %var_coeffs
    %check if it is a structure array
    if ~isstruct(var_coeffs)
        disp('ERROR: var_coeffs should be a structure array');
        result_model = ERR_PARAMS;
        result_coeffs = ERR_PARAMS;
        return
    end
    
    if ~(exist('var_coeffs.MEAN','var') && exist('var_coeff.STD','var'))
        disp('ERROR: var_coeffs must contain .MEAN and .STD fields');
        result_model = ERR_PARAMS;
        result_coeffs = ERR_PARAMS;
        return
    end
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %%  SAVE DATA
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %save libsvm model (needs Mex Function)
    result_model = svmsavemodel( var_model, [file_name '.model'] );
    
    %save normalization coefficients
    ncoeffs = length ( var_coeffs.MEAN ) ;
    ndevs = length ( var_coeffs.STD ) ;
    
    if (ncoeffs ~= ndevs)
        disp('ERROR: Number of coefficients mismatch');
        result_coeffs = ERR_NUM_MISMATCH;
        return
    end
    
    
    fid = fopen([file_name '.norm'], 'w');
    fprintf ( fid , '%d\n' , ncoeffs );
    %print means
    for icoeff = 1:ncoeffs
        fprintf( fid ,'%d:%f ' , icoeff, var_coeffs.MEAN(icoeff));
    end
    fprintf( fid , '\n');
    %print stdev
    for icoeff = 1:ncoeffs
        fprintf( fid ,'%d:%f ' , icoeff, var_coeffs.STD(icoeff));
    end
    fprintf( fid , '\n');
    result_coeffs = fclose(fid);
    
    return
    