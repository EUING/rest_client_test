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

TEST_F(HttpTest, InsertItem) {	
	monitor_client::common_utility::ItemInfo item_info;
	item_info.name = L"�� �ؽ�Ʈ ����.txt";
	item_info.size = 10;

	bool result = item_http->InsertItem(item_info);
	ASSERT_TRUE(result);
}

TEST_F(HttpTest, ModifyItem) {
	monitor_client::common_utility::ItemInfo item_info;
	item_info.name = L"�� �ؽ�Ʈ ����.txt";
	item_info.size = 10;

	bool result = item_http->InsertItem(item_info);
	ASSERT_TRUE(result);
}

TEST_F(HttpTest, RenamedFile) {
	monitor_client::common_utility::ChangeNameInfo info;
	info.old_name = L"�� �ؽ�Ʈ ����.txt";
	info.new_name = L"�� �ؽ�Ʈ ����2.txt";

	bool result = item_http->RenameItem(info);
	ASSERT_TRUE(result);
}

TEST_F(HttpTest, RemoveItem) {
	bool result = item_http->RemoveItem(L"�� �ؽ�Ʈ ����.txt");
	ASSERT_TRUE(result);
}