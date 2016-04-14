/*
 * prop_test.cc
 *
 *  Created on: 2012-11-5
 *      Author: edward
 */


#include <iostream>

#include <nemo/network.hpp>

void do_test(void)
{
	nemo::network::basic_server srv;
    srv.listen(8091);
    srv.accept();
}

int main(int argc, char *argv[])
{
    do_test();
    return 0;
}
