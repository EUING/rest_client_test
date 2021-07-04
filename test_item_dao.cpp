#include "pch.h"

#include <optional>

#include "../monitor_client/common_utility.h"
#include "../monitor_client/item_dao_sqlite.h"

#include "../monitor_client/sqlite_wrapper.h"

const wchar_t* const kTestDb = L"C:\\Users\\ABO\\Desktop\\�� ����\\test_items.db";

class ItemDao : public ::testing::Test {
public:
	void SetUp() override {
		std::unique_ptr<sqlite_manager::utf16::SqliteWrapper> sqlite_wrapper = sqlite_manager::utf16::SqliteWrapper::Create(kTestDb);

		sqlite_wrapper->ExecuteUpdate(L"CREATE TABLE items(id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, parent_id INTEGER NOT NULL, name TEXT NOT NULL, size INTERGER, UNIQUE(parent_id, name));");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, parent_id) VALUES(10, '�� �ؽ�Ʈ ����.txt', 0);");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, parent_id) VALUES(-1, '�� ����', 0);");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, parent_id) VALUES(5000, 'test.jpg', 2);");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, parent_id) VALUES(100, '�� �ؽ�Ʈ ����.txt', 2);");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, parent_id) VALUES(-1, '�� ����', 2);");
	}

	void TearDown() override {
		std::unique_ptr<sqlite_manager::utf16::SqliteWrapper> sqlite_wrapper = sqlite_manager::utf16::SqliteWrapper::Create(L"C:\\Users\\ABO\\Desktop\\�� ����\\test_items.db");

		sqlite_wrapper->ExecuteUpdate(L"DROP TABLE items");
	}
};

TEST_F(ItemDao, OpenMemory) {
	monitor_client::ItemDaoSqlite item_dao;
	bool result = item_dao.OpenDatabase(L":memory:");
	ASSERT_TRUE(result);
}

TEST_F(ItemDao, GetItemId) {
	monitor_client::ItemDaoSqlite item_dao;
	ASSERT_TRUE(item_dao.OpenDatabase(kTestDb));

	std::optional<int> result = item_dao.GetItemId(L"�� ����", 0);
	ASSERT_TRUE(result.has_value());
    
	ASSERT_EQ(result.value(), 2);
}

TEST_F(ItemDao, GetFileInfo) {
	monitor_client::ItemDaoSqlite item_dao;
	ASSERT_TRUE(item_dao.OpenDatabase(kTestDb));

	std::optional<monitor_client::common_utility::FileInfo> result = item_dao.GetFileInfo(L"�� �ؽ�Ʈ ����.txt", -1);
	ASSERT_FALSE(result.has_value());

	result = item_dao.GetFileInfo(L"�� �ؽ�Ʈ ����.txt", 0);
	ASSERT_TRUE(result.has_value());

	auto info = result.value();
	ASSERT_EQ(info.name, L"�� �ؽ�Ʈ ����.txt");
	ASSERT_EQ(info.size, 10);

	result = item_dao.GetFileInfo(L"�� �ؽ�Ʈ ����.txt", 2);
	ASSERT_TRUE(result.has_value());

	info = result.value();
	ASSERT_EQ(info.name, L"�� �ؽ�Ʈ ����.txt");
	ASSERT_EQ(info.size, 100);
}

TEST_F(ItemDao, GetFolderContainList) {
	monitor_client::ItemDaoSqlite item_dao;
	ASSERT_TRUE(item_dao.OpenDatabase(kTestDb));

	std::optional<std::vector<monitor_client::common_utility::FileInfo>> result = item_dao.GetFolderContainList(0);
	ASSERT_TRUE(result.has_value());

	auto v = result.value();
	std::vector<monitor_client::common_utility::FileInfo> v2;

	monitor_client::common_utility::FileInfo info;
	info.name = L"�� �ؽ�Ʈ ����.txt";
	info.size = 10;
	v2.push_back(info);
	
	info.name = L"�� ����";
	info.size = -1;
	v2.push_back(info);

	ASSERT_EQ(v.size(), v2.size());

	for (int i = 0; i < v.size(); ++i) {
		ASSERT_EQ(v[i].name, v2[i].name);
		ASSERT_EQ(v[i].size, v2[i].size);
	}

	result = item_dao.GetFolderContainList(2);
	ASSERT_TRUE(result.has_value());

	v = result.value();
	v2.clear();

	info.name = L"test.jpg";
	info.size = 5000;
	v2.push_back(info);

	info.name = L"�� �ؽ�Ʈ ����.txt";
	info.size = 100;
	v2.push_back(info);

	info.name = L"�� ����";
	info.size = -1;
	v2.push_back(info);
	
	ASSERT_EQ(v.size(), v2.size());

	for (int i = 0; i < v.size(); ++i) {
		ASSERT_EQ(v[i].name, v2[i].name);
		ASSERT_EQ(v[i].size, v2[i].size);
	}
}

