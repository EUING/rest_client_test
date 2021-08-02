#include "pch.h"

#include <Windows.h>

#include <vector>
#include <string>
#include <algorithm>

#include "../monitor_client/diff_check.h"
#include "../monitor_client/item_dao_sqlite.h"
#include "../monitor_client/local_db.h"
#include "../monitor_client/item_http.h"

class InitialDiffCheckTest : public ::testing::Test {
public:
	PROCESS_INFORMATION process_info;

	void SetUp() override {
		std::wstring file_path = L"C:\\Users\\ABO\\Desktop\\1";
		_wmkdir(file_path.c_str());

		file_path = L"C:\\Users\\ABO\\Desktop\\1\\1_1";
		_wmkdir(file_path.c_str());

		file_path = L"C:\\Users\\ABO\\Desktop\\1\\1_2";
		_wmkdir(file_path.c_str());

		file_path = L"C:\\Users\\ABO\\Desktop\\1\\1_3";
		_wmkdir(file_path.c_str());

		file_path = L"C:\\Users\\ABO\\Desktop\\1\\1_4";
		_wmkdir(file_path.c_str());

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
	}
};

TEST_F(InitialDiffCheckTest, NoConflict) {
	std::wstring target_folder = L"C:\\Users\\ABO\\Desktop\\1";
	ASSERT_TRUE(SetCurrentDirectory(target_folder.c_str()));
	
	std::unique_ptr<monitor_client::ItemDaoSqlite> item_dao = std::make_unique<monitor_client::ItemDaoSqlite>();
	item_dao->OpenDatabase(L":memory:");

	monitor_client::common_utility::ItemInfo item_info;
	item_info.name = L"1_1";
	item_info.size = -1;
	item_info.hash.clear();

	item_dao->UpdateItemInfo(item_info, 0);

	item_info.name = L"1_2";
	item_info.size = -1;
	item_info.hash.clear();

	item_dao->UpdateItemInfo(item_info, 0);

	item_info.name = L"1_3";
	item_info.size = -1;
	item_info.hash.clear();

	item_dao->UpdateItemInfo(item_info, 0);

	item_info.name = L"1_4";
	item_info.size = -1;
	item_info.hash.clear();

	item_dao->UpdateItemInfo(item_info, 0);

	monitor_client::common_utility::NetworkInfo network_info;
	network_info.host = L"localhost";
	network_info.port = 8080;

	std::shared_ptr<monitor_client::LocalDb> local_db = std::make_shared<monitor_client::LocalDb>(std::move(item_dao));
	std::shared_ptr<monitor_client::ItemHttp> item_http = std::make_shared<monitor_client::ItemHttp>(network_info);

	monitor_client::diff_check::ServerDiffList server_diff_list = monitor_client::diff_check::InitialDiffCheck(local_db, item_http);

	ASSERT_TRUE(server_diff_list.conflict_list.empty());
	ASSERT_TRUE(server_diff_list.upload_request_list.empty());
	ASSERT_TRUE(server_diff_list.remove_list.empty());

	std::vector<std::wstring> v;
	v.push_back(L"1_1/1_1_1");
	v.push_back(L"1_1/1_1_1/1_1_1_1.txt");
	v.push_back(L"1_2/1_2_1");
	v.push_back(L"1_2/1_2_1/1_2_1_1.txt");
	v.push_back(L"1_3/1_3_1");
	v.push_back(L"1_3/1_3_1/1_3_1_1.txt");
	v.push_back(L"1_4/1_4_1");
	v.push_back(L"1_4/1_4_1/1_4_1_1.txt");

	ASSERT_EQ(server_diff_list.download_request_list.size(), v.size());

	for (int i = 0; i < server_diff_list.download_request_list.size(); ++i) {
		ASSERT_EQ(v[i], server_diff_list.download_request_list[i]);
	}
}

