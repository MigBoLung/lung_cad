#include "mig_ut_ini.h"
#include "mig_ut_str.h"

/***************************************************************************/
/* private functions */

static void mig_ut_ini_add_entry ( mig_dic_t *d , char *sec ,
                                   char *key , char *val );

static char* strlwc   ( char *s );
static char* strupc   ( char *s );
static char* strskp   ( char *s );
static char* strcrop  ( char *s );
static char* strstrip ( char * s );

static unsigned   dictionary_hash  ( char *key );
static mig_dic_t* dictionary_new   ( int size );
static void       dictionary_del   ( mig_dic_t *d );
static char*      dictionary_get   ( mig_dic_t *d , char *key , char *def );
static void	  dictionary_set   ( mig_dic_t *d , char *key , char *val );
static void	  dictionary_unset ( mig_dic_t *d , char *key );
static void	  dictionary_dump  ( mig_dic_t *d , FILE *f );

/** Maximum value size for integers and doubles. */
#define MAXVALSZ    1024

/** Minimal allocated number of entries in a dictionary */
#define DICTMINSZ   128

#define ASCIILINESZ         1024

#define INI_INVALID_KEY     ((char*)-1)

/***************************************************************************/
/* exports */

int
mig_ut_ini_getnsec ( mig_dic_t *d )
{
        int i;
        int nsec = 0;

        if ( d == NULL )
                return -1;

        for ( i = 0 ; i < d->size ; ++i )
        {
                if ( d->key[i] == NULL )
                        continue;

                if ( strchr( d->key[i] , ':' ) == NULL )
                        ++nsec;
        }

        return nsec;
}

char*
mig_ut_ini_getsecname ( mig_dic_t *d , int n )
{
        int i;
        int foundsec = 0;

        if ( d == NULL || n < 0 )
                return NULL;

        for ( i = 0 ; i < d->size ; ++i )
        {
                if ( d->key[i] == NULL )
                        continue;

                if ( strchr( d->key[i] , ':' ) == NULL )
                {
                        ++foundsec;
                        if ( foundsec > n )
                                break;
                }
        }

        if ( foundsec <= n )
        {
                return NULL;
        }

        return d->key[i];
}

void
mig_ut_ini_dump ( mig_dic_t *d , FILE *f )
{
        dictionary_dump ( d , f );
}

#define TMP_BUFF_SIZE   128

#if defined(WIN32)
# if !defined(snprintf)
#  define snprintf _snprintf
# endif
#endif

int
mig_ut_ini_dump_buffer ( mig_dic_t *d , char *buffer , int len )
{
        int i , j , curr_len = 0 , tot_len = 0;

        static char* fmt = "[%s]=[%s]\n";
        static char tmp[TMP_BUFF_SIZE];

        if ( ( d == NULL ) ||
             ( buffer == NULL ) ||
             ( len <= 0 ) )
                return -1;

        for ( i = 0 ; i < d->size ; ++i )
        {
                if ( d->key[i] == NULL )
                        continue ;

                if ( d->val[i] != NULL )
                {
                        snprintf ( tmp , TMP_BUFF_SIZE , fmt , d->key[i] , d->val[i] );
                }
                else
                {
                        continue;
                }

                curr_len = strlen ( tmp );
                tot_len += curr_len;

                if ( tot_len >= ( len - 1 ) )
                {
                        *buffer = '\0';
                        return -1;
                }

                j = 0;
                while ( j < curr_len )
                {
                        *buffer = *( tmp + j );
                        ++buffer;
                        ++j;
                }
        }

        *buffer = '\0';

    return 0;
}

#undef TMP_BUFF_SIZE

void
mig_ut_ini_dump_ini ( mig_dic_t *d , FILE *f )
{
        int i , j;
        char keym[ASCIILINESZ+1];
        int nsec;
        char *secname;
        int seclen;

        if ( d == NULL ||
             f == NULL )
                return;

        nsec = mig_ut_ini_getnsec ( d );
        if ( nsec < 1 )
        {
                /* No section in file: dump all keys as they are */
                for ( i = 0 ; i < d->size ; ++i )
                {
                        if ( d->key[i] == NULL )
                                continue;

                        fprintf ( f , "%s = %s\n" ,
                                d->key[i] , d->val[i] );
                }
                return ;
        }

        for ( i = 0 ; i < nsec ; ++i )
        {
                secname = mig_ut_ini_getsecname( d , i );
                seclen = (int) strlen( secname );

                fprintf ( f , "\n[%s]\n", secname );
                sprintf ( keym , "%s:" , secname );

                for ( j = 0 ; j < d->size ; ++j )
                {
                        if ( d->key[j] == NULL )
                                continue;

                        if ( !strncmp( d->key[j] ,
                              keym , seclen +1 ) )
                        {
                                fprintf ( f , "%-30s = %s\n" ,
                                        d->key[j]+seclen+1 ,
                                        d->val[j] ? d->val[j] : "" );
                        }
                }
        }

        fprintf ( f , "\n" );
        return;
}

