//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server.hpp"
#include <signal.h>
#include <utility>

#define max_connections 4096

namespace http {
namespace server {

server::server(const std::string& address, const std::string& port, const size_t& pool_size,
    const std::string& doc_root)
  :	io_service_pool_(pool_size),
    signals_(io_service_pool_.get_io_service()),
    acceptor_(io_service_pool_.get_io_service()),
    connection_manager_(),
    request_handler_(doc_root),
	new_connection_()
{
  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
#if defined(SIGQUIT)
  signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)

  do_await_stop();

  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  boost::asio::ip::tcp::resolver resolver(acceptor_.get_io_service());
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({address, port});
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen(max_connections);

  do_accept();
}

void server::run()
{
	io_service_pool_.run();
}

void server::stop()
{
	io_service_pool_.stop();
}

void server::register_path_handler(const std::string& url, const HTTPHandler& handler)
{
	request_handler_.register_handler(url, handler);
}

void server::do_accept()
{
  new_connection_.reset(new connection(
			  io_service_pool_.get_io_service(), connection_manager_, request_handler_));
  acceptor_.async_accept(new_connection_->socket(),
      [this](boost::system::error_code ec)
      {
        // Check whether the server was stopped by a signal before this
        // completion handler had a chance to run.
        if (!acceptor_.is_open())
        {
          return;
        }

        if (!ec)
        {
		 
//		  connection_manager_.start(new_connection_);
		  new_connection_->start();
         // connection_manager_.start(std::make_shared<connection>(
         //     std::move(socket_), connection_manager_, request_handler_));
        }

        do_accept();
      });
}

void server::do_await_stop()
{
  signals_.async_wait(
      [this](boost::system::error_code /*ec*/, int /*signo*/)
      {
        // The server is stopped by cancelling all outstanding asynchronous
        // operations. Once all operations have finished the io_service::run()
        // call will exit.
        acceptor_.close();
        connection_manager_.stop_all();
		io_service_pool_.stop();
      });
}

} // namespace server
} // namespace http
