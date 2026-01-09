/**
 * @file websocketstream.hpp
 * @brief WebSocketStream 类的声明。
 */
#ifndef WEBSOCKETSTREAM_HPP
#define WEBSOCKETSTREAM_HPP

#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>

#include "iwebsocketstream.hpp"

/**
 * @class WebSocketStream
 * @brief WebSocket stream 类
 *
 * 该类是 WebSocket stream 的实现类。
 * 用于实现 WebSocket stream 的接口。
 */
class WebSocketStream final : public IWebSocketStream {
 public:
  explicit WebSocketStream(boost::asio::ip::tcp::socket socket) : ws(std::move(socket)) {}

  void async_accept(AcceptHandler handler) override { ws.async_accept(handler); }

  void async_read(boost::beast::flat_buffer& buffer, ReadHandler handler) override { ws.async_read(buffer, handler); }

  void async_write(const boost::asio::const_buffer& buffer, WriteHandler handler) override {
    ws.async_write(buffer, handler);
  }

  void close(const boost::beast::websocket::close_reason&) override {
    ws.close(boost::beast::websocket::close_code::normal);
  }

  auto get_executor() -> boost::asio::any_io_executor override { return ws.get_executor(); }

 private:
  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws;
};

#endif  // WEBSOCKETSTREAM_HPP