char*
mig_ut_ini_getstr ( mig_dic_t *d , char *key )
{
    return mig_ut_ini_getstring ( d , key , NULL );
}


char*
mig_ut_ini_getstring ( mig_dic_t *d , char *key , char *def )
{
        char *lc_key;
        char *sval;

        if ( d == NULL || key == NULL )
                return def;

        lc_key = strdup( strlwc ( key ) );
        sval = dictionary_get( d , lc_key , def );
        free ( lc_key );
        return sval;
}

#define STORAGE_START_LEN       3

#if defined(WIN32)
#define mig_strdup      _strdup
#define mig_strtok      strtok_s
#else
#define mig_strdup      strdup
#define mig_strtok      strtok_r
#endif

int
mig_ut_ini_getint ( mig_dic_t *d , char *key , int notfound )
{
        char *str =
                mig_ut_ini_getstring( d , key , INI_INVALID_KEY );
        if ( str == INI_INVALID_KEY )
                return notfound;
        return ((int)atoi(str));
}


int*
mig_ut_ini_getintarray ( mig_dic_t *d , char *key , int *len )
{
	int i , StorageSize , StorageElements;
        int  *Storage;
        char *CurrentToken , *NextToken;
        char *InputString =
                mig_ut_ini_getstring ( d , key , INI_INVALID_KEY );

	if ( InputString == INI_INVALID_KEY )
                return NULL;

        /* allocate memory to store parsed integers */
        Storage = (int*)
                malloc ( STORAGE_START_LEN * sizeof( int ) );
        if ( Storage == NULL )
                return NULL;
        StorageSize = STORAGE_START_LEN;

        for ( CurrentToken =
        	mig_strtok( InputString , " []{},"  , &NextToken ) , i = 0;
              CurrentToken != NULL ;
              CurrentToken =
              	mig_strtok( NULL , " []{}," , &NextToken ) , ++i )
        {
                if ( i >= StorageSize )
                {
                        Storage = (int*)
                                realloc ( Storage ,
				( StorageSize << 1 ) * sizeof( int ) );
                        if ( Storage == NULL )
                                return NULL;
                        StorageSize <<= 1;
                }

                if ( mig_ut_str_isdec ( CurrentToken ) )
                {
                        Storage[i] = (int)
                        	strtol( CurrentToken , NULL , 10 );
                }
                else
                {
                	free ( Storage );
                        return NULL;
                }
        }

        *len = StorageElements = i;
	return Storage;
}

float
mig_ut_ini_getfloat ( mig_dic_t *d , char *key , float notfound )
{
        char *str =
                mig_ut_ini_getstring( d , key , INI_INVALID_KEY );
        if ( str == INI_INVALID_KEY )
                return notfound;
        return ((float)atof(str));
}

float*
mig_ut_ini_getfloatarray ( mig_dic_t *d , char *key , int *len )
{
	int i , StorageSize , StorageElements;
        float  *Storage;
        char *CurrentToken , *NextToken;
        char *InputString =
                mig_ut_ini_getstring ( d , key , INI_INVALID_KEY );

	if ( InputString == INI_INVALID_KEY )
                return NULL;

        /* allocate memory to store parsed integers */
        Storage = (float*)
                malloc ( STORAGE_START_LEN * sizeof( float ) );
        if ( Storage == NULL )
                return NULL;
        StorageSize = STORAGE_START_LEN;

        for ( CurrentToken =
        	mig_strtok( InputString , "[]{},"  , &NextToken ) , i = 0;
              CurrentToken != NULL ;
              CurrentToken =
              	mig_strtok( NULL , "[]{}," , &NextToken ) , ++i )
        {
                if ( i >= StorageSize )
                {
                        Storage = (float*)
                                realloc ( Storage ,
				( StorageSize << 1 ) * sizeof( float ) );
                        if ( Storage == NULL )
                                return NULL;
                        StorageSize <<= 1;
                }

                //if ( mig_ut_str_isfloat ( CurrentToken ) )
                //{
                        Storage[i] = (float)
                        	strtod( CurrentToken , NULL );
                //}
                //else
                //{
                //	free ( Storage );
                 //       return NULL;
                //}
        }

        *len = StorageElements = i;
	return Storage;
}

