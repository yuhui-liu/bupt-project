/**
 * @file      network.cpp
 * @brief     网络部分的实现。使用WebSocket协议实现前后端通信。
 */
#include "network.hpp"

#include <spdlog/spdlog.h>

#include <cstring>
#include <memory>
#include <thread>
#include <utility>

#include "ast.hpp"
#include "exception.hpp"
#include "interpreter.hpp"

/**
 * @brief WebSocket 服务器类的构造函数。
 *
 * @param ioc IO 上下文
 * @param endpoint 服务器端点
 * @param program 程序指针
 * @param c 常量表
 * @param p 过程表
 * @param factory WebSocket 流工厂
 */
WebSocketServer::WebSocketServer(net::io_context& ioc, const tcp::endpoint& endpoint, std::shared_ptr<Program> program,
                                 std::map<std::string, std::string>& c, std::map<std::string, std::size_t>& p,
                                 std::shared_ptr<IWebSocketStreamFactory> factory)
    : acceptor(ioc), program(std::move(std::move(program))), constants(c), procedures(p), factory(std::move(factory)) {
  beast::error_code ec;

  // ReSharper disable once CppDFAUnreadVariable
  // ReSharper disable once CppDFAUnusedValue
  auto _ = acceptor.open(endpoint.protocol(), ec);
  if (ec) {
    fail(ec, "open");
  }

  // ReSharper disable once CppDFAUnusedValue
  _ = acceptor.set_option(net::socket_base::reuse_address(true), ec);
  if (ec) {
    fail(ec, "set_option");
  }

  // ReSharper disable once CppDFAUnusedValue
  _ = acceptor.bind(endpoint, ec);
  if (ec) {
    fail(ec, "bind");
  }

  // ReSharper disable once CppDFAUnusedValue
  _ = acceptor.listen(net::socket_base::max_listen_connections, ec);
  if (ec) {
    fail(ec, "listen");
  }
}

/// @brief 启动服务器
void WebSocketServer::run() { doAccept(); }

/**
 * @brief 接受新连接
 *
 * 该函数会异步接受新的连接，并创建一个 WebSocketSession 对象与之通信。
 */
void WebSocketServer::doAccept() {
  acceptor.async_accept([this](const beast::error_code& ec, tcp::socket socket) {
    if (!ec) {
      try {
        auto wsStream = factory->create(std::move(socket));
        std::make_shared<WebSocketSession>(wsStream, program, constants, procedures, uid++)->run();
      } catch (const boost::wrapexcept<boost::system::system_error>& e) {
        if (strcmp(e.what(), "End of file") != 0) {
          spdlog::error("WebSocketSession Error: {}", e.what());
        }
      }
    }
    doAccept();
  });
}

/**
 * @brief 处理错误
 *
 * @param ec 错误码
 * @param what 错误信息
 */
void WebSocketServer::fail(const beast::error_code& ec, const char* what) {
  spdlog::error(std::string(what) + ": " + ec.message());
}

/**
 * @brief 启动 WebSocket 会话
 *
 * 该函数设置发送消息的回调函数，并异步接受 WebSocket 连接。
 */
void WebSocketSession::run() {
  interpreter.setSend([this](const std::string& message) { this->send(message); });
  ws->async_accept(beast::bind_front_handler(&WebSocketSession::onAccept, shared_from_this()));
}

/**
 * @brief 处理 WebSocket 连接接受事件
 *
 * 该函数在 WebSocket 连接接受后被调用。
 * 如果接受成功，则在新线程中调用程序的 accept 函数。并且开始读取数据。
 *
 * @param ec 错误码
 */
void WebSocketSession::onAccept(const beast::error_code& ec) {
  if (ec) {
    return fail(ec, "accept");
  }
  spdlog::info("New connection.");
  std::thread([this]() {
    try {
      program->accept(interpreter);
    } catch (const ExitException&) {
      spdlog::info("Program exited.");
      this->ws->close(beast::websocket::close_code::normal);
    } catch (std::exception& e) {
      spdlog::error(e.what());
    }
  }).detach();
  doRead();
}

/**
 * @brief 开始读取数据
 *
 * 该函数开始异步读取数据。
 */
void WebSocketSession::doRead() {
  buffer.consume(buffer.size());
  ws->async_read(buffer, beast::bind_front_handler(&WebSocketSession::onRead, shared_from_this()));
}

/**
 * @brief 处理读取事件
 *
 * 该函数在读取到数据后被调用。
 * 读取到的数据会被解析，并传递给解释器。
 *
 * @param ec 错误码
 * @param bytesTransferred 读取的字节数
 */
void WebSocketSession::onRead(const beast::error_code& ec, std::size_t bytesTransferred) {
  boost::ignore_unused(bytesTransferred);
  if (ec) {
    return fail(ec, "read");
  }
  std::string receivedMessage = beast::buffers_to_string(buffer.data());
  interpreter.setInputFromNetwork(receivedMessage);
  spdlog::info("Received message: {}", receivedMessage);
  doRead();
}

/**
 * @brief 处理写事件
 *
 * 该函数在写入数据后被调用。
 * 如果还有未发送的消息，则继续发送下一个消息。
 *
 * @param ec 错误码
 * @param bytesTransferred 写入的字节数
 */
void WebSocketSession::onWrite(const beast::error_code& ec, std::size_t bytesTransferred) {
  boost::ignore_unused(bytesTransferred);
  if (ec) {
    return fail(ec, "write");
  }
  write_msgs.pop_front();
  if (!write_msgs.empty()) {
    // 继续发送下一个消息
    ws->async_write(net::buffer(write_msgs.front()),
                    beast::bind_front_handler(&WebSocketSession::onWrite, shared_from_this()));
  }
}

/**
 * @brief 处理错误
 *
 * @param ec 错误码
 * @param what 错误信息
 */
void WebSocketSession::fail(const beast::error_code& ec, char const* what) {
  if (ec.message() != "End of file") spdlog::error(std::string(what) + ": " + ec.message());
}

/**
 * @brief 发送消息
 *
 * 该函数将消息添加到待发送消息队列，并尝试发送。
 *
 * @param message 消息内容
 */
void WebSocketSession::send(const std::string& message) {
  // 将发送任务提交到 io_context 的执行器中，确保线程安全
  post(ws->get_executor(), [self = shared_from_this(), message]() {
    const bool writeInProgress = !self->write_msgs.empty();
    self->write_msgs.push_back(message);
    if (!writeInProgress) {
      self->ws->async_write(net::buffer(self->write_msgs.front()),
                            beast::bind_front_handler(&WebSocketSession::onWrite, self));
    }
  });
}
