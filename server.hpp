//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "connection.hpp"
#include "connection_manager.hpp"
#include "request_handler.hpp"
#include "io_service_pool.hpp"
#include "handler.hpp"
#include <boost/asio.hpp>
#include <string>
#include <boost/function.hpp>

namespace http {
namespace server {

/// The top-level class of the HTTP server.
class server
{
public:
  server(const server&) = delete;
  server& operator=(const server&) = delete;

  /// Construct the server to listen on the specified TCP address and port, and
  /// serve up files from the given directory.
  explicit server(const std::string& address, const std::string& port, const size_t& pool_size,
      const std::string& doc_root);

  /// Run the server's io_service loop.
  void run();

  /// Stop the server's io-service loop.
  void stop();

  /// Set handler to server
  void register_path_handler(const std::string& url, const HTTPHandler& handler);

private:
  /// Perform an asynchronous accept operation.
  void do_accept();

  /// Wait for a request to stop the server.
  void do_await_stop();

  /// The pool of io_service objects used to perform asynchronous operations.
  io_service_pool io_service_pool_;

  /// The io_service used to perform aynnchronous operations.
  /// boost::asio::io_service& io_service_;

  /// The signal_set is used to register for process termination notifications.
  boost::asio::signal_set signals_;

  /// Acceptor used to listen for incoming connections.
  boost::asio::ip::tcp::acceptor acceptor_;

  /// The connection manager which owns all live connections.
  connection_manager connection_manager_;

  /// The handler for all incoming requests.
  request_handler request_handler_;
  
  /// The next socket to be accepted.
  ///boost::asio::ip::tcp::socket socket_;
  /// The next connection to be accepted.
  connection_ptr new_connection_;

};

} // namespace server
} // namespace http

#endif // HTTP_SERVER_HPP