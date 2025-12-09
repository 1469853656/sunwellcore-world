#ifndef NETWORKING_ASYNCACCEPT_H
#define NETWORKING_ASYNCACCEPT_H

#include "IoContext.h"
#include "Log.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/thread_pool.hpp>
#include <functional>
#include <atomic>

using boost::asio::ip::tcp;

class AsyncAcceptor
{
public:
    typedef void(*AcceptCallback)(tcp::socket&& newSocket, uint32 threadIndex);

    AsyncAcceptor(std::string const& bindIp, uint16 port) :
        _execution_context(1),
        _acceptor(_execution_context), // this is one-thread pool only for accepting incoming connections
        _endpoint(boost::asio::ip::make_address(bindIp), port),
        _socket(_execution_context), _closed(false), _socketFactory(std::bind(&AsyncAcceptor::DefeaultSocketFactory, this))
    {
    }

    template<class T>
    void AsyncAccept();

    template<AcceptCallback acceptCallback>
    void AsyncAcceptWithCallback()
    {
        tcp::socket* socket;
        uint32 threadIndex;
        std::tie(socket, threadIndex) = _socketFactory();
        _acceptor.async_accept(*socket, [this, socket, threadIndex](boost::system::error_code error)
        {
            if (!error)
            {
                try
                {
                    socket->non_blocking(true);

                    acceptCallback(std::move(*socket), threadIndex);
                }
                catch (boost::system::system_error const& err)
                {
                    sLog->outError("Failed to initialize client's socket %s", err.what());
                }
            }

            if (!_closed)
                this->AsyncAcceptWithCallback<acceptCallback>();
        });
    }

    bool Bind()
    {
        boost::system::error_code errorCode;
        _acceptor.open(_endpoint.protocol(), errorCode);
        if (errorCode)
        {
            sLog->outError("Failed to open acceptor %s", errorCode.message().c_str());
            return false;
        }

#if PLATFORM != PLATFORM_WINDOWS
        _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), errorCode);
        if (errorCode)
        {
            sLog->outError("Failed to set reuse_address option on acceptor %s", errorCode.message().c_str());
            return false;
        }
#endif

        _acceptor.bind(_endpoint, errorCode);
        if (errorCode)
        {
            sLog->outError("Could not bind to %s:%u %s", _endpoint.address().to_string().c_str(), _endpoint.port(), errorCode.message().c_str());
            return false;
        }

        _acceptor.listen(boost::asio::socket_base::max_listen_connections, errorCode);
        if (errorCode)
        {
            sLog->outError("Failed to start listening on %s:%u %s", _endpoint.address().to_string().c_str(), _endpoint.port(), errorCode.message().c_str());
            return false;
        }

        return true;
    }

    void Close()
    {
        if (_closed.exchange(true))
            return;

        boost::system::error_code err;
        _acceptor.close(err);
    }

    void SetSocketFactory(std::function<std::pair<tcp::socket*, uint32>()> func) { _socketFactory = func; }

private:
    std::pair<tcp::socket*, uint32> DefeaultSocketFactory() { return std::make_pair(&_socket, 0); }

    boost::asio::thread_pool _execution_context;
    tcp::acceptor _acceptor;
    tcp::endpoint _endpoint;
    tcp::socket _socket;
    std::atomic<bool> _closed;
    std::function<std::pair<tcp::socket*, uint32>()> _socketFactory;
};

template<class T>
void AsyncAcceptor::AsyncAccept()
{
    _acceptor.async_accept(_socket, [this](boost::system::error_code error)
    {
        if (!error)
        {
            try
            {
                // this-> is required here to fix an segmentation fault in gcc 4.7.2 - reason is lambdas in a templated class
                std::make_shared<T>(std::move(this->_socket))->Start();
            }
            catch (boost::system::system_error const& err)
            {
                sLog->outError("Failed to retrieve client's remote address %s", err.what());
            }
        }

        // lets slap some more this-> on this so we can fix this bug with gcc 4.7.2 throwing internals in yo face
        if (!_closed)
            this->AsyncAccept<T>();
    });
}

#endif
