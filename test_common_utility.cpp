#include "pch.h"

#include <variant>
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

TEST(UtilityTest, GetFileInfo) {
	std::wstring file_path = L"C:\\Users\\ABO\\Desktop\\Visual Studio 2019.lnk";

	std::variant<std::monostate, monitor_client::common_utility::FileInfo, monitor_client::common_utility::FolderInfo> result = monitor_client::common_utility::GetItemInfo(file_path);
	ASSERT_EQ(result.index(), 1);

	auto file_info = std::get<monitor_client::common_utility::FileInfo>(result);
	ASSERT_EQ(file_info.name, file_path);
	ASSERT_EQ(file_info.size, 1787);
	ASSERT_EQ(file_info.creation_time, L"2021-06-17 09:48:43");
	ASSERT_EQ(file_info.last_modified_time, L"2021-06-17 22:16:23");
}

TEST(UtilityTest, GetFolderInfo) {
	std::wstring file_name = L"C:\\project";

	std::variant<std::monostate, monitor_client::common_utility::FileInfo, monitor_client::common_utility::FolderInfo> result = monitor_client::common_utility::GetItemInfo(file_name);
	ASSERT_EQ(result.index(), 2);

	auto file_info = std::get<monitor_client::common_utility::FolderInfo>(result);
	ASSERT_EQ(file_info.name, file_name);
	ASSERT_EQ(file_info.creation_time, L"2021-06-16 21:45:26");
}

TEST(UtilityTest, GetItemInfoFromWrongPath) {
	std::wstring file_name = L"WrongPath";

	std::variant<std::monostate, monitor_client::common_utility::FileInfo, monitor_client::common_utility::FolderInfo> result = monitor_client::common_utility::GetItemInfo(file_name);
	ASSERT_EQ(result.index(), 0);
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
	ASSERT_EQ(info.old_name, L"C:\\Users\\ABO\\Desktop\\Visual Studio 2019.lnk");
	ASSERT_EQ(info.new_name, L"C:\\Users\\ABO\\Desktop\\새 텍스트 문서.txt");
}

TEST(UtilityTest, FormatName) {
	std::wstring str = monitor_client::common_utility::format_wstring(L"name=%s, id=%d", L"테스트", 3);
	ASSERT_EQ(str, L"name=테스트, id=3");
}

TEST(UtilityTest, SplitPath) {
	std::vector<std::wstring> split_parent_path;
	std::wstring item_name;

	bool result = monitor_client::common_utility::SplitPath(L"", &split_parent_path, item_name);
	ASSERT_FALSE(result);

	result = monitor_client::common_utility::SplitPath(L"새 폴더", &split_parent_path, item_name);
	ASSERT_TRUE(result);
	ASSERT_EQ(item_name, L"새 폴더");
	ASSERT_TRUE(split_parent_path.empty());

	result = monitor_client::common_utility::SplitPath(L"새 폴더/새 폴더", &split_parent_path, item_name);
	ASSERT_TRUE(result);
	ASSERT_EQ(item_name, L"새 폴더");
	ASSERT_EQ(split_parent_path[0], L"새 폴더");

	result = monitor_client::common_utility::SplitPath(L"새 폴더/새 폴더.txt", &split_parent_path, item_name);
	ASSERT_TRUE(result);
	ASSERT_EQ(item_name, L"새 폴더.txt");
	ASSERT_EQ(split_parent_path[0], L"새 폴더");

	result = monitor_client::common_utility::SplitPath(L"새 폴더/new folder/새 폴더.txt", &split_parent_path, item_name);
	ASSERT_TRUE(result);
	ASSERT_EQ(item_name, L"새 폴더.txt");
	ASSERT_EQ(split_parent_path[0], L"새 폴더");
	ASSERT_EQ(split_parent_path[1], L"new folder");

	result = monitor_client::common_utility::SplitPath(L"새 폴더", &split_parent_path, item_name);
	ASSERT_TRUE(result);
	ASSERT_EQ(item_name, L"새 폴더");
	ASSERT_TRUE(split_parent_path.empty());

	result = monitor_client::common_utility::SplitPath(L"새 폴더\\새 폴더", &split_parent_path, item_name);
	ASSERT_TRUE(result);
	ASSERT_EQ(item_name, L"새 폴더");
	ASSERT_EQ(split_parent_path[0], L"새 폴더");

	result = monitor_client::common_utility::SplitPath(L"새 폴더\\새 폴더.txt", &split_parent_path, item_name);
	ASSERT_TRUE(result);
	ASSERT_EQ(item_name, L"새 폴더.txt");
	ASSERT_EQ(split_parent_path[0], L"새 폴더");

	result = monitor_client::common_utility::SplitPath(L"새 폴더\\new folder\\새 폴더.txt", &split_parent_path, item_name);
	ASSERT_TRUE(result);
	ASSERT_EQ(item_name, L"새 폴더.txt");
	ASSERT_EQ(split_parent_path[0], L"새 폴더");
	ASSERT_EQ(split_parent_path[1], L"new folder");
}