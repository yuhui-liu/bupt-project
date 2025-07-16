/**
 * @file websocketstreamfactory.hpp
 * @brief WebSocketStreamFactory 类的声明。
 */
#ifndef WEBSOCKETSTREAMFACTORY_HPP
#define WEBSOCKETSTREAMFACTORY_HPP

#include <memory>

#include "iwebsocketstreamfactory.hpp"
#include "websocketstream.hpp"

/**
 * @class WebSocketStreamFactory
 * @brief WebSocket stream 工厂类
 *
 * 该类是 WebSocket stream 工厂的实现类。
 * 用于实现 WebSocket stream 工厂的接口。
 */
class WebSocketStreamFactory : public IWebSocketStreamFactory {
 public:
  auto create(boost::asio::ip::tcp::socket socket) -> std::shared_ptr<IWebSocketStream> override {
    return std::make_shared<WebSocketStream>(std::move(socket));
  }
};

#endif  // WEBSOCKETSTREAMFACTORY_HPP
