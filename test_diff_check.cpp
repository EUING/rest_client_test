#include "pch.h"

#include <unordered_set>
#include <vector>
#include <string>
#include <algorithm>

#include "../monitor_client/diff_check.h"
#include "../monitor_client/item_dao_sqlite.h"
#include "../monitor_client/local_db.h"
#include "../monitor_client/item_http.h"

class DiffTest : public ::testing::Test {
public:
	PROCESS_INFORMATION process_info;

	void SetUp() override {
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
	}

	monitor_client::common_utility::ItemList GetReferece() {
		monitor_client::common_utility::ItemList reference;
		monitor_client::common_utility::ItemInfo info;
		info.name = L"1_1";
		info.size = -1;
		info.hash.clear();
		reference.insert(info);

		info.name = L"1_1/1_1_1";
		info.size = -1;
		info.hash.clear();
		reference.insert(info);

		info.name = L"1_1/1_1_1/1_1_1_1.txt";
		info.size = 0;
		info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
		reference.insert(info);

		info.name = L"1_2";
		info.size = -1;
		info.hash.clear();
		reference.insert(info);

		info.name = L"1_2/1_2_1";
		info.size = -1;
		info.hash.clear();
		reference.insert(info);

		info.name = L"1_2/1_2_1/1_2_1_1.txt";
		info.size = 0;
		info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
		reference.insert(info);

		info.name = L"1_3";
		info.size = -1;
		info.hash.clear();
		reference.insert(info);

		info.name = L"1_3/1_3_1";
		info.size = -1;
		info.hash.clear();
		reference.insert(info);

		info.name = L"1_3/1_3_1/1_3_1_1.txt";
		info.size = 0;
		info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
		reference.insert(info);

		info.name = L"1_4";
		info.size = -1;
		info.hash.clear();
		reference.insert(info);

		info.name = L"1_4/1_4_1";
		info.size = -1;
		info.hash.clear();
		reference.insert(info);

		info.name = L"1_4/1_4_1/1_4_1_1.txt";
		info.size = 0;
		info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
		reference.insert(info);

		return reference;
	}

	void ItemInfoSort(std::vector< monitor_client::common_utility::ItemInfo>& v) {
		auto cmp = [](const monitor_client::common_utility::ItemInfo& lhs, const monitor_client::common_utility::ItemInfo& rhs) {
			return lhs.name < rhs.name;
		};

		std::sort(v.begin(), v.end(), cmp);
	}

	void DiffInfoSort(std::vector< monitor_client::diff_check::DiffInfo>& v) {
		auto cmp = [](const monitor_client::diff_check::DiffInfo& lhs, const monitor_client::diff_check::DiffInfo& rhs) {
			return lhs.os_item.name < rhs.os_item.name;
		};

		std::sort(v.begin(), v.end(), cmp);
	}
};

TEST_F(DiffTest, GetWindowSubFolder) {
	std::wstring target_folder = L"C:\\Users\\ABO\\Desktop\\1";
	ASSERT_TRUE(SetCurrentDirectory(target_folder.c_str()));

	monitor_client::common_utility::ItemList item_list;
	GetSubFolderInfo(L".", item_list);

	monitor_client::common_utility::ItemList reference = GetReferece();

	ASSERT_EQ(item_list.size(), reference.size());

	auto iter1 = item_list.begin();
	auto iter2 = reference.begin();

	for (int i = 0; i < item_list.size(); ++i) {
		ASSERT_EQ(iter1->name, iter2->name);
		ASSERT_EQ(iter1->size, iter2->size);
		ASSERT_EQ(iter1->hash, iter2->hash);

		++iter1;
		++iter2;
	}
}

