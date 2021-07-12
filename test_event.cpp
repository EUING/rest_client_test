#include "pch.h"

#include <Windows.h>
#include <stdint.h>

#include <optional>
#include <memory>
#include <string>

#include "../monitor_client/common_utility.h"
#include "../monitor_client/common_struct.h"
#include "../monitor_client/item_dao_sqlite.h"
#include "../monitor_client/common_struct.h"
#include "../monitor_client/base_event.h"
#include "../monitor_client/upload_event.h"
#include "../monitor_client/download_event.h"
#include "../monitor_client/conflict_event.h"
#include "../monitor_client/remove_event.h"
#include "../monitor_client/local_remove_event.h"
#include "../monitor_client/rename_event.h"

class EventTest : public ::testing::Test {
public:
	monitor_client::ItemRequest* item_request;
	PROCESS_INFORMATION process_info;

	void SetUp() override {
		auto manager_db = std::make_unique<monitor_client::ItemDaoSqlite>();
		manager_db->OpenDatabase(L":memory:");
		monitor_client::common_utility::NetworkInfo network_info{ L"localhost", 8080 };

		monitor_client::common_utility::ItemInfo info;
		info.name = L"1_1";
		info.size = -1;
		info.hash.clear();
		manager_db->UpdateItemInfo(info, 0);

		info.name = L"1_1_1";
		info.size = -1;
		info.hash.clear();
		manager_db->UpdateItemInfo(info, 1);

		info.name = L"1_1_1_1.txt";
		info.size = 0;
		info.hash = L"hash";
		manager_db->UpdateItemInfo(info, 2);

		item_request = new monitor_client::ItemRequest(network_info, std::move(manager_db));
		std::wstring file_path = L"C:\\Users\\ABO\\Desktop\\1";
		_wmkdir(file_path.c_str());

		file_path = L"C:\\Users\\ABO\\Desktop\\1\\1_1";
		_wmkdir(file_path.c_str());

		file_path = L"C:\\Users\\ABO\\Desktop\\1\\1_1\\1_1_1";
		_wmkdir(file_path.c_str());

		file_path = L"C:\\Users\\ABO\\Desktop\\1\\1_2";
		_wmkdir(file_path.c_str());

		file_path = L"C:\\Users\\ABO\\Desktop\\1\\1_2\\1_2_1";
		_wmkdir(file_path.c_str());

		file_path = L"C:\\Users\\ABO\\Desktop\\1\\1_3";
		_wmkdir(file_path.c_str());

		file_path = L"C:\\Users\\ABO\\Desktop\\1\\1_3\\1_3_1";
		_wmkdir(file_path.c_str());

		file_path = L"C:\\Users\\ABO\\Desktop\\1\\1_4";
		_wmkdir(file_path.c_str());

		file_path = L"C:\\Users\\ABO\\Desktop\\1\\1_4\\1_4_1";
		_wmkdir(file_path.c_str());

		std::wstring file_name = L"C:\\Users\\ABO\\Desktop\\1\\1_1\\1_1_1\\1_1_1_1.txt";
		std::ofstream temp{ file_name };
		temp.close();

		temp.open(L"C:\\Users\\ABO\\Desktop\\1\\1_2\\1_2_1\\1_2_1_1.txt");
		temp.close();

		temp.open(L"C:\\Users\\ABO\\Desktop\\1\\1_3\\1_3_1\\1_3_1_1.txt");
		temp.close();

		temp.open(L"C:\\Users\\ABO\\Desktop\\1\\1_4\\1_4_1\\1_4_1_1.txt");
		temp.close();

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
		_wsystem(L"rmdir /s /q C:\\Users\\ABO\\Desktop\\1");
		delete item_request;
	}
};

TEST_F(EventTest, Upload) {
	std::wstring target_folder = L"C:\\Users\\ABO\\Desktop\\1";
	ASSERT_TRUE(SetCurrentDirectory(target_folder.c_str()));

	monitor_client::common_utility::ItemInfo info;
	info.name = L"1_1/1_1_1/1_2_1_1.txt";
	info.size = 123;
	info.hash = L"hash";

	monitor_client::UploadEvent event(info);
	ASSERT_TRUE(event(*item_request));
}

TEST_F(EventTest, Download) {
	std::wstring target_folder = L"C:\\Users\\ABO\\Desktop\\1";
	ASSERT_TRUE(SetCurrentDirectory(target_folder.c_str()));

	monitor_client::common_utility::ItemInfo info;
	std::wstring file_name = L"1_1/1_1_1/1_1_1_1.txt";

	monitor_client::DownloadEvent event(file_name);
	ASSERT_TRUE(event(*item_request));
}

TEST_F(EventTest, Conflict) {
	std::wstring target_folder = L"C:\\Users\\ABO\\Desktop\\1";
	ASSERT_TRUE(SetCurrentDirectory(target_folder.c_str()));

	monitor_client::common_utility::ItemInfo info;
	std::wstring file_name = L"1_1/1_1_1/1_1_1_1.txt";

	monitor_client::ConflictEvent event(file_name);
	ASSERT_TRUE(event(*item_request));
	
	std::wstring conflict_name = L"1_1/1_1_1/1_1_1_1.txt.conflict";
	std::optional<bool> is_dir = monitor_client::common_utility::IsDirectory(conflict_name);
	ASSERT_TRUE(is_dir.has_value());
	ASSERT_FALSE(is_dir.value());
}

TEST_F(EventTest, Remove) {
	std::wstring target_folder = L"C:\\Users\\ABO\\Desktop\\1";
	ASSERT_TRUE(SetCurrentDirectory(target_folder.c_str()));

	monitor_client::common_utility::ItemInfo info;
	std::wstring file_name = L"1_1/1_1_1/1_1_1_1.txt";

	monitor_client::RemoveEvent event(file_name);
	ASSERT_TRUE(event(*item_request));
}

TEST_F(EventTest, LocalRemove) {
	std::wstring target_folder = L"C:\\Users\\ABO\\Desktop\\1";
	ASSERT_TRUE(SetCurrentDirectory(target_folder.c_str()));

	monitor_client::common_utility::ItemInfo info;
	std::wstring file_name = L"1_1/1_1_1";

	monitor_client::LocalRemoveEvent event(file_name);
	ASSERT_TRUE(event(*item_request));

	std::optional<bool> is_dir = monitor_client::common_utility::IsDirectory(L"1_1/1_1_1/1_1_1_1.txt");
	ASSERT_FALSE(is_dir.has_value());

	is_dir = monitor_client::common_utility::IsDirectory(L"1_1/1_1_1");
	ASSERT_FALSE(is_dir.has_value());

	ASSERT_FALSE(item_request->LocalRemoveRequest(L"1_1/1_1_1/1_1_1_1.txt"));
	ASSERT_FALSE(item_request->LocalRemoveRequest(L"1_1/1_1_1"));
}

TEST_F(EventTest, Rename) {
	std::wstring target_folder = L"C:\\Users\\ABO\\Desktop\\1";
	ASSERT_TRUE(SetCurrentDirectory(target_folder.c_str()));

	monitor_client::common_utility::ChangeNameInfo info;
	info.old_name = L"1_1/1_1_1/1_1_1_1.txt";
	info.new_name = L"1_1/1_1_1/1_1_1_1.jpg";

	monitor_client::RenameEvent event(info);
	ASSERT_TRUE(event(*item_request));
}