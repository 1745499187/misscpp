/*
 * prop_test.cc
 *
 *  Created on: 2012-11-5
 *      Author: edward
 */


#include <iostream>

#include <nemo/object_pool.hpp>
#include <nemo/util.hpp>

using namespace std;

static int index = 0;
#define INIT_SIZE 2000
#define TRY_TIMES 30000

class test_class
{
public:
    test_class()
    {
        str = new char[25];
        size_t len = sprintf(str, "this is %d", index++);
        str[len] = 0;
        //cout<<"Constructing tets_class ["<<str<<"] ..."<<endl;
    }
    ~test_class()
    {
        //cout<<"Destructing test_class ["<<str<<"] ..."<<endl;
        delete str;
    }

    void hello(int i)
    {
        //printf("[%d] hello from [%s]\n", i, str);
    }
public:
    char *str;
};

void do_test(void)
{
    index = 0;
    timeval tv1, tv2;
    gettimeofday(&tv1, NULL);
    cout<<"*************************************************************************\n"<<nemo::util::time::format_local_time_ms(tv1)<<endl;
    nemo::object_pool<test_class> pool(INIT_SIZE);
    pool.print_stat();
    nemo::util::random::seed();

    std::list<nemo::object_pool<test_class>::object_ptr> objs;
    for(int lp0=0;lp0<TRY_TIMES;lp0++)
    {
        objs.clear();

        int index = nemo::util::random::random()%INIT_SIZE;
        //cout<<"Round "<<lp0<<", borrow = "<<index<<endl;
        for(int lp1=0;lp1<index;lp1++)
        {
            nemo::object_pool<test_class>::object_ptr obj = pool.alloc();
            obj->obj->hello(lp1);
            objs.push_back(obj);
        }

        std::list<nemo::object_pool<test_class>::object_ptr>::iterator itr = objs.begin();
        while(itr != objs.end())
        {
            nemo::object_pool<test_class>::object_ptr obj = *itr;
            itr = objs.erase(itr);
            pool.free(obj);
        }
    }
    gettimeofday(&tv2, NULL);
    pool.print_stat();
    cout<<"Total take time: "<<nemo::util::time::time_diff_ms(tv1, tv2)<<endl;
    cout<<"*************************************************************************"<<endl;
}

void do_test2(void)
{
    index = 0;
    timeval tv1, tv2;
    gettimeofday(&tv1, NULL);
    cout<<"*************************************************************************\n"<<nemo::util::time::format_local_time_ms(tv1)<<endl;
    nemo::object_pool1<test_class> pool(INIT_SIZE);
    pool.print_stat();
    nemo::util::random::seed();

    std::list<nemo::object_pool1<test_class>::object_ptr> objs;
    for(int lp0=0;lp0<TRY_TIMES;lp0++)
    {
        objs.clear();

        int index = nemo::util::random::random()%INIT_SIZE;
        //cout<<"Round "<<lp0<<", borrow = "<<index<<endl;
        for(int lp1=0;lp1<index;lp1++)
        {
            nemo::object_pool1<test_class>::object_ptr obj = pool.alloc();
            obj->hello(lp1);
            objs.push_back(obj);
        }

        std::list<nemo::object_pool1<test_class>::object_ptr>::iterator itr = objs.begin();
        while(itr != objs.end())
        {
            nemo::object_pool1<test_class>::object_ptr obj = *itr;
            itr = objs.erase(itr);
            pool.free(obj);
        }
    }

    gettimeofday(&tv2, NULL);
    pool.print_stat();
    cout<<"Total take time: "<<nemo::util::time::time_diff_ms(tv1, tv2)<<endl;
    cout<<"*************************************************************************"<<endl;
}

int main(int argc, char *argv[])
{
    do_test();
    //do_test2();
    return 0;
}
