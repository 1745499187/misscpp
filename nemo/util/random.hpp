/*
 * random.hpp
 *
 *  Created on: 2012-11-5
 *      Author: edward
 */

#ifndef RANDOM_HPP_
#define RANDOM_HPP_

#include <time.h>
#include <stdlib.h>

namespace nemo
{

namespace util
{

namespace random
{

void seed(unsigned int seed = 0)
{
    if(seed == 0)
    {
        time_t t;
        ::time(&t);
        seed = (unsigned int)t;
    }
    srand(seed);
}

int random(void)
{
    return rand();
}

}

}

}

#endif /* RANDOM_HPP_ */
