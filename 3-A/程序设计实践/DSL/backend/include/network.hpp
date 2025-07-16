/**
 * @file  network.hpp
 * @brief WebSocket 服务器和会话类的声明。
 */
#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <cstddef>
#include <deque>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "ast.hpp"
#include "interpreter.hpp"
#include "iwebsocketstreamfactory.hpp"

namespace beast = boost::beast;          // from <boost/beast.hpp>
namespace websocket = beast::websocket;  // from <boost/beast/websocket.hpp>
namespace net = boost::asio;             // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;
using MessageQueue = std::deque<std::string>;

/**
 * @class WebSocketServer
 * @brief WebSocket 服务器类
 *
 * 该类表示一个WebSocket服务器，用于接收客户端的连接。
 * 每个客户端连接后，服务器会创建一个WebSocketSession对象与之通信。
 */
class WebSocketServer {
 public:
  WebSocketServer(net::io_context&, const tcp::endpoint&, std::shared_ptr<Program>, std::map<std::string, std::string>&,
                  std::map<std::string, std::size_t>&, std::shared_ptr<IWebSocketStreamFactory>);
  void run();

 private:
  void doAccept();
  static void fail(const beast::error_code&, const char*);
  /// @brief 接收器
  tcp::acceptor acceptor;
  /// @brief 指向program的智能指针
  std::shared_ptr<Program> program;
  /// @brief 常量表
  std::map<std::string, std::string>& constants;
  /// @brief 过程表
  std::map<std::string, std::size_t>& procedures;
  /// @brief WebSocket流工厂
  std::shared_ptr<IWebSocketStreamFactory> factory;
  static constexpr int DEFAULTUID = 101;
  int uid{DEFAULTUID};
};

/**
 * @class WebSocketSession
 * @brief WebSocket 会话类
 *
 * 该类表示一个WebSocket会话，用于与客户端通信。
 */
class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
 public:
  /// @param wsStream WebSocket流
  /// @param program 指向program的智能指针
  /// @param c 常量表
  /// @param p 过程表
  /// @param uid 用户id
  WebSocketSession(std::shared_ptr<IWebSocketStream> wsStream, const std::shared_ptr<Program>& program,
                   std::map<std::string, std::string> c, std::map<std::string, std::size_t> p, const int uid)
      : ws(std::move(wsStream)), interpreter(program, "browser", std::move(c), std::move(p)), program(program) {
    interpreter.setUid(uid);
  }
  void run();

 private:
  void onAccept(const beast::error_code&);
  void doRead();
  void onRead(const beast::error_code&, std::size_t);
  void onWrite(const beast::error_code&, std::size_t);
  static void fail(const beast::error_code&, const char*);
  /// @brief WebSocket流
  std::shared_ptr<IWebSocketStream> ws;
  /// @brief 用于接收数据的缓冲区
  beast::flat_buffer buffer;
  /// @brief 解释器
  Interpreter interpreter;
  /// @brief 指向program的智能指针
  std::shared_ptr<Program> program;
  void send(const std::string& message);
  /// @brief 待发送消息队列
  MessageQueue write_msgs;
};
#endif  // NETWORK_HPP
