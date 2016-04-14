/*
 * asio_basic_client.hpp
 *
 *  Created on: 2012-11-6
 *      Author: edward
 */

#ifndef ASIO_BASIC_CLIENT_HPP_
#define ASIO_BASIC_CLIENT_HPP_

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/enable_shared_from_this.hpp>

class asio_client_base : public boost::enable_shared_from_this<asio_client_base>
{
public:
    typedef boost::asio::ip::tcp::socket tcp_socket;

public:
    asio_client_base(boost::asio::io_service &io_svc) :
        m_ioSvc(io_svc), m_sock(io_svc)
    {

    }
    virtual ~asio_client_base(void) {
        disconnect();
    }

    void connect(const char *addr, int port)
    {
        boost::asio::ip::tcp::resolver _resolver(m_ioSvc);
        boost::asio::ip::tcp::resolver::query _query(addr, boost::lexical_cast<std::string>(port));
        boost::asio::ip::tcp::resolver::iterator _ep_itr = _resolver.resolve(_query);

        m_sock.async_connect(
            *_ep_itr,
            boost::bind(
                &asio_client_base::on_connect_wrap,
                shared_from_this(),
                boost::asio::placeholders::error)
        );
    }
    virtual void on_connect(const boost::system::error_code &ec) = 0;

    void read_some(char *buf, size_t buf_len)
    {
        m_sock.async_read_some(
            boost::asio::buffer(buf, buf_len),
            boost::bind(
                &asio_client_base::on_read_some,
                shared_from_this(),
                boost::asio::placeholders::error,
                buf,
                boost::asio::placeholders::bytes_transferred)
        );
    }
    virtual void on_read_some(const boost::system::error_code &ec, const char *buf, size_t bytes_read) = 0;

    void write_some(const char *buf, size_t write_len)
    {
        m_sock.async_write_some(
            boost::asio::buffer(buf, write_len),
            boost::bind(
                &asio_client_base::on_write_some,
                shared_from_this(),
                boost::asio::placeholders::error,
                buf,
                boost::asio::placeholders::bytes_transferred)
        );
    }
    virtual void on_write_some(const boost::system::error_code &ec, const char *buf, size_t bytes_write) = 0;

    void disconnect(void)
    {
        if(is_open())
        {
            m_sock.cancel();
            m_sock.shutdown(boost::asio::socket_base::shutdown_both);
            m_sock.close();
        }
    }
    virtual void on_disconnect(void) = 0;

    bool is_open(void)
    {
        return m_sock.is_open();
    }

protected:
    void on_connect_wrap(const boost::system::error_code &ec)
    {
        if(!ec)
        {
            on_connect(ec);
        }
    }

protected:
    boost::asio::io_service &m_ioSvc;
    tcp_socket m_sock;
};


#endif /* ASIO_BASIC_CLIENT_HPP_ */
