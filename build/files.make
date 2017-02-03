##############################################
# FILES
##############################################

##############################################
# GLOBAL INCLUDES
##############################################
H_GLOBAL := \
	mig_config.h \
	mig_defs.h \
	mig_data_dicom.h \
	mig_data_image.h \
	mig_data_types.h \
   mig_data_cad.h \
	mig_error_codes.h \
   mig_params_cad.h

##############################################
# LIBMIGIM library files
##############################################

H_LIBMIGIM := \
	libmig_im.h \
	mig_im_alg.h \
	mig_im_bb.h \
   mig_im_bin.h \
   mig_im_build_3d.h \
   mig_im_conv.h \
   mig_im_draw.h \
	mig_im_drv.h \
	mig_im_flt.h \
   mig_im_fradial.h \
	mig_im_gauss.h \
   mig_im_geom.h \
   mig_im_kernel.h \
	mig_im_lab.h \
   mig_im_log.h \
	mig_im_mom.h \
    mig_im_mor.h \
   mig_im_mor_vincent.h \
    mig_im_polar.h \
    mig_im_proj.h \
	mig_im_regc.h \
   mig_im_reg.h \
   mig_im_region.h \
    mig_im_scale.h \
   mig_im_sspace.h \
	mig_im_thr.h \
	mig_im_util.h 

SRC_LIBMIGIM := \
	mig_im_alg.c \
	mig_im_bb.c \
   mig_im_bin.c \
   mig_im_build_3d.c \
   mig_im_conv.c \
   mig_im_draw.c \
	mig_im_drv.c \
	mig_im_flt.c \
   mig_im_fradial.c \
	mig_im_gauss.c \
	mig_im_geom.c \
   mig_im_kernel.c \
  	mig_im_lab_3d.c \
   mig_im_lab.c \
   mig_im_log.c \
    mig_im_mom.c \
   mig_im_mor.c \
   mig_im_mor_vincent.c \
    mig_im_polar.c \
    mig_im_proj.c \
	mig_im_reg.c \
   mig_im_regc.c \
	mig_im_rot.c \
    mig_im_scale.c \
   mig_im_sspace.c \
	mig_im_thr.c \
	mig_im_util.c

OBJ_LIBMIGIM := $(SRC_LIBMIGIM:.c=.o)
DEP_LIBMIGIM := $(OBJ_LIBMIGIM:.o=.d)
LIB_LIBMIGIM := migim.a

##############################################
# LIBMIGIO library files
##############################################

H_LIBMIGIO = \
	libmigio.h \
	mig_io_dcm.h \
	mig_io_mat.h \
	mig_io_tif.h

SRC_LIBMIGIO_C = \
   mig_io_mat.c \
	mig_io_tif.c 
SRC_LIBMIGIO_CPP = mig_io_dcm.cpp

OBJ_LIBMIGIO_C := $(SRC_LIBMIGIO_C:.c=.o)
OBJ_LIBMIGIO_CPP := $(SRC_LIBMIGIO_CPP:.cpp=.o)
OBJ_LIBMIGIO = $(OBJ_LIBMIGIO_C) $(OBJ_LIBMIGIO_CPP)
DEP_LIBMIGIO := $(OBJ_LIBMIGIO:.o=.d)
LIB_LIBMIGIO := migio.a

##############################################
# LIBMIGMATH library files
##############################################

H_LIBMIGMATH = \
	libmigmath.h \
	mig_math_fit.h

SRC_LIBMIGMATH = \
   mig_math_fit.c

OBJ_LIBMIGMATH := $(SRC_LIBMMATH:.c=.o)
OBJ_LIBMIGMATH := $(OBJ_LIBMMATH:.cpp=.o)
DEP_LIBMIGMATH := $(OBJ_LIBMMATH:.o=.d)
LIB_LIBMIGMATH := migmath.a

##############################################
# LIBMIGST library files
##############################################

H_LIBMIGST = \
	libmig_st.h \
	mig_st_hsh.h \
	mig_st_lst.h \
   mig_st_queue.h \
   mig_st_stack.h

SRC_LIBMIGST = \
	mig_st_hsh.c \
	mig_st_lst.c \
   mig_st_queue.c \
   mig_st_stack.c

OBJ_LIBMIGST := $(SRC_LIBMIGST:.c=.o)
DEP_LIBMIGST := $(OBJ_LIBMIGST:.o=.d)
LIB_LIBMIGST = migst.a

##############################################
# LIBMIGUT library files
##############################################

