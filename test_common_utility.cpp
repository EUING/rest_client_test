#include "pch.h"

#include <fstream>
#include <optional>
#include <string>
#include <future>
#include <chrono>

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

TEST(UtilityTest, Sha256) {
	std::optional<std::wstring> result = monitor_client::common_utility::GetSha256(L"WrongPath");
	ASSERT_FALSE(result.has_value());

	result = monitor_client::common_utility::GetSha256(L"C:\\Users\\ABO\\Desktop\\monitor");
	ASSERT_FALSE(result.has_value());

	result = monitor_client::common_utility::GetSha256(L"C:\\Users\\ABO\\Desktop\\monitor");
	ASSERT_FALSE(result.has_value());

	std::wstring file_name = L"C:\\Users\\ABO\\Desktop\\empty.txt";
	std::ofstream temp{ file_name };
	temp.close();

	result = monitor_client::common_utility::GetSha256(file_name);
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", result.value());

	_wremove(file_name.c_str());
}

TEST(UtilityTest, WaitTimeForAccess) {
	std::wstring file_name = L"C:\\Users\\ABO\\Desktop\\empty.txt";
	std::ofstream temp{ file_name };
	
	ASSERT_FALSE(monitor_client::common_utility::WaitTimeForAccess(file_name));
	temp.close();
	ASSERT_TRUE(monitor_client::common_utility::WaitTimeForAccess(file_name));

	ASSERT_FALSE(monitor_client::common_utility::WaitTimeForAccess(L"Wrong"));

	std::ofstream temp2{ file_name };
	auto future = std::async(std::launch::async, [&temp2]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		temp2.close();
	});

	ASSERT_EQ(std::future_status::timeout, future.wait_for(std::chrono::milliseconds(0)));
	ASSERT_TRUE(monitor_client::common_utility::WaitTimeForAccess(file_name));

	std::wstring folder = L"C:\\Users\\ABO\\Desktop\\old";
	_wmkdir(folder.c_str());

	ASSERT_TRUE(monitor_client::common_utility::WaitTimeForAccess(folder));

	_wrmdir(folder.c_str());
}