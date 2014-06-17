#ifndef NETWORK_H_
#define NETWORK_H_

#include <array>
#include <vector>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace Castle
{
    namespace Network
    {
        class Connection
        {
            std::array<char, 256> mReadBuffer;
            std::vector<char> mDataBuffer;
            boost::asio::ip::tcp::socket mSock;
            void StartReceive();
            void ReceiveHandler(const boost::system::error_code &e, size_t size);
    
        public:
            explicit Connection(boost::asio::ip::tcp::socket sock);
            Connection(Connection const&) = delete;
            Connection& operator=(Connection const&) = delete;
            Connection& operator=(Connection&&) = default;
            Connection(Connection&&) = default;
            virtual ~Connection() = default;
        };

        class Server
        {
            boost::asio::io_service mIO;
            short mPort;
            boost::asio::ip::tcp::endpoint mAddr;
            boost::asio::ip::tcp::acceptor mAccept;
            boost::asio::ip::tcp::socket mSock;
            std::vector<Connection> mConnections;
        public:
            explicit Server(short port);
            Server(Server const&) = delete;
            Server& operator=(Server const&) = delete;
            virtual ~Server() = default;
        
            void StartAccept();
            void Poll();
            void AcceptHandler(const boost::system::error_code &e);
        };

        class Client
        {
            boost::asio::io_service &mIO;
            short mPort;
            boost::asio::ip::tcp::endpoint mAddr;
            Connection mConnection;

        public:
            explicit Client(boost::asio::io_service &io, boost::asio::ip::tcp::endpoint addr, short port);
            Client(Client const&) = delete;
            Client& operator=(Client const&) = delete;
            virtual ~Client() = default;

            void StartConnect();
            void EndConnect();
        };
    }
}

#endif  // NETWORK_H_
