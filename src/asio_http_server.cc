#include <cstddef>
#include <cstdint>
#include <iostream>
#include <istream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "asio_http_server.h"
#include "http_msgs.h"

namespace asio_http {
using namespace std;
using namespace boost;
using namespace boost::asio;

string
http_session::get_request_line(std::streambuf& sbuf) {
  istream is{&sbuf};
  char buf[1024];
  if (!is.getline(buf, sizeof(buf), '\n'))
    throw runtime_error{"bad request line!"};
  string line{buf};
  trim_if(line, is_any_of("\r\t "));
  return line;
}

void
http_session::start() {
  async_read_until(sock, sbuf, '\n', boost::bind(&http_session::handle_request_resource_line, this, asio::placeholders::error, asio::placeholders::bytes_transferred));
}

void
http_session::handle_request_resource_line(const system::error_code& ec, size_t cnt) {
  cout << "handle_request_resource_line" << endl;
  if (ec) {
    cout << static_cast<ostringstream&>(ostringstream{} << "handle_request_resource_line ec=" << ec).str() << endl;
    delete this;
    return;
  }
  req = unique_ptr<http_request>{make_http_request_fn(get_request_line(sbuf))};
  async_read_until(sock, sbuf, '\n', boost::bind(&http_session::handle_request_header_line, this, asio::placeholders::error, asio::placeholders::bytes_transferred));
}

void
http_session::handle_request_header_line(const system::error_code& ec, size_t cnt) {
  cout << "handle_request_header_line" << endl;
  if (ec) {
    cout << static_cast<ostringstream&>(ostringstream{} << "handle_request_header_line ec=" << ec).str() << endl;
    delete this;
    return;
  }
  if (req->add_header(get_request_line(sbuf)))
    async_read_until(sock, sbuf, '\n', boost::bind(&http_session::handle_request_header_line, this, asio::placeholders::error, asio::placeholders::bytes_transferred));
  else {
    const auto content_length{req->content_length()};
    if (content_length > 0) {
      content = make_shared<vector<unsigned char>>(content_length);
      async_read(sock, buffer(content.get(), content_length), boost::bind(&http_session::handle_request_content, this, asio::placeholders::error, asio::placeholders::bytes_transferred));
    } else {
      auto resp{make_shared<string>(req->response().str())};
      async_write(sock, buffer(resp->data(), resp->length()), boost::bind(&http_session::handle_response, this, asio::placeholders::error));
    }
  }
}

void
http_session::handle_request_content(const system::error_code& ec, size_t cnt) {
  cout << "handle_request_content" << endl;
  if (ec) {
    cout << static_cast<ostringstream&>(ostringstream{} << "handle_request_content ec=" << ec).str() << endl;
    delete this;
    return;
  }
  req->set_content(content);
  std::shared_ptr<string> resp{make_shared<string>(req->response().str())};
  async_write(sock, buffer(resp->data(), resp->length()), boost::bind(&http_session::handle_response, this, asio::placeholders::error));
}

void
http_session::handle_response(const system::error_code& ec) {
  cout << "handle_response" << endl;
  if (ec) {
    cout << static_cast<ostringstream&>(ostringstream{} << "handle_response ec=" << ec).str() << endl;
    delete this;
    return;
  }
  async_read_until(sock, sbuf, '\n', boost::bind(&http_session::handle_request_resource_line, this, asio::placeholders::error, asio::placeholders::bytes_transferred));
}

http_server::http_server(io_service& service, uint16_t port, const http_request_factory& make_http_request_fn) : service(service), acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), port)), make_http_request_fn{make_http_request_fn} {
  http_session* new_session = new http_session(service, make_http_request_fn);
  acceptor.async_accept(new_session->socket(), boost::bind(&http_server::handle_accept, this, new_session, asio::placeholders::error));
}

string
http_server::get_password() const {
  return "test";
}

void
http_server::handle_accept(http_session* new_session, const system::error_code& ec) {
  cout << "handle_accept" << endl;
  if (ec) {
    cout << static_cast<ostringstream&>(ostringstream{} << "handle_accept ec=" << ec).str() << endl;
    delete new_session;
    return;
  }
  new_session->start();
  new_session = new http_session(service, make_http_request_fn);
  acceptor.async_accept(new_session->socket(), boost::bind(&http_server::handle_accept, this, new_session, asio::placeholders::error));
}
}
