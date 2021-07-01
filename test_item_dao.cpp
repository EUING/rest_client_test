#include "pch.h"

#include <optional>

#include "../monitor_client/common_utility.h"
#include "../monitor_client/item_dao.h"

monitor_client::ItemDao item_dao;
int parent_id = -1;

class TestEnvironment : public ::testing::Environment {
	public:
	void SetUp() override {
		item_dao.OpenDatabase(L"C:\\Users\\ABO\\Desktop\\새 폴더\\test_items.db");
		monitor_client::common_utility::FileInfo info;
		info.name = L"새 텍스트 파일.txt";
		info.size = 10;

		item_dao.InsertFileInfo(info, 0);

		monitor_client::common_utility::FolderInfo folder_info;
		folder_info.name = L"새 폴더";
		item_dao.InsertFolderInfo(folder_info, 0);

		std::optional<int> result = item_dao.GetItemId(folder_info.name, 0);
		if (result.has_value()) {
			parent_id = result.value();
		}

		info.name = L"test.jpg";
		info.size = 5000;
		item_dao.InsertFileInfo(info, parent_id);

		info.name = L"새 텍스트 파일.txt";
		info.size = 100;
		item_dao.InsertFileInfo(info, parent_id);

		folder_info.name = L"새 폴더";
		item_dao.InsertFolderInfo(folder_info, parent_id);
	}

	void TearDown() override {
		item_dao.DeleteItemInfo(L"새 텍스트 파일.txt", 0);
		item_dao.DeleteItemInfo(L"새 폴더", 0);
		item_dao.DeleteItemInfo(L"test.jpg", parent_id);
		item_dao.DeleteItemInfo(L"새 텍스트 파일.txt", parent_id);
		item_dao.DeleteItemInfo(L"새 폴더", parent_id);
	}
};

TEST(ItemDao, OpenMemory) {
	monitor_client::ItemDao item_dao;
	bool result = item_dao.OpenDatabase(L":memory:");
	ASSERT_TRUE(result);
}

TEST(ItemDao, GetItemId) {
	std::optional<int> result = item_dao.GetItemId(L"새 폴더", 0);
	ASSERT_TRUE(result.has_value());
    
	ASSERT_EQ(result.value(), parent_id);
}

TEST(ItemDao, GetFileInfo) {
	std::optional<monitor_client::common_utility::FileInfo> result = item_dao.GetFileInfo(L"새 텍스트 파일.txt", -1);
	ASSERT_FALSE(result.has_value());

	result = item_dao.GetFileInfo(L"새 텍스트 파일.txt", 0);
	ASSERT_TRUE(result.has_value());

	auto info = result.value();
	ASSERT_EQ(info.name, L"새 텍스트 파일.txt");
	ASSERT_EQ(info.size, 10);

	result = item_dao.GetFileInfo(L"새 텍스트 파일.txt", parent_id);
	ASSERT_TRUE(result.has_value());

	info = result.value();
	ASSERT_EQ(info.name, L"새 텍스트 파일.txt");
	ASSERT_EQ(info.size, 100);
}

TEST(ItemDao, GetFolderContainList) {
	std::optional<std::vector<monitor_client::common_utility::FileInfo>> result = item_dao.GetFolderContainList(0);
	ASSERT_TRUE(result.has_value());

	auto v = result.value();
	std::vector<monitor_client::common_utility::FileInfo> v2;

	monitor_client::common_utility::FileInfo info;
	info.name = L"새 텍스트 파일.txt";
	info.size = 10;
	v2.push_back(info);
	
	info.name = L"새 폴더";
	info.size = -1;
	v2.push_back(info);

	ASSERT_EQ(v.size(), v2.size());

	for (int i = 0; i < v.size(); ++i) {
		ASSERT_EQ(v[i].name, v2[i].name);
		ASSERT_EQ(v[i].size, v2[i].size);
	}

	result = item_dao.GetFolderContainList(parent_id);
	ASSERT_TRUE(result.has_value());

	v = result.value();
	v2.clear();

	info.name = L"test.jpg";
	info.size = 5000;
	v2.push_back(info);

	info.name = L"새 텍스트 파일.txt";
	info.size = 100;
	v2.push_back(info);

	info.name = L"새 폴더";
	info.size = -1;
	v2.push_back(info);
	
	ASSERT_EQ(v.size(), v2.size());

	for (int i = 0; i < v.size(); ++i) {
		ASSERT_EQ(v[i].name, v2[i].name);
		ASSERT_EQ(v[i].size, v2[i].size);
	}
}

