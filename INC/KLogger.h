
/*
 * Written by ... 2014/9/19
 * KLogger Library main header file.
 */

#include <vector>
#include <string>

#include <time.h>

#ifndef _KLOGGER_H_
#define _KLOGGER_H_

class LogEntry
{

public:

    LogEntry(time_t, int, int, const char*, const char*);

    time_t getDate() const;

    int getLevel() const;

    int getLineNumber() const;

    const std::string& getFileName() const;

    const std::string& getMessage() const;

    void print() const;

private:

    time_t        _date;
  
    int           _level;

    int           _lineNumber;

    std::string   _fileName;

    std::string   _message;

};


/*
 * Seperate the query interface, Behavior abstract.
 * The query interface seems be better, if provided return collection values
 * For example:  virtual bool top(int, std::vector& result) const = 0;
 * But for demo usage, it's enough.
 */

class IQuery
{
public:
    virtual void top(int, std::vector<LogEntry>& ) const = 0;

    virtual void tail(int, std::vector<LogEntry>& ) const = 0;

    virtual void logsBefore(const char* time, std::vector<LogEntry>& ) const = 0;

    virtual void logsAfter(const char* time, std::vector<LogEntry>& ) const = 0;

    virtual void logsBetween(const char* time1
                           , const char* time2, std::vector<LogEntry>& ) const = 0;

    virtual void all(std::vector<LogEntry>&) const = 0;

    virtual ~IQuery() 
    {}
};

/*
 * To enscapulate the implementation into another class
 * The purpose is to make sure
 */
class KLoggerImpl;

class KLogger
{

public:
    enum LOGLEVEL
    {
        VERBOSE = 1,
        INFO = 2,
        DEBUG = 4,
        ERROR = 8
    };

public:

    KLogger(int capacity, int logFilter, int entrySize);

    void setLogFilter(int);

    /* explicit definition to disable this function */
    explicit KLogger(const KLogger&);

    ~KLogger();

    const IQuery* query() const;

    void log(LOGLEVEL, const char* file, const int lineNumber, const char* message, ...);

private:

    KLoggerImpl* _pLogImpl;
};

/*
 * Provided function should be used.
 */
#define KERROR(p, o, ...) (p)->log(KLogger::ERROR, __FILE__, __LINE__, o, __VA_ARGS__)

#define KVERBOSE(p, o, ...) (p)->log(KLogger::VERBOSE, __FILE__, __LINE__, o, __VA_ARGS__)

#define KINFO(p, o, ...) (p)->log(KLogger::INFO, __FILE__, __LINE__, o, __VA_ARGS__)

#define KDEBUG(p, o, ...) (p)->log(KLogger::DEBUG, __FILE__, __LINE__, o, __VA_ARGS__)

#endif



