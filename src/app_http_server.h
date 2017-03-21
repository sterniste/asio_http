#ifndef APP_HTTP_SERVER_H
#define APP_HTTP_SERVER_H

#include <cstdint>
#include <string>

namespace boost {
namespace asio {
class io_service;
}
}

#include "asio_http_server.h"
#include "http_msgs.h"

namespace app {

class app_http_request : public asio_http::http_request {
  static void log_request(const asio_http::http_request& req);
  static void log_response(const asio_http::http_request& req, const asio_http::http_response& resp);

  std::string build_get_tasks_reply_json() const;
  asio_http::http_response handle_tasks_get_request() const;

 public:
  static asio_http::http_request* make_http_request(const std::string& http_resource_line);

  app_http_request(const std::string& http_resource_line);

  asio_http::http_response response() const override;
};

struct http_server : private asio_http::http_server {
  http_server(boost::asio::io_service& service, uint16_t port);
};
}
#endif
