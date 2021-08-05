#include "pch.h"

#include <optional>
#include <string>

#include "../monitor_client/yaml_parser.h"

std::wstring path = L"C:\\Users\\ABO\\Desktop\\monitor\\.ignore\\config.yaml";

TEST(YamlParserTest, ParsingTest) {
	monitor_client::YamlParser parser(path);
	std::optional<std::wstring> get_string = parser.GetString();
	ASSERT_FALSE(get_string.has_value());

	get_string = parser.GetString("Wrong path");
	ASSERT_FALSE(get_string.has_value());

	get_string = parser.GetString("network");
	ASSERT_FALSE(get_string.has_value());

	get_string = parser.GetString("network", "host");
	ASSERT_TRUE(get_string.has_value());

	auto host = get_string.value();
	ASSERT_EQ(host, L"ggulmo.iptime.org");

	get_string = parser.GetString("network", "port");
	ASSERT_TRUE(get_string.has_value());

	auto port = get_string.value();
	ASSERT_EQ(port, L"56380");

	std::optional<int> get_int = parser.GetInt("network");
	ASSERT_FALSE(get_int.has_value());

	get_int = parser.GetInt("network", "host");
	ASSERT_FALSE(get_int.has_value());

	get_int = parser.GetInt("network", "port");
	ASSERT_TRUE(get_int.has_value());

	auto port_num = get_int.value();
	ASSERT_EQ(port_num, 56380);
}