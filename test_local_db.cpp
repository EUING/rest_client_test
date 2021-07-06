#include "pch.h"

#include <optional>

#include "../monitor_client/sqlite_wrapper.h"

#include "../monitor_client/common_utility.h"
#include "../monitor_client/item_dao_sqlite.h"
#include "../monitor_client/local_db.h"

const wchar_t* const kTestDb = L"C:\\Users\\ABO\\Desktop\\새 폴더\\test_items.db";

class LocalDb : public ::testing::Test {
public:
	void SetUp() override {
		std::unique_ptr<sqlite_manager::utf16::SqliteWrapper> sqlite_wrapper = sqlite_manager::utf16::SqliteWrapper::Create(kTestDb);

		sqlite_wrapper->ExecuteUpdate(L"CREATE TABLE items(id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, parent_id INTEGER NOT NULL, name TEXT NOT NULL, size INTERGER, UNIQUE(parent_id, name));");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, parent_id) VALUES(10, '새 텍스트 파일.txt', 0);");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, parent_id) VALUES(-1, '새 폴더', 0);");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, parent_id) VALUES(5000, 'test.jpg', 2);");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, parent_id) VALUES(100, '새 텍스트 파일.txt', 2);");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, parent_id) VALUES(-1, '새 폴더', 2);");
	}

	void TearDown() override {
		std::unique_ptr<sqlite_manager::utf16::SqliteWrapper> sqlite_wrapper = sqlite_manager::utf16::SqliteWrapper::Create(L"C:\\Users\\ABO\\Desktop\\새 폴더\\test_items.db");

		sqlite_wrapper->ExecuteUpdate(L"DROP TABLE items");
	}
};

TEST_F(LocalDb, AddItem) {
	std::unique_ptr<monitor_client::ItemDao> item_dao_sqlite = std::make_unique<monitor_client::ItemDaoSqlite>();
	ASSERT_TRUE(item_dao_sqlite->OpenDatabase(kTestDb));
	monitor_client::LocalDb sql(std::move(item_dao_sqlite));

	monitor_client::common_utility::ItemInfo item_info;
	item_info.name = L"새 텍스트 파일.txt";
	item_info.size = 10;

	ASSERT_TRUE(sql.InsertItem(item_info));
	item_info.name = L"image.bmp";
	item_info.size = 10000;

	ASSERT_TRUE(sql.InsertItem(item_info));

	std::optional<monitor_client::common_utility::ItemInfo> result = sql.GetItemInfo(L"image.bmp");
	ASSERT_TRUE(result.has_value());

	auto info = result.value();
	ASSERT_EQ(info.name, L"image.bmp");
	ASSERT_EQ(info.size, 10000);
}

TEST_F(LocalDb, ModifyItem) {
	std::unique_ptr<monitor_client::ItemDao> item_dao_sqlite = std::make_unique<monitor_client::ItemDaoSqlite>();
	ASSERT_TRUE(item_dao_sqlite->OpenDatabase(kTestDb));
	monitor_client::LocalDb sql(std::move(item_dao_sqlite));

	monitor_client::common_utility::ItemInfo item_info;
	item_info.name = L"새 텍스트 파일.txt";
	item_info.size = 1234;
	
	ASSERT_TRUE(sql.InsertItem(item_info));

	std::optional<monitor_client::common_utility::ItemInfo> result = sql.GetItemInfo(L"새 텍스트 파일.txt");
	ASSERT_TRUE(result.has_value());

	auto info = result.value();
	ASSERT_EQ(info.name, L"새 텍스트 파일.txt");
	ASSERT_EQ(info.size, 1234);
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