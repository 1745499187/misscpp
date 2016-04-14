/*
 * time.hpp
 *
 *  Created on: 2012-11-5
 *      Author: edward
 */

#ifndef TIME_HPP_
#define TIME_HPP_

#include <nemo/config.hpp>

#include <string>
#include <time.h>
#include <sys/time.h>

namespace nemo
{

namespace util
{

namespace time
{

std::string
get_sys_localtime_str(void)
{
    time_t timep;
    struct tm *p;
    ::time(&timep);
    p = localtime(&timep);
    char str[NEMO_BUF_SZ];
    size_t len = sprintf(str,
            "%04d-%02d-%02d %02d:%02d:%02d",
            p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min,
            p->tm_sec);
    str[len] = 0;
    return std::string(str);
}

std::string
get_sys_gmttime_str(void)
{
    time_t current;
    struct tm *cmt;

    ::time(&current);
    cmt = gmtime(&current);

    char s[NEMO_BUF_SZ];
    strftime(s, sizeof(s), "%a, %d %b %Y %H:%M:%S GMT", cmt);

    std::string str(s);
    return str;
}

std::string
format_local_time(const time_t t)
{
    char str[NEMO_BUF_SZ];
    struct tm *tm;
    tm = localtime(&t);
    strftime(str, sizeof(str), "%a, %d %b %Y %H:%M:%S Local", tm);
    return std::string(str);
}

std::string
format_local_time_ms(const struct timeval &t)
{
    char str[NEMO_BUF_SZ];
    struct tm *tm;
    tm = localtime(&t.tv_sec);
    size_t len = strftime(str, sizeof(str), "%a, %d %b %Y %H:%M:%S", tm);
    len += sprintf(str + len, ".%lu", t.tv_usec/1000);
    str[len] = 0;
    return std::string(str);
}

std::string
format_local_time_us(const struct timeval &t)
{
    char str[NEMO_BUF_SZ];
    struct tm *tm;
    tm = localtime(&t.tv_sec);
    size_t len = strftime(str, sizeof(str), "%a, %d %b %Y %H:%M:%S", tm);
    len += sprintf(str + len, ".%lu", t.tv_usec);
    str[len] = 0;
    return std::string(str);
}

long time_diff_s(const time_t &time1, const time_t &time2)
{
    return (long)(time2-time1);
};

//duration in millisec from time1 to time2
long time_diff_ms(const struct timeval &time1, const struct timeval &time2)
{
    return (long)(time2.tv_sec-time1.tv_sec)*1000+(long)((time2.tv_usec-time1.tv_usec)/1000);
};

//duration in microsec from time1 to time2
long time_diff_us(const struct timeval &time1, const struct timeval &time2)
{
    return (long)(time2.tv_sec-time1.tv_sec)*1000*1000+(long)(time2.tv_usec-time1.tv_usec);
};

}

}

}

#endif /* TIME_HPP_ */
