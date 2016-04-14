/*
 * tester.cc
 *
 *  Created on: 2012-11-5
 *      Author: edward
 */

#ifndef TESTER_CC_
#define TESTER_CC_

#include <nemo/thread.hpp>
#include <iostream>

using namespace std;

class my_thread: public nemo::thread::thread
{
public:
    my_thread(): nemo::thread::thread::thread(), m_id(0) {}
    virtual ~my_thread() {}

    void set_id(int id)
    {
        m_id = id;
    }

    virtual void run()
    {
        cout<<"hello: "<<m_id<<endl;
        usleep(200*1000);
    }

protected:
    int m_id;
};

void do_test(void)
{
    my_thread th1, th2;
    th1.set_id(1 );
    th2.set_id(2);
    th1.start();
    th2.start();
    sleep(1);
    th2.pause();
    sleep(1);
    th2.resume();
    sleep(1);
    th1.stop();
    th2.stop();
}

int main(int argc, char *argv[])
{
    do_test();
    return 0;
}


#endif /* TESTER_CC_ */
