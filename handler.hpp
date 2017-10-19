//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_HANDLE_HPP
#define HTTP_HANDLE_HPP

#include <string>
#include <boost/function.hpp>
#include "request.hpp"
#include "reply.hpp"

namespace http {
namespace server {

typedef boost::function<void (const request&, reply&)> HTTPHandler;

} // namespace server
} // namespace http

#endif // HTTP_SERVER_HPP