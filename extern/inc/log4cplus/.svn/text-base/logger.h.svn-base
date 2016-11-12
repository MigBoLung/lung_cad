#ifndef _LOG4CPLUS_LOGGERHEADER_
#define _LOG4CPLUS_LOGGERHEADER_

#include <log4cplus/config.h>
#include <log4cplus/loglevel.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/tstring.h>
#include <log4cplus/streams.h>
#include <log4cplus/helpers/pointer.h>
#include <log4cplus/spi/appenderattachable.h>
#include <log4cplus/spi/loggerfactory.h>

#include <memory>
#include <vector>

namespace log4cplus {

    namespace spi {
        class LoggerImpl;
        typedef helpers::SharedObjectPtr<LoggerImpl> SharedLoggerImplPtr;
    }
    class Appender;
    class Hierarchy;
    class HierarchyLocker;
    class DefaultLoggerFactory;

    typedef std::vector<Logger> LoggerList;


    class LOG4CPLUS_EXPORT Logger : public log4cplus::spi::AppenderAttachable {
    public:

        static bool exists(const log4cplus::tstring& name);
        static LoggerList getCurrentLoggers();
        static Hierarchy& getDefaultHierarchy();
        static Logger getInstance(const log4cplus::tstring& name);
        static Logger getInstance(const log4cplus::tstring& name, spi::LoggerFactory& factory);

        static Logger getRoot();
        static void shutdown();

        void assertion(bool assertionVal, const log4cplus::tstring& msg) {
            if(!assertionVal) {
                log(FATAL_LOG_LEVEL, msg);
            }
        }

        void closeNestedAppenders();
        bool isEnabledFor(LogLevel ll) const;
        void log(LogLevel ll, const log4cplus::tstring& message,
                 const char* file=NULL, int line=-1);

        void forcedLog(LogLevel ll, const log4cplus::tstring& message,
                       const char* file=NULL, int line=-1);

        /**
         * Call the appenders in the hierrachy starting at
         * <code>this</code>.  If no appenders could be found, emit a
         * warning.
         * <p>
         * This method calls all the appenders inherited from the
         * hierarchy circumventing any evaluation of whether to log or not
         * to log the particular log request.
         *
         * @param spi::InternalLoggingEvent the event to log.
         */
        void callAppenders(const spi::InternalLoggingEvent& event);

        /**
         * Starting from this logger, search the logger hierarchy for a
         * "set" LogLevel and return it. Otherwise, return the LogLevel of the
         * root logger.
         * <p>
         * The Logger class is designed so that this method executes as
         * quickly as possible.
         */
        LogLevel getChainedLogLevel() const;

        /**
         * Returns the assigned {@link LogLevel}, if any, for this Logger.
         *
         * @return LogLevel - the assigned LogLevel, can be <code>NOT_SET_LOG_LEVEL</code>.
         */
        LogLevel getLogLevel() const;

        /**
         * Set the LogLevel of this Logger.
         */
        void setLogLevel(LogLevel);

        /**
         * Return the the {@link Hierarchy} where this <code>Logger</code> instance is
         * attached.
         */
        Hierarchy& getHierarchy() const;

        /**
         * Return the logger name.
         */
        log4cplus::tstring getName() const;

        /**
         * Get the additivity flag for this Logger instance.
         */
        bool getAdditivity() const;

        /**
         * Set the additivity flag for this Logger instance.
         */
        void setAdditivity(bool additive);


      // AppenderAttachable Methods
        virtual void addAppender(SharedAppenderPtr newAppender);

        virtual SharedAppenderPtrList getAllAppenders();

        virtual SharedAppenderPtr getAppender(const log4cplus::tstring& name);

        virtual void removeAllAppenders();

        virtual void removeAppender(SharedAppenderPtr appender);

        virtual void removeAppender(const log4cplus::tstring& name);

      // Copy Ctor
        Logger(const Logger& rhs);
        Logger& operator=(const Logger& rhs);

      // Dtor
        ~Logger();

        /**
         * Used to retrieve the parent of this Logger in the
         * Logger tree.
         */
        Logger getParent();

    protected:
      // Data
        /** This is a pointer to the implementation class. */
        spi::LoggerImpl *value;

    private:
      // Ctors
        /**
         * This constructor created a new <code>Logger</code> instance
         * with a pointer to a Logger implementation.
         * <p>
         * You should not create loggers directly.
         *
         * @param ptr A pointer to the Logger implementation.  This value
         *            cannot be NULL.
         */
        Logger(spi::LoggerImpl *ptr);
        Logger(const spi::SharedLoggerImplPtr& val);

      // Methods
        void init();
        void validate(const char *file, int line) const;

      // Friends
        friend class log4cplus::spi::LoggerImpl;
        friend class log4cplus::Hierarchy;
        friend class log4cplus::HierarchyLocker;
        friend class log4cplus::DefaultLoggerFactory;
    };


    /**
     * This class is used to create the default implementation of
     * the Logger class
     */
    class LOG4CPLUS_EXPORT DefaultLoggerFactory : public spi::LoggerFactory {
    public:
        Logger makeNewLoggerInstance(const log4cplus::tstring& name, Hierarchy& h);
    };



    /**
     * This class is used to produce "Trace" logging.  When an instance of
     * this class is created, it will log a <code>"ENTER: " + msg</code>
     * log message if TRACE_LOG_LEVEL is enabled for <code>logger</code>.
     * When an instance of this class is destroyed, it will log a
     * <code>"ENTER: " + msg</code> log message if TRACE_LOG_LEVEL is enabled
     * for <code>logger</code>.
     * <p>
     * @see LOG4CPLUS_TRACE
     */
    class LOG4CPLUS_EXPORT TraceLogger {
    public:
        TraceLogger(const Logger& l, const log4cplus::tstring& _msg,
                    const char* _file=NULL, int _line=-1)
          : logger(l), msg(_msg), file(_file), line(_line)
        { if(logger.isEnabledFor(TRACE_LOG_LEVEL))
              logger.forcedLog(TRACE_LOG_LEVEL, LOG4CPLUS_TEXT("ENTER: ") + msg, file, line);
        }

        ~TraceLogger()
        { if(logger.isEnabledFor(TRACE_LOG_LEVEL))
              logger.forcedLog(TRACE_LOG_LEVEL, LOG4CPLUS_TEXT("EXIT:  ") + msg, file, line);
        }

    private:
        Logger logger;
        log4cplus::tstring msg;
        const char* file;
        int line;
    };

} // end namespace log4cplus


#endif // _LOG4CPLUS_LOGGERHEADER_

