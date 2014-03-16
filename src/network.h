#ifndef NETWORK_H_
#define NETWORK_H_

#include <array>
#include <vector>
#include "SDL.h"
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

class Connection
{
    std::array<char, 256> readBuffer;
    boost::asio::ip::tcp::socket m_sock;
    void StartReceive();
    void ReceiveHandler(const boost::system::error_code &e, size_t size);
    
public:
    Connection(boost::asio::ip::tcp::socket sock);
};

class Server
{
    std::vector<Connection> connections;
    boost::asio::io_service &m_io;
    short m_port;
    boost::asio::ip::tcp::endpoint m_addr;
    boost::asio::ip::tcp::acceptor m_accept;
    boost::asio::ip::tcp::socket m_sock;
    
public:
    Server(boost::asio::io_service &io, short port);
    void StartAccept();
    void AcceptHandler(const boost::system::error_code &e);
};

#endif
