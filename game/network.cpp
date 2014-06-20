#include "network.h"

#include <cassert>

#include <stdexcept>
#include <string>
#include <iostream>

#include <boost/asio/placeholders.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>

namespace castle
{
    namespace net
    {
        Connection::Connection(boost::asio::ip::tcp::socket socket)
            : mReadBuffer()
            , mDataBuffer()
            , mSock(std::move(socket))
        {
            boost::asio::ip::tcp::endpoint peer = mSock.remote_endpoint();
            std::cout << peer.address() << std::endl;
            StartReceive();
        }

        void Connection::StartReceive()
        {
            mSock.async_receive(
                boost::asio::buffer(mReadBuffer),
                boost::bind(&Connection::ReceiveHandler, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
        }

        void Connection::ReceiveHandler(const boost::system::error_code &code, size_t size)
        {
            if(!code) {
                assert(size <= mReadBuffer.size());
                std::string msg(mReadBuffer.data(), mReadBuffer.data() + size);
                std::cout << msg << std::endl;
                StartReceive();
            } else {
                // Report a disconnection issue to the server
            }
        }

        Server::Server(short port)
            : mIO()
            , mPort(port)
            , mAddr(boost::asio::ip::tcp::v4(), port)
            , mAccept(mIO, mAddr)
            , mSock(mIO)
            , mConnections()
        { }

        void Server::StartAccept()
        {
            mAccept.async_accept(
                mSock,
                boost::bind(&Server::AcceptHandler, this, boost::asio::placeholders::error));
        }

        void Server::Poll()
        {
            mIO.poll();
        }
    
        void Server::AcceptHandler(const boost::system::error_code &code)
        {
            if(!code) {
                mConnections.emplace_back(std::move(mSock));
                StartAccept();
            } else {
                // Report an error
            }
        }
    } // namespace net
}
