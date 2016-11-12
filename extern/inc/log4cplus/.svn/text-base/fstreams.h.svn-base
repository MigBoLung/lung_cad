#ifndef LOG4CPLUS_FSTREAMS_HEADER_
#define LOG4CPLUS_FSTREAMS_HEADER_

#include <log4cplus/config.h>

#include <fstream>

#if defined(__DECCXX) && !defined(__USE_STD_IOSTREAM)
#  define LOG4CPLUS_FSTREAM_NAMESPACE
#else
#  define LOG4CPLUS_FSTREAM_NAMESPACE std
#endif

namespace log4cplus
{
	typedef LOG4CPLUS_FSTREAM_NAMESPACE::ofstream tofstream;
	typedef LOG4CPLUS_FSTREAM_NAMESPACE::ifstream tifstream;
}


#endif // LOG4CPLUS_FSTREAMS_HEADER_