TEST(ItemDao, ChangeItemName) {
	monitor_client::common_utility::ChangeNameInfo info;
	info.old_name = L"새 텍스트 파일.txt";
	info.new_name = L"change.txt";

	std::optional<int> result = item_dao.ChangeItemName(info, parent_id);
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(1, result.value());

	std::optional<monitor_client::common_utility::FileInfo> get = item_dao.GetFileInfo(L"새 텍스트 파일.txt", parent_id);
	ASSERT_FALSE(get.has_value());

	get = item_dao.GetFileInfo(L"change.txt", parent_id);
	ASSERT_TRUE(get.has_value());
	auto file_info = get.value();

	ASSERT_EQ(file_info.name, L"change.txt");
	ASSERT_EQ(file_info.size, 100);

	info.old_name = L"change.txt";
	info.new_name = L"새 텍스트 파일.txt";
	item_dao.ChangeItemName(info, parent_id);
}

TEST(ItemDao, DeleteItemInfo) {
	std::optional<int> num = item_dao.DeleteItemInfo(L"test.jpg", parent_id);
	ASSERT_TRUE(num.has_value());
	ASSERT_EQ(1, num.value());

	std::optional<std::vector<monitor_client::common_utility::FileInfo>> result = item_dao.GetFolderContainList(parent_id);
	ASSERT_TRUE(result.has_value());

	auto v = result.value();
	std::vector<monitor_client::common_utility::FileInfo> v2;
	v2.clear();

	monitor_client::common_utility::FileInfo info;
	info.name = L"새 텍스트 파일.txt";
	info.size = 100;
	v2.push_back(info);

	info.name = L"새 폴더";
	info.size = -1;
	v2.push_back(info);

	ASSERT_EQ(v.size(), v2.size());

	for (int i = 0; i < v.size(); ++i) {
		ASSERT_EQ(v[i].name, v2[i].name);
		ASSERT_EQ(v[i].size, v2[i].size);
	}
}

TEST(ItemDao, InsertFileInfo) {
	monitor_client::common_utility::FileInfo info;
	info.name = L"test.xlsx";
	info.size = 1024;

	std::optional<int> num = item_dao.InsertFileInfo(info, 0);
	ASSERT_TRUE(num.has_value());
	ASSERT_EQ(1, num.value());

	std::optional<std::vector<monitor_client::common_utility::FileInfo>> result = item_dao.GetFolderContainList(0);
	ASSERT_TRUE(result.has_value());

	auto v = result.value();
	std::vector<monitor_client::common_utility::FileInfo> v2;

	info.name = L"새 텍스트 파일.txt";
	info.size = 10;
	v2.push_back(info);

	info.name = L"새 폴더";
	info.size = -1;
	v2.push_back(info);

	info.name = L"test.xlsx";
	info.size = 1024;
	v2.push_back(info);

	ASSERT_EQ(v.size(), v2.size());

	for (int i = 0; i < v.size(); ++i) {
		ASSERT_EQ(v[i].name, v2[i].name);
		ASSERT_EQ(v[i].size, v2[i].size);
	}

	item_dao.DeleteItemInfo(L"test.xlsx", 0);
}

TEST(ItemDao, ModifyFileInfo) {
	monitor_client::common_utility::FileInfo info;
	info.name = L"새 텍스트 파일.txt";
	info.size = 500;

	std::optional<int> num = item_dao.ModifyFileInfo(info, 0);
	ASSERT_TRUE(num.has_value());
	ASSERT_EQ(1, num.value());

	std::optional<monitor_client::common_utility::FileInfo> result = item_dao.GetFileInfo(info.name, 0);
	ASSERT_TRUE(result.has_value());

	auto new_info = result.value();
	ASSERT_EQ(L"새 텍스트 파일.txt", new_info.name);
	ASSERT_EQ(500, new_info.size);

	info.name = L"새 텍스트 파일.txt";
	info.size = 10;

	item_dao.ModifyFileInfo(info, 0);
}

TEST(ItemDao, InsertFolderInfo) {
	monitor_client::common_utility::FolderInfo folder_info;
	folder_info.name = L"new folder";

	item_dao.InsertFolderInfo(folder_info, 0);

	std::optional<std::vector<monitor_client::common_utility::FileInfo>> result = item_dao.GetFolderContainList(0);
	ASSERT_TRUE(result.has_value());

	auto v = result.value();
	std::vector<monitor_client::common_utility::FileInfo> v2;

	monitor_client::common_utility::FileInfo file_info;
	file_info.name = L"새 텍스트 파일.txt";
	file_info.size = 10;
	v2.push_back(file_info);

	file_info.name = L"새 폴더";
	file_info.size = -1;
	v2.push_back(file_info);

	file_info.name = L"new folder";
	file_info.size = -1;
	v2.push_back(file_info);

	ASSERT_EQ(v.size(), v2.size());

	for (int i = 0; i < v.size(); ++i) {
		ASSERT_EQ(v[i].name, v2[i].name);
		ASSERT_EQ(v[i].size, v2[i].size);
	}

	item_dao.DeleteItemInfo(L"new folder", 0);
}

int main(int argc, char* argv[]) {
	::testing::InitGoogleTest(&argc, argv);

	::testing::AddGlobalTestEnvironment(new TestEnvironment);

	return RUN_ALL_TESTS();
}