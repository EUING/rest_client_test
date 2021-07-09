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

TEST_F(HttpTest, GetItemInfo) {
	std::optional<monitor_client::common_utility::ItemInfo> result = item_http->GetItemInfo(L"test");
	ASSERT_TRUE(result.has_value());

	auto item_info = result.value();
	ASSERT_EQ(item_info.name, L"test");
	ASSERT_EQ(item_info.size, 0);
	ASSERT_EQ(item_info.hash, L"hash");
}

TEST_F(HttpTest, GetFolderContainList) {
	std::optional<std::vector<monitor_client::common_utility::ItemInfo>> result = item_http->GetFolderContainList();
	ASSERT_TRUE(result.has_value());

	auto v = result.value();

	ASSERT_EQ(v[0].name, L"1_1");
	ASSERT_EQ(v[0].size, -1);
	ASSERT_TRUE(v[0].hash.empty());

	ASSERT_EQ(v[1].name, L"1_2");
	ASSERT_EQ(v[1].size, -1);
	ASSERT_TRUE(v[1].hash.empty());

	ASSERT_EQ(v[2].name, L"1_3");
	ASSERT_EQ(v[2].size, -1);
	ASSERT_TRUE(v[2].hash.empty());

	ASSERT_EQ(v[3].name, L"1_4");
	ASSERT_EQ(v[3].size, -1);
	ASSERT_TRUE(v[3].hash.empty());
}

TEST_F(HttpTest, UpdateItem) {	
	monitor_client::common_utility::ItemInfo item_info;
	item_info.name = L"새 텍스트 문서.txt";
	item_info.size = 10;

	bool result = item_http->UpdateItem(item_info);
	ASSERT_TRUE(result);
}

TEST_F(HttpTest, ModifyItem) {
	monitor_client::common_utility::ItemInfo item_info;
	item_info.name = L"새 텍스트 문서.txt";
	item_info.size = 10;

	bool result = item_http->UpdateItem(item_info);
	ASSERT_TRUE(result);
}

TEST_F(HttpTest, RenamedFile) {
	monitor_client::common_utility::ChangeNameInfo info;
	info.old_name = L"새 텍스트 문서.txt";
	info.new_name = L"새 텍스트 문서2.txt";

	bool result = item_http->RenameItem(info);
	ASSERT_TRUE(result);
}

TEST_F(HttpTest, RemoveItem) {
	bool result = item_http->RemoveItem(L"새 텍스트 문서.txt");
	ASSERT_TRUE(result);
}