#include "network.h"

Server::Server(boost::asio::io_service &io, short port)
    : m_io(io)
    , m_port(port)
    , m_addr(boost::asio::ip::tcp::v4(), m_port)
    , m_accept(m_io, m_addr)
    , m_sock(m_io)
{
}

void Server::StartAccept()
{
    m_accept.async_accept(
        m_sock,
        boost::bind(&Server::AcceptHandler, this, boost::asio::placeholders::error));
}

void Server::AcceptHandler(const boost::system::error_code &code)
{
    if(!code) {
        connections.emplace_back(std::move(m_sock));
        StartAccept();
    } else {
        // Report an error
    }
}

Connection::Connection(boost::asio::ip::tcp::socket socket)
    : m_sock(std::move(socket))
{
    boost::asio::ip::tcp::endpoint peer = m_sock.remote_endpoint();
    std::cout << peer.address() << std::endl;
    StartReceive();
}

void Connection::StartReceive()
{
    m_sock.async_receive(
        boost::asio::buffer(readBuffer),
        boost::bind(&Connection::ReceiveHandler, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void Connection::ReceiveHandler(const boost::system::error_code &code, size_t size)
{
    if(!code) {
        std::string msg(&readBuffer[0], &readBuffer[0] + size);
        std::cout << msg << std::endl;
        StartReceive();
    } else {
        // Report a disconnection issue to the server
    }
}
