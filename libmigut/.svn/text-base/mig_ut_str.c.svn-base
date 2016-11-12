#include "mig_ut_str.h"
#include "mig_error_codes.h"

/**************************************************************/
/* PRIVATE FUNCTIONS */
/**************************************************************/

static unsigned int
_is_udec ( const char *str )
{
        char *p = (char*) &str[0];

        while ( *p )
                if ( !isdigit ( *p++ ) )
                        return 0;

        return str[0];
}

/**************************************************************/
static unsigned int
_is_ufloat ( const char *str )
{
        int flag = 0;
        char *p = (char*) &str[0];

        while ( *p )
        {
                if ( *p == '.' )
                        flag = 1;

                if ( ( !isdigit ( *p++ ) ) &&
                     ( flag == 0 ) )
                        return 0;
        }

        return str[0];
}


/**************************************************************/
/* EXPORTS */
/**************************************************************/
unsigned int
mig_ut_str_isdec ( const char *str )
{
        return ( _is_udec ( &str[(str[0] == '-') ||
                 ( str[0] == '+' ) ] ) );
}

/**************************************************************/
unsigned int
mig_ut_str_isfloat ( const char *str )
{
        return ( _is_ufloat ( &str[(str[0] == '-') ||
                 ( str[0] == '+' ) ] ) );
}

/**************************************************************/
void
mig_ut_strip_sep ( char *str )
{
        char *idx;

        if ( str == NULL )
                return;

        /* get pointer to last char in string */
        idx = str + ( strlen ( str ) - 1 );

        while ( ( *idx == MIG_PATH_SEPARATOR ) &&
                ( idx != str ) )
        {
                        *idx = 0x00;
                        -- idx;
        }
}

