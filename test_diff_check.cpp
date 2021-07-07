#include "pch.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <map>

#include "../monitor_client/diff_check.h"
#include "../monitor_client/item_dao_sqlite.h"
#include "../monitor_client/local_db.h"

class DiffTest : public ::testing::Test {
public:
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
	}

	std::map<std::wstring, monitor_client::common_utility::ItemInfo> GetReferece() {
		std::map<std::wstring, monitor_client::common_utility::ItemInfo> reference;
		monitor_client::common_utility::ItemInfo info;
		info.name = L"1_1";
		info.size = -1;
		info.hash.clear();

		reference.insert({ info.name, info });

		info.name = L"1_2";
		reference.insert({ info.name, info });

		info.name = L"1_3";
		reference.insert({ info.name, info });

		info.name = L"1_4";
		reference.insert({ info.name, info });

		info.name = L"1_1/1_1_1";
		reference.insert({ info.name, info });

		info.name = L"1_2/1_2_1";
		reference.insert({ info.name, info });

		info.name = L"1_3/1_3_1";
		reference.insert({ info.name, info });

		info.name = L"1_4/1_4_1";
		reference.insert({ info.name, info });

		info.name = L"1_1/1_1_1/1_1_1_1.txt";
		info.size = 0;
		info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
		reference.insert({ info.name, info });

		info.name = L"1_2/1_2_1/1_2_1_1.txt";
		reference.insert({ info.name, info });

		info.name = L"1_3/1_3_1/1_3_1_1.txt";
		reference.insert({ info.name, info });

		info.name = L"1_4/1_4_1/1_4_1_1.txt";
		reference.insert({ info.name, info });

		return reference;
	}

	void TearDown() override {
		_wsystem(L"rmdir /s /q C:\\Users\\ABO\\Desktop\\1");
	}
};

TEST_F(DiffTest, GetWindowSubFolder) {
	std::wstring target_folder = L"C:\\Users\\ABO\\Desktop\\1";
	ASSERT_TRUE(SetCurrentDirectory(target_folder.c_str()));

	std::unordered_map<std::wstring, monitor_client::common_utility::ItemInfo> item_list;
	GetSubFolderInfo(L".", item_list);

	std::map<std::wstring, monitor_client::common_utility::ItemInfo> result(item_list.begin(), item_list.end());
	std::map<std::wstring, monitor_client::common_utility::ItemInfo> reference = GetReferece();

	ASSERT_EQ(result.size(), reference.size());

	auto iter1 = reference.begin();
	auto iter2 = result.begin();

	for (int i = 0; i < result.size(); ++i) {
		ASSERT_EQ(iter1->second.name, iter2->second.name);
		ASSERT_EQ(iter1->second.size, iter2->second.size);
		ASSERT_EQ(iter1->second.hash, iter2->second.hash);
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

	std::unordered_map<std::wstring, monitor_client::common_utility::ItemInfo> item_list;
	monitor_client::common_utility::GetSubFolderInfo(local_db, L"", item_list);

	std::map<std::wstring, monitor_client::common_utility::ItemInfo> result(item_list.begin(), item_list.end());
	std::map<std::wstring, monitor_client::common_utility::ItemInfo> reference = GetReferece();

	ASSERT_EQ(result.size(), reference.size());

	auto iter1 = reference.begin();
	auto iter2 = result.begin();

	for (int i = 0; i < result.size(); ++i) {
		ASSERT_EQ(iter1->second.name, iter2->second.name);
		ASSERT_EQ(iter1->second.size, iter2->second.size);
		ASSERT_EQ(iter1->second.hash, iter2->second.hash);
	}
}

TEST_F(DiffTest, MakeDiffList) {
	std::map<std::wstring, monitor_client::common_utility::ItemInfo> reference = GetReferece();
	std::unordered_map<std::wstring, monitor_client::common_utility::ItemInfo> db(reference.begin(), reference.end());
	std::unordered_map<std::wstring, monitor_client::common_utility::ItemInfo> os(reference.begin(), reference.end());

	std::vector<monitor_client::common_utility::ItemInfo> create_list;
	std::vector<monitor_client::common_utility::ItemInfo> equal_list;
	std::vector<monitor_client::diff_check::DiffInfo> modify_list;	
	
	for (const auto& iter : reference) {
		equal_list.push_back(iter.second);
	}
	
	int index = -1;
	for (int i = 0; i < equal_list.size(); ++i) {
		if (equal_list[i].name == L"1_1/1_1_1/1_1_1_1.txt") {
			index = i;
			break;
		}
	}

	equal_list.erase(equal_list.begin() + index);
	auto cmp = [](const monitor_client::common_utility::ItemInfo& lhs, const monitor_client::common_utility::ItemInfo& rhs) {
		return lhs.name > rhs.name;
	};

	std::sort(equal_list.begin(), equal_list.end(), cmp);

	monitor_client::common_utility::ItemInfo info;
	info.name = L"1_5.txt";
	info.size = 0;
	info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
	os.insert({ info.name, info });
	create_list.push_back(info);

	info.name = L"1_1/1_1_1/1_1_1_1.txt";
	info.size = 0;
	info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";	

	monitor_client::common_utility::ItemInfo modify_info;
	modify_info.name = L"1_1/1_1_1/1_1_1_1.txt";
	modify_info.size = 1;
	modify_info.hash = L"modify";
	modify_list.push_back({ info, modify_info });

	os[L"1_1/1_1_1/1_1_1_1.txt"].size = 1;
	os[L"1_1/1_1_1/1_1_1_1.txt"].hash = L"modify";

	monitor_client::diff_check::DiffList list = monitor_client::diff_check::MakeDiffList(os, db);
	std::sort(list.equal_list.begin(), list.equal_list.end(), cmp);

	ASSERT_EQ(create_list.size(), list.create_list.size());
	for (int i = 0; i < create_list.size(); ++i) {
		ASSERT_EQ(create_list[i].name, list.create_list[i].name);
		ASSERT_EQ(create_list[i].size, list.create_list[i].size);
		ASSERT_EQ(create_list[i].hash, list.create_list[i].hash);
	}

	ASSERT_EQ(equal_list.size(), list.equal_list.size());
	for (int i = 0; i < equal_list.size(); ++i) {
		ASSERT_EQ(equal_list[i].name, list.equal_list[i].name);
		ASSERT_EQ(equal_list[i].size, list.equal_list[i].size);
		ASSERT_EQ(equal_list[i].hash, list.equal_list[i].hash);
	}

	ASSERT_EQ(modify_list.size(), list.modify_list.size());
	for (int i = 0; i < modify_list.size(); ++i) {
		ASSERT_EQ(modify_list[i].db_item.name, list.modify_list[i].db_item.name);
		ASSERT_EQ(modify_list[i].db_item.size, list.modify_list[i].db_item.size);
		ASSERT_EQ(modify_list[i].db_item.hash, list.modify_list[i].db_item.hash);

		ASSERT_EQ(modify_list[i].os_item.name, list.modify_list[i].os_item.name);
		ASSERT_EQ(modify_list[i].os_item.size, list.modify_list[i].os_item.size);
		ASSERT_EQ(modify_list[i].os_item.hash, list.modify_list[i].os_item.hash);
	}
}