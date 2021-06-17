#include "pch.h"

#include <optional>
#include <string>

#include "../rest_client/my_client.h"

TEST(PingTest, ReturnPong) {
  bool result = my_rest_client::Ping().has_value();
  ASSERT_TRUE(result);

  std::wstring message = my_rest_client::Ping().value();
  EXPECT_EQ(message, L"pong");
}