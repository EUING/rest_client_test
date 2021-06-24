#include "pch.h"

#include <optional>
#include <string>

#include "../monitor_client/common_utility.h"

TEST(UtilityTest, ConvertTime) {
	FILETIME time;
	time.dwLowDateTime = 2366466959;
	time.dwHighDateTime = 30893927;

	std::optional<std::wstring> result = monitor_client::common_utility::ConvertTimestamp(time);
	ASSERT_TRUE(result.has_value());

	auto iso_time = result.value();
	ASSERT_EQ(L"2021-06-22 22:07:27", iso_time);
}

TEST(UtilityTest, GetFileName) {
	std::wstring file_path = L"C:\\Users\\ABO\\Desktop\\Visual Studio 2019.lnk";

	std::optional<std::wstring> result = monitor_client::common_utility::GetFileName(file_path);
	ASSERT_TRUE(result.has_value());

	auto file_name = result.value();
	ASSERT_EQ(file_name, L"Visual Studio 2019.lnk");
}

TEST(UtilityTest, WrongFileName) {
	std::wstring file_path = L"C:\\Users\\ABO\\Desktop\\Visual Studio 2019";

	std::optional<std::wstring> result = monitor_client::common_utility::GetFileName(file_path);
	ASSERT_FALSE(result.has_value());
}

TEST(UtilityTest, GetFileInfo) {
	std::wstring file_path = L"C:\\Users\\ABO\\Desktop\\Visual Studio 2019.lnk";

	std::optional<monitor_client::common_utility::FileInfo> result = monitor_client::common_utility::GetFileInfo(file_path);
	ASSERT_TRUE(result.has_value());

	auto file_info = result.value();
	ASSERT_EQ(file_info.name, L"Visual Studio 2019.lnk");
	ASSERT_EQ(file_info.size, 1787);
	ASSERT_EQ(file_info.creation_iso_time, L"2021-06-17 09:48:43");
	ASSERT_EQ(file_info.last_modified_iso_time, L"2021-06-17 22:16:23");
}

TEST(UtilityTest, Folder) {
	std::wstring file_name = L"C:\\Users\\ABO\\Desktop\\test";

	std::optional<monitor_client::common_utility::FileInfo> result = monitor_client::common_utility::GetFileInfo(file_name);
	ASSERT_FALSE(result.has_value());
}

TEST(UtilityTest, WrongPath) {
	std::wstring file_name = L"WrongPath";

	std::optional<monitor_client::common_utility::FileInfo> result = monitor_client::common_utility::GetFileInfo(file_name);
	ASSERT_FALSE(result.has_value());
}

TEST(UtilityTest, SplitName) {
	std::wstring file_name;

	std::optional<monitor_client::common_utility::ChangeNameInfo> result = monitor_client::common_utility::SplitChangeName(file_name);
	ASSERT_FALSE(result.has_value());

	file_name = L"test1?test2?test3";
	result = monitor_client::common_utility::SplitChangeName(file_name);
	ASSERT_FALSE(result.has_value());

	file_name = L"C:\\Users\\ABO\\Desktop\\Visual Studio 2019.lnk?C:\\Users\\ABO\\Desktop\\새 텍스트 문서.txt";
	result = monitor_client::common_utility::SplitChangeName(file_name);
	ASSERT_TRUE(result.has_value());

	auto info = result.value();
	ASSERT_EQ(info.old_name, L"Visual Studio 2019.lnk");
	ASSERT_EQ(info.new_name, L"새 텍스트 문서.txt");
}