double
mig_ut_ini_getdouble ( mig_dic_t *d , char *key , double notfound )
{
        char *str =
                mig_ut_ini_getstring( d , key , INI_INVALID_KEY );
    if ( str == INI_INVALID_KEY )
            return notfound;
    return ((double)atof(str));
}

double*
mig_ut_ini_getdoublearray ( mig_dic_t *d , char *key , int *len )
{
	int i , StorageSize , StorageElements;
        double  *Storage;
        char *CurrentToken , *NextToken;
        char *InputString =
                mig_ut_ini_getstring ( d , key , INI_INVALID_KEY );

	if ( InputString == INI_INVALID_KEY )
                return NULL;

        /* allocate memory to store parsed integers */
        Storage = (double*)
                malloc ( STORAGE_START_LEN * sizeof( int ) );
        if ( Storage == NULL )
                return NULL;
        StorageSize = STORAGE_START_LEN;

        for ( CurrentToken =
        	mig_strtok( InputString , "[]{},"  , &NextToken ) , i = 0;
              CurrentToken != NULL ;
              CurrentToken =
              	mig_strtok( NULL , "[]{}," , &NextToken ) , ++i )
        {
                if ( i >= StorageSize )
                {
                        Storage = (double*)
                                realloc ( Storage ,
				( StorageSize << 1 ) * sizeof( double ) );
                        if ( Storage == NULL )
                                return NULL;
                        StorageSize <<= 1;
                }

                if ( mig_ut_str_isdec ( CurrentToken ) )
                {
                        Storage[i] =
                        	strtod( CurrentToken , NULL );
                }
                else
                {
                	free ( Storage );
                        return NULL;
                }
        }

        *len = StorageElements = i;
	return Storage;
}

int
mig_ut_ini_getboolean ( mig_dic_t *d , char *key , int notfound )
{
        char *c;
        int ret;

        c = mig_ut_ini_getstring ( d , key , INI_INVALID_KEY );
        if ( c == INI_INVALID_KEY )
                return notfound;

        if ( c[0] == 'y' ||
             c[0] == 'Y' ||
             c[0] == '1' ||
             c[0] == 't' ||
             c[0] == 'T' )
        {
                ret = 1;
        }
        else if ( c[0] == 'n' ||
                  c[0] == 'N' ||
                  c[0] == '0' ||
                  c[0] == 'f' ||
                  c[0] == 'F' )
        {
                ret = 0;
        }
        else
        {
                ret = notfound;
        }
        return ret;
}

int
mig_ut_ini_find_entry ( mig_dic_t *d , char *entry )
{
        int found = 0;

        if ( mig_ut_ini_getstring ( d ,
                entry , INI_INVALID_KEY ) != INI_INVALID_KEY )
                found = 1;

        return found;
}

int
mig_ut_ini_setstr ( mig_dic_t *d , char *entry , char *val )
{
        dictionary_set ( d , strlwc ( entry ) , val );
        return 0;
}

void
mig_ut_ini_unset ( mig_dic_t *d , char *entry )
{
        dictionary_unset ( d , strlwc ( entry ) );
}

