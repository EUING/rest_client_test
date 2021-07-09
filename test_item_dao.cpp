#include "pch.h"

#include <optional>

#include "../monitor_client/common_utility.h"
#include "../monitor_client/item_dao_sqlite.h"

#include "../monitor_client/sqlite_wrapper.h"

const wchar_t* const kTestDb = L"C:\\Users\\ABO\\Desktop\\새 폴더\\test_items.db";

class ItemDao : public ::testing::Test {
public:
	void SetUp() override {
		std::unique_ptr<sqlite_manager::utf16::SqliteWrapper> sqlite_wrapper = sqlite_manager::utf16::SqliteWrapper::Create(kTestDb);

		sqlite_wrapper->ExecuteUpdate(L"CREATE TABLE items(id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, parent_id INTEGER NOT NULL, name TEXT NOT NULL, size INTERGER NOT NULL, hash TEXT NOT NULL, UNIQUE(parent_id, name));");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, hash, parent_id) VALUES(10, '새 텍스트 파일.txt', 'hash', 0);");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, hash, parent_id) VALUES(-1, '새 폴더', '', 0);");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, hash, parent_id) VALUES(5000, 'test.jpg', 'hash', 2);");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, hash, parent_id) VALUES(100, '새 텍스트 파일.txt', 'hash', 2);");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, hash, parent_id) VALUES(-1, '새 폴더', '', 2);");
	}

	void TearDown() override {
		std::unique_ptr<sqlite_manager::utf16::SqliteWrapper> sqlite_wrapper = sqlite_manager::utf16::SqliteWrapper::Create(L"C:\\Users\\ABO\\Desktop\\새 폴더\\test_items.db");

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

	std::optional<int> result = item_dao.GetItemId(L"새 폴더", 0);
	ASSERT_TRUE(result.has_value());
    
	ASSERT_EQ(result.value(), 2);
}

TEST_F(ItemDao, GetItemInfo) {
	monitor_client::ItemDaoSqlite item_dao;
	ASSERT_TRUE(item_dao.OpenDatabase(kTestDb));

	std::optional<monitor_client::common_utility::ItemInfo> result = item_dao.GetItemInfo(L"새 텍스트 파일.txt", -1);
	ASSERT_FALSE(result.has_value());

	result = item_dao.GetItemInfo(L"새 텍스트 파일.txt", 0);
	ASSERT_TRUE(result.has_value());

	auto info = result.value();
	ASSERT_EQ(info.name, L"새 텍스트 파일.txt");
	ASSERT_EQ(info.size, 10);
	ASSERT_EQ(info.hash, L"hash");

	result = item_dao.GetItemInfo(L"새 텍스트 파일.txt", 2);
	ASSERT_TRUE(result.has_value());

	info = result.value();
	ASSERT_EQ(info.name, L"새 텍스트 파일.txt");
	ASSERT_EQ(info.size, 100);
	ASSERT_EQ(info.hash, L"hash");
}

TEST_F(ItemDao, GetFolderContainList) {
	monitor_client::ItemDaoSqlite item_dao;
	ASSERT_TRUE(item_dao.OpenDatabase(kTestDb));

	std::optional<std::vector<monitor_client::common_utility::ItemInfo>> result = item_dao.GetFolderContainList(0);
	ASSERT_TRUE(result.has_value());

	auto v = result.value();
	std::vector<monitor_client::common_utility::ItemInfo> v2;

	monitor_client::common_utility::ItemInfo info;
	info.name = L"새 텍스트 파일.txt";
	info.size = 10;
	info.hash =  L"hash";
	v2.push_back(info);
	
	info.name = L"새 폴더";
	info.size = -1;
	info.hash.clear();
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
	info.hash = L"hash";
	v2.push_back(info);

	info.name = L"새 텍스트 파일.txt";
	info.size = 100;
	info.hash = L"hash";
	v2.push_back(info);

	info.name = L"새 폴더";
	info.size = -1;
	info.hash.clear();
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
	info.old_name = L"새 텍스트 파일.txt";
	info.new_name = L"change.txt";

	std::optional<int> result = item_dao.ChangeItemName(info, 2);
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(1, result.value());

	std::optional<monitor_client::common_utility::ItemInfo> get = item_dao.GetItemInfo(L"새 텍스트 파일.txt", 2);
	ASSERT_FALSE(get.has_value());

	get = item_dao.GetItemInfo(L"change.txt", 2);
	ASSERT_TRUE(get.has_value());
	auto item_info = get.value();

	ASSERT_EQ(item_info.name, L"change.txt");
	ASSERT_EQ(item_info.size, 100);
	ASSERT_EQ(item_info.hash, L"hash");
}