H_LIBMIGUT := \
	libmig_ut.h \
   mig_ut_bit.h \
   mig_ut_cpu.h \
	mig_ut_dll.h \
	mig_ut_fs.h \
	mig_ut_ini.h \
	mig_ut_lock.h \
	mig_ut_mem.h \
	mig_ut_str.h \
	mig_ut_time.h

SRC_LIBMIGUT := \
	mig_ut_bit.c \
   mig_ut_cpu.c \
   mig_ut_dll.c \
	mig_ut_fs.c \
	mig_ut_ini.c \
	mig_ut_lock.c \
	mig_ut_mem.c \
	mig_ut_str.c \
	mig_ut_time.c
 
OBJ_LIBMIGUT := $(SRC_LIBMIGUT:.c=.o) mig_cpuid.o
DEP_LIBMIGUT := $(OBJ_LIBMIGUT:.o=.d)
LIB_LIBMIGUT := migut.a

##############################################
# LIBMIGDB library files
##############################################
H_LIBMIGDB := libmigdb.h
SRC_LIBMIGDB := mig_db.cpp
OBJ_LIBMIGDB := $(SRC_LIBMIGDB:.cpp=.o)
DEP_LIBMIGDB := $(OBJ_LIBMIGDB:.o=.d)
LIB_LIBMIGDB := migdb.a

##############################################
# LIBMIGTAG library files
##############################################
H_LIBMIGTAG := libmigtag.h
SRC_LIBMIGTAG := mig_tag.c
OBJ_LIBMIGTAG := $(SRC_LIBMIGTAG:.c=.o)
DEP_LIBMIGTAG := $(OBJ_LIBMIGTAG:.o=.d)
LIB_LIBMIGTAG := migtag.a

##############################################
# LIBMIGDICOM library files
##############################################

H_LIBMIGDICOM := libmigdicom.h
SRC_LIBMIGDICOM := libmigdicom.cpp
OBJ_LIBMIGDICOM := $(SRC_LIBMIGDICOM:.cpp=.o)
DEP_LIBMIGDICOM := $(OBJ_LIBMIGDICOM:.o=.d)
LIB_LIBMIGDICOM = migdicom.so

##############################################
# LIBMIGWHITENING library files
##############################################

H_LIBMIGWHITENING := libmigwhitening.h \
                        eigenwhitener.h
SRC_LIBMIGWHITENING := libmigwhitening.cpp \
                        eigenwhitener.cpp
OBJ_LIBMIGWHITENING := $(SRC_LIBMIGWHITENING:.cpp=.o)
DEP_LIBMIGWHITENING := $(OBJ_LIBMIGWHITENING:.o=.d)
LIB_LIBMIGWHITENING = migwhitening.a

##############################################
# LIBMIGFEAT library files
##############################################

H_LIBMIGFEAT := libmig_feat_zernike_mip.h
SRC_LIBMIGFEAT := libmig_feat_zernike_mip.c
OBJ_LIBMIGFEAT := $(SRC_LIBMIGFEAT:.c=.o)
DEP_LIBMIGFEAT := $(OBJ_LIBMIGFEAT:.o=.d)
LIB_LIBMIGFEAT = migfeat.a

##############################################
# LIBSVM library files
##############################################

H_LIBMIGSVM := libmigsvm.h \
               svm.h
SRC_LIBMIGSVM_C := libmigsvm.c
SRC_LIBMIGSVM_CPP := svm.cpp
OBJ_LIBMIGSVM_C := $(SRC_LIBMIGSVM_C:.c=.o)
OBJ_LIBMIGSVM_CPP := $(SRC_LIBMIGSVM_CPP:.cpp=.o)
OBJ_LIBMIGSVM := $(OBJ_LIBMIGSVM_C) $(OBJ_LIBMIGSVM_CPP)
DEP_LIBMIGSVM := $(OBJ_LIBMIGSVM:.o=.d)
LIB_LIBMIGSVM = migsvm.a


##############################################
# LIBMIGSEG library files
##############################################

H_LIBMIGSEG := \
	libmigseg.h \
	mig_seg_close.h \
   mig_seg_params.h \
   mig_seg_sep.h

SRC_LIBMIGSEG := \
   mig_seg.cpp \
   mig_seg_close.cpp \
   mig_seg_sep.c

OBJ_LIBMIGSEG := $(SRC_LIBMIGSEG:.c=.o)
OBJ_LIBMIGSEG := $(OBJ_LIBMIGSEG:.cpp=.o)
DEP_LIBMIGSEG := $(OBJ_LIBMIGSEG:.o=.d)
LIB_LIBMIGSEG = migseg.so

