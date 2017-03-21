#include <functional>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/asio.hpp>

#include "app_http_server.h"
#include "asio_http_server.h"
#include "http_msgs.h"

namespace app {
using namespace std;
using namespace boost::asio;
using namespace asio_http;

void
app_http_request::log_request(const http_request& req) {
  ostringstream oss;
  oss << "HTTP request: " << req.get_method() << ' ' << req.uri_str();
  for (const auto& header : req.get_headers())
    oss << '<' << header.first << "='" << header.second << "'>";
  const auto content_length = req.content_length();
  if (content_length > 0) {
    assert(req.get_content() && req.get_content()->size() == content_length);
    oss << '{';
    for (unsigned char c : *req.get_content()) {
      switch (c) {
      case '\\':
        oss << "\\\\";
        break;
      case '\t':
        oss << "\\t";
        break;
      case '\r':
        oss << "\\r";
        break;
      case '\n':
        oss << "\\n";
        break;
      case '?':
        oss << "\\?";
        break;
      default:
        if (c < ' ' || c >= 0x7f)
          oss << '?';
        else
          oss << c;
        break;
      }
    }
    oss << '}';
  }
  cout << oss.str() << endl;
}

void
app_http_request::log_response(const http_request& req, const http_response& resp) {
  ostringstream oss;
  oss << "HTTP reply to: " << req.get_method() << ' ' << req.uri_str();
  for (const auto& header : resp.get_headers())
    oss << '<' << header.first << "='" << header.second << "'>";
  cout << oss.str() << endl;
}

string
app_http_request::build_get_tasks_reply_json() const {
  return "[]";
}

http_response
app_http_request::handle_tasks_get_request() const {
  return http_response{http_status_code::ok, {{http_response::content_type_key, "application/json"}}, build_get_tasks_reply_json()};
}

http_request*
app_http_request::make_http_request(const string& http_resource_line) {
  return new app_http_request{http_resource_line};
}

app_http_request::app_http_request(const string& http_resource_line) {
  const auto resource{parse_resource_line(http_resource_line)};
  method = resource.first;
  uri_path = resource.second.first;
  uri_query = resource.second.second;
}

http_response
app_http_request::response() const {
  log_request(*this);
  http_response resp{};
  if (method == "GET" && uri_str() == "/app")
    resp = handle_tasks_get_request();
  log_response(*this, resp);
  return resp;
}

http_server::http_server(io_service& service, uint16_t port) : asio_http::http_server(service, port, [](const string& line){ return app_http_request::make_http_request(line); }) {}
}