TEST_F(ItemDao, RemoveItemInfo) {
	monitor_client::ItemDaoSqlite item_dao;
	ASSERT_TRUE(item_dao.OpenDatabase(kTestDb));

	std::optional<int> num = item_dao.RemoveItemInfo(L"test.jpg", 2);
	ASSERT_TRUE(num.has_value());
	ASSERT_EQ(1, num.value());

	std::optional<std::vector<monitor_client::common_utility::ItemInfo>> result = item_dao.GetFolderContainList(2);
	ASSERT_TRUE(result.has_value());

	auto v = result.value();
	std::vector<monitor_client::common_utility::ItemInfo> v2;
	v2.clear();

	monitor_client::common_utility::ItemInfo info;
	info.name = L"새 텍스트 파일.txt";
	info.size = 100;
	info.hash = L"hash";
	v2.push_back(info);

	info.name = L"새 폴더";
	info.size = -1;
	info.hash.clear();
	v2.push_back(info);

	ASSERT_EQ(v.size(), v2.size());

	for (int i = 0; i < v.size(); ++i) {
		ASSERT_EQ(v[i].name, v2[i].name);
		ASSERT_EQ(v[i].size, v2[i].size);
	}
}

TEST_F(ItemDao, UpdateItemInfo) {
	monitor_client::ItemDaoSqlite item_dao;
	ASSERT_TRUE(item_dao.OpenDatabase(kTestDb));

	monitor_client::common_utility::ItemInfo info;
	info.name = L"test.xlsx";
	info.size = 1024;
	info.hash = L"hash";

	std::optional<int> num = item_dao.UpdateItemInfo(info, 0);
	ASSERT_TRUE(num.has_value());
	ASSERT_EQ(1, num.value());

	std::optional<std::vector<monitor_client::common_utility::ItemInfo>> result = item_dao.GetFolderContainList(0);
	ASSERT_TRUE(result.has_value());

	auto v = result.value();
	std::vector<monitor_client::common_utility::ItemInfo> v2;

	info.name = L"test.xlsx";
	info.size = 1024;
	info.hash = L"hash";
	v2.push_back(info);

	info.name = L"새 텍스트 파일.txt";
	info.size = 10;
	info.hash = L"hash";
	v2.push_back(info);

	info.name = L"새 폴더";
	info.size = -1;
	info.hash.clear();
	v2.push_back(info);	

	ASSERT_EQ(v.size(), v2.size());

	for (int i = 0; i < v.size(); ++i) {
		ASSERT_EQ(v[i].name, v2[i].name);
		ASSERT_EQ(v[i].size, v2[i].size);
	}
}

TEST_F(ItemDao, ModifyItemInfo) {
	monitor_client::ItemDaoSqlite item_dao;
	ASSERT_TRUE(item_dao.OpenDatabase(kTestDb));

	monitor_client::common_utility::ItemInfo info;
	info.name = L"새 텍스트 파일.txt";
	info.size = 500;
	info.hash = L"hash";

	std::optional<int> num = item_dao.ModifyItemInfo(info, 0);
	ASSERT_TRUE(num.has_value());
	ASSERT_EQ(1, num.value());

	std::optional<monitor_client::common_utility::ItemInfo> result = item_dao.GetItemInfo(info.name, 0);
	ASSERT_TRUE(result.has_value());

	auto new_info = result.value();
	ASSERT_EQ(L"새 텍스트 파일.txt", new_info.name);
	ASSERT_EQ(500, new_info.size);
	ASSERT_EQ(L"hash", new_info.hash);
}