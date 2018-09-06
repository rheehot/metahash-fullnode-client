#include "http_server.h"
#include "http_session.h"
#include "settings/settings.h"

#include <iostream>

http_server::http_server(unsigned short port /*= 9999*/, int thread_count /*= 4*/)
    : m_io_ctx(thread_count)
{
    m_ep.port(port);
}

http_server::~http_server()
{

}

void http_server::run()
{
    tcp::acceptor acceptor(m_io_ctx, m_ep, true);
    accept(acceptor);

    std::cout << "Service runing at " << m_ep.address().to_string() << ":" << m_ep.port() << std::endl;

    m_io_ctx.run();
}

void http_server::stop()
{

}

void http_server::accept(tcp::acceptor& acceptor)
{
    acceptor.async_accept(m_sock, m_peer, [&](beast::error_code ec)
    {
        if (!ec)
        {
            if (check_access(m_peer))
                std::make_shared<http_session>(std::move(m_sock))->run();
            else
            {
                m_sock.shutdown(tcp::socket::shutdown_both);
                m_sock.close();
            }
        }
        accept(acceptor);
    });
}

bool http_server::check_access(tcp::endpoint& ep)
{
    if (ep.address().is_loopback())
        return true;

    if (std::find(settings::service::access.begin(),
              settings::service::access.end(),
              ep.address().to_string()) != settings::service::access.end())
        return true;

    return false;
}