mig_dic_t*
mig_ut_ini_new ( char *ininame )
{
        mig_dic_t *d;
        char lin[ASCIILINESZ+1];
        char sec[ASCIILINESZ+1];
        char key[ASCIILINESZ+1];
        char val[ASCIILINESZ+1];
        char *where;
        FILE *ini;
        int lineno = 0;

        if ( ( ini = fopen ( ininame , "r" ) ) == NULL )
                return NULL;

        sec[0] = 0;

        /* Initialize a new dictionary entry */
        d = dictionary_new ( 0 );
        while ( fgets( lin , ASCIILINESZ , ini ) != NULL )
        {
                ++lineno;
                where = strskp ( lin );    /* Skip leading spaces */

                if ( *where == ';' ||
                     *where == '#' ||
                     *where == 0 )
                        continue;       /* Comment lines */
                else
                {
                        if ( sscanf ( where , "[%[^]]" , sec ) == 1 )
                        {
                                /* Valid section name */
                                strcpy ( sec , strlwc ( sec ) );
                                mig_ut_ini_add_entry ( d , sec ,  NULL , NULL );
                        }
                        else if ( sscanf ( where , "%[^=] = \"%[^\"]\"" , key , val ) == 2 ||
                                  sscanf ( where , "%[^=] = '%[^\']'"   , key , val ) == 2 ||
                                  sscanf ( where , "%[^=] = %[^;#]"     , key , val ) == 2 )
                        {
                                strcpy ( key , strlwc ( strcrop ( key ) ) );

                                /* sscanf cannot handle "" or '' as empty value,
                                   this is done here */
                                if ( !strcmp ( val , "\"\"" ) ||
                                     !strcmp ( val , "''" ) )
                                {
                                        val[0] = (char)0;
                                }
                                else
                                {
                                        strcpy ( val , strcrop ( val ) );
                                }

                                mig_ut_ini_add_entry ( d , sec , key , val );
                        }
                }
        }

        fclose ( ini );
        return d;
}

void
mig_ut_ini_free ( mig_dic_t *d )
{
        dictionary_del ( d );
}

/*****************************************************************************/
/* PRIVATE FUNCTIONS */

static void
mig_ut_ini_add_entry ( mig_dic_t *d , char *sec ,
                       char *key , char *val )
{
        char longkey[2*ASCIILINESZ+1];

        /* Make a key as section:keyword */
        if ( key != NULL )
        {
                sprintf ( longkey , "%s:%s", sec , key );
        }
        else
        {
                strcpy( longkey , sec );
        }

        /* Add (key,val) to dictionary */
        dictionary_set ( d , longkey , val );
        return;
}

/* string */

static char*
strlwc ( char *s )
{
        static char l[ASCIILINESZ+1];
        int i = 0;

        if ( s == NULL )
                return NULL;

        memset ( l , 0 , ASCIILINESZ+1 );

        while ( s[i] &&
                i < ASCIILINESZ )
        {
                l[i] = (char)tolower((int)s[i]);
                ++i;
        }

        l[ASCIILINESZ] = (char)0;
        return l;
}

static char*
strupc ( char *s )
{
        static char l[ASCIILINESZ+1];
        int i = 0;

        if ( s == NULL )
                return NULL;

        memset ( l , 0 , ASCIILINESZ+1 );

        while ( s[i] &&
                i < ASCIILINESZ )
        {
                l[i] = (char)toupper((int)s[i]);
                ++i;
        }

        l[ASCIILINESZ] = (char)0;
        return l;
}

static char*
strskp ( char *s )
{
        char *skip = s;

        if ( s == NULL )
                return NULL;

        while ( isspace( (int)*skip ) && *skip )
                ++skip;
        return skip;
}

static char*
strcrop ( char *s )
{
        static char l[ASCIILINESZ+1];
        char *last;

        if ( s == NULL )
                return NULL;

        memset ( l , 0 , ASCIILINESZ+1 );
        strcpy ( l , s );

        last = l + strlen( l );
        while ( last > l )
        {
                if ( !isspace((int)*(last-1)) )
                break;
                --last;
        }

        *last = (char)0;
        return l;
}

static char*
strstrip ( char * s )
{
        static char l[ASCIILINESZ+1];
        char *last;

        if ( s == NULL )
                return NULL;

        while ( isspace((int)*s) && *s )
                ++s;

        memset ( l , 0 , ASCIILINESZ+1 );
        strcpy( l , s );
        last = l + strlen(l);

        while ( last > l )
        {
                if ( !isspace((int)*(last-1)) )
                        break;
                --last;
        }

        *last = (char)0;
        return (char*)l;
}

static unsigned
dictionary_hash ( char *key )
{
        int len ;
        unsigned hash;
        int i;

        len = strlen(key);

        for ( hash = 0 , i = 0 ; i < len ; ++i )
        {
                hash += (unsigned)key[i];
                hash += (hash<<10);
                hash ^= (hash>>6);
        }

        hash += (hash <<3);
        hash ^= (hash >>11);
        hash += (hash <<15);
        return hash;
}

static mig_dic_t*
dictionary_new ( int size )
{
        mig_dic_t *d;

        /* If no size was specified ,
           allocate space for DICTMINSZ */
        if ( size < DICTMINSZ )
                size = DICTMINSZ;

        d = (mig_dic_t*)
                calloc ( 1 , sizeof(mig_dic_t) );
        d->size = size;
        d->val  = (char **)calloc ( size , sizeof(char*) );
        d->key  = (char **)calloc ( size , sizeof(char*) );
        d->hash = (unsigned int*) calloc ( size , sizeof(unsigned) );

        return d;
}

