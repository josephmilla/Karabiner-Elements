#pragma once

#include "constants.hpp"
#include "local_datagram_server.hpp"
#include "session.hpp"

class grabber_server final {
public:
  void start(void) {
    const char* path = constants::get_grabber_socket_file_path();
    unlink(path);
    server_ = std::make_unique<local_datagram_server>(path);

    uid_t uid;
    if (session::get_current_console_user_id(uid)) {
      chown(path, uid, 0);
    }
    chmod(path, 0600);

    thread_ = std::thread([this] { this->worker(); });
  }

  void stop(void) {
    if (!thread_.joinable()) {
      return;
    }
    if (!server_) {
      return;
    }

    exit_loop_ = true;
    thread_.join();
    server_.reset(nullptr);
  }

  void worker(void) {
    if (!server_) {
      return;
    }

    while (!exit_loop_) {
      boost::system::error_code ec;
      std::size_t n = server_->receive(boost::asio::buffer(buffer_), boost::posix_time::seconds(1), ec);

      if (ec) {
        std::cout << "Receive error: " << ec.message() << "\n";
      } else {
        std::cout << n << std::endl;
      }
    }
  }

private:
  enum {
    buffer_length = 8 * 1024 * 1024,
  };
  std::array<uint8_t, buffer_length> buffer_;
  std::unique_ptr<local_datagram_server> server_;
  std::thread thread_;
  volatile bool exit_loop_;
};
