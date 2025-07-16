/**
 * @file iwebsocketstreamfactory.hpp
 * @brief IWebSocketStreamFactory 接口类的声明。
 */
#ifndef IWEBSOCKETSTREAMFACTORY_HPP
#define IWEBSOCKETSTREAMFACTORY_HPP

#include <boost/asio.hpp>

#include "iwebsocketstream.hpp"

/**
 * @class IWebSocketStreamFactory
 * @brief WebSocket stream 工厂接口类
 *
 * 该类是 WebSocket stream 工厂的接口类。
 * 用于定义 WebSocket stream 工厂的接口。
 * 用于Google Test的mock。
 */
class IWebSocketStreamFactory {
 public:
  virtual ~IWebSocketStreamFactory() = default;
  /**
   * @brief 创建 WebSocket stream
   *
   * @param socket TCP socket
   * @return 指向 WebSocket stream 的智能指针
   */
  virtual auto create(boost::asio::ip::tcp::socket socket) -> std::shared_ptr<IWebSocketStream> = 0;
};

#endif  // IWEBSOCKETSTREAMFACTORY_HPP