##############################################
# LIBMIGDET_2D library files
##############################################

H_LIBMIGDET_2D := \
	libmigdet_2d.h

SRC_LIBMIGDET_2D := \
   libmigdet_2d.cpp

OBJ_LIBMIGDET_2D := $(SRC_LIBMIGDET_2D:.c=.o)
OBJ_LIBMIGDET_2D := $(OBJ_LIBMIGDET_2D:.cpp=.o)
DEP_LIBMIGDET_2D := $(OBJ_LIBMIGDET_2D:.o=.d)
LIB_LIBMIGDET_2D = migdet_2d.so

##############################################
# LIBMIGDET_3D library files
##############################################

H_LIBMIGDET_3D := \
	libmigdet_3d.h

SRC_LIBMIGDET_3D := \
   libmigdet_3d.cpp

OBJ_LIBMIGDET_3D := $(SRC_LIBMIGDET_3D:.c=.o)
OBJ_LIBMIGDET_3D := $(OBJ_LIBMIGDET_3D:.cpp=.o)
DEP_LIBMIGDET_3D := $(OBJ_LIBMIGDET_3D:.o=.d)
LIB_LIBMIGDET_3D = migdet_3d.so

##############################################
# LIBMIGFPR_1 library files
##############################################

H_LIBMIGFPR_1 := libmigfpr_1.h
SRC_LIBMIGFPR_1 := libmigfpr_1.cpp

OBJ_LIBMIGFPR_1 := $(SRC_LIBMIGFPR_1:.c=.o)
OBJ_LIBMIGFPR_1 := $(OBJ_LIBMIGFPR_1:.cpp=.o)
DEP_LIBMIGFPR_1 := $(OBJ_LIBMIGFPR_1:.o=.d)
LIB_LIBMIGFPR_1 = migfpr_1.so

##############################################
# LIBMIGFPR_2 library files
##############################################

H_LIBMIGFPR_2 := libmigfpr_2_zernike_mip.h
SRC_LIBMIGFPR_2 := libmigfpr_2_zernike_mip.cpp

OBJ_LIBMIGFPR_2 := $(SRC_LIBMIGFPR_2:.c=.o)
OBJ_LIBMIGFPR_2 := $(OBJ_LIBMIGFPR_2:.cpp=.o)
DEP_LIBMIGFPR_2 := $(OBJ_LIBMIGFPR_2:.o=.d)
LIB_LIBMIGFPR_2 = migfpr_2_zernike_mip.so


##############################################
# CAD files
##############################################

H_CAD := \
	mig_cad.h
	
SRC_CAD := \
	mig_cad.cpp \
	mig_cad_start.cpp
	
OBJ_CAD := $(SRC_CAD:.c=.o)
OBJ_CAD := $(OBJ_CAD:.cpp=.o)
DEP_CAD := $(OBJ_CAD:.o=.d)
EXE_CAD := lungcad

##############################################
# SCP files
##############################################
H_SCP := \
	mig_scp_error_codes.h \
	mig_scp_param_keys.h \
	mig_scp_store.h

SRC_SCP := \
	mig_scp_server.cpp \
	mig_scp_store.cpp

OBJ_SCP := $(SRC_SCP:.c=.o)
OBJ_SCP := $(OBJ_SCP:.cpp=.o)
DEP_SCP := $(OBJ_SCP:.o=.d)
EXE_SCP := lungscp

##############################################
# LUNG TOOLS
##############################################
SRC_DB_INSERT := db_insert.cpp
OBJ_DB_INSERT := $(SRC_DB_INSERT:.cpp=.o)
DEP_DB_INSERT := $(OBJ_DB_INSERT:.o=.d)
EXE_DB_INSERT := insert

SRC_CONVERTER := converter.cpp
OBJ_CONVERTER := $(SRC_CONVERTER:.cpp=.o)
DEP_CONVERTER := $(OBJ_CONVERTER:.o=.d)
EXE_CONVERTER := converter

SRC_RESIZE := resizez.cpp
OBJ_RESIZE := $(SRC_RESIZE:.cpp=.o)
DEP_RESIZE := $(OBJ_RESIZE:.o=.d)
EXE_RESIZE := resize

SRC_MARK := mark.cpp
OBJ_MARK := $(SRC_MARK:.cpp=.o)
DEP_MARK := $(OBJ_MARK:.o=.d)
EXE_MARK := mark

SRC_TRAINING := training.cpp
OBJ_TRAINING := $(SRC_TRAINING:.cpp=.o)
DEP_TRAINING := $(OBJ_TRAINING:.o=.d)
EXE_TRAINING := training

