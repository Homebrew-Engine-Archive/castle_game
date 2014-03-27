#ifndef NETWORK_H_
#define NETWORK_H_

#include <array>
#include <vector>
#include <SDL.h>
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace Network
{

    class Connection
    {
        std::array<char, 256> mReadBuffer;
        boost::asio::ip::tcp::socket mSock;
        void StartReceive();
        void ReceiveHandler(const boost::system::error_code &e, size_t size);
    
    public:
        Connection(boost::asio::ip::tcp::socket sock);
    };

    class Server
    {
        boost::asio::io_service &mIO;
        short mPort;
        boost::asio::ip::tcp::endpoint mAddr;
        boost::asio::ip::tcp::acceptor mAccept;
        boost::asio::ip::tcp::socket mSock;
        std::vector<Connection> mConnections;
    public:
        Server(boost::asio::io_service &io, short port);
        void StartAccept();
        void AcceptHandler(const boost::system::error_code &e);
    };
    
}

#endif
