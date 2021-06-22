#include "pch.h"

#include <optional>
#include <string>

#include "../rest_client/common_utility.h"

TEST(UtilityTest, CorrectTime) {
	FILETIME time;
	time.dwLowDateTime = 2366466959;
	time.dwHighDateTime = 30893927;

	std::optional<std::wstring> result = my_rest_client::common_utility::ConvertIsoTime(time);
	ASSERT_TRUE(result.has_value());

	auto iso_time = result.value();
	ASSERT_EQ(L"2021-06-22T22:07:27", iso_time);
}

TEST(UtilityTest, GetFileInfo) {
	std::wstring file_name = L"C:\\Users\\ABO\\Desktop\\time.txt";

	std::optional<my_rest_client::common_utility::FileInfo> result = my_rest_client::common_utility::GetFileInfo(file_name);
	ASSERT_TRUE(result.has_value());

	auto file_info = result.value();
	ASSERT_EQ(file_info.file_name, file_name);
	ASSERT_EQ(file_info.file_size, L"5");
	ASSERT_EQ(file_info.creation_iso_time, L"2021-06-22T22:07:27");
	ASSERT_EQ(file_info.last_modified_iso_time, L"2021-06-22T22:30:25");
}