#include <exception>
#include <iostream>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp> 

#include "app_http_listener.h"

namespace {
using namespace std;
using namespace boost;
using namespace app;
}

int
main(int argc, const char* argv[]) {
  try {
    app_http_listener listener{7658};
    this_thread::sleep(posix_time::seconds(3600));
  } catch (const std::exception& e) {
    cerr << "caught exception: " << e.what() << endl;
    return 1;
  } catch (...) {
    cerr << "caught ..." << endl;
    return 1;
  }
}
