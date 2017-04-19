
#include <iostream>
#include <pthread.h>

#include "../../INC/KLogger.h"
#include "../../INC/CMutex.h"

#include "../src/UnitTest++.h"
#include "../src/ReportAssert.h"

#include "../../SRC/Utility.h"
#include "../../SRC/MemoryPool.h"

namespace
{
    /*
     * Testing time operation functionality.
     */
    TEST(TimeTest)
    {
        time_t now;
        GetTime(now);

        std::string time = ConvertToString(now);
        time_t ret;
        CHECK(ConvertToTimet(time.c_str(), ret));

        CHECK_EQUAL(now, ret);
    };

    /*
     * Testing does CMutex work?
     */
    int  g_Value = 0;
    CMutex g_Mutex;
    void* workThread(void*)
    {
        g_Mutex.lock();
        g_Value ++;
        sleep(3);
        CHECK_EQUAL(1, g_Value);
        g_Mutex.unLock();
        pthread_exit((void*)0);
    };

    TEST(CMutexTest)
    {
        pthread_t thread;
        pthread_create(&thread, NULL, workThread, NULL);
        sleep(1); /* Sleep 1s to let work thread first */
        g_Mutex.lock();
        CHECK_EQUAL(1, g_Value);
        g_Value ++;
        g_Mutex.unLock();
        CHECK_EQUAL(2, g_Value);
    };

    /*
     * Testing memoryPool all(n) function.
     */
    TEST(MemoryPoolTest_All)
    {
        const char* DEMO = "test";
        const int LINENO = 200;

        const char* MESSAGE = "MESSAGE";

        std::vector<LogEntry> vec;
        MemoryPool memoryPool(401, 80);

        for(int i = 0; i <= 100; ++i)
        {
            CHECK(memoryPool.add(KLogger::ERROR, DEMO
                           , strlen(DEMO), LINENO,MESSAGE, strlen(MESSAGE)));
        }

        /* There should be only 5 records remained. others are overrided */
        IQuery* pQuery = &memoryPool;
        pQuery->all(vec);
        CHECK(5 == vec.size());

        for(unsigned int i = 0; i < vec.size(); ++i)
        {
            const LogEntry& entry = vec[i];
            CHECK_EQUAL(entry.getLevel(), KLogger::ERROR);
            CHECK_EQUAL(entry.getLineNumber(), LINENO);
            CHECK(0 == strcmp(entry.getFileName().c_str(), DEMO));
            CHECK(0 == strcmp(entry.getMessage().c_str(), MESSAGE));
        }
    }

    /*
     * Testing the top(n) function.
     */
    TEST(MemoryPoolTest_Top)
    {
        const char* DEMO = "test";
        const int LINENO = 200;

        const char* MESSAGE = "MESSAGE";

        std::vector<LogEntry> vec;
        MemoryPool memoryPool(401, 80);

        for(int i = 0; i <= 100; ++i)
        {
            CHECK(memoryPool.add(KLogger::ERROR, DEMO
                           , strlen(DEMO), LINENO,MESSAGE, strlen(MESSAGE)));
        }

        /* There should be only 5 records remained. others are overrided */
        IQuery* pQuery = &memoryPool;
        pQuery->top(9, vec);
        CHECK(5 == vec.size());
        for(unsigned int i = 0; i < vec.size(); ++i)
        {
            const LogEntry& entry = vec[i];
            CHECK_EQUAL(entry.getLevel(), KLogger::ERROR);
            CHECK_EQUAL(entry.getLineNumber(), LINENO);
            CHECK(0 == strcmp(entry.getFileName().c_str(), DEMO));
            CHECK(0 == strcmp(entry.getMessage().c_str(), MESSAGE));
        }
    }

    /*
     * Testing the tail(n) function
     */
    TEST(MemoryPoolTest_Tail)
    {
        const char* DEMO = "test";
        const int LINENO = 200;

        const char* MESSAGE = "MESSAGE";

        std::vector<LogEntry> vec;
        MemoryPool memoryPool(401, 80);

        for(int i = 0; i <= 100; ++i)
        {
            CHECK(memoryPool.add(KLogger::ERROR, DEMO
                           , strlen(DEMO), LINENO,MESSAGE, strlen(MESSAGE)));
        }

        /* There should be only 5 records remained. others are overrided */
        IQuery* pQuery = &memoryPool;
        pQuery->tail(2, vec);
        CHECK(2 == vec.size());
        for(unsigned int i = 0; i < vec.size(); ++i)
        {
            const LogEntry& entry = vec[i];
            CHECK_EQUAL(entry.getLevel(), KLogger::ERROR);
            CHECK_EQUAL(entry.getLineNumber(), LINENO);
            CHECK(0 == strcmp(entry.getFileName().c_str(), DEMO));
            CHECK(0 == strcmp(entry.getMessage().c_str(), MESSAGE));
        }
    }

