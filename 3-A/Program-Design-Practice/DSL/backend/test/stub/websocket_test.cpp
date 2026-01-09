#include <future>

#include "MockWebSocketStream.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "interpreter.hpp"
#include "lexer.hpp"
#include "network.hpp"
#include "parser.hpp"
#include "semantic_analysis.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

TEST(WebSocketSessionTest, AcceptsConnectionSuccessfully) {
  boost::asio::io_context ioc;
  auto mockWsStream = std::make_shared<MockWebSocketStream>(ioc);

  Lexer lexer(R"(procedure main { })");
  lexer.scan();
  Parser parser(lexer.getTokens());
  auto program = parser.parse();
  const SemanticAnalysis semanticAnalysis(program);
  auto constants = semanticAnalysis.getConstants();
  auto procedures = semanticAnalysis.getProcedures();

  // 创建 WebSocketSession 实例
  int uid = 101;
  const auto session = std::make_shared<WebSocketSession>(mockWsStream, program, constants, procedures, uid);
  // 设置期望：async_accept 被调用，并模拟成功
  EXPECT_CALL(*mockWsStream, async_accept(_)).WillOnce(Invoke([&](const IWebSocketStream::AcceptHandler& handler) {
    handler(boost::system::error_code());  // 无错误
  }));

  // 运行会话
  session->run();

  // 运行 io_context 以触发异步操作
  ioc.run();
}

TEST(WebSocketSessionTest, SendsMessageThroughWebSocket) {
  boost::asio::io_context ioc;
  auto mockWsStream = std::make_shared<MockWebSocketStream>(ioc);
  std::string capturedMessage;

  Lexer lexer(R"(procedure main { say "Hello" })");
  lexer.scan();
  Parser parser(lexer.getTokens());
  auto program = parser.parse();
  const SemanticAnalysis semanticAnalysis(program);
  auto constants = semanticAnalysis.getConstants();
  auto procedures = semanticAnalysis.getProcedures();
  int uid = 101;
  const auto session = std::make_shared<WebSocketSession>(mockWsStream, program, constants, procedures, uid);

  std::promise<void> writePromise;
  std::future<void> writeFuture = writePromise.get_future();

  // 设置期望：async_accept 被调用，并模拟成功
  EXPECT_CALL(*mockWsStream, async_accept(_)).WillOnce(Invoke([&](const IWebSocketStream::AcceptHandler& handler) {
    handler(boost::system::error_code());  // 无错误
  }));

  // 设置期望：async_write 被调用，并捕获发送的消息
  EXPECT_CALL(*mockWsStream, async_write(_, _))
      .WillOnce(Invoke([&](const boost::asio::const_buffer& buffer, const IWebSocketStream::WriteHandler& handler) {
        // 从 buffer 中提取消息
        capturedMessage.assign(boost::asio::buffer_cast<const char*>(buffer), boost::asio::buffer_size(buffer));
        // 调用回调，模拟写入成功
        handler(boost::system::error_code(), capturedMessage.size());
        writePromise.set_value();
      }));

  // 运行会话
  session->run();
  // 在不同的线程中运行 io_context
  std::thread ioThread([&ioc]() { ioc.run(); });

  // 等待 async_write 被调用
  if (writeFuture.wait_for(std::chrono::seconds(1)) != std::future_status::ready) {
    FAIL() << "async_write was not called within the expected time.";
  }

  // 停止 io_context 并等待线程结束
  ioc.stop();
  ioThread.join();

  // 验证发送的消息是否正确
  EXPECT_EQ(capturedMessage, "Hello");
}

TEST(WebSocketSessionTest, ReceivesMessageAndPassesToInterpreter) {
  boost::asio::io_context ioc;
  auto mockWsStream = std::make_shared<MockWebSocketStream>(ioc);
  std::string capturedMessage;
  Lexer lexer(R"(procedure main {
  listen timelimit 2 {
    has "Test Message" ? A
  }
}
procedure A { say "got" })");
  lexer.scan();
  Parser parser(lexer.getTokens());
  auto program = parser.parse();
  const SemanticAnalysis semanticAnalysis(program);
  auto constants = semanticAnalysis.getConstants();
  auto procedures = semanticAnalysis.getProcedures();
  int uid = 101;
  auto session = std::make_shared<WebSocketSession>(mockWsStream, program, constants, procedures, uid);

  // 设置期望：async_accept 被调用，并模拟成功
  EXPECT_CALL(*mockWsStream, async_accept(_)).WillOnce(Invoke([&](const IWebSocketStream::AcceptHandler& handler) {
    handler(boost::system::error_code());  // 无错误
  }));

  std::promise<void> writePromise;
  std::future<void> writeFuture = writePromise.get_future();
  // 模拟接收消息
  std::string testMessage = "Test Message";

  // 设置期望：async_read 被调用，并模拟接收一条消息
  EXPECT_CALL(*mockWsStream, async_read(_, _))
      .WillOnce(Invoke([&](boost::beast::flat_buffer& buffer, const IWebSocketStream::ReadHandler& handler) {
        std::this_thread::sleep_for(std::chrono::seconds(1));  // 等待Interpreter
        buffer.prepare(testMessage.size());
        std::memcpy(boost::beast::buffers_front(buffer.data()).data(), testMessage.data(), testMessage.size());
        buffer.commit(testMessage.size());
        handler(boost::system::error_code(), testMessage.size());
      }))
      .WillOnce(Invoke([&](boost::beast::flat_buffer& buffer, const IWebSocketStream::ReadHandler& handler) {}));

  // 设置期望：async_write 被调用，并捕获发送的消息
  EXPECT_CALL(*mockWsStream, async_write(_, _))
      .WillOnce(Invoke([&](const boost::asio::const_buffer& buffer, const IWebSocketStream::WriteHandler& handler) {
        // 从 buffer 中提取消息
        capturedMessage.assign(boost::asio::buffer_cast<const char*>(buffer), boost::asio::buffer_size(buffer));
        // 调用回调，模拟写入成功
        handler(boost::system::error_code(), capturedMessage.size());
        writePromise.set_value();
      }));

  // 运行会话
  session->run();
  // 在不同的线程中运行 io_context
  std::thread ioThread([&ioc]() { ioc.run(); });

  // 等待 async_write 被调用
  if (writeFuture.wait_for(std::chrono::seconds(1)) != std::future_status::ready) {
    FAIL() << "async_write was not called within the expected time.";
  }

  // 停止 io_context 并等待线程结束
  ioc.stop();
  ioThread.join();
  EXPECT_EQ(capturedMessage, "got");
}