TEST_F(ItemDao, ChangeItemName) {
	monitor_client::ItemDaoSqlite item_dao;
	ASSERT_TRUE(item_dao.OpenDatabase(kTestDb));

	monitor_client::common_utility::ChangeNameInfo info;
	info.old_name = L"�� �ؽ�Ʈ ����.txt";
	info.new_name = L"change.txt";

	std::optional<int> result = item_dao.ChangeItemName(info, 2);
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(1, result.value());

	std::optional<monitor_client::common_utility::FileInfo> get = item_dao.GetFileInfo(L"�� �ؽ�Ʈ ����.txt", 2);
	ASSERT_FALSE(get.has_value());

	get = item_dao.GetFileInfo(L"change.txt", 2);
	ASSERT_TRUE(get.has_value());
	auto file_info = get.value();

	ASSERT_EQ(file_info.name, L"change.txt");
	ASSERT_EQ(file_info.size, 100);
}

TEST_F(ItemDao, DeleteItemInfo) {
	monitor_client::ItemDaoSqlite item_dao;
	ASSERT_TRUE(item_dao.OpenDatabase(kTestDb));

	std::optional<int> num = item_dao.DeleteItemInfo(L"test.jpg", 2);
	ASSERT_TRUE(num.has_value());
	ASSERT_EQ(1, num.value());

	std::optional<std::vector<monitor_client::common_utility::FileInfo>> result = item_dao.GetFolderContainList(2);
	ASSERT_TRUE(result.has_value());

	auto v = result.value();
	std::vector<monitor_client::common_utility::FileInfo> v2;
	v2.clear();

	monitor_client::common_utility::FileInfo info;
	info.name = L"�� �ؽ�Ʈ ����.txt";
	info.size = 100;
	v2.push_back(info);

	info.name = L"�� ����";
	info.size = -1;
	v2.push_back(info);

	ASSERT_EQ(v.size(), v2.size());

	for (int i = 0; i < v.size(); ++i) {
		ASSERT_EQ(v[i].name, v2[i].name);
		ASSERT_EQ(v[i].size, v2[i].size);
	}
}

TEST_F(ItemDao, InsertFileInfo) {
	monitor_client::ItemDaoSqlite item_dao;
	ASSERT_TRUE(item_dao.OpenDatabase(kTestDb));

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

	info.name = L"test.xlsx";
	info.size = 1024;
	v2.push_back(info);

	info.name = L"�� �ؽ�Ʈ ����.txt";
	info.size = 10;
	v2.push_back(info);

	info.name = L"�� ����";
	info.size = -1;
	v2.push_back(info);	

	ASSERT_EQ(v.size(), v2.size());

	for (int i = 0; i < v.size(); ++i) {
		ASSERT_EQ(v[i].name, v2[i].name);
		ASSERT_EQ(v[i].size, v2[i].size);
	}
}

TEST_F(ItemDao, ModifyFileInfo) {
	monitor_client::ItemDaoSqlite item_dao;
	ASSERT_TRUE(item_dao.OpenDatabase(kTestDb));

	monitor_client::common_utility::FileInfo info;
	info.name = L"�� �ؽ�Ʈ ����.txt";
	info.size = 500;

	std::optional<int> num = item_dao.ModifyFileInfo(info, 0);
	ASSERT_TRUE(num.has_value());
	ASSERT_EQ(1, num.value());

	std::optional<monitor_client::common_utility::FileInfo> result = item_dao.GetFileInfo(info.name, 0);
	ASSERT_TRUE(result.has_value());

	auto new_info = result.value();
	ASSERT_EQ(L"�� �ؽ�Ʈ ����.txt", new_info.name);
	ASSERT_EQ(500, new_info.size);
}

TEST_F(ItemDao, InsertFolderInfo) {
	monitor_client::ItemDaoSqlite item_dao;
	ASSERT_TRUE(item_dao.OpenDatabase(kTestDb));

	monitor_client::common_utility::FolderInfo folder_info;
	folder_info.name = L"new folder";

	item_dao.InsertFolderInfo(folder_info, 0);

	std::optional<std::vector<monitor_client::common_utility::FileInfo>> result = item_dao.GetFolderContainList(0);
	ASSERT_TRUE(result.has_value());

	auto v = result.value();
	std::vector<monitor_client::common_utility::FileInfo> v2;

	monitor_client::common_utility::FileInfo file_info;
	file_info.name = L"new folder";
	file_info.size = -1;
	v2.push_back(file_info);
	
	file_info.name = L"�� �ؽ�Ʈ ����.txt";
	file_info.size = 10;
	v2.push_back(file_info);

	file_info.name = L"�� ����";
	file_info.size = -1;
	v2.push_back(file_info);	

	ASSERT_EQ(v.size(), v2.size());

	for (int i = 0; i < v.size(); ++i) {
		ASSERT_EQ(v[i].name, v2[i].name);
		ASSERT_EQ(v[i].size, v2[i].size);
	}
}