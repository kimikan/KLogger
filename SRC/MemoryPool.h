
/*
 * MemoryPool Implementation.
 * Written by ... 2019/9/19
 */

#ifndef _MEMORY_POOL_H_
#define _MEMORY_POOL_H_

#include <vector>

#include "../INC/KLogger.h"

/*
 * Log Entry item definition. All entry items are placed linear. 
 */
struct LogEntryItem
{
/* 
 * Modify it carefully no virtual, 
 * no operation allowed 
 */
public:
     
    void print() const;    

    void pushInto(std::vector<LogEntry>&) const;

private:
    const char* getString(int start, int len) const;

public:

    time_t      _date;
    int         _level;
    int         _lineNumber;
    int         _fileNameLen;
    int         _msgLen;
    
    char        _buf[0];
};

/*
 * MemoryPool implementation.
 * Manage all of the Entry items.
 */
class MemoryPool : public IQuery
{
public:

    MemoryPool(int, int);

    ~MemoryPool();

    int getEntrySize() const;

    bool add(KLogger::LOGLEVEL level, const char* file
        , int fileLen, const int lineNumber
        , const char* message, int msgLen);

    void top(int count, std::vector<LogEntry>& ) const;

    void tail(int count, std::vector<LogEntry>& ) const;

    void logsBefore(const char* time, std::vector<LogEntry>& ) const;

    void logsAfter(const char* time, std::vector<LogEntry>& ) const;

    void logsBetween(const char* time1, const char* time2
                                      , std::vector<LogEntry>& ) const;

    void all(std::vector<LogEntry>& ) const;

private:
    
    void getItems(int head, int tail, std::vector<LogEntry>&) const;

    void getItems(const time_t& start, const time_t& end
                                     , std::vector<LogEntry>&) const;
    
    void freeOld();

private:
    char            *_pBuf;
    
    int             _size;
    int             _entrySize;
    
    int             _count;
    int             _used;
    
    int             _start;
    int             _end;
};

#endif
