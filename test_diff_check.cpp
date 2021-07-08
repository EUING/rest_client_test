#include "pch.h"

#include <unordered_set>
#include <vector>
#include <string>
#include <map>
#include <set>

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

	std::set<monitor_client::common_utility::ItemInfo> create_list;
	monitor_client::common_utility::ItemList equal_list = reference;
	
	std::unordered_set<monitor_client::diff_check::DiffInfo> modify_list;

	equal_list.erase({ L"1_1/1_1_1/1_1_1_1.txt" });

	monitor_client::common_utility::ItemInfo info;
	info.name = L"1_5.txt";
	info.size = 0;
	info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
	os.insert(info);
	create_list.insert(info);

	info.name = L"1_1/1_1_1/1_1_1_1.txt";
	info.size = 0;
	info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";	

	monitor_client::common_utility::ItemInfo modify_info;
	modify_info.name = L"1_1/1_1_1/1_1_1_1.txt";
	modify_info.size = 1;
	modify_info.hash = L"modify";
	modify_list.insert({ info, modify_info});

	auto change_value = os.extract({ L"1_1/1_1_1/1_1_1_1.txt" });
	ASSERT_FALSE(change_value.empty());

	change_value.value().size = 1;
	change_value.value().hash = L"modify";
	os.insert(std::move(change_value));

	monitor_client::diff_check::LocalDiffList list = monitor_client::diff_check::MakeLocalDiffList(os, db);
	std::set<monitor_client::common_utility::ItemInfo> result_create_list = list.create_list;
	monitor_client::common_utility::ItemList result_equal_list = list.equal_list;
	std::unordered_set<monitor_client::diff_check::DiffInfo> result_modify_list = list.modify_list;

	ASSERT_EQ(create_list.size(), result_create_list.size());
	auto iter1 = create_list.begin();
	auto iter2 = result_create_list.begin();

	for (int i = 0; i < create_list.size(); ++i) {
		ASSERT_EQ(iter1->name, iter2->name);
		ASSERT_EQ(iter1->size, iter2->size);
		ASSERT_EQ(iter1->hash, iter2->hash);

		++iter1;
		++iter2;
	}

	ASSERT_EQ(equal_list.size(), result_equal_list.size());
	auto iter3 = equal_list.begin();
	auto iter4 = result_equal_list.begin();

	for (int i = 0; i < equal_list.size(); ++i) {
		ASSERT_EQ(iter3->name, iter4->name);
		ASSERT_EQ(iter3->size, iter4->size);
		ASSERT_EQ(iter3->hash, iter4->hash);

		++iter3;
		++iter4;
	}

	ASSERT_EQ(modify_list.size(), result_modify_list.size());
	auto iter5 = modify_list.begin();
	auto iter6 = result_modify_list.begin();

	for (int i = 0; i < modify_list.size(); ++i) {
		ASSERT_EQ(iter5->other_item.name, iter6->other_item.name);
		ASSERT_EQ(iter5->other_item.size, iter6->other_item.size);
		ASSERT_EQ(iter5->other_item.hash, iter6->other_item.hash);

		ASSERT_EQ(iter5->os_item.name, iter6->os_item.name);
		ASSERT_EQ(iter5->os_item.size, iter6->os_item.size);
		ASSERT_EQ(iter5->os_item.hash, iter6->os_item.hash);

		++iter5;
		++iter6;
	}
}

TEST_F(DiffTest, NoLocalChange) {
	monitor_client::common_utility::ItemList reference = GetReferece();

	monitor_client::diff_check::LocalDiffList local_diff_list =  monitor_client::diff_check::MakeLocalDiffList(reference, reference);
	ASSERT_TRUE(local_diff_list.create_list.empty());
	ASSERT_TRUE(local_diff_list.modify_list.empty());
	ASSERT_EQ(reference.size(), local_diff_list.equal_list.size());

	std::set<monitor_client::common_utility::ItemInfo> m1(reference.begin(), reference.end());
	std::set<monitor_client::common_utility::ItemInfo> m2(local_diff_list.equal_list.begin(), local_diff_list.equal_list.end());

	auto iter1 = m1.begin();
	auto iter2 = m2.begin();

	for (int i = 0; i < m1.size(); ++i) {
		ASSERT_EQ(iter1->name, iter2->name);
		ASSERT_EQ(iter1->size, iter2->size);
		ASSERT_EQ(iter1->hash, iter2->hash);

		++iter1;
		++iter2;
	}
	
	monitor_client::diff_check::ServerDiffList server_diff_list = monitor_client::diff_check::MakeServerDiffList(reference, local_diff_list);
	ASSERT_TRUE(server_diff_list.upload_request_list.empty());
	ASSERT_TRUE(server_diff_list.download_request_list.empty());
	ASSERT_TRUE(server_diff_list.conflict_list.empty());
}

