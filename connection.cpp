//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"
#include <utility>
#include <vector>
#include "connection_manager.hpp"
#include "request_handler.hpp"

namespace http {
namespace server {

connection::connection(boost::asio::io_service& io_service,
    connection_manager& manager, request_handler& handler,
		int timeoutmillsecs)
  : socket_(io_service),
    connection_manager_(manager),
    request_handler_(handler),
		millsec_(timeoutmillsecs),
		timer_(io_service)
{
}

connection::~connection()
{
	stop();
}
void connection::start()
{
//  timer_.expires_from_now(boost::posix_time::millisec(millsec_)); 
  do_read();
	// timer_.async_wait([this](boost::system::error_code ec)
  // {
	//   if ( !ec ){
  //      do_timeout();
	//   }
	// });
}

void connection::stop()
{
  socket_.close();
}


void connection::do_timeout()
{
  reply_ = reply::stock_reply(reply::no_content);
	do_write();
}


void connection::do_read()
{
  auto self(shared_from_this());
  socket_.async_read_some(boost::asio::buffer(buffer_),
      [this, self](boost::system::error_code ec, std::size_t bytes_transferred)
      {
        if (!ec)
        {
          request_parser::result_type result;
          std::tie(result, std::ignore) = request_parser_.parse(
              request_, buffer_.data(), buffer_.data() + bytes_transferred);

          if (result == request_parser::good)
          {
            request_handler_.handle_request(request_, reply_);
            do_write();
          }
          else if (result == request_parser::bad)
          {
            reply_ = reply::stock_reply(reply::bad_request);
            do_write();
          }
          else
          {
            do_read();
          }
        }
        else if (ec != boost::asio::error::operation_aborted)
        {
          connection_manager_.stop(shared_from_this());
        }
				timer_.cancel();
      });
}

void connection::do_write()
{
  auto self(shared_from_this());
  boost::asio::async_write(socket_, reply_.to_buffers(),
      [this, self](boost::system::error_code ec, std::size_t)
      {
        if (!ec)
        {
          // Initiate graceful connection closure.
          boost::system::error_code ignored_ec;
          socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
            ignored_ec);
        }

        if (ec != boost::asio::error::operation_aborted)
        {
  //        connection_manager_.stop(shared_from_this());
            stop();
        }
      });
}

} // namespace server
} // namespace http