TEST_F(DiffTest, GetDbSubFolder) {
	auto manager_db = std::make_unique<monitor_client::ItemDaoSqlite>();
	ASSERT_TRUE(manager_db->OpenDatabase(L":memory:"));
	monitor_client::LocalDb local_db(std::move(manager_db));	

	monitor_client::common_utility::ItemInfo info;
	info.name = L"1_1";
	info.size = -1;
	info.hash.clear();

	local_db.InsertItem(info);

	info.name = L"1_2";
	local_db.InsertItem(info);

	info.name = L"1_3";
	local_db.InsertItem(info);

	info.name = L"1_4";
	local_db.InsertItem(info);

	info.name = L"1_1/1_1_1";
	local_db.InsertItem(info);

	info.name = L"1_2/1_2_1";
	local_db.InsertItem(info);

	info.name = L"1_3/1_3_1";
	local_db.InsertItem(info);

	info.name = L"1_4/1_4_1";
	local_db.InsertItem(info);

	info.name = L"1_1/1_1_1/1_1_1_1.txt";
	info.size = 0;
	info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
	local_db.InsertItem(info);

	info.name = L"1_2/1_2_1/1_2_1_1.txt";
	local_db.InsertItem(info);

	info.name = L"1_3/1_3_1/1_3_1_1.txt";
	local_db.InsertItem(info);

	info.name = L"1_4/1_4_1/1_4_1_1.txt";
	local_db.InsertItem(info);

	monitor_client::common_utility::ItemList item_list;
	monitor_client::common_utility::GetSubFolderInfo(local_db, L"", item_list);

	monitor_client::common_utility::ItemList reference = GetReferece();

	ASSERT_EQ(item_list.size(), reference.size());

	auto iter1 = item_list.begin();
	auto iter2 = reference.begin();

	for (int i = 0; i < item_list.size(); ++i) {
		ASSERT_EQ(iter1->name, iter2->name);
		ASSERT_EQ(iter1->size, iter2->size);
		ASSERT_EQ(iter1->hash, iter2->hash);

		++iter1;
		++iter2;
	}
}

TEST_F(DiffTest, GetServerSubFolder) {
	monitor_client::ItemHttp item_http({ L"localhost", 8080 });

	monitor_client::common_utility::ItemList item_list;
	monitor_client::common_utility::GetSubFolderInfo(item_http, L"", item_list);

	monitor_client::common_utility::ItemList reference = GetReferece();

	ASSERT_EQ(item_list.size(), reference.size());

	auto iter1 = item_list.begin();
	auto iter2 = reference.begin();

	for (int i = 0; i < item_list.size(); ++i) {
		ASSERT_EQ(iter1->name, iter2->name);
		ASSERT_EQ(iter1->size, iter2->size);
		ASSERT_EQ(iter1->hash, iter2->hash);

		++iter1;
		++iter2;
	}
}

