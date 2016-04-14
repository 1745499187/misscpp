/*
 * prop_test.cc
 *
 *  Created on: 2012-11-5
 *      Author: edward
 */


#include <iostream>

#include <nemo/logging.hpp>

void do_test(void)
{
    nemo::log::init_logger("log4cxx.conf");
    nemo::log::logger log1 = nemo::log::get_logger("common");
    nemo::log::logger log2 = nemo::log::get_logger("error");

    // below are not recommend, because no __FILE__, __LINE__ ...
    log1->debug("this is debug");
    log1->info("this is info");
    log1->trace("this is trace");
    log1->error("this is error");

    // recommended methods are below, try use them
    NEMO_DEBUG(log2, "debug");
    NEMO_INFO(log2, "info");
    NEMO_TRACE(log2, "trace");
    NEMO_ERROR(log2, "error");
    NEMO_ASSERT(log2, 1==2, "assert 1=2")
    NEMO_ASSERT(log2, 1==1, "assert 1=1")
}

int main(int argc, char *argv[])
{
    do_test();
    return 0;
}