TEST_F(DiffTest, LocalChangeNoConflict) {
	monitor_client::common_utility::ItemList server = GetReferece();
	monitor_client::common_utility::ItemList os = server;
	monitor_client::common_utility::ItemList db = server;

	monitor_client::common_utility::ItemInfo info;
	info.name = L"1_5.txt";
	info.size = 0;
	info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
	os.insert(info);

	info.name = L"1_4/1_4_1/1_4_1_2.txt";
	info.size = 100;
	info.hash = L"new file";
	os.insert(info);

	auto change_value = os.extract({ L"1_4/1_4_1/1_4_1_1.txt" });
	ASSERT_FALSE(change_value.empty());

	change_value.value().size = 10;
	change_value.value().hash = L"modify";
	os.insert(std::move(change_value));

	auto change_value2 = os.extract({ L"1_2/1_2_1/1_2_1_1.txt" });
	ASSERT_FALSE(change_value2.empty());

	change_value2.value().size = 5000;
	change_value2.value().hash = L"modify";
	os.insert(std::move(change_value2));

	os.erase({ L"1_1/1_1_1/1_1_1_1.txt" });

	monitor_client::diff_check::LocalDiffList local_diff_list = monitor_client::diff_check::MakeLocalDiffList(os, db);
	ASSERT_EQ(local_diff_list.create_list.size(), 2);

	auto iter = local_diff_list.create_list.begin();
	ASSERT_EQ(iter->name, L"1_4/1_4_1/1_4_1_2.txt");
	ASSERT_EQ(iter->size, 100);
	ASSERT_EQ(iter->hash, L"new file");
	++iter;

	ASSERT_EQ(iter->name, L"1_5.txt");
	ASSERT_EQ(iter->size, 0);
	ASSERT_EQ(iter->hash, L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");

	ASSERT_EQ(local_diff_list.modify_list.size(), 2);
	for (const auto& iter : local_diff_list.modify_list) {
		if (iter.os_item.name == L"1_4/1_4_1/1_4_1_1.txt") {
			std::wstring name = L"1_4/1_4_1/1_4_1_1.txt";

			auto db_iter = db.find({ name });
			ASSERT_TRUE(db.end() != db_iter);
			ASSERT_EQ(iter.other_item.size, db_iter->size);
			ASSERT_EQ(iter.other_item.hash, db_iter->hash);

			auto os_iter = os.find({ name });
			ASSERT_TRUE(os.end() != os_iter);
			ASSERT_EQ(iter.os_item.size, os_iter->size);
			ASSERT_EQ(iter.os_item.hash, os_iter->hash);
		}
		else if (iter.os_item.name == L"1_2/1_2_1/1_2_1_1.txt") {
			std::wstring name = L"1_2/1_2_1/1_2_1_1.txt";
			auto db_iter = db.find({ name });
			ASSERT_TRUE(db.end() != db_iter);
			ASSERT_EQ(iter.other_item.size, db_iter->size);
			ASSERT_EQ(iter.other_item.hash, db_iter->hash);

			auto os_iter = os.find({ name });
			ASSERT_TRUE(os.end() != os_iter);
			ASSERT_EQ(iter.os_item.size, os_iter->size);
			ASSERT_EQ(iter.os_item.hash, os_iter->hash);
		}
		else {
			ASSERT_TRUE(false);
		}
	}

	ASSERT_EQ(local_diff_list.equal_list.size(), 9);

	server.erase({ L"1_3/1_3_1/1_3_1_1.txt" });

	monitor_client::diff_check::ServerDiffList server_diff_list = monitor_client::diff_check::MakeServerDiffList(server, local_diff_list);
	ASSERT_TRUE(server_diff_list.conflict_list.empty());

	ASSERT_EQ(server_diff_list.upload_request_list.size(), 4);

	for (auto iter : server_diff_list.upload_request_list) {
		if (iter.name == L"1_5.txt") {
			std::wstring name = L"1_5.txt";
			auto os_iter = os.find({ name });
			ASSERT_TRUE(os.end() != os_iter);
			ASSERT_EQ(iter.size, os_iter->size);
			ASSERT_EQ(iter.hash, os_iter->hash);
		}
		else if (iter.name == L"1_4/1_4_1/1_4_1_2.txt") {
			std::wstring name = L"1_4/1_4_1/1_4_1_2.txt";
			auto os_iter = os.find({ name });
			ASSERT_TRUE(os.end() != os_iter);
			ASSERT_EQ(iter.size, os_iter->size);
			ASSERT_EQ(iter.hash, os_iter->hash);
		}
		else if (iter.name == L"1_4/1_4_1/1_4_1_1.txt") {
			std::wstring name = L"1_4/1_4_1/1_4_1_1.txt";
			auto os_iter = os.find({ name });
			ASSERT_TRUE(os.end() != os_iter);
			ASSERT_EQ(iter.size, os_iter->size);
			ASSERT_EQ(iter.hash, os_iter->hash);
		}
		else if (iter.name == L"1_2/1_2_1/1_2_1_1.txt") {
			std::wstring name = L"1_2/1_2_1/1_2_1_1.txt";
			auto os_iter = os.find({ name });
			ASSERT_TRUE(os.end() != os_iter);
			ASSERT_EQ(iter.size, os_iter->size);
			ASSERT_EQ(iter.hash, os_iter->hash);
		}
		else {
			ASSERT_TRUE(false);
		}
	}

	ASSERT_EQ(server_diff_list.download_request_list.size(), 1);
	ASSERT_EQ(server_diff_list.download_request_list[0], L"1_1/1_1_1/1_1_1_1.txt");

	ASSERT_EQ(server_diff_list.delete_list.size(), 1);
	ASSERT_EQ(server_diff_list.delete_list[0], L"1_3/1_3_1/1_3_1_1.txt");
}

TEST_F(DiffTest, LocalChangeConflict) {
	monitor_client::common_utility::ItemList server = GetReferece();
	monitor_client::common_utility::ItemList os = server;
	monitor_client::common_utility::ItemList db = server;

	monitor_client::common_utility::ItemInfo info;
	info.name = L"1_5.txt";
	info.size = 0;
	info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
	os.insert(info);

	info.name = L"1_6.txt";
	info.size = 0;
	info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
	os.insert(info);

	info.name = L"1_7";
	info.size = -1;
	info.hash.clear();
	os.insert(info);

	info.name = L"1_4/1_4_1/1_4_1_2.txt";
	info.size = 100;
	info.hash = L"new file";
	os.insert(info);

	auto change_value = os.extract({ L"1_4/1_4_1/1_4_1_1.txt" });
	ASSERT_FALSE(change_value.empty());

	change_value.value().size = 10;
	change_value.value().hash = L"modify";
	os.insert(std::move(change_value));

	auto change_value2 = os.extract({ L"1_2/1_2_1/1_2_1_1.txt" });
	ASSERT_FALSE(change_value2.empty());

	change_value2.value().size = 5000;
	change_value2.value().hash = L"modify";
	os.insert(std::move(change_value2));

	os.erase({L"1_1/1_1_1/1_1_1_1.txt"});

	monitor_client::diff_check::LocalDiffList local_diff_list = monitor_client::diff_check::MakeLocalDiffList(os, db);
	ASSERT_EQ(local_diff_list.create_list.size(), 4);

	auto iter = local_diff_list.create_list.begin();
	ASSERT_EQ(iter->name, L"1_4/1_4_1/1_4_1_2.txt");
	ASSERT_EQ(iter->size, 100);
	ASSERT_EQ(iter->hash, L"new file");
	++iter;

	ASSERT_EQ(iter->name, L"1_5.txt");
	ASSERT_EQ(iter->size, 0);
	ASSERT_EQ(iter->hash, L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
	++iter;

	ASSERT_EQ(iter->name, L"1_6.txt");
	ASSERT_EQ(iter->size, 0);
	ASSERT_EQ(iter->hash, L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
	++iter;

	ASSERT_EQ(iter->name, L"1_7");
	ASSERT_EQ(iter->size, -1);
	ASSERT_TRUE(iter->hash.empty());

	for (auto iter : local_diff_list.modify_list) {
		if (iter.os_item.name == L"1_4/1_4_1/1_4_1_1.txt") {
			std::wstring name = L"1_4/1_4_1/1_4_1_1.txt";
			auto db_iter = db.find({ name });
			ASSERT_TRUE(db.end() != db_iter);
			ASSERT_EQ(iter.other_item.size, db_iter->size);
			ASSERT_EQ(iter.other_item.hash, db_iter->hash);

			auto os_iter = os.find({ name });
			ASSERT_TRUE(os.end() != os_iter);
			ASSERT_EQ(iter.os_item.size, os_iter->size);
			ASSERT_EQ(iter.os_item.hash, os_iter->hash);
		}
		else if (iter.os_item.name == L"1_2/1_2_1/1_2_1_1.txt") {
			std::wstring name = L"1_2/1_2_1/1_2_1_1.txt";
			auto db_iter = db.find({ name });
			ASSERT_TRUE(db.end() != db_iter);
			ASSERT_EQ(iter.other_item.size, db_iter->size);
			ASSERT_EQ(iter.other_item.hash, db_iter->hash);

			auto os_iter = os.find({ name });
			ASSERT_TRUE(os.end() != os_iter);
			ASSERT_EQ(iter.os_item.size, os_iter->size);
			ASSERT_EQ(iter.os_item.hash, os_iter->hash);
		}
		else {
			ASSERT_TRUE(false);
		}
	}

	ASSERT_EQ(local_diff_list.equal_list.size(), 9);

	info.name = L"1_5.txt";
	info.size = 10;
	info.hash = L"already make";
	server.insert(info);

	info.name = L"1_6.txt";
	info.size = 0;
	info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
	server.insert(info);

	auto change_value3 = server.extract({ L"1_2/1_2_1/1_2_1_1.txt" });
	ASSERT_FALSE(change_value3.empty());

	change_value3.value().size = 1000;
	change_value3.value().hash = L"already modify";
	server.insert(std::move(change_value3));

	info.name = L"1_3/1_3_1/1_3_1_2.txt";
	info.size = 10;
	info.hash = L"new server file";
	server.insert(info);

	monitor_client::diff_check::ServerDiffList server_diff_list = monitor_client::diff_check::MakeServerDiffList(server, local_diff_list);
	ASSERT_EQ(server_diff_list.conflict_list.size(), 2);
	ASSERT_TRUE(server_diff_list.delete_list.empty());
	
	for (auto iter : server_diff_list.conflict_list) {
		if (iter.other_item.name == L"1_5.txt") {
			std::wstring name = L"1_5.txt";
			auto server_iter = server.find({ name });
			ASSERT_TRUE(server.end() != server_iter);
			ASSERT_EQ(iter.other_item.size, server_iter->size);
			ASSERT_EQ(iter.other_item.hash, server_iter->hash);

			auto os_iter = os.find({ name });
			ASSERT_TRUE(os.end() != os_iter);
			ASSERT_EQ(iter.os_item.size, os_iter->size);
			ASSERT_EQ(iter.os_item.hash, os_iter->hash);
		}
		else if (iter.other_item.name == L"1_2/1_2_1/1_2_1_1.txt") {
			std::wstring name = L"1_2/1_2_1/1_2_1_1.txt";
			auto server_iter = server.find({ name });
			ASSERT_TRUE(server.end() != server_iter);
			ASSERT_EQ(iter.other_item.size, server_iter->size);
			ASSERT_EQ(iter.other_item.hash, server_iter->hash);

			auto os_iter = os.find({ name });
			ASSERT_TRUE(os.end() != os_iter);
			ASSERT_EQ(iter.os_item.size, os_iter->size);
			ASSERT_EQ(iter.os_item.hash, os_iter->hash);
		}
		else {
			ASSERT_TRUE(false);
		}
	}

	ASSERT_EQ(server_diff_list.upload_request_list.size(), 3);

	for (auto iter : server_diff_list.upload_request_list) {
		if (iter.name == L"1_7") {
			std::wstring name = L"1_7";
			auto os_iter = os.find({ name });
			ASSERT_TRUE(os.end() != os_iter);
			ASSERT_EQ(iter.size, os_iter->size);
			ASSERT_EQ(iter.hash, os_iter->hash);
		}
		else if (iter.name == L"1_4/1_4_1/1_4_1_2.txt") {
			std::wstring name = L"1_4/1_4_1/1_4_1_2.txt";
			auto os_iter = os.find({ name });
			ASSERT_TRUE(os.end() != os_iter);
			ASSERT_EQ(iter.size, os_iter->size);
			ASSERT_EQ(iter.hash, os_iter->hash);
		}
		else if (iter.name == L"1_4/1_4_1/1_4_1_1.txt") {
			std::wstring name = L"1_4/1_4_1/1_4_1_1.txt";
			auto os_iter = os.find({ name });
			ASSERT_TRUE(os.end() != os_iter);
			ASSERT_EQ(iter.size, os_iter->size);
			ASSERT_EQ(iter.hash, os_iter->hash);
		}
		else {
			ASSERT_TRUE(false);
		}
	}

	ASSERT_EQ(server_diff_list.download_request_list.size(), 2);

	for (auto iter : server_diff_list.download_request_list) {
		if (iter != L"1_1/1_1_1/1_1_1_1.txt" && iter != L"1_3/1_3_1/1_3_1_2.txt") {
			ASSERT_TRUE(false);
		}
	}
}