#include <exception>
#include <iostream>
#include <thread>

#include "app_http_listener.h"

namespace {
using namespace std;
using namespace app;
}

int
main(int argc, const char* argv[]) {
  try {
    app_http_listener listener{8567};
    this_thread::sleep_for(3600s);
  } catch (const exception& e) {
    cerr << "caught exception: " << e.what() << endl;
    return 1;
  } catch (...) {
    cerr << "caught ..." << endl;
    return 1;
  }
}
