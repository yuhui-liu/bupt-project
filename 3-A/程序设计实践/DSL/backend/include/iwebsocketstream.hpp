/**
 * @file iwebsocketstream.hpp
 * @brief IWebSocketStream 接口类的声明。
 */
#ifndef IWEBSOCKETSTREAM_HPP
#define IWEBSOCKETSTREAM_HPP

#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>
#include <functional>
#include <string>

/**
 * @class IWebSocketStream
 * @brief WebSocket stream 接口类
 *
 * 该类是 WebSocket stream 的接口类。
 * 用于定义 WebSocket stream 的接口。
 * 用于Google Test的mock。
 */
class IWebSocketStream {
 public:
  virtual ~IWebSocketStream() = default;
  // NOLINTBEGIN(readability-identifier-naming)
  using AcceptHandler = std::function<void(const boost::system::error_code&)>;
  virtual void async_accept(AcceptHandler handler) = 0;

  using ReadHandler = std::function<void(const boost::system::error_code&, std::size_t)>;
  virtual void async_read(boost::beast::flat_buffer& buffer, ReadHandler handler) = 0;

  using WriteHandler = std::function<void(const boost::system::error_code&, std::size_t)>;
  virtual void async_write(const boost::asio::const_buffer& buffer, WriteHandler handler) = 0;

  virtual void close(const boost::beast::websocket::close_reason&) = 0;
  virtual auto get_executor() -> boost::asio::any_io_executor = 0;
  // NOLINTEND(readability-identifier-naming)
};

#endif  // IWEBSOCKETSTREAM_HPP
