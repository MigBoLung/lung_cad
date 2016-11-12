#ifndef __MIG_IM_MOR_VINCENT_H__
#define __MIG_IM_MOR_VINCENT_H__

#include "mig_config.h"
#include "mig_defs.h"

MIG_C_LINKAGE_START

#define CONN 8

typedef struct __pix_t
{
    int x , y;
    unsigned char *ptr;

} pix_t;

typedef struct _loop_t
{
    int len;
    pix_t p0;
    unsigned char *dir;
    int dir_len;
    struct _loop_t *next;

} loop_t;

typedef struct _loop_list_t
{
    int len;
    int w;
    loop_t *head;

} loop_list_t;

typedef struct _sel_t
{
    int num_pix;
    pix_t o;
    pix_t *pix;
    int num_dir[CONN];
    pix_t *dir[CONN];

} sel_t;

int
encode_sel ( unsigned char *im , int w , int h , sel_t *out );

int
encode_disk ( int r , sel_t *out );

void
free_sel ( sel_t *sel );

void
dump_sel ( sel_t *sel );

int
mdilate ( unsigned char *im , int w , int h , sel_t *sel );

int
merode ( unsigned char *im , int w , int h , sel_t *sel );

int
mopen ( unsigned char *im , int w , int h , sel_t *sel );

int
mclose ( unsigned char *im , int w , int h , sel_t *sel );

MIG_C_LINKAGE_END

#endif /* __MIG_IM_MOR_VINCENT_H__ */
