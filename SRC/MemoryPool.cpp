
/*
 * MemoryPool Implementation.
 * Written by ... 2019/9/19
 */

#include <iostream>

#include "MemoryPool.h"
#include "Utility.h"

/*
 * Start for LogEntryItem implementation, Print self.
 */

void LogEntryItem::pushInto(std::vector<LogEntry>& vec) const
{
    const char *pFileName = getString(0, _fileNameLen);
    const char *pMessage = getString(_fileNameLen, _msgLen);

    vec.push_back(LogEntry(_date, _level, _lineNumber, pFileName, pMessage));

    if(pFileName != NULL) 
    {    
        delete[] pFileName;
    }

    if(pMessage != NULL)
    {
        delete[] pMessage;
    }
}

/*
 * For keep an additional char. 
 * So will do one more copy while querying.
 */
const char* LogEntryItem::getString( int start, int len) const
{
    if(len > 0)
    {
        char* pBuf = new char[len + 1];
        CHECK_POINTER_EX(pBuf, NULL);
        memset(pBuf, 0, len + 1);

        memcpy(pBuf, _buf + start, len);

        return pBuf;
    }

    return NULL;
}

/*
 * Print self, for debug usage.
 */
void LogEntryItem::print() const
{

    int bufLen = (_fileNameLen > _msgLen ? _fileNameLen : _msgLen) + 1;
    char *pBuf = new char[bufLen];
    CHECK_POINTER(pBuf);    
        
    if(_fileNameLen > 0)
    {
        memset(pBuf, 0, bufLen);
        memcpy(pBuf, _buf, _fileNameLen);
    }
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
    std::cout<<" "<<pBuf;

    if(_lineNumber > 0)
    {
        memset(pBuf, 0, bufLen);
        memcpy(pBuf, _buf + _fileNameLen, _msgLen);
    }
    std::cout<<" "<<_lineNumber<<"  "<<pBuf<<std::endl;

    delete[] pBuf;
}

/*
 * Start for MemoryPool implementation
 */

MemoryPool::MemoryPool(int size, int entrySize) : _pBuf(NULL), _size(size)
        , _entrySize(entrySize), _count(0)
        , _used(0), _start(0), _end(0)
{
    if(size <= 0 )
    {
        size = 1;
        DBGPRINTF("Error capacity value input. capacity was reset 1 to let app continue.\n");
    }

    if(entrySize <= 0)
    {
        entrySize = sizeof(LogEntryItem);
        DBGPRINTF("Error entry size input. was reset to %d to let app continute. \n", entrySize);
    }

    _pBuf = (char*)malloc(_size);
    if(_pBuf != NULL)
    {
        _count = _size / entrySize;

        if(_count <= 0)
        {
            DBGPRINTF("No enough memory for place even a single log. \n");
        }
    }
}

MemoryPool::~MemoryPool()
{
    if(_pBuf != NULL)
    {
        free(_pBuf);
        _pBuf = NULL;
    }
}

/*
 * if no capacity, free.
 */
void MemoryPool::freeOld() 
{
    if(_start == _end && _used <= 0)
        return;

    ++ _start;
    if(_start >= _count)
    {
        _start -= _count;
    }

    -- _used;
}

int MemoryPool::getEntrySize() const
{
    return _entrySize;
}

/*
 * MemoryPool main function.
 */
bool MemoryPool::add(KLogger::LOGLEVEL level, const char* file, int fileLen
                    , const int lineNumber, const char* message, int msgLen)
{
    /*
     * No additional pointer checking more, 
     */
    if(_size <= 0 || _count <= 0 || _entrySize < (int)sizeof(LogEntryItem))
        return false;

    if(_used == _count)
    {
        freeOld();
    }

    LogEntryItem *pItem = (LogEntryItem*)(_pBuf + _end * _entrySize);
    pItem->_level = level;
    GetTime(pItem->_date);
    pItem->_lineNumber = lineNumber;
    pItem->_msgLen = 0;
        
    int remainSize = _entrySize - sizeof(LogEntryItem);
    pItem->_fileNameLen = (fileLen > remainSize ? remainSize : fileLen);

    if(pItem->_fileNameLen > 0)
    {
        memcpy(pItem->_buf, file, pItem->_fileNameLen);
    }

    remainSize -= fileLen;
    if(remainSize > 0)
    {
        pItem->_msgLen = (msgLen > remainSize ? remainSize : msgLen);
        memcpy(pItem->_buf + fileLen, message, pItem->_msgLen);
    } 

    ++ _end; 
    ++ _used;
    if( _end >= _count)
    {
        _end -= _count;
    }

    return true;
}


void MemoryPool::getItems(int head, int tail, std::vector<LogEntry>& vec) const
{
    vec.clear();

    for(int i = 0; i < _used; ++i)
    {
        if(i < head || i >= tail)
            continue;
                
        int index = i + _start;
        index %= _count;

        LogEntryItem *pItem = (LogEntryItem*)(_pBuf + index * _entrySize);

        pItem->pushInto(vec);
    }
}

void MemoryPool::getItems(const time_t& start, const time_t& end
                                             , std::vector<LogEntry>& vec) const
{
    vec.clear();

    for(int i = 0; i < _used; ++i)
    {       
        int index = i + _start;
        index %= _count;

        LogEntryItem *pItem = (LogEntryItem*)(_pBuf + index * _entrySize);
        if(pItem->_date < start || pItem->_date > end)
        {
            continue;
        }

        pItem->pushInto(vec);
    }
}


/*
 * Start for query...
 * Top() means, latest...
 */
void MemoryPool::top(int count, std::vector<LogEntry>& vec) const
{
    getItems(_used - count, _used, vec);
}

void MemoryPool::tail(int count, std::vector<LogEntry>& vec) const
{
    getItems(0, count, vec);
}

/*
 * Time format should be "0000.00.00 00:00:00"
 */
void MemoryPool::logsBefore(const char* time, std::vector<LogEntry>& vec) const
{
    CHECK_POINTER(time);

    time_t      startTime;
    time_t      endTime;
        
    memset(&startTime, 0, sizeof(time_t));
    if(ConvertToTimet(time, endTime))
    {
        getItems(startTime, endTime, vec);
    }
    else
    {
        DBGPRINTF("Wrong format \n");
    }
}

void MemoryPool::logsAfter(const char* time, std::vector<LogEntry>& vec) const
{
    CHECK_POINTER(time);

    time_t      startTime;

    vec.clear();
    memset(&startTime, 0, sizeof(time_t));

    if(ConvertToTimet(time, startTime))
    {
        for(int i = 0; i < _used; ++i)
        {       
            int index = i + _start;
            index %= _count;

            LogEntryItem *pItem = (LogEntryItem*)(_pBuf + index * _entrySize);
            if(pItem->_date <= startTime)
                continue;

            pItem->pushInto(vec);
        }
    }
    else
    {
        DBGPRINTF("Wrong format \n");
    }
}

void MemoryPool::logsBetween(const char* time1, const char* time2
                                              , std::vector<LogEntry>& vec) const
{
    CHECK_POINTER(time1);
    CHECK_POINTER(time2);

    time_t      startTime;
    time_t      endTime;
        
    if(ConvertToTimet(time1, startTime) &&
        ConvertToTimet(time2, endTime))
    {
        getItems(startTime, endTime, vec);
    }
    else
    {
        DBGPRINTF("Wrong format \n");
    }
}

/*
 * Fetch all, time old first.
 */
void MemoryPool::all(std::vector<LogEntry>& vec) const
{
    getItems(0, _used, vec);
}

