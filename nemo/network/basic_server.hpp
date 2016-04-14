/*
 * basic_server.hpp
 *
 *  Created on: 2012-11-6
 *      Author: edward
 */

#ifndef BASIC_SERVER_HPP_
#define BASIC_SERVER_HPP_

#include <nemo/config.hpp>

#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <map>
#include <tr1/memory>

namespace nemo {
namespace network {

class endpoint
{
public:
    endpoint(void)
    {
        memset(&m_sa, 0, sizeof(m_sa));
    }
    endpoint(const struct sockaddr_in &sa)
    {
        memcpy(&m_sa, &sa, sizeof(m_sa));
    }
    endpoint(const std::string &addr, unsigned short port)
    {
        m_sa.sin_addr.s_addr = inet_addr(addr.c_str());
        m_sa.sin_port = htons(port);
    }
    virtual ~endpoint(void) {}
    endpoint &operator =(const endpoint &ep)
    {
        if(&ep == this){
            return *this;
        }
        memcpy(&m_sa, &ep.m_sa, sizeof(m_sa));
        return *this;
    }

    void addr(const std::string &addr)
    {
        m_sa.sin_addr.s_addr = inet_addr(addr.c_str());
    }
    std::string addr(void) const
    {
        return std::string(inet_ntoa(m_sa.sin_addr));
    }
    void port(unsigned short port)
    {
        m_sa.sin_port = htons(port);
    }
    unsigned short port(void) const
    {
        return ntohs(m_sa.sin_port);
    }
    const struct sockaddr_in &sockaddr(void)
    {
        return m_sa;
    }

protected:
    struct sockaddr_in m_sa;
};

class basic_socket
{
public:
    basic_socket(void): m_sock_fd(0), m_isopen(false) {}
    basic_socket(int sock_fd): m_sock_fd(sock_fd), m_isopen(false) {}
    virtual ~basic_socket(void) {}
    basic_socket &operator =(const basic_socket &obj)
    {
        if(&obj == this){
            return *this;
        }
        m_sock_fd = obj.m_sock_fd;
        m_isopen = obj.m_isopen;
        m_ep_local = obj.m_ep_local;
        m_ep_remote = obj.m_ep_remote;
        return *this;
    }

    void local_endpoint(const endpoint &ep)
    {
        m_ep_local = ep;
    }
    const endpoint &local_endpoint(void) const
    {
        return m_ep_local;
    }
    void remote_endpoint(const endpoint &ep)
    {
        m_ep_remote = ep;
    }
    const endpoint &remote_endpoint(void) const
    {
        return m_ep_remote;
    }
    void close()
    {
        if(m_isopen)
        {
            ::shutdown(m_sock_fd, SHUT_RDWR);
            ::close(m_sock_fd);
        }
    }
    void fd(int fd)
    {
        m_sock_fd = fd;
    }
    int fd(void) const
    {
        return m_sock_fd;
    }

protected:
    int m_sock_fd;
    bool m_isopen;
    endpoint m_ep_local;
    endpoint m_ep_remote;
};

class basic_session
{
public:
    basic_session(basic_socket &sock)
        :m_sock(sock), m_buf_offset(0)
    {

    }
    virtual ~basic_session()
    {

    }

    basic_socket &sock(void)
    {
        return m_sock;
    }

    size_t consume_data(void)
    {
        m_buf[m_buf_offset] = 0;
        printf("consumed %zd bytes\n", m_buf_offset);
        std::string reply = "HTTP/1.1 200 OK\r\n"\
                    "Date: Wed, 08 Sep 2010 01:52:12 GMT\r\n"\
                    "Content-Length: 50\r\n"\
                    "Content-Type: text/html; charset=\"utf-8\"\r\n"\
                    "\r\n"\
                    "<html><body><h1>Hello, world!</h1></body></html>\r\n";
        write_some(reply.c_str(), reply.size());
        return m_buf_offset;
    }

    int read_some(int fd, int flag = 0)
    {
        printf("I am reading\n");
        int ret = recv(fd, m_buf, sizeof(m_buf)-m_buf_offset, flag);
        if(ret > 0)
        {
            m_buf_offset += ret;
            m_buf_offset -= consume_data();
        }
        return ret;
    }
    int write_some(const char *buf, size_t len)
    {
        return send(m_sock.fd(), buf, len, 0);
    }

protected:
    basic_socket m_sock;
    char m_buf[NEMO_BUF_SZ_XL];
    size_t m_buf_offset;
};

class basic_server
{
public:
    typedef std::map<int, std::tr1::shared_ptr<basic_session> > session_map_t;
    typedef std::tr1::shared_ptr<basic_session> session_ptr_t;

public:
    basic_server(void)
    {
        m_sock_listen = 0;
        m_listen_flag = true;
    }
    virtual ~basic_server(void) {}

