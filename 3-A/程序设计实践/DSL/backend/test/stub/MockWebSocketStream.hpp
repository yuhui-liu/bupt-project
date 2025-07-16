
// MockWebSocketStream.hpp
#ifndef MOCKWEBSOCKETSTREAM_HPP
#define MOCKWEBSOCKETSTREAM_HPP

#include <gmock/gmock.h>

#include <boost/asio/io_context.hpp>

#include "iwebsocketstream.hpp"

class MockWebSocketStream : public IWebSocketStream {
 public:
  explicit MockWebSocketStream(boost::asio::io_context& io_ctx) : io_ctx_(io_ctx) {}

  MOCK_METHOD(void, async_accept, (AcceptHandler handler), (override));
  MOCK_METHOD(void, async_read, (boost::beast::flat_buffer & buffer, ReadHandler handler), (override));
  MOCK_METHOD(void, async_write, (const boost::asio::const_buffer& buffer, WriteHandler handler), (override));
  MOCK_METHOD(void, close, (const boost::beast::websocket::close_reason&), (override));

  // 实现 get_executor
  boost::asio::any_io_executor get_executor() override { return io_ctx_.get_executor(); }

 private:
  boost::asio::io_context& io_ctx_;
};

#endif  // MOCKWEBSOCKETSTREAM_HPP