TEST_F(DiffTest, MakeLocalDiffList) {
	monitor_client::common_utility::ItemList reference = GetReferece();
	monitor_client::common_utility::ItemList db(reference.begin(), reference.end());
	monitor_client::common_utility::ItemList os(reference.begin(), reference.end());

	std::vector<monitor_client::common_utility::ItemInfo> create_list;
	std::vector<monitor_client::common_utility::ItemInfo> equal_list(reference.begin(), reference.end());
	std::vector<monitor_client::diff_check::DiffInfo> modify_list;

	int index = -1;
	for (int i = 0; i < equal_list.size(); ++i) {
		if (equal_list[i].name == L"1_1/1_1_1/1_1_1_1.txt") {
			index = i;
			break;
		}
	}

	equal_list.erase(equal_list.begin() + index);

	monitor_client::common_utility::ItemInfo info;
	info.name = L"1_5.txt";
	info.size = 0;
	info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
	os.insert(info);
	create_list.push_back(info);

	info.name = L"1_1/1_1_1/1_1_1_1.txt";
	info.size = 0;
	info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";	

	monitor_client::common_utility::ItemInfo modify_info;
	modify_info.name = L"1_1/1_1_1/1_1_1_1.txt";
	modify_info.size = 1;
	modify_info.hash = L"modify";
	modify_list.push_back({ info.hash, modify_info});

	auto change_value = os.extract({ L"1_1/1_1_1/1_1_1_1.txt" });
	ASSERT_FALSE(change_value.empty());

	change_value.value().size = 1;
	change_value.value().hash = L"modify";
	os.insert(std::move(change_value));

	monitor_client::diff_check::LocalDiffList list = monitor_client::diff_check::MakeLocalDiffList(os, db);
	std::vector<monitor_client::common_utility::ItemInfo> result_create_list = list.create_list;
	std::vector<monitor_client::common_utility::ItemInfo> result_equal_list = list.equal_list;
	std::vector<monitor_client::diff_check::DiffInfo> result_modify_list = list.modify_list;

	ASSERT_EQ(create_list.size(), result_create_list.size());
	ASSERT_EQ(equal_list.size(), result_equal_list.size());
	ASSERT_EQ(modify_list.size(), result_modify_list.size());

	ItemInfoSort(create_list);
	ItemInfoSort(equal_list);
	DiffInfoSort(modify_list);

	ItemInfoSort(result_create_list);
	ItemInfoSort(result_equal_list);
	DiffInfoSort(result_modify_list);

	for (int i = 0; i < create_list.size(); ++i) {
		ASSERT_EQ(create_list[i].name, result_create_list[i].name);
		ASSERT_EQ(create_list[i].size, result_create_list[i].size);
		ASSERT_EQ(create_list[i].hash, result_create_list[i].hash);
	}

	for (int i = 0; i < equal_list.size(); ++i) {
		ASSERT_EQ(equal_list[i].name, result_equal_list[i].name);
		ASSERT_EQ(equal_list[i].size, result_equal_list[i].size);
		ASSERT_EQ(equal_list[i].hash, result_equal_list[i].hash);
	}

	for (int i = 0; i < modify_list.size(); ++i) {
		ASSERT_EQ(modify_list[i].prev_hash, result_modify_list[i].prev_hash);
		ASSERT_EQ(modify_list[i].os_item.name, result_modify_list[i].os_item.name);
		ASSERT_EQ(modify_list[i].os_item.size, result_modify_list[i].os_item.size);
		ASSERT_EQ(modify_list[i].os_item.hash, result_modify_list[i].os_item.hash);
	}
}

TEST_F(DiffTest, NoLocalChange) {
	monitor_client::common_utility::ItemList reference = GetReferece();

	monitor_client::diff_check::LocalDiffList local_diff_list =  monitor_client::diff_check::MakeLocalDiffList(reference, reference);
	ASSERT_TRUE(local_diff_list.create_list.empty());
	ASSERT_TRUE(local_diff_list.modify_list.empty());
	ASSERT_EQ(reference.size(), local_diff_list.equal_list.size());

	std::vector<monitor_client::common_utility::ItemInfo> v(reference.begin(), reference.end());
	ItemInfoSort(v);
	ItemInfoSort(local_diff_list.equal_list);

	for (int i = 0; i < local_diff_list.equal_list.size(); ++i) {
		ASSERT_EQ(local_diff_list.equal_list[i].name, v[i].name);
		ASSERT_EQ(local_diff_list.equal_list[i].size, v[i].size);
		ASSERT_EQ(local_diff_list.equal_list[i].hash, v[i].hash);
	}
	
	monitor_client::diff_check::ServerDiffList server_diff_list = monitor_client::diff_check::MakeServerDiffList(reference, local_diff_list);
	ASSERT_TRUE(server_diff_list.upload_request_list.empty());
	ASSERT_TRUE(server_diff_list.download_request_list.empty());
	ASSERT_TRUE(server_diff_list.conflict_list.empty());
	ASSERT_TRUE(server_diff_list.delete_list.empty());
}

