
/*
 * KLogger Implementation.
 * Written by ... 2019/9/19
 */
#include <iostream>
#include <stdarg.h>
#include <string>

#include "../INC/KLogger.h"
#include "../INC/CMutex.h"
#include "Config.h"
#include "Utility.h"
#include "MemoryPool.h"

/*
 * LogEntry implementation.
 */
LogEntry::LogEntry(time_t time, int level
               , int lineNumber, const char* fileName
               , const char* message) : _date(time), _level(level)
               , _lineNumber(lineNumber), _fileName(fileName)
               , _message(message)
{

}

time_t LogEntry::getDate() const
{
    return _date;
}

int LogEntry::getLevel() const
{
    return _level;
}

int LogEntry::getLineNumber() const
{
    return _lineNumber;
}

const std::string& LogEntry::getFileName() const
{
    return _fileName;
}

const std::string& LogEntry::getMessage() const
{
    return _message;
}

/*
 * This function can print self to std::out
 */
void LogEntry::print() const
{

    std::cout<<ConvertToString(_date).c_str()<<" ";

    switch(_level)
    {
    case KLogger::DEBUG:
        std::cout<<" * DEBUG * ";
        break;
    case KLogger::ERROR:
        std::cout<<" * ERROR * ";
        break;
    case KLogger::INFO:
        std::cout<<" * INFO * ";
        break;
    case KLogger::VERBOSE:
        std::cout<<" * VERBOSE * ";
        break;

    default:
        std::cout<<" * UNKNOWN * ";
        break;
    };
    std::cout<<" "<<_fileName.c_str();

    std::cout<<" "<<_lineNumber<<"  "<<_message.c_str()<<std::endl;
}


/*
 * Real KLogger wrapper implementation.
 */
class KLoggerImpl
{
public:
    KLoggerImpl( int capacity, int logFilter , int entrySize);

    ~KLoggerImpl();

    void log(KLogger::LOGLEVEL level, const char* file
        , const int lineNumber, const char* message);

    const IQuery* query() const;

    int getEntrySize() const;

private:

    MemoryPool      _memoryPool;

    int             _capacity;

    int             _logFilter;
    
    CMutex          _mutex;
};

KLoggerImpl::KLoggerImpl(int capacity, int logFilter, int entrySize) 
    : _memoryPool(capacity, entrySize), _capacity(capacity)
    ,  _logFilter(logFilter), _mutex()
{

}

KLoggerImpl::~KLoggerImpl()
{

}

const IQuery* KLoggerImpl::query() const
{
    return &_memoryPool;
}

int KLoggerImpl::getEntrySize() const
{
    return _memoryPool.getEntrySize();
}

void KLoggerImpl::log(KLogger::LOGLEVEL level, const char* file
                      , const int lineNumber, const char* message)
{
    CHECK_POINTER(file);
    CHECK_POINTER(message);

    _mutex.lock();

    if(level & _logFilter) 
    {
        if(!_memoryPool.add(level, file, strlen(file), lineNumber, message, strlen(message)))
        {
            DBGPRINTF("No enough memory. \n");
        }
    }

    _mutex.unLock();
}

/*
 * KLogger implementation
 */

KLogger::KLogger(const int capacity, int logFilter, int entrySize) : _pLogImpl(NULL)
{
    _pLogImpl = new KLoggerImpl(capacity, logFilter, entrySize);
}

KLogger::~KLogger()
{
    if(_pLogImpl)
    {
        delete _pLogImpl;
        _pLogImpl = NULL;
    }
}

const IQuery* KLogger::query() const
{
    CHECK_POINTER_EX(_pLogImpl, NULL);

    return _pLogImpl->query();
}

void KLogger::log(LOGLEVEL level, const char* file, 
                  const int lineNumber, const char* message, ...)
{
    CHECK_POINTER(file);
    CHECK_POINTER(message);
    CHECK_POINTER(_pLogImpl);

    int             entrySize = _pLogImpl->getEntrySize();
    va_list         args;
    char*           pBuffer = new char[entrySize + 1];
    
    if(NULL != pBuffer)
    {
        memset(pBuffer, 0, entrySize + 1);
    
        va_start(args, message);
        VSNPRINTF(pBuffer, entrySize, message, args);
        va_end(args);

        _pLogImpl->log(level, file, lineNumber, pBuffer);
        delete[] pBuffer;
    }
}
    
