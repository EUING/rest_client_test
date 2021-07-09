#include "pch.h"

#include <optional>
#include <vector>

#include "../monitor_client/sqlite_wrapper.h"

#include "../monitor_client/common_utility.h"
#include "../monitor_client/item_dao_sqlite.h"
#include "../monitor_client/local_db.h"

const wchar_t* const kTestDb = L"C:\\Users\\ABO\\Desktop\\새 폴더\\test_items.db";

class LocalDb : public ::testing::Test {
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

TEST_F(LocalDb, GetParentId) {
	std::unique_ptr<monitor_client::ItemDao> item_dao_sqlite = std::make_unique<monitor_client::ItemDaoSqlite>();
	ASSERT_TRUE(item_dao_sqlite->OpenDatabase(kTestDb));
	monitor_client::LocalDb sql(std::move(item_dao_sqlite));

	std::optional<int> result = sql.GetParentId(L"새 폴더");
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(0, result.value());

	result = sql.GetParentId(L"없는 폴더/없는 파일");
	ASSERT_FALSE(result.has_value());
	
	result = sql.GetParentId(L"새 폴더/test.jpg");
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(2, result.value());
}

TEST_F(LocalDb, GetItemInfo) {
	std::unique_ptr<monitor_client::ItemDao> item_dao_sqlite = std::make_unique<monitor_client::ItemDaoSqlite>();
	ASSERT_TRUE(item_dao_sqlite->OpenDatabase(kTestDb));
	monitor_client::LocalDb sql(std::move(item_dao_sqlite));

	std::optional<monitor_client::common_utility::ItemInfo> result = sql.GetItemInfo(L"새 폴더");
	ASSERT_TRUE(result.has_value());
	auto item_info = result.value();

	result = sql.GetItemInfo(L"없음.txt");
	ASSERT_FALSE(result.has_value());

	ASSERT_EQ(item_info.name, L"새 폴더");
	ASSERT_EQ(item_info.size, -1);
	ASSERT_TRUE(item_info.hash.empty());

	result = sql.GetItemInfo(L"새 텍스트 파일.txt");
	ASSERT_TRUE(result.has_value());
	item_info = result.value();

	ASSERT_EQ(item_info.name, L"새 텍스트 파일.txt");
	ASSERT_EQ(item_info.size, 10);
	ASSERT_EQ(item_info.hash, L"hash");	

	result = sql.GetItemInfo(L"새 폴더/새 텍스트 파일.txt");
	ASSERT_TRUE(result.has_value());
	item_info = result.value();

	ASSERT_EQ(item_info.name, L"새 텍스트 파일.txt");
	ASSERT_EQ(item_info.size, 100);
	ASSERT_EQ(item_info.hash, L"hash");
}

TEST_F(LocalDb, GetFolderContainList) {
	std::unique_ptr<monitor_client::ItemDao> item_dao_sqlite = std::make_unique<monitor_client::ItemDaoSqlite>();
	ASSERT_TRUE(item_dao_sqlite->OpenDatabase(kTestDb));
	monitor_client::LocalDb sql(std::move(item_dao_sqlite));

	std::optional<std::vector<monitor_client::common_utility::ItemInfo>> result = sql.GetFolderContainList(L"없는 폴더");
	ASSERT_FALSE(result.has_value());

	result = sql.GetFolderContainList(L"새 텍스트 파일.txt");
	ASSERT_FALSE(result.has_value());

	result = sql.GetFolderContainList();
	ASSERT_TRUE(result.has_value());

	std::vector<monitor_client::common_utility::ItemInfo> v = result.value();
	std::vector<monitor_client::common_utility::ItemInfo> v2;

	monitor_client::common_utility::ItemInfo info;
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
		ASSERT_EQ(v[i].hash, v2[i].hash);
	}

	v.clear();
	v2.clear();

	result = sql.GetFolderContainList(L"새 폴더");
	ASSERT_TRUE(result.has_value());
	v = result.value();

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
		ASSERT_EQ(v[i].hash, v2[i].hash);
	}
}

