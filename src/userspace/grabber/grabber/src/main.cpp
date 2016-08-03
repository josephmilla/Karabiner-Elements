#include "grabber.hpp"

int main(int argc, const char* argv[]) {
  const char* version = "version "
#include "version.hpp"
      ;

  logger::get_logger()->info(version);

  event_grabber observer;

  grabber_server server;
  server.start();

  userspace_connection_manager connection_manager;

  CFRunLoopRun();
  return 0;
}
