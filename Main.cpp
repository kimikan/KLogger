
#include <iostream>
#include "INC/KLogger.h"

int main() 
{
    KLogger logger(500, KLogger::ERROR | KLogger::DEBUG, 70);

    logger.log(KLogger::ERROR, "hh", 1, NULL);
    for(int i = 0; i < 9; ++i)
    {
        KERROR(&logger, "This is %d test", i);
    }
    logger.log(KLogger::INFO, NULL, 20, "xxxxx");

    const IQuery* pQuery = logger.query();

    std::vector<LogEntry> vec;
    std::cout<<"All of the logs"<<std::endl;
    pQuery->all(vec);
    for(unsigned int i = 0; i < vec.size(); ++i)
        vec[i].print();

    std::cout<<std::endl<<"Top 3: "<<std::endl;
    pQuery->top(3, vec);
    for(unsigned int i = 0; i < vec.size(); ++i)
        vec[i].print();

    std::cout<<std::endl<<"Tail 4: "<<std::endl;
    pQuery->tail(4, vec);

    for(unsigned int i = 0; i < vec.size(); ++i)
        vec[i].print();

    std::cout<<std::endl<<"Logs After:"<<std::endl;
    pQuery->logsAfter("2015.1.1 1:1:1", vec);

    for(unsigned int i = 0; i < vec.size(); ++i)
        vec[i].print();

    std::cout<<std::endl<<"Logs Before:"<<std::endl;
    pQuery->logsBefore("2015.1.1 1:1:2", vec);

    for(unsigned int i = 0; i < vec.size(); ++i)
        vec[i].print();

    std::cout<<std::endl<<"Logs Between:"<<std::endl;
    pQuery->logsBetween("2014.1.1 1:1:1", "2014.1.1 1:1:1", vec);

    for(unsigned int i = 0; i < vec.size(); ++i)
        vec[i].print();

    return 0;
}


