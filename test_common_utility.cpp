#include "pch.h"

#include <optional>
#include <string>

#include "../monitor_client/common_utility.h"

TEST(UtilityTest, IsDirectory) {
	std::wstring file_path = L"C:\\Users\\ABO\\Desktop\\Visual Studio 2019.lnk";
	std::optional<bool> result = monitor_client::common_utility::IsDirectory(file_path);
	ASSERT_TRUE(result.has_value());
	ASSERT_FALSE(result.value());

	std::wstring folder_path = L"C:\\project";
	result = monitor_client::common_utility::IsDirectory(folder_path);
	ASSERT_TRUE(result.has_value());
	ASSERT_TRUE(result.value());

	std::wstring wrong_path = L"WrongPath";
	result = monitor_client::common_utility::IsDirectory(wrong_path);
	ASSERT_FALSE(result.has_value());
}

TEST(UtilityTest, GetItemInfo) {
	std::wstring file_path = L"C:\\Users\\ABO\\Desktop\\Visual Studio 2019.lnk";

	std::optional<monitor_client::common_utility::ItemInfo> result = monitor_client::common_utility::GetItemInfo(file_path);
	ASSERT_TRUE(result.has_value());

	auto item_info = result.value();
	ASSERT_EQ(item_info.name, file_path);
	ASSERT_EQ(item_info.size, 1787);

	std::wstring folder_path = L"C:\\project";

	result = monitor_client::common_utility::GetItemInfo(folder_path);
	ASSERT_TRUE(result.has_value());

	item_info = result.value();
	ASSERT_EQ(item_info.name, folder_path);
	ASSERT_EQ(item_info.size, -1);

	std::wstring wrong_path = L"WrongPath";

	result = monitor_client::common_utility::GetItemInfo(wrong_path);
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