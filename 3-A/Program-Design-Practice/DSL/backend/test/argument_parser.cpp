#include "argument_parser.hpp"

#include <gtest/gtest.h>

#include <boost/program_options/errors.hpp>
#include <stdexcept>

class ArgumentParserTest : public ::testing::Test {
 protected:
  ArgumentParser parser;
};

TEST_F(ArgumentParserTest, DefaultValues) {
  const char* argv[] = {"program"};
  parser.parse(1, argv);
  EXPECT_EQ(parser.getScript(), "scripts/sample");
  EXPECT_EQ(parser.getMode(), "browser");
  EXPECT_EQ(parser.getUid(), 101);
}

TEST_F(ArgumentParserTest, ParsesScriptCorrectly) {
  const char* argv[] = {"program", "--script", "myscript.py"};
  parser.parse(3, argv);
  EXPECT_EQ(parser.getScript(), "myscript.py");
}

TEST_F(ArgumentParserTest, ParsesModeCorrectly) {
  const char* argv[] = {"program", "--mode", "cli"};
  parser.parse(3, argv);
  EXPECT_EQ(parser.getMode(), "cli");
}

TEST_F(ArgumentParserTest, ParsesUidCorrectly) {
  const char* argv[] = {"program", "--userid", "202"};
  parser.parse(3, argv);
  EXPECT_EQ(parser.getUid(), 202);
}

TEST_F(ArgumentParserTest, MissingArguments) {
  const char* argv[] = {"program", "--script"};
  EXPECT_THROW(parser.parse(2, argv), po::error);
}

TEST_F(ArgumentParserTest, InvalidArguments) {
  const char* argv[] = {"program", "--unknown", "value"};
  EXPECT_THROW(parser.parse(3, argv), po::error);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
