//
// Created by fran on 06/07/2026.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <ctime>
#include <string>
#include "SimulatedFinanceDataSource.hpp"

std::string make_daytime_string()
{
    std::time_t now = std::time(0);
    
    return std::ctime(&now);
}

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    static std::shared_ptr<TcpConnection> create(boost::asio::io_context& io_context)
    {
        return std::shared_ptr<TcpConnection>(new TcpConnection(io_context));
    }

    boost::asio::ip::tcp::socket& getSocket()
    {
        return m_socket;
    }

    void start()
    {
        m_message = make_daytime_string();
        boost::asio::async_write(m_socket, boost::asio::buffer(m_message),
            std::bind(&TcpConnection::handle_write, shared_from_this(), 
                boost::asio::placeholders::error, 
                boost::asio::placeholders::bytes_transferred));
    }

private:
    TcpConnection(boost::asio::io_context& io_context)
        : m_socket(io_context)
    {
    }
    
    void handle_write(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/)
    {
        
    }

    boost::asio::ip::tcp::socket m_socket;
    std::string m_message;
};

class TcpServer {
public:
    TcpServer(boost::asio::io_context& io_context, unsigned int port)
        : m_ioContext(io_context)
        , m_acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    {
        start_accept();
    }

private:
    void start_accept()
    {
        std::shared_ptr<TcpConnection> new_connection = TcpConnection::create(m_ioContext);
        m_acceptor.async_accept(new_connection->getSocket(),
            std::bind(&TcpServer::handle_accept, this, new_connection, boost::asio::placeholders::error));
    }

    void handle_accept(std::shared_ptr<TcpConnection> new_connection, const boost::system::error_code& error)
    {
        if (!error)
            new_connection->start();
        start_accept();
    }
    
    boost::asio::io_context& m_ioContext;
    boost::asio::ip::tcp::acceptor m_acceptor;
};


void tcp_server(boost::asio::io_context& io_context, unsigned int port)
{
    try {
        boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));

        for (;;) {
            boost::asio::ip::tcp::socket socket(io_context);
            acceptor.accept(socket);
            std::string message = make_daytime_string();
            boost::system::error_code ignored_error;
            boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
            std::cout << "Sent message: " << message << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

int main()
{
    std::cout << "Hello from the server !" << std::endl;

    try {
        boost::asio::io_context io;
        TcpServer server(io, 13);
        io.run();
        // tcp_server(io, 13);
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;

    // boost::asio::io_context io;
    // FinancialAssetConfig config{"GOLD", 180.0, 0.0001, 0.02};
    // SimulatedFinanceDataSource source(io, boost::asio::chrono::milliseconds(20), config);

    // std::vector<Message> collected;
    // source.start([&](const Message& m) {
    //     collected.push_back(m);
    // });
    // io.run_for(std::chrono::seconds(5));
    // source.stop();

    // std::cout << "collected " << collected.size() << " messages\n";
    // std::ofstream out("finance_output.csv");
    // out << "timestamp_ns,topic,price\n";
    // for (const auto& m : collected)
    //     out << m.timestampNs << "," << m.topic << "," << m.values.at(0) << "\n";
    // std::cout << "CSV written to finance_output.csv\n";
    // return 0;
}