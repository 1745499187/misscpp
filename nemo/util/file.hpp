/*
 * file.hpp
 *
 *  Created on: 2012-11-5
 *      Author: edward
 */

#ifndef FILE_HPP_
#define FILE_HPP_

#include <nemo/config.hpp>

#include <string>
#include <fstream>
#include <sys/stat.h>

namespace nemo
{

namespace util
{

namespace file
{

// F_OK    0
// X_OK    1
// W_OK    2
// R_OK    4
bool
is_file_exists(const std::string& file)
{
    return (access(file.c_str(), F_OK) == 0) ? true : false;
}

bool
is_file_readable(const std::string& file)
{
    return (access(file.c_str(), R_OK) == 0) ? true : false;
}

bool
is_file_writable(const std::string& file)
{
    return (access(file.c_str(), W_OK) == 0) ? true : false;
}

bool
is_file_executable(const std::string& file)
{
    return (access(file.c_str(), X_OK) == 0) ? true : false;
}

bool
make_dir(const std::string& path)
{
    bool ret = true;
    if (!is_file_exists(path))
    {
        if (mkdir(path.c_str(), S_IRWXU | S_IRGRP | S_IROTH) != 0)
        {
            ret = false;
        }
    }
    return ret;
}

size_t
get_file_size(const char *filename)
{
    struct stat buf;
    if (stat(filename, &buf) < 0)
    {
        return 0;
    }
    return (size_t) buf.st_size;
}

long
get_file_content(const std::string& file, std::string& output)
{
    std::ifstream ifs;
    ifs.open(file.c_str(), std::ios::in);

    if (ifs == 0 || ifs.bad())
    {
        return -1;
    }

    output.clear();

    char buf[NEMO_BUF_SZ_S];
    while (ifs.read(buf, sizeof(buf)).gcount() > 0)
    {
        output.append(buf, ifs.gcount());
    }

    ifs.close();

    return output.size();
}

}

}

}

#endif /* FILE_HPP_ */