static void
dictionary_del ( mig_dic_t *d )
{
        int i;

        if ( d == NULL )
                return;

        for ( i = 0 ; i < d->size ; ++i )
        {
                if ( d->key[i] != NULL )
                        free ( d->key[i] );
                if ( d->val[i] != NULL )
                        free ( d->val[i] );
        }

        free(d->val);
        free(d->key);
        free(d->hash);
        free(d);

        return;
}

static char*
dictionary_get ( mig_dic_t *d , char *key , char *def )
{
        unsigned hash;
        int i;

        hash = dictionary_hash( key );
        for ( i = 0 ; i < d->size ; ++i )
        {
                if ( d->key == NULL )
                        continue;

                /* Compare hash */
                if ( hash == d->hash[i] )
                {
                        /* Compare string,
                           to avoid hash collisions */
                        if ( !strcmp( key , d->key[i] ) )
                                return d->val[i];
                }
        }

        return def;
}

static void
dictionary_set ( mig_dic_t *d , char *key , char *val )
{
        int i;
        unsigned hash;

        if ( d == NULL ||
             key == NULL )
                return;

        /* Compute hash for this key */
        hash = dictionary_hash( key ) ;

        /* Find if value is already in blackboard */
        if ( d->n > 0 )
        {
                for ( i = 0 ; i < d->size ; ++i )
                {
                        if ( d->key[i] == NULL )
                                continue;

                        if ( hash == d->hash[i] )
                        { /* Same hash value */
                                if ( !strcmp ( key , d->key[i] ) )
                                {   /* Same key */
                                        /* Found a value: modify and return */
                                        if ( d->val[i] != NULL )
                                                free ( d->val[i] );
                                        d->val[i] = val ? strdup(val) : NULL;
                                        /* Value has been modified: return */
                                        return;
                                }
                        }
                }
        }

        /* Add a new value */
        /* See if dictionary needs to grow */
        if ( d->n == d->size )
        {
                /* Reached maximum size: reallocate */
                d->val = (char**)
                        realloc ( d->val , ( d->size << 1 ) * sizeof(char*) );

                d->key = (char**)
                        realloc ( d->key , ( d->size << 1 ) * sizeof(char*) );

                d->hash = (unsigned int *)
                        realloc ( d->hash , ( d->size << 1 ) * sizeof(unsigned) );

                /* Double size */
                d->size <<= 1;
        }

        /* Insert key in the first empty slot */
        for ( i = 0 ; i < d->size ; ++i )
        {
                if ( d->key[i] == NULL )
                {
                        /* Add key here */
                        break ;
                }
        }

        /* Copy key */
        d->key[i]  = strdup(key);
        d->val[i]  = val ? strdup(val) : NULL;
        d->hash[i] = hash;
        d->n++;
        return;
}

static void
dictionary_unset ( mig_dic_t *d , char *key )
{
        unsigned hash;
        int i;

        hash = dictionary_hash ( key );
        for ( i = 0 ; i < d->size ; ++i )
        {
                if ( d->key[i] == NULL )
                        continue;

                /* Compare hash */
                if ( hash == d->hash[i] )
                {
                        /* Compare string,
                           to avoid hash collisions */
                        if ( !strcmp( key , d->key[i] ) )
                        {
                                /* Found key */
                                break ;
                        }
                }
        }

        if ( i >= d->size )
                /* Key not found */
                return ;

        free ( d->key[i] );
        d->key[i] = NULL;

        if ( d->val[i] != NULL )
        {
                free ( d->val[i] );
                d->val[i] = NULL;
        }

        d->hash[i] = 0;
        d->n--;
        return;
}

static void
dictionary_dump ( mig_dic_t *d , FILE *f )
{
        int i;

        if ( d == NULL ||
             f == NULL )
                return;

        for ( i = 0 ; i < d->size ; ++i )
        {
                if ( d->key[i] == NULL )
                        continue;

                if ( d->val[i] != NULL )
                        fprintf ( f , "[%s]=[%s]\n" , d->key[i] , d->val[i] );
                else
                        fprintf ( f , "[%s]=UNDEF\n" , d->key[i] );
        }

        return;
}

