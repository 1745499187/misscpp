/*
 * prop_test.cc
 *
 *  Created on: 2012-11-5
 *      Author: edward
 */


#include <nemo/properties.hpp>

#include <iostream>

void do_test(void)
{
	nemo::prop::sect_kv_prop prop("for_test.prop");
    prop.dump_properties(std::cout);
}

int main(int argc, char *argv[])
{
    do_test();
    return 0;
}
