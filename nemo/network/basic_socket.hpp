/*
 * basic_socket.hpp
 *
 *  Created on: 2012-11-8
 *      Author: edward
 */

#ifndef BASIC_SOCKET_HPP_
#define BASIC_SOCKET_HPP_

#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

namespace nemo {
namespace network {

class basic_socket
{
public:
    basic_socket()
    {
        m_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        memset(&m_sa, 0, sizeof(m_sa));
    }
    basic_socket(int port)
    {
        m_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        memset(&m_sa, 0, sizeof(m_sa));
        m_sa.sin_family = AF_INET;
        m_sa.sin_addr.s_addr = INADDR_ANY;
        m_sa.sin_port = htons(port);
    }

protected:
    int m_fd;
    struct sockaddr_in m_sa;
};

}
}


#endif /* BASIC_SOCKET_HPP_ */