    void listen(int port, const std::string &addr = "")
    {
        int ret = 0;
        ret = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        m_sock_listen = ret;
        if(ret < 0)
        {
            perror("create socket error");
            return;
        }

        int so_reuseaddr = 1;
        ret = setsockopt(m_sock_listen, SOL_SOCKET, SO_REUSEADDR, (void *)&so_reuseaddr, sizeof(so_reuseaddr));
        if(ret < 0)
        {
            perror("setsockopt error");
            return;
        }
        int flags = fcntl(m_sock_listen, F_GETFL, 0);
        ret = fcntl(m_sock_listen, F_SETFL, flags | O_NONBLOCK);
        if(ret < 0)
        {
            perror("fcntl error");
            return;
        }

        struct sockaddr_in sa;
        memset(&sa, 0, sizeof(sa));
        sa.sin_family = AF_INET;
        if(addr.size() <= 0)
        {
            sa.sin_addr.s_addr = INADDR_ANY;
        }
        else
        {
            sa.sin_addr.s_addr = inet_addr(addr.c_str());
        }
        sa.sin_port = htons(port);
        ret = bind(m_sock_listen, (struct sockaddr *)&sa, sizeof(sa));
        if(ret < 0)
        {
            perror("bind socket error");
            return;
        }

        ret = ::listen(m_sock_listen, NEMO_SOCK_MAX_CONN);
        if(ret < 0)
        {
            perror("listen socket error");
            return;
        }

        m_sock_local.fd(m_sock_listen);
    }

