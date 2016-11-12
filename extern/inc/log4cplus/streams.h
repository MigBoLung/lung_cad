#ifndef LOG4CPLUS_STREAMS_HEADER_
#define LOG4CPLUS_STREAMS_HEADER_

#include <log4cplus/config.h>
#include <log4cplus/tstring.h>

#include <iostream>
#ifdef HAVE_SSTREAM
#  include <sstream>
#  define LOG4CPLUS_STREAM_NAMESPACE std
#elif defined(HAVE_STRSTREAM)
#  include <strstream>
#  if defined(__DECCXX) && !defined(__USE_STD_IOSTREAM)
#    define LOG4CPLUS_STREAM_NAMESPACE
#  else
#    define LOG4CPLUS_STREAM_NAMESPACE std
#  endif
#elif defined(HAVE_STRSTREAM_H)
#  include <strstream.h>
#  define LOG4CPLUS_STREAM_NAMESPACE
#else
#  error "There doesn't appear to be any s*stream headers!!"
#endif

	namespace log4cplus
	{
        	typedef LOG4CPLUS_STREAM_NAMESPACE::ostream tostream;
        	typedef LOG4CPLUS_STREAM_NAMESPACE::istream tistream;
        	static tostream &tcout = LOG4CPLUS_STREAM_NAMESPACE::cout;
        	static tostream &tcerr = LOG4CPLUS_STREAM_NAMESPACE::cerr;

#ifdef HAVE_SSTREAM
		typedef LOG4CPLUS_STREAM_NAMESPACE::ostringstream tostringstream;
#else
	class tostringstream : public LOG4CPLUS_STREAM_NAMESPACE::ostrstream {
        public:
            tstring str() {
                char *ptr = LOG4CPLUS_STREAM_NAMESPACE::ostrstream::str();
                if(ptr) {
                    return tstring(ptr, pcount());
                }
                else {
                    return tstring();
                }
            }
        };

#endif // HAVE_SSTREAM
    }

#endif // LOG4CPLUS_STREAMS_HEADER_
