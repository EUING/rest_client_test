#include "pch.h"

#include <Windows.h>

#include <optional>
#include <vector>

#include "../monitor_client/common_utility.h"
#include "../monitor_client/item_http.h"

class HttpTest : public ::testing::Test {
public:
	monitor_client::ItemHttp* item_http;
	PROCESS_INFORMATION process_info;

	void SetUp() override {
		item_http = new monitor_client::ItemHttp({ L"localhost", 8080 });

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
		delete item_http;
	}
};

TEST_F(HttpTest, RenamedFile) {
	monitor_client::common_utility::ChangeNameInfo info;
	info.old_name = L"새 텍스트 문서.txt";
	info.new_name = L"새 텍스트 문서2.txt";

	bool result = item_http->RenameItem(info);
	ASSERT_TRUE(result);
}

TEST_F(HttpTest, AddFile) {	
	monitor_client::common_utility::FileInfo file_info;
	file_info.name = L"새 텍스트 문서.txt";
	file_info.size = 10;
	file_info.creation_time = L"2021-06-22T22:07:27";
	file_info.last_modified_time = L"2021-06-22T22:07:27";

	bool result = item_http->AddFile(file_info);
	ASSERT_TRUE(result);
}

TEST_F(HttpTest, ModifyFile) {
	monitor_client::common_utility::FileInfo file_info;
	file_info.name = L"새 텍스트 문서.txt";
	file_info.size = 10;
	file_info.creation_time = L"2021-06-22T22:07:27";
	file_info.last_modified_time = L"2021-06-22T22:07:27";

	bool result = item_http->ModifyFile(file_info);
	ASSERT_TRUE(result);
}

TEST_F(HttpTest, AddFolder) {
	monitor_client::common_utility::FolderInfo folder_info;
	folder_info.name = L"new folder";
	folder_info.creation_time = L"2021-06-29 11:30:00";

	bool result = item_http->AddFolder(folder_info);
	ASSERT_TRUE(result);
}