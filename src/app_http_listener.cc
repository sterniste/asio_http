#include <cstdint>

#include "app_http_listener.h"

namespace app {
using namespace std;

app_http_listener::app_http_listener(uint16_t port) : server{service, port} {
  service.run();
}
}
