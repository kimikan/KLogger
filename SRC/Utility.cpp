
/*
 * Utility helper.
 * Written by ..., 2014/9/19
 */
#include "Utility.h"

/*
 * Now "%04d.%02d.%02d %02d:%02d:%02d" is the only support format.
 * Export as parameter.
 */
const std::string ConvertToString(const time_t& theTime)
{
    const int   BUFFSIZE = 64;
    char        currentTime[BUFFSIZE];
    tm          tm1;
    tm          *pTm = &tm1;

    memset(currentTime, 0, BUFFSIZE);
    memset(pTm, 0, sizeof(tm));
    
    LOCALTIME(pTm, &theTime);
    SNPRINTF(currentTime, BUFFSIZE - 1, "%04d.%02d.%02d %02d:%02d:%02d", 
        pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday,
        pTm->tm_hour, pTm->tm_min, pTm->tm_sec);

    return std::string(currentTime);
}

bool ConvertToTimet(const char* strTime, time_t& time)
{
    CHECK_POINTER_EX(strTime, false);

    struct tm   tm1;
    memset(&tm1, 0, sizeof(tm));
    int i = SSCANF(strTime, "%d.%d.%d %d:%d:%d" ,       
                &(tm1.tm_year),   
                &(tm1.tm_mon),   
                &(tm1.tm_mday),  
                &(tm1.tm_hour),  
                &(tm1.tm_min),  
                &(tm1.tm_sec));  
             
    if(i <= 0)
        return false;

    tm1.tm_year -= 1900;  
    tm1.tm_mon --;  
    tm1.tm_isdst = -1;  
    time = mktime(&tm1);  

    return true;
}

/*
 * There are a little difference between Windows&Linux
 */
void GetTime(time_t& currentTime)
{
    currentTime = time(NULL);
}