    void
    accept()
    {
        pthread_t th;
        int retval;
        retval = pthread_create(&th, NULL, basic_server::do_accept, this);
        if(retval < 0)
        {
            perror("create thread error");
            return;
        }
        while(true)
        {
            printf("Current [%d] sessions\n", m_sessions.size());
            session_map_t::const_iterator itr = m_sessions.begin();
            while(itr != m_sessions.end())
            {
                printf("fd=%d, %s:%d\n", itr->first, itr->second->sock().remote_endpoint().addr().c_str(), itr->second->sock().remote_endpoint().port());
                itr++;
            }
            sleep(3);
        }
        //ret_int = pthread_join(th, NULL);
    }

protected:
    static void *do_accept(void *param)
    {
        basic_server *obj = static_cast<basic_server *>(param);

        int retval;
        struct epoll_event ep_evt, ep_evts[NEMO_EPOLL_MAX_EVENT];
        int ep_fd, ep_evt_num;
        struct sockaddr_in sa;
        socklen_t sa_len;

        retval = epoll_create(NEMO_EPOLL_MAX_FD);
        if(retval < 0)
        {
            perror("create epoll error");
            return NULL;
        }
        ep_fd = retval;

        ep_evt.data.fd = obj->m_sock_local.fd();
        ep_evt.events = EPOLLIN | EPOLLET; // listen fd在并发不大的情况下大可不必采用ET模式
        retval = epoll_ctl(ep_fd, EPOLL_CTL_ADD, obj->m_sock_local.fd(), &ep_evt);

//        char buf[NEMO_BUF_SZ_XL];
//        size_t recv_len;
        while(obj->m_listen_flag)
        {
            retval = epoll_wait(ep_fd, ep_evts, NEMO_EPOLL_MAX_EVENT, 100);
            if(retval < 0)
            {
                perror("epoll wait error");
                break;
            }
            ep_evt_num = retval;
            for(int lp0 = 0;lp0 < ep_evt_num;lp0++)
            {
                const int evt_fd = ep_evts[lp0].data.fd;
                const uint32_t evts = ep_evts[lp0].events;

                DBG("evt_num = %d, index = %d", ep_evt_num, lp0);

                if(evt_fd == obj->m_sock_local.fd())
                {
                    DBG("enter");
                    sa_len = sizeof(sa);
                    while((retval = ::accept(obj->m_sock_local.fd(), (struct sockaddr *)&sa, &sa_len)) > 0)
                    {
                        int _flag = fcntl(retval, F_GETFL, 0);
                        DBG("fd flag of new connection = %d, nonblock=%d, is nonblock=%s", _flag, O_NONBLOCK, (_flag&O_NONBLOCK) == O_NONBLOCK?"true":"false");
                        retval = fcntl(retval, F_SETFL, _flag | O_NONBLOCK);
                        printf("new connections fd=%d\n", retval);
                        basic_socket sock_peer(retval);
                        char tmp_buf[64];
                        strcpy(tmp_buf, inet_ntoa(sa.sin_addr));
                        endpoint ep_peer(tmp_buf, ntohs(sa.sin_port));
                        sock_peer.remote_endpoint(ep_peer);
                        std::cout<<sock_peer.remote_endpoint().addr()<<":"<<sock_peer.remote_endpoint().port()<<" has connected!"<<std::endl;

                        ep_evt.data.fd = sock_peer.fd();
                        ep_evt.events = EPOLLERR | EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLPRI | EPOLLET;
                        retval = epoll_ctl(ep_fd, EPOLL_CTL_ADD, sock_peer.fd(), &ep_evt);
                        if(retval < 0)
                        {
                            perror("epoll add peer fd error");
                        }

                        std::tr1::shared_ptr<basic_session> new_sess(new basic_session(sock_peer));
                        obj->m_sessions.insert(session_map_t::value_type(new_sess->sock().fd(), new_sess));
                        DBG("exit 1");
                    }
                    DBG("middle, retval = %d", retval);
                    if(retval <= 0)
                    {
                        printf("%s.%s:%d - retval=%d\n", __FILE__, __FUNCTION__, __LINE__, retval);
                        if(errno == EAGAIN || errno == EWOULDBLOCK)
                        {
                            //recv done
                            perror("recv done");
                        }
                        else
                        {
                            perror("accept error");
                        }
                    }
                    DBG("exit");
                    continue;
                }
                else
                {
                    session_map_t::iterator sitr = obj->m_sessions.find(evt_fd);
                    if(sitr == obj->m_sessions.end())
                    {
                        printf("Session must be managed, BUG!!!\n");
                        epoll_ctl(ep_fd, EPOLL_CTL_DEL, evt_fd, NULL);
                        continue;
                    }
                    if(evts & EPOLLERR)
                    {
                        perror("socket error");
                        //close(evt_fd);
                        sitr->second->sock().close();
                        obj->m_sessions.erase(sitr);

                        retval = epoll_ctl(ep_fd, EPOLL_CTL_DEL, evt_fd, NULL);
                        if(retval < 0)
                        {
                            perror("epoll delete fd error");
                            continue;
                        }
                        continue;
                    }
                    else if(evts & EPOLLIN)
                    {
                        /*
                         * 这里采用的是ET模式，如果不一次读取完所有的缓存内容，那么该fd将再也不会被触发EPOLLIN
                         */
                        while(true)
                        {
//                            recv_len = 0;
                            // 直接接收到session的buffer中避免一次copy操作
                            retval = sitr->second->read_some(evt_fd);
                            if(retval < 0) {
                                // 由于是ET模式，这里必须采用非阻塞的模式,所以当errno为EAGAIN时,表示当前缓冲区已无数据可读
                                // 在这里就当作是该次事件已处理处.
                                // 只有当错误为EAGAIN或EWOULDBLOCK的时候才表示接收完毕
                                if(errno == EAGAIN || errno == EWOULDBLOCK)
                                {
                                    perror("recv done");
                                }
                                else // 其他错误，关闭fd并注销epoll监听
                                {
                                    perror("recv error");
                                    //close(evt_fd);
                                    sitr->second->sock().close();
                                    obj->m_sessions.erase(sitr);
                                    epoll_ctl(ep_fd, EPOLL_CTL_DEL, evt_fd, NULL);
                                }
                                break;
                            }
                            else if(retval == 0) // 对端正常关闭，关闭fd并注销epoll监听
                            {
                                printf("peer reset\n");
                                //close(evt_fd);
                                sitr->second->sock().close();
                                obj->m_sessions.erase(sitr);
                                epoll_ctl(ep_fd, EPOLL_CTL_DEL, evt_fd, NULL);
                                break;
                            }
                            else
                            {
//                                recv_len += retval;
                                continue;
                            }
                        }
                        // 本次EPOLLIN事件读取完成
                        // TODO - 不需要了，在session的read_some中处理数据
//                        if(recv_len > 0)
//                        {
//                            printf("%zd bytes receved\n", recv_len);
//                        }
                    }
                    else if(evts & EPOLLOUT)
                    {
                        printf("epoll_out triggered\n");
                    }
                    else
                    {
                        printf("Other epoll events\n");
                    }
                }
            }
        }

        close(ep_fd);
        obj->m_sock_local.close();

        return NULL;
    }

protected:
    int m_sock_listen;
    basic_socket m_sock_local;
    bool m_listen_flag;
    session_map_t m_sessions;
};

}
}


#endif /* BASIC_SERVER_HPP_ */
