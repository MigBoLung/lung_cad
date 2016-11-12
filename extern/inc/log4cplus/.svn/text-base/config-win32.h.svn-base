#ifndef LOG4CPLUS_CONFIG_WIN32_HEADER_
#define LOG4CPLUS_CONFIG_WIN32_HEADER_

/* Define if you have the ftime function.  */
#define HAVE_FTIME 1

/* Define if you have the <sstream> header file.  */
#define HAVE_SSTREAM 1

#ifdef LOG4CPLUS_STATIC
#  define LOG4CPLUS_EXPORT
#else
#  ifdef LOG4CPLUS_BUILD_DLL
#    define LOG4CPLUS_EXPORT __declspec(dllexport)
#  else
#    define LOG4CPLUS_EXPORT __declspec(dllimport)
#  endif
#endif

#ifndef LOG4CPLUS_SINGLE_THREADED
#  define LOG4CPLUS_USE_WIN32_THREADS
#endif

#if defined(_MSC_VER)
  // Warning about: identifier was truncated to '255' characters in the debug information
#  pragma warning( disable : 4786 )
  // Warning about: <type1> needs to have dll-interface to be used by clients of class <type2>
#  pragma warning( disable : 4251 )
#endif

#endif // LOG4CPLUS_CONFIG_WIN32_HEADER_