TEST_F(DiffTest, LocalChangeNoConflict) {
	monitor_client::common_utility::ItemList server = GetReferece();
	monitor_client::common_utility::ItemList os = server;
	monitor_client::common_utility::ItemList db = server;

	std::wstring server_delete_file1 = L"1_3/1_3_1/1_3_1_1.txt";
	server.erase({ server_delete_file1 });
	
	std::wstring local_delete_file1 = L"1_1/1_1_1/1_1_1_1.txt";
	os.erase({ local_delete_file1 });

	monitor_client::common_utility::ItemInfo new_file1;
	new_file1.name = L"1_4/1_4_1/1_4_1_2.txt";
	new_file1.size = 100;
	new_file1.hash = L"new file";
	os.insert(new_file1);

	monitor_client::common_utility::ItemInfo new_file2;
	new_file2.name = L"1_5.txt";
	new_file2.size = 0;
	new_file2.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
	os.insert(new_file2);

	monitor_client::common_utility::ItemInfo modify_file1;
	modify_file1.name = L"1_2/1_2_1/1_2_1_1.txt";
	modify_file1.size = 500;
	modify_file1.hash = L"modify";

	monitor_client::common_utility::ItemInfo modify_file2;
	modify_file2.name = L"1_4/1_4_1/1_4_1_1.txt";
	modify_file2.size = 10;
	modify_file2.hash = L"modify";

	auto change_value = os.extract(modify_file1);
	ASSERT_FALSE(change_value.empty());

	change_value.value().size = modify_file1.size;
	change_value.value().hash = modify_file1.hash;
	os.insert(std::move(change_value));

	change_value = os.extract(modify_file2);
	ASSERT_FALSE(change_value.empty());

	change_value.value().size = modify_file2.size;
	change_value.value().hash = modify_file2.hash;
	os.insert(std::move(change_value));	

	monitor_client::diff_check::LocalDiffList local_diff_list = monitor_client::diff_check::MakeLocalDiffList(os, db);

	auto create_list = local_diff_list.create_list;
	auto equal_list = local_diff_list.equal_list;
	auto modify_list = local_diff_list.modify_list;

	ItemInfoSort(create_list);
	ItemInfoSort(equal_list);
	DiffInfoSort(modify_list);

	ASSERT_EQ(create_list.size(), 2);

	ASSERT_EQ(create_list[0].name, new_file1.name);
	ASSERT_EQ(create_list[0].size, new_file1.size);
	ASSERT_EQ(create_list[0].hash, new_file1.hash);

	ASSERT_EQ(create_list[1].name, new_file2.name);
	ASSERT_EQ(create_list[1].size, new_file2.size);
	ASSERT_EQ(create_list[1].hash, new_file2.hash);

	ASSERT_EQ(modify_list.size(), 2);

	ASSERT_EQ(modify_list[0].prev_hash, L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
	ASSERT_EQ(modify_list[0].os_item.name, modify_file1.name);
	ASSERT_EQ(modify_list[0].os_item.size, modify_file1.size);
	ASSERT_EQ(modify_list[0].os_item.hash, modify_file1.hash);
	
	ASSERT_EQ(modify_list[1].prev_hash, L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
	ASSERT_EQ(modify_list[1].os_item.name, modify_file2.name);
	ASSERT_EQ(modify_list[1].os_item.size, modify_file2.size);
	ASSERT_EQ(modify_list[1].os_item.hash, modify_file2.hash);

	ASSERT_EQ(equal_list.size(), 9);

	monitor_client::diff_check::ServerDiffList server_diff_list = monitor_client::diff_check::MakeServerDiffList(server, local_diff_list);
	ASSERT_TRUE(server_diff_list.conflict_list.empty());

	ASSERT_EQ(server_diff_list.upload_request_list.size(), 4);

	ASSERT_EQ(server_diff_list.upload_request_list[0].name, modify_file1.name);
	ASSERT_EQ(server_diff_list.upload_request_list[0].size, modify_file1.size);
	ASSERT_EQ(server_diff_list.upload_request_list[0].hash, modify_file1.hash);

	ASSERT_EQ(server_diff_list.upload_request_list[1].name, modify_file2.name);
	ASSERT_EQ(server_diff_list.upload_request_list[1].size, modify_file2.size);
	ASSERT_EQ(server_diff_list.upload_request_list[1].hash, modify_file2.hash);

	ASSERT_EQ(server_diff_list.upload_request_list[2].name, new_file1.name);
	ASSERT_EQ(server_diff_list.upload_request_list[2].size, new_file1.size);
	ASSERT_EQ(server_diff_list.upload_request_list[2].hash, new_file1.hash);

	ASSERT_EQ(server_diff_list.upload_request_list[3].name, new_file2.name);
	ASSERT_EQ(server_diff_list.upload_request_list[3].size, new_file2.size);
	ASSERT_EQ(server_diff_list.upload_request_list[3].hash, new_file2.hash);

	ASSERT_EQ(server_diff_list.download_request_list.size(), 1);
	ASSERT_EQ(server_diff_list.download_request_list[0], local_delete_file1);

	ASSERT_EQ(server_diff_list.delete_list.size(), 1);
	ASSERT_EQ(server_diff_list.delete_list[0], server_delete_file1);
}
TEST_F(DiffTest, LocalChangeConflict) {
	monitor_client::common_utility::ItemList server = GetReferece();
	monitor_client::common_utility::ItemList os = server;
	monitor_client::common_utility::ItemList db = server;

	std::wstring local_delete_file1 = L"1_1/1_1_1/1_1_1_1.txt";
	os.erase({ local_delete_file1 });

	monitor_client::common_utility::ItemInfo new_file1;
	new_file1.name = L"1_4/1_4_1/1_4_1_2.txt";
	new_file1.size = 100;
	new_file1.hash = L"new file";
	os.insert(new_file1);

	monitor_client::common_utility::ItemInfo new_file2;
	new_file2.name = L"1_5.txt";
	new_file2.size = 0;
	new_file2.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
	os.insert(new_file2);

	monitor_client::common_utility::ItemInfo new_same;
	new_same.name = L"1_1/1_1_1/1_1_1_2.txt";
	new_same.size = 0;
	new_same.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
	os.insert(new_same);
	server.insert(new_same);

	monitor_client::common_utility::ItemInfo new_local_conflict1;
	new_local_conflict1.name = L"1_6.txt";
	new_local_conflict1.size = 10;
	new_local_conflict1.hash = L"local conflict";
	os.insert(new_local_conflict1);

	monitor_client::common_utility::ItemInfo new_server_conflict1 = new_local_conflict1;
	new_server_conflict1.size = 100;
	new_server_conflict1.hash = L"server conflict";
	server.insert(new_server_conflict1);

	monitor_client::common_utility::ItemInfo modify_file1;
	modify_file1.name = L"1_2/1_2_1/1_2_1_1.txt";
	modify_file1.size = 500;
	modify_file1.hash = L"modify";

	monitor_client::common_utility::ItemInfo modify_file2;
	modify_file2.name = L"1_4/1_4_1/1_4_1_1.txt";
	modify_file2.size = 10;
	modify_file2.hash = L"modify";

	monitor_client::common_utility::ItemInfo modify_local_conflict1;
	modify_local_conflict1.name = L"1_3/1_3_1/1_3_1_1.txt";
	modify_local_conflict1.size = 10;
	modify_local_conflict1.hash = L"local conflict";

	monitor_client::common_utility::ItemInfo modify_server_conflict1 = modify_local_conflict1;
	modify_server_conflict1.name = L"1_3/1_3_1/1_3_1_1.txt";
	modify_server_conflict1.size = 100;
	modify_server_conflict1.hash = L"server conflict";

	auto change_value = os.extract(modify_file1);
	ASSERT_FALSE(change_value.empty());

	change_value.value().size = modify_file1.size;
	change_value.value().hash = modify_file1.hash;
	os.insert(std::move(change_value));

	change_value = os.extract(modify_file2);
	ASSERT_FALSE(change_value.empty());

	change_value.value().size = modify_file2.size;
	change_value.value().hash = modify_file2.hash;
	os.insert(std::move(change_value));

	change_value = os.extract(modify_local_conflict1);
	ASSERT_FALSE(change_value.empty());

	change_value.value().size = modify_local_conflict1.size;
	change_value.value().hash = modify_local_conflict1.hash;
	os.insert(std::move(change_value));

	change_value = server.extract(modify_server_conflict1);
	ASSERT_FALSE(change_value.empty());

	change_value.value().size = modify_server_conflict1.size;
	change_value.value().hash = modify_server_conflict1.hash;
	server.insert(std::move(change_value));

	monitor_client::diff_check::LocalDiffList local_diff_list = monitor_client::diff_check::MakeLocalDiffList(os, db);

	auto create_list = local_diff_list.create_list;
	auto equal_list = local_diff_list.equal_list;
	auto modify_list = local_diff_list.modify_list;

	ItemInfoSort(create_list);
	ItemInfoSort(equal_list);
	DiffInfoSort(modify_list);

	ASSERT_EQ(create_list.size(), 4);

	ASSERT_EQ(create_list[0].name, new_same.name);
	ASSERT_EQ(create_list[0].size, new_same.size);
	ASSERT_EQ(create_list[0].hash, new_same.hash);

	ASSERT_EQ(create_list[1].name, new_file1.name);
	ASSERT_EQ(create_list[1].size, new_file1.size);
	ASSERT_EQ(create_list[1].hash, new_file1.hash);

	ASSERT_EQ(create_list[2].name, new_file2.name);
	ASSERT_EQ(create_list[2].size, new_file2.size);
	ASSERT_EQ(create_list[2].hash, new_file2.hash);

	ASSERT_EQ(create_list[3].name, new_local_conflict1.name);
	ASSERT_EQ(create_list[3].size, new_local_conflict1.size);
	ASSERT_EQ(create_list[3].hash, new_local_conflict1.hash);

	ASSERT_EQ(modify_list.size(), 3);

	ASSERT_EQ(modify_list[0].prev_hash, L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
	ASSERT_EQ(modify_list[0].os_item.name, modify_file1.name);
	ASSERT_EQ(modify_list[0].os_item.size, modify_file1.size);
	ASSERT_EQ(modify_list[0].os_item.hash, modify_file1.hash);

	ASSERT_EQ(modify_list[1].prev_hash, L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
	ASSERT_EQ(modify_list[1].os_item.name, modify_local_conflict1.name);
	ASSERT_EQ(modify_list[1].os_item.size, modify_local_conflict1.size);
	ASSERT_EQ(modify_list[1].os_item.hash, modify_local_conflict1.hash);

	ASSERT_EQ(modify_list[2].prev_hash, L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
	ASSERT_EQ(modify_list[2].os_item.name, modify_file2.name);
	ASSERT_EQ(modify_list[2].os_item.size, modify_file2.size);
	ASSERT_EQ(modify_list[2].os_item.hash, modify_file2.hash);

	ASSERT_EQ(equal_list.size(), 8);

	monitor_client::diff_check::ServerDiffList server_diff_list = monitor_client::diff_check::MakeServerDiffList(server, local_diff_list);
	ASSERT_EQ(server_diff_list.upload_request_list.size(), 4);

	ASSERT_EQ(server_diff_list.upload_request_list[0].name, modify_file1.name);
	ASSERT_EQ(server_diff_list.upload_request_list[0].size, modify_file1.size);
	ASSERT_EQ(server_diff_list.upload_request_list[0].hash, modify_file1.hash);

	ASSERT_EQ(server_diff_list.upload_request_list[1].name, modify_file2.name);
	ASSERT_EQ(server_diff_list.upload_request_list[1].size, modify_file2.size);
	ASSERT_EQ(server_diff_list.upload_request_list[1].hash, modify_file2.hash);

	ASSERT_EQ(server_diff_list.upload_request_list[2].name, new_file1.name);
	ASSERT_EQ(server_diff_list.upload_request_list[2].size, new_file1.size);
	ASSERT_EQ(server_diff_list.upload_request_list[2].hash, new_file1.hash);

	ASSERT_EQ(server_diff_list.upload_request_list[3].name, new_file2.name);
	ASSERT_EQ(server_diff_list.upload_request_list[3].size, new_file2.size);
	ASSERT_EQ(server_diff_list.upload_request_list[3].hash, new_file2.hash);

	ASSERT_EQ(server_diff_list.download_request_list.size(), 1);
	ASSERT_EQ(server_diff_list.download_request_list[0], local_delete_file1);

	ASSERT_EQ(server_diff_list.conflict_list.size(), 2);

	std::sort(server_diff_list.conflict_list.begin(), server_diff_list.conflict_list.end());
	ASSERT_EQ(server_diff_list.conflict_list[0], modify_server_conflict1.name);
	ASSERT_EQ(server_diff_list.conflict_list[1], new_server_conflict1.name);

	ASSERT_TRUE(server_diff_list.delete_list.empty());
}

TEST_F(DiffTest, SortTest) {
	monitor_client::common_utility::ItemList server = GetReferece();
	monitor_client::common_utility::ItemList os = server;
	monitor_client::common_utility::ItemList db = server;

	std::wstring local_delete_file1 = L"1_1/1_1_1/1_1_1_1.txt";
	os.erase({ local_delete_file1 });

	std::wstring local_delete_file2 = L"1_1/1_1_1";
	os.erase({ local_delete_file2 });

	std::wstring local_delete_file3 = L"1_1";
	os.erase({ local_delete_file3 });

	monitor_client::common_utility::ItemInfo new_file1;
	new_file1.name = L"1_5";
	new_file1.size = -1;
	new_file1.hash.clear();
	os.insert(new_file1);

	monitor_client::common_utility::ItemInfo new_file2;
	new_file2.name = L"1_5/1_5_1";
	new_file2.size = -1;
	new_file2.hash.clear();
	os.insert(new_file2);

	monitor_client::common_utility::ItemInfo new_file3;
	new_file3.name = L"1_5/1_5_1/1_5_1_1.txt";
	new_file3.size = 0;
	new_file3.hash = L"new file";
	os.insert(new_file3);

	monitor_client::diff_check::LocalDiffList local_diff_list = monitor_client::diff_check::MakeLocalDiffList(os, db);

	auto create_list = local_diff_list.create_list;
	auto equal_list = local_diff_list.equal_list;
	auto modify_list = local_diff_list.modify_list;

	ItemInfoSort(create_list);
	ItemInfoSort(equal_list);
	DiffInfoSort(modify_list);

	ASSERT_EQ(create_list.size(), 3);

	ASSERT_EQ(create_list[0].name, new_file1.name);
	ASSERT_EQ(create_list[0].size, new_file1.size);
	ASSERT_EQ(create_list[0].hash, new_file1.hash);

	ASSERT_EQ(create_list[1].name, new_file2.name);
	ASSERT_EQ(create_list[1].size, new_file2.size);
	ASSERT_EQ(create_list[1].hash, new_file2.hash);

	ASSERT_EQ(create_list[2].name, new_file3.name);
	ASSERT_EQ(create_list[2].size, new_file3.size);
	ASSERT_EQ(create_list[2].hash, new_file3.hash);

	ASSERT_TRUE(modify_list.empty());

	ASSERT_EQ(equal_list.size(), 9);

	monitor_client::diff_check::ServerDiffList server_diff_list = monitor_client::diff_check::MakeServerDiffList(server, local_diff_list);
	ASSERT_TRUE(server_diff_list.conflict_list.empty());

	ASSERT_EQ(server_diff_list.upload_request_list.size(), 3);

	ASSERT_EQ(server_diff_list.upload_request_list[0].name, new_file1.name);
	ASSERT_EQ(server_diff_list.upload_request_list[0].size, new_file1.size);
	ASSERT_EQ(server_diff_list.upload_request_list[0].hash, new_file1.hash);

	ASSERT_EQ(server_diff_list.upload_request_list[1].name, new_file2.name);
	ASSERT_EQ(server_diff_list.upload_request_list[1].size, new_file2.size);
	ASSERT_EQ(server_diff_list.upload_request_list[1].hash, new_file2.hash);

	ASSERT_EQ(server_diff_list.upload_request_list[2].name, new_file3.name);
	ASSERT_EQ(server_diff_list.upload_request_list[2].size, new_file3.size);
	ASSERT_EQ(server_diff_list.upload_request_list[2].hash, new_file3.hash);

	ASSERT_EQ(server_diff_list.download_request_list.size(), 3);
	ASSERT_EQ(server_diff_list.download_request_list[0], local_delete_file3);
	ASSERT_EQ(server_diff_list.download_request_list[1], local_delete_file2);
	ASSERT_EQ(server_diff_list.download_request_list[2], local_delete_file1);

	ASSERT_TRUE(server_diff_list.delete_list.empty());
}