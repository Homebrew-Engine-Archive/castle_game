#ifndef NETWORK_H_
#define NETWORK_H_

#include <exception>
#include <array>
#include <vector>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace castle
{
    namespace net
    {
        class accept_error : std::exception
        {
        public:
            explicit accept_error(const boost::system::error_code &code) throw();
            inline const boost::system::error_code& Code() const throw();
            const char* what() const throw();
        private:
            boost::system::error_code mCode;
        };

        class receive_error : std::exception
        {
        public:
            explicit receive_error(const boost::system::error_code &code) throw();
            inline const boost::system::error_code& Code() const throw();
            const char* what() const throw();
        private:
            boost::system::error_code mCode;
        };
        
        class Connection
        {    
        public:
            explicit Connection(boost::asio::ip::tcp::socket sock);
            Connection(Connection const&) = delete;
            Connection& operator=(Connection const&) = delete;
            Connection& operator=(Connection&&) = default;
            Connection(Connection&&) = default;
            virtual ~Connection() = default;

        protected:
            std::array<char, 256> mReadBuffer;
            std::vector<char> mDataQueue;
            boost::asio::ip::tcp::socket mSock;
            void StartReceive();
            void ReceiveHandler(const boost::system::error_code &e, size_t size);
        };

        class Server
        {
        public:
            explicit Server(short port);
            Server(Server const&) = delete;
            Server& operator=(Server const&) = delete;
            virtual ~Server() = default;
            void StartAccept();
            void StopAccept();
            void Poll();
            void AcceptHandler(const boost::system::error_code &e);
        private:
            boost::asio::io_service mIO;
            short mPort;
            boost::asio::ip::tcp::endpoint mAddr;
            boost::asio::ip::tcp::acceptor mAccept;
            boost::asio::ip::tcp::socket mSock;
            std::vector<Connection> mConnections;
        };

        class Client
        {
        public:
            explicit Client(boost::asio::io_service &io, boost::asio::ip::tcp::endpoint addr, short port);
            Client(Client const&) = delete;
            Client& operator=(Client const&) = delete;
            virtual ~Client() = default;
            void StartConnect();
            void EndConnect();
        private:
            boost::asio::io_service &mIO;
            short mPort;
            boost::asio::ip::tcp::endpoint mAddr;
            Connection mConnection;

        };
    }
}

#endif  // NETWORK_H_