TEST_F(LocalDb, AddItem) {
	std::unique_ptr<monitor_client::ItemDao> item_dao_sqlite = std::make_unique<monitor_client::ItemDaoSqlite>();
	ASSERT_TRUE(item_dao_sqlite->OpenDatabase(kTestDb));
	monitor_client::LocalDb sql(std::move(item_dao_sqlite));

	monitor_client::common_utility::ItemInfo item_info;
	item_info.name = L"새 텍스트 파일.txt";
	item_info.size = 10;
	item_info.hash = L"hash";

	ASSERT_TRUE(sql.UpdateItem(item_info));
	item_info.name = L"image.bmp";
	item_info.size = 10000;
	item_info.hash = L"hash";

	ASSERT_TRUE(sql.UpdateItem(item_info));

	std::optional<monitor_client::common_utility::ItemInfo> result = sql.GetItemInfo(L"image.bmp");
	ASSERT_TRUE(result.has_value());

	auto info = result.value();
	ASSERT_EQ(info.name, L"image.bmp");
	ASSERT_EQ(info.size, 10000);
	ASSERT_EQ(info.hash, L"hash");
}

TEST_F(LocalDb, ModifyItem) {
	std::unique_ptr<monitor_client::ItemDao> item_dao_sqlite = std::make_unique<monitor_client::ItemDaoSqlite>();
	ASSERT_TRUE(item_dao_sqlite->OpenDatabase(kTestDb));
	monitor_client::LocalDb sql(std::move(item_dao_sqlite));

	monitor_client::common_utility::ItemInfo item_info;
	item_info.name = L"새 텍스트 파일.txt";
	item_info.size = 1234;
	item_info.hash = L"hashhash";

	ASSERT_TRUE(sql.UpdateItem(item_info));

	std::optional<monitor_client::common_utility::ItemInfo> result = sql.GetItemInfo(L"새 텍스트 파일.txt");
	ASSERT_TRUE(result.has_value());

	auto info = result.value();
	ASSERT_EQ(info.name, L"새 텍스트 파일.txt");
	ASSERT_EQ(info.size, 1234);
	ASSERT_EQ(info.hash, L"hashhash");
}

TEST_F(LocalDb, RenamedItem) {
	std::unique_ptr<monitor_client::ItemDao> item_dao_sqlite = std::make_unique<monitor_client::ItemDaoSqlite>();
	ASSERT_TRUE(item_dao_sqlite->OpenDatabase(kTestDb));
	monitor_client::LocalDb sql(std::move(item_dao_sqlite));

	monitor_client::common_utility::ChangeNameInfo name_info;
	name_info.old_name = L"새 폴더/새 텍스트 파일.txt";
	name_info.new_name = L"새 폴더/새 텍스트 파일2.txt";

	ASSERT_TRUE(sql.RenameItem(name_info));

	std::optional<monitor_client::common_utility::ItemInfo> result = sql.GetItemInfo(L"새 폴더/새 텍스트 파일2.txt");
	ASSERT_TRUE(result.has_value());

	auto item_info = result.value();
	ASSERT_EQ(L"새 텍스트 파일2.txt", item_info.name);
	ASSERT_EQ(100, item_info.size);
	ASSERT_EQ(item_info.hash, L"hash");
}

TEST_F(LocalDb, RemoveItem) {
	std::unique_ptr<monitor_client::ItemDao> item_dao_sqlite = std::make_unique<monitor_client::ItemDaoSqlite>();
	ASSERT_TRUE(item_dao_sqlite->OpenDatabase(kTestDb));
	monitor_client::LocalDb sql(std::move(item_dao_sqlite));

	ASSERT_TRUE(sql.RemoveItem(L"새 폴더/새 폴더"));

	std::optional<monitor_client::common_utility::ItemInfo> result = sql.GetItemInfo(L"새 폴더/새 폴더");
	ASSERT_FALSE(result.has_value());

	ASSERT_TRUE(sql.RemoveItem(L"새 폴더"));

	result = sql.GetItemInfo(L"새 폴더/test.jpg");
	ASSERT_FALSE(result.has_value());

	result = sql.GetItemInfo(L"새 폴더/새 텍스트 파일.txt");
	ASSERT_FALSE(result.has_value());
}