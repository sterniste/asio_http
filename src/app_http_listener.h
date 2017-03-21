#ifndef APP_HTTP_LISTENER_H
#define APP_HTTP_LISTENER_H

#include <cstdint>

#include <boost/asio.hpp>

#include "app_http_server.h"

namespace app {

class app_http_listener {
  boost::asio::io_service service;
  http_server server;

 public:
  app_http_listener(uint16_t port);
};
}
#endif
