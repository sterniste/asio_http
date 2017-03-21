#ifndef ASIO_HTTP_SERVER_H
#define ASIO_HTTP_SERVER_H

#include <cstddef>
#include <cstdint>
#include <functional>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include <boost/asio.hpp>

#include "http_msgs.h"

namespace asio_http {

using http_request_factory = std::function<http_request*(const std::string&)>;

class http_session {
  boost::asio::ip::tcp::socket sock;
  const http_request_factory make_http_request_fn;
  boost::asio::streambuf sbuf;
  std::unique_ptr<http_request> req;
  std::shared_ptr<std::vector<unsigned char>> content;

  static std::string get_request_line(std::streambuf& sbuf);

 public:
  http_session(boost::asio::io_service& service, const http_request_factory& make_http_request_fn) : sock(service), make_http_request_fn{make_http_request_fn} {}

  boost::asio::ip::tcp::socket& socket() { return sock; }

  void start();

  void handle_request_resource_line(const boost::system::error_code& ec, size_t cnt);
  void handle_request_header_line(const boost::system::error_code& ec, size_t cnt);
  void handle_request_content(const boost::system::error_code& ec, size_t cnt);
  void handle_response(const boost::system::error_code& ec);
};

class http_server {
  boost::asio::io_service& service;
  boost::asio::ip::tcp::acceptor acceptor;
  const http_request_factory make_http_request_fn;

 public:
  http_server(boost::asio::io_service& service, uint16_t port, const http_request_factory& make_http_request_fn);

  std::string get_password() const;
  void handle_accept(http_session* new_session, const boost::system::error_code& ec);
};
}
#endif