TEST_F(InitialDiffCheckTest, UploadAndDownload) {
	std::wstring target_folder = L"C:\\Users\\ABO\\Desktop\\1";
	ASSERT_TRUE(SetCurrentDirectory(target_folder.c_str()));

	_wmkdir(L"C:\\Users\\ABO\\Desktop\\1\\1_5");

	std::wstring file_name = L"C:\\Users\\ABO\\Desktop\\1\\1_5\\1_5_1.txt";
	std::ofstream temp{ file_name };
	temp.close();

	std::unique_ptr<monitor_client::ItemDaoSqlite> item_dao = std::make_unique<monitor_client::ItemDaoSqlite>();
	item_dao->OpenDatabase(L":memory:");

	monitor_client::common_utility::ItemInfo item_info;
	item_info.name = L"1_1";
	item_info.size = -1;
	item_info.hash.clear();

	item_dao->UpdateItemInfo(item_info, 0);

	item_info.name = L"1_2";
	item_info.size = -1;
	item_info.hash.clear();

	item_dao->UpdateItemInfo(item_info, 0);

	item_info.name = L"1_3";
	item_info.size = -1;
	item_info.hash.clear();

	item_dao->UpdateItemInfo(item_info, 0);

	item_info.name = L"1_4";
	item_info.size = -1;
	item_info.hash.clear();

	item_dao->UpdateItemInfo(item_info, 0);

	monitor_client::common_utility::NetworkInfo network_info;
	network_info.host = L"localhost";
	network_info.port = 8080;

	std::shared_ptr<monitor_client::LocalDb> local_db = std::make_shared<monitor_client::LocalDb>(std::move(item_dao));
	std::shared_ptr<monitor_client::ItemHttp> item_http = std::make_shared<monitor_client::ItemHttp>(network_info);

	monitor_client::diff_check::ServerDiffList server_diff_list = monitor_client::diff_check::InitialDiffCheck(local_db, item_http);

	ASSERT_TRUE(server_diff_list.conflict_list.empty());
	ASSERT_TRUE(server_diff_list.remove_list.empty());

	std::vector<std::wstring> v;
	v.push_back(L"1_1/1_1_1");
	v.push_back(L"1_1/1_1_1/1_1_1_1.txt");
	v.push_back(L"1_2/1_2_1");
	v.push_back(L"1_2/1_2_1/1_2_1_1.txt");
	v.push_back(L"1_3/1_3_1");
	v.push_back(L"1_3/1_3_1/1_3_1_1.txt");
	v.push_back(L"1_4/1_4_1");
	v.push_back(L"1_4/1_4_1/1_4_1_1.txt");

	ASSERT_EQ(server_diff_list.download_request_list.size(), v.size());

	for (int i = 0; i < server_diff_list.download_request_list.size(); ++i) {
		ASSERT_EQ(v[i], server_diff_list.download_request_list[i]);
	}

	ASSERT_EQ(server_diff_list.upload_request_list.size(), 2);
	ASSERT_EQ(server_diff_list.upload_request_list[0].name, L"1_5");
	ASSERT_EQ(server_diff_list.upload_request_list[0].size, -1);
	ASSERT_TRUE(server_diff_list.upload_request_list[0].hash.empty());

	ASSERT_EQ(server_diff_list.upload_request_list[1].name, L"1_5/1_5_1.txt");
	ASSERT_EQ(server_diff_list.upload_request_list[1].size, 0);
	ASSERT_EQ(server_diff_list.upload_request_list[1].hash, L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

TEST_F(InitialDiffCheckTest, Conflict) {
	std::wstring target_folder = L"C:\\Users\\ABO\\Desktop\\1";
	ASSERT_TRUE(SetCurrentDirectory(target_folder.c_str()));

	_wmkdir(L"C:\\Users\\ABO\\Desktop\\1\\1_1\\1_1_1");

	std::wstring file_name = L"C:\\Users\\ABO\\Desktop\\1\\1_1\\1_1_1\\1_1_1_1.txt";
	std::ofstream temp{ file_name };
	temp << L"test";
	temp.close();

	std::unique_ptr<monitor_client::ItemDaoSqlite> item_dao = std::make_unique<monitor_client::ItemDaoSqlite>();
	item_dao->OpenDatabase(L":memory:");

	monitor_client::common_utility::ItemInfo item_info;
	item_info.name = L"1_1";
	item_info.size = -1;
	item_info.hash.clear();

	item_dao->UpdateItemInfo(item_info, 0);

	item_info.name = L"1_1/1_1_1";
	item_info.size = -1;
	item_info.hash.clear();

	item_dao->UpdateItemInfo(item_info, 0);

	item_info.name = L"1_2";
	item_info.size = -1;
	item_info.hash.clear();

	item_dao->UpdateItemInfo(item_info, 0);

	item_info.name = L"1_3";
	item_info.size = -1;
	item_info.hash.clear();

	item_dao->UpdateItemInfo(item_info, 0);

	item_info.name = L"1_4";
	item_info.size = -1;
	item_info.hash.clear();

	item_dao->UpdateItemInfo(item_info, 0);

	monitor_client::common_utility::NetworkInfo network_info;
	network_info.host = L"localhost";
	network_info.port = 8080;

	std::shared_ptr<monitor_client::LocalDb> local_db = std::make_shared<monitor_client::LocalDb>(std::move(item_dao));
	std::shared_ptr<monitor_client::ItemHttp> item_http = std::make_shared<monitor_client::ItemHttp>(network_info);

	monitor_client::diff_check::ServerDiffList server_diff_list = monitor_client::diff_check::InitialDiffCheck(local_db, item_http);

	ASSERT_TRUE(server_diff_list.upload_request_list.empty());
	ASSERT_TRUE(server_diff_list.remove_list.empty());

	std::vector<std::wstring> v;
	v.push_back(L"1_2/1_2_1");
	v.push_back(L"1_2/1_2_1/1_2_1_1.txt");
	v.push_back(L"1_3/1_3_1");
	v.push_back(L"1_3/1_3_1/1_3_1_1.txt");
	v.push_back(L"1_4/1_4_1");
	v.push_back(L"1_4/1_4_1/1_4_1_1.txt");

	ASSERT_EQ(server_diff_list.download_request_list.size(), v.size());

	for (int i = 0; i < server_diff_list.download_request_list.size(); ++i) {
		ASSERT_EQ(v[i], server_diff_list.download_request_list[i]);
	}

	ASSERT_EQ(server_diff_list.conflict_list.size(), 1);
	ASSERT_EQ(server_diff_list.conflict_list[0], L"1_1/1_1_1/1_1_1_1.txt");
}