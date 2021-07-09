#include "pch.h"

#include <optional>
#include <vector>

#include "../monitor_client/sqlite_wrapper.h"

#include "../monitor_client/item_dao_sqlite.h"
#include "../monitor_client/local_db.h"
#include "../monitor_client/item_http.h"
#include "../monitor_client/item_request.h"

const wchar_t* const kTestDb = L"C:\\Users\\ABO\\Desktop\\새 폴더\\test_items.db";

class RequestTest : public ::testing::Test {
public:
	monitor_client::ItemRequest* item_request;
	PROCESS_INFORMATION process_info;

	void SetUp() override {
		std::unique_ptr<sqlite_manager::utf16::SqliteWrapper> sqlite_wrapper = sqlite_manager::utf16::SqliteWrapper::Create(kTestDb);

		sqlite_wrapper->ExecuteUpdate(L"CREATE TABLE items(id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, parent_id INTEGER NOT NULL, name TEXT NOT NULL, size INTERGER NOT NULL, hash TEXT NOT NULL, UNIQUE(parent_id, name));");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, hash, parent_id) VALUES(10, '새 텍스트 파일.txt', 'hash', 0);");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, hash, parent_id) VALUES(-1, '새 폴더', '', 0);");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, hash, parent_id) VALUES(5000, 'test.jpg', 'hash', 2);");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, hash, parent_id) VALUES(100, '새 텍스트 파일.txt', 'hash', 2);");

		sqlite_wrapper->ExecuteUpdate(L"INSERT INTO items(size, name, hash, parent_id) VALUES(-1, '새 폴더', '', 2);");

		auto item_dao = std::make_unique<monitor_client::ItemDaoSqlite>();
		item_dao->OpenDatabase(kTestDb);

		item_request = new monitor_client::ItemRequest({ L"localhost", 8080 }, std::move(item_dao));

		memset(&process_info, 0, sizeof(process_info));

		STARTUPINFO startup_info;
		memset(&startup_info, 0, sizeof(startup_info));
		startup_info.cb = sizeof(STARTUPINFO);
		startup_info.dwFlags = STARTF_USESHOWWINDOW;
		startup_info.wShowWindow = SW_HIDE;

		wchar_t command_line[] = L"C:\\Users\\ABO\\Desktop\\repos\\monitor_dummy_server\\x64\\Release\\monitor_dummy_server.exe";
		CreateProcess(NULL, command_line, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startup_info, &process_info);
		Sleep(500);
	}

	void TearDown() override {
		TerminateProcess(process_info.hProcess, 0);
		WaitForSingleObject(process_info.hProcess, 500);
		CloseHandle(process_info.hProcess);
		CloseHandle(process_info.hThread);

		delete item_request;

		std::unique_ptr<sqlite_manager::utf16::SqliteWrapper> sqlite_wrapper = sqlite_manager::utf16::SqliteWrapper::Create(L"C:\\Users\\ABO\\Desktop\\새 폴더\\test_items.db");

		sqlite_wrapper->ExecuteUpdate(L"DROP TABLE items");
	}
};

TEST_F(RequestTest, UploadTest) {
	monitor_client::common_utility::ItemInfo info;
	info.name = L"test";
	info.size = 1000;
	info.hash = L"HASH";

	ASSERT_TRUE(item_request->UploadRequest(info));
}

TEST_F(RequestTest, DownloadTest) {
	ASSERT_TRUE(item_request->DownloadRequest(L"Test"));
}

TEST_F(RequestTest, RenameTest) {
	monitor_client::common_utility::ChangeNameInfo info;
	info.old_name = L"새 텍스트 파일.txt";
	info.new_name = L"바꿈.txt";

	ASSERT_TRUE(item_request->RenameRequest(info));
}

TEST_F(RequestTest, RemoveTest) {
	ASSERT_TRUE(item_request->RemoveRequest(L"새 폴더"));
}