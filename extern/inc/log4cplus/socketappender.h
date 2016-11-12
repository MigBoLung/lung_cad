#ifndef _LOG4CPLUS_SOCKET_APPENDER_HEADER_
#define _LOG4CPLUS_SOCKET_APPENDER_HEADER_

#include <log4cplus/config.h>
#include <log4cplus/appender.h>
#include <log4cplus/helpers/socket.h>

#define LOG4CPLUS_MAX_MESSAGE_SIZE (2*8*1024)

namespace log4cplus {


    class LOG4CPLUS_EXPORT SocketAppender : public Appender {
    public:
      // Ctors
        SocketAppender(const log4cplus::tstring& host, int port,
                       const log4cplus::tstring& serverName = tstring());
        SocketAppender(const log4cplus::helpers::Properties properties);

      // Dtor
        ~SocketAppender();

      // Methods
        virtual void close();

    protected:
        void openSocket();
        virtual void append(const spi::InternalLoggingEvent& event);

      // Data
        log4cplus::helpers::Socket socket;
        log4cplus::tstring host;
        int port;
        log4cplus::tstring serverName;

    private:
      // Disallow copying of instances of this class
        SocketAppender(const SocketAppender&);
        SocketAppender& operator=(const SocketAppender&);
    };

    namespace helpers {
        LOG4CPLUS_EXPORT
        SocketBuffer convertToBuffer(const log4cplus::spi::InternalLoggingEvent& event,
                                     const log4cplus::tstring& serverName);

        LOG4CPLUS_EXPORT
        log4cplus::spi::InternalLoggingEvent readFromBuffer(SocketBuffer& buffer);
    } // end namespace helpers

} // end namespace log4cplus

#endif // _LOG4CPLUS_SOCKET_APPENDER_HEADER_