    /*
     * Testing get&put all items before a specific timestamp.
     */
    TEST(MemoryPoolTest_Before)
    {
        const char* DEMO = "test";
        const int LINENO = 200;

        const char* MESSAGE = "MESSAGE";

        std::vector<LogEntry> vec;
        MemoryPool memoryPool(401, 80);
        time_t time;

        for(int i = 0; i <= 4; ++i)
        {
            if(i == 3) /* 3 means 3 before, 2 after */
            {
                sleep(1);/* sleep 1s*/
                GetTime(time);
                sleep(1);/* Maybe no need */
            }
            CHECK(memoryPool.add(KLogger::ERROR, DEMO
                           , strlen(DEMO), LINENO,MESSAGE, strlen(MESSAGE)));
        }

        /* There should be only 5 records remained. others are overrided */
        IQuery* pQuery = &memoryPool;
        pQuery->logsBefore(ConvertToString(time).c_str(), vec);
        CHECK(3 == vec.size());
        for(unsigned int i = 0; i < vec.size(); ++i)
        {
            const LogEntry& entry = vec[i];
            CHECK_EQUAL(entry.getLevel(), KLogger::ERROR);
            CHECK_EQUAL(entry.getLineNumber(), LINENO);
            CHECK(0 == strcmp(entry.getFileName().c_str(), DEMO));
            CHECK(0 == strcmp(entry.getMessage().c_str(), MESSAGE));
        }
    }

    /*
     * Testing get&put all items after a specific timestamp.
     */
    TEST(MemoryPoolTest_After)
    {
        const char* DEMO = "test";
        const int LINENO = 200;

        const char* MESSAGE = "MESSAGE";

        std::vector<LogEntry> vec;
        MemoryPool memoryPool(401, 80);
        time_t time;

        for(int i = 0; i <= 4; ++i)
        {
            if(i == 3) /* 3 means 3 before, 2 after */
            {
                sleep(1);/* sleep 1s*/
                GetTime(time);
                sleep(1);/* Maybe no need */
            }
            CHECK(memoryPool.add(KLogger::ERROR, DEMO
                           , strlen(DEMO), LINENO,MESSAGE, strlen(MESSAGE)));
        }

        /* There should be only 5 records remained. others are overrided */
        IQuery* pQuery = &memoryPool;
        pQuery->logsAfter(ConvertToString(time).c_str(), vec);
        CHECK(2 == vec.size());
        for(unsigned int i = 0; i < vec.size(); ++i)
        {
            const LogEntry& entry = vec[i];
            CHECK_EQUAL(entry.getLevel(), KLogger::ERROR);
            CHECK_EQUAL(entry.getLineNumber(), LINENO);
            CHECK(0 == strcmp(entry.getFileName().c_str(), DEMO));
            CHECK(0 == strcmp(entry.getMessage().c_str(), MESSAGE));
        }
    }

    /*
     * Testing get all items between 2 time stamp.
     */
    TEST(MemoryPoolTest_Between)
    {
        const char* DEMO = "test";
        const int LINENO = 200;

        const char* MESSAGE = "MESSAGE";

        std::vector<LogEntry> vec;
        MemoryPool memoryPool(401, 80);
        time_t time1, time2;

        GetTime(time1);
        for(int i = 0; i <= 4; ++i)
        {
            if(i == 3) /* 3 means 3 before, 2 after */
            {
                sleep(1);/* sleep 1s*/
                GetTime(time2);
                sleep(1);/* Maybe no need */
            }
            CHECK(memoryPool.add(KLogger::ERROR, DEMO
                           , strlen(DEMO), LINENO,MESSAGE, strlen(MESSAGE)));
        }

        /* There should be only 5 records remained. others are overrided */
        IQuery* pQuery = &memoryPool;
        pQuery->logsBetween(ConvertToString(time1).c_str(), ConvertToString(time2).c_str(), vec);
        CHECK(3 == vec.size());
        for(unsigned int i = 0; i < vec.size(); ++i)
        {
            const LogEntry& entry = vec[i];
            CHECK_EQUAL(entry.getLevel(), KLogger::ERROR);
            CHECK_EQUAL(entry.getLineNumber(), LINENO);
            CHECK(0 == strcmp(entry.getFileName().c_str(), DEMO));
            CHECK(0 == strcmp(entry.getMessage().c_str(), MESSAGE));
        }
    }

    /*
     * Testing edge value, no capacity to place even one item.
     */
    TEST(MemoryPoolTest_Zero)
    {
        const char* DEMO = "test";
        const int LINENO = 200;

        const char* MESSAGE = "MESSAGE";

        std::vector<LogEntry> vec;
        MemoryPool memoryPool(79, 80);

        for(int i = 0; i <= 40; ++i)
        {
            CHECK(!memoryPool.add(KLogger::ERROR, DEMO
                           , strlen(DEMO), LINENO,MESSAGE, strlen(MESSAGE)));
        }

        /* There should be only 5 records remained. others are overrided */
        CHECK(0 == vec.size());
    }

    /*
     * Since KLogger IQuery just a wrapper of MemoryPool
     * So subfeature no need to seperate test.
     * Just add a performance testing for KLogger Add
     */
    TEST(KLoggerTest_Performance)
    {
        std::vector<LogEntry> vec;

        KLogger logger(500, KLogger::ERROR | KLogger::DEBUG, 80);

        time_t start, end;
        GetTime(start);
        for(int i = 0; i < 10000000; ++i)
        {
            KERROR(&logger, "This is %d test", i);
        }
        GetTime(end);

        std::cout<<"10000000 times log takes time: "<<1000.0 * difftime(end, start)<<" ms"<<std::endl;
    }
};
