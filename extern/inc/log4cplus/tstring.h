#ifndef LOG4CPLUS_TSTRING_HEADER_
#define LOG4CPLUS_TSTRING_HEADER_

#include <log4cplus/config.h>
#include <string>

#define LOG4CPLUS_TEXT(STRING) STRING

namespace log4cplus {
    typedef char tchar;
    typedef std::string tstring;
}

#define LOG4CPLUS_C_STR_TO_TSTRING(STRING) std::string(STRING)
#define LOG4CPLUS_STRING_TO_TSTRING(STRING) STRING
#define LOG4CPLUS_TSTRING_TO_STRING(STRING) STRING


#endif // LOG4CPLUS_TSTRING_HEADER_
