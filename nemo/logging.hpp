/*
 * logger.hpp
 *
 *  Created on: 2012-11-6
 *      Author: edward
 */

#ifndef LOGGER_HPP_
#define LOGGER_HPP_

/*
 * You must install log4cxx first
 *
 */

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>

#include <nemo/util.hpp>


#define NEMO_DEBUG   LOG4CXX_DEBUG
#define NEMO_TRACE   LOG4CXX_TRACE
#define NEMO_INFO    LOG4CXX_INFO
#define NEMO_WARN    LOG4CXX_WARN
#define NEMO_ERROR   LOG4CXX_ERROR
/**
 * Logs a error if the condition is not true.
 * Param: LOG4CXX_ASSERT(logger, condition, message)
 */
#define NEMO_ASSERT  LOG4CXX_ASSERT
#define NEMO_FATAL   LOG4CXX_FATAL


namespace nemo {
namespace log {

typedef log4cxx::LoggerPtr logger;

/*
 * Usage:
 * 1. nemo::log::init_logger(log4cxx.conf);
 * 2. nemo::log::logger logger = nemo::log::get_logger("logger_name");
 * 3. NEMO_INFO(logger, "how are you!")
 */

bool init_logger(const std::string &cfg_file)
{
    if(nemo::util::file::is_file_readable(cfg_file))
    {
        log4cxx::PropertyConfigurator::configureAndWatch(cfg_file);
        return true;
    }
    else
    {
        return false;
    }
}

logger get_logger(const std::string &name)
{
    return log4cxx::Logger::getLogger(name);
}

}
}


#endif /* LOGGER_HPP_ */
