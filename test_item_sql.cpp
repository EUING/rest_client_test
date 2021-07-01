#include "pch.h"

#include <optional>

#include "../monitor_client/sqlite_wrapper.h"

#include "../monitor_client/common_utility.h"
#include "../monitor_client/item_dao.h"
#include "../monitor_client/item_sql.h"

const wchar_t* const kTestDb = L"C:\\Users\\ABO\\Desktop\\�� ����\\test_items.db";

class ItemSql : public ::testing::Test {
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

TEST_F(ItemSql, RenamedItem) {
	monitor_client::ItemDao item_dao;
	ASSERT_TRUE(item_dao.OpenDatabase(kTestDb));
	monitor_client::ItemSql sql(&item_dao);

	monitor_client::common_utility::ChangeNameInfo info;
	info.old_name = L"�� ����/�� �ؽ�Ʈ ����.txt";
	info.new_name = L"�� ����/�� �ؽ�Ʈ ����2.txt";

	ASSERT_TRUE(sql.RenameItem(info));

	std::optional<monitor_client::common_utility::FileInfo> result = sql.GetFileInfo(L"�� ����/�� �ؽ�Ʈ ����2.txt");
	ASSERT_TRUE(result.has_value());

	auto file_info = result.value();
	ASSERT_EQ(L"�� �ؽ�Ʈ ����2.txt", file_info.name);
	ASSERT_EQ(100, file_info.size);
}

TEST_F(ItemSql, RemoveItem) {
	monitor_client::ItemDao item_dao;
	ASSERT_TRUE(item_dao.OpenDatabase(kTestDb));
	monitor_client::ItemSql sql(&item_dao);

	ASSERT_TRUE(sql.RemoveItem(L"�� ����/�� ����"));

	std::optional<monitor_client::common_utility::FileInfo> result = sql.GetFileInfo(L"�� ����/�� ����");
	ASSERT_FALSE(result.has_value());

	ASSERT_TRUE(sql.RemoveItem(L"�� ����"));

	result = sql.GetFileInfo(L"�� ����/test.jpg");
	ASSERT_FALSE(result.has_value());

	result = sql.GetFileInfo(L"�� ����/�� �ؽ�Ʈ ����.txt");
	ASSERT_FALSE(result.has_value());
}

TEST_F(ItemSql, AddFile) {
	monitor_client::ItemDao item_dao;
	ASSERT_TRUE(item_dao.OpenDatabase(kTestDb));
	monitor_client::ItemSql sql(&item_dao);

	monitor_client::common_utility::FileInfo file_info;
	file_info.name = L"�� �ؽ�Ʈ ����.txt";
	file_info.size = 10;

	ASSERT_FALSE(sql.AddFile(file_info));
	file_info.name = L"image.bmp";
	file_info.size = 10000;

	ASSERT_TRUE(sql.AddFile(file_info));

	std::optional<monitor_client::common_utility::FileInfo> result = sql.GetFileInfo(L"image.bmp");
	ASSERT_TRUE(result.has_value());

	auto info = result.value();
	ASSERT_EQ(info.name, L"image.bmp");
	ASSERT_EQ(info.size, 10000);
}

TEST_F(ItemSql, ModifyFile) {
	monitor_client::ItemDao item_dao;
	ASSERT_TRUE(item_dao.OpenDatabase(kTestDb));
	monitor_client::ItemSql sql(&item_dao);

	monitor_client::common_utility::FileInfo file_info;
	file_info.name = L"�� �ؽ�Ʈ ����.txt";
	file_info.size = 1234;
	
	ASSERT_TRUE(sql.ModifyFile(file_info));

	std::optional<monitor_client::common_utility::FileInfo> result = sql.GetFileInfo(L"�� �ؽ�Ʈ ����.txt");
	ASSERT_TRUE(result.has_value());

	auto info = result.value();
	ASSERT_EQ(info.name, L"�� �ؽ�Ʈ ����.txt");
	ASSERT_EQ(info.size, 1234);
}

TEST_F(ItemSql, AddFolder) {
	monitor_client::ItemDao item_dao;
	ASSERT_TRUE(item_dao.OpenDatabase(kTestDb));
	monitor_client::ItemSql sql(&item_dao);

	monitor_client::common_utility::FolderInfo folder_info;
	folder_info.name = L"�� ����/new folder";

	ASSERT_TRUE(sql.AddFolder(folder_info));

	std::optional<monitor_client::common_utility::FileInfo> result = sql.GetFileInfo(L"�� ����/new folder");
	ASSERT_TRUE(result.has_value());

	auto file_info = result.value();
	ASSERT_EQ(file_info.name, L"new folder");
	ASSERT_EQ(-1, file_info.size);
}