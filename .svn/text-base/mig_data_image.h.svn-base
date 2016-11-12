#ifndef ___MIG_IM_TYPES_H__
#define ___MIG_IM_TYPES_H__

/* pixel for linked lists */
typedef struct
{
        int     x;
        int     y;
        int     idx;
        float   val;

} mig_pix_t;

/* region of interest */
typedef struct
{
        int x0 , y0 , z0;
        int x1 , y1 , z1;
        int w , h , z;

} mig_roi_t;

/* 2d - 3d image size */
typedef struct
{
        int     w, h, slices;
        int     dim, dim_stack;
        unsigned int  size, size_stack;
        float   h_res, v_res, z_res;
        float   thickness;

} mig_size_t;

#endif /* __MIG_IM_TYPES_H__ */
