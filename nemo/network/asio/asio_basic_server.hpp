/*
 * asio_basic_server.hpp
 *
 *  Created on: 2012-11-6
 *      Author: edward
 */

#ifndef ASIO_BASIC_SERVER_HPP_
#define ASIO_BASIC_SERVER_HPP_

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace nemo {
namespace network {

class asio_basic_server : public boost::enable_shared_from_this<asio_basic_server>
{
public:
    typedef boost::asio::ip::tcp::socket tcp_socket;

public:
    asio_basic_server(boost::asio::io_service &io_svc) :
            m_io_svc(io_svc), m_acceptor(io_svc)
    {
    }

    virtual
    ~asio_basic_server(void)
    {

    }

    void
    listen(int port)
    {
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), port);
        boost::system::error_code ec;
        m_acceptor.open(ep.protocol(), ec);
        if (ec)
        {
            printf("error open: %s\n", ec.message().c_str());
        }
        m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        m_acceptor.bind(ep, ec);
        if (ec)
        {
            printf("error bind: %s\n", ec.message().c_str());
        }
        m_acceptor.listen(boost::asio::ip::tcp::socket::max_connections, ec);
        if (ec)
        {
            printf("error listen: %s\n", ec.message().c_str());
        }
        accept();
    }

    void
    accept(void)
    {
        boost::shared_ptr<tcp_socket> peer(new tcp_socket(m_io_svc));
        printf("listen port = %d\n", m_acceptor.local_endpoint().port());
        m_acceptor.async_accept(
                *peer,
                boost::bind(
                        &asio_basic_server::on_accept_wrapper,
                        shared_from_this(),
                        boost::asio::placeholders::error, peer)
        );
    }
    virtual void
    on_accept(const boost::system::error_code &ec, boost::shared_ptr<tcp_socket> peer) = 0;

protected:
    void
    on_accept_wrapper(const boost::system::error_code &ec, boost::shared_ptr<tcp_socket> peer)
    {
        on_accept(ec, peer);
        if (!ec)
        {
            accept();
        }
    }

protected:
    boost::asio::io_service &m_io_svc;
    boost::asio::ip::tcp::acceptor m_acceptor;
    boost::asio::ip::tcp::endpoint m_endpoint;
};

}
}

#endif /* ASIO_BASIC_SERVER_HPP_ */
