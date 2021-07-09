#include "pch.h"

#include <Windows.h>
#include <stdint.h>

#include <fstream>
#include <string>

#include "../monitor_client/event_handler.h"

class EventTest : public ::testing::Test {
public:
	monitor_client::EventHandler* handler;
	std::shared_ptr<monitor_client::NotifyQueue> notify_queue;
	void SetUp() override {
		notify_queue = std::make_shared<monitor_client::NotifyQueue>();
		handler = new monitor_client::EventHandler(notify_queue);

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

	void TearDown() override {
		_wsystem(L"rmdir /s /q C:\\Users\\ABO\\Desktop\\1");
		delete handler;
	}

	void MakeNotify(uint8_t* buffer, const monitor_client::common_utility::ChangeItemInfo& change_item_info) {
		FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&buffer[0]);

		fni->Action = change_item_info.action;
		fni->FileNameLength = change_item_info.relative_path.size() * sizeof(wchar_t);
		memcpy(fni->FileName, change_item_info.relative_path.c_str(), change_item_info.relative_path.size() * sizeof(wchar_t));
		fni->NextEntryOffset = 0;
	}

	void MakeChangeNameNotify(uint8_t* buffer, const monitor_client::common_utility::ChangeNameInfo& change_name_info) {
		FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&buffer[0]);
		
		fni->Action = FILE_ACTION_RENAMED_OLD_NAME;
		fni->FileNameLength = change_name_info.old_name.size() * sizeof(wchar_t);
		memcpy(fni->FileName, change_name_info.old_name.c_str(), change_name_info.old_name.size() * sizeof(wchar_t));
		int align = fni->FileNameLength / 4 * 4;
		fni->NextEntryOffset = sizeof(FILE_NOTIFY_INFORMATION) + align;

		fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&buffer[fni->NextEntryOffset]);
		fni->Action = FILE_ACTION_RENAMED_NEW_NAME;
		fni->FileNameLength = change_name_info.new_name.size() * sizeof(wchar_t);
		memcpy(fni->FileName, change_name_info.new_name.c_str(), change_name_info.new_name.size() * sizeof(wchar_t));
		fni->NextEntryOffset = 0;
	}
};

TEST_F(EventTest, FileAdd) {
	uint8_t* buffer = new uint8_t[1024 * 1024];
	memset(buffer, 0, 1024 * 1024);

	std::wstring file_path = L"C:\\Users\\ABO\\Desktop\\1\\1_1\\1_1_1\\1_1_1_1.txt";

	monitor_client::common_utility::ChangeItemInfo reference;
	reference.action = FILE_ACTION_ADDED;
	reference.relative_path = file_path;

	MakeNotify(buffer, reference);

	handler->PushEvent(buffer);

	std::optional<monitor_client::common_utility::ChangeItemInfo> result = notify_queue->Front();

	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(reference.action, result.value().action);
	ASSERT_EQ(reference.relative_path, result.value().relative_path);
	
	delete[] buffer;
}

TEST_F(EventTest, FolderAdd) {
	uint8_t* buffer = new uint8_t[1024 * 1024];
	memset(buffer, 0, 1024 * 1024);

	monitor_client::common_utility::ChangeItemInfo reference;
	reference.action = FILE_ACTION_ADDED;
	reference.relative_path = L"C:\\Users\\ABO\\Desktop\\1";

	MakeNotify(buffer, reference);

	handler->PushEvent(buffer);

	std::vector<std::wstring> v{
		L"C:\\Users\\ABO\\Desktop\\1", L"C:\\Users\\ABO\\Desktop\\1\\1_1", L"C:\\Users\\ABO\\Desktop\\1\\1_1\\1_1_1", L"C:\\Users\\ABO\\Desktop\\1\\1_1\\1_1_1\\1_1_1_1.txt",
		L"C:\\Users\\ABO\\Desktop\\1\\1_2", L"C:\\Users\\ABO\\Desktop\\1\\1_2\\1_2_1", L"C:\\Users\\ABO\\Desktop\\1\\1_2\\1_2_1\\1_2_1_1.txt",
		L"C:\\Users\\ABO\\Desktop\\1\\1_3", L"C:\\Users\\ABO\\Desktop\\1\\1_3\\1_3_1", L"C:\\Users\\ABO\\Desktop\\1\\1_3\\1_3_1\\1_3_1_1.txt",
		L"C:\\Users\\ABO\\Desktop\\1\\1_4", L"C:\\Users\\ABO\\Desktop\\1\\1_4\\1_4_1", L"C:\\Users\\ABO\\Desktop\\1\\1_4\\1_4_1\\1_4_1_1.txt"
	};

	std::vector<std::wstring> v2;
	for (int i = 0; i < v.size(); ++i) {
		auto result = notify_queue->Front();
		ASSERT_TRUE(result.has_value());
		notify_queue->Pop();
		v2.push_back(result.value().relative_path);
	}

	ASSERT_EQ(v.size(), v2.size());
	for (int i = 0; i < v.size(); ++i) {
		ASSERT_EQ(v[i], v2[i]);
	}

	delete[] buffer;
}

TEST_F(EventTest, ItemNewName) {
	uint8_t* buffer = new uint8_t[1024 * 1024];
	memset(buffer, 0, 1024 * 1024);

	std::wstring old_name = L"C:\\Users\\ABO\\Desktop\\1\\1_3\\1_3_1";
	std::wstring new_name = L"C:\\Users\\ABO\\Desktop\\1\\1_3\\1_3_2";

	monitor_client::common_utility::ChangeNameInfo change_name_info;
	change_name_info.old_name = old_name;
	change_name_info.new_name = new_name;

	MakeChangeNameNotify(buffer, change_name_info);

	handler->PushEvent(buffer);

	monitor_client::common_utility::ChangeItemInfo reference;
	reference.action = FILE_ACTION_RENAMED_NEW_NAME;
	reference.relative_path = old_name + L'?' + new_name;

	std::optional<monitor_client::common_utility::ChangeItemInfo> result = notify_queue->Front();

	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(reference.action, result.value().action);
	ASSERT_EQ(reference.relative_path, result.value().relative_path);

	delete[] buffer;
}

TEST_F(EventTest, Modify) {
	uint8_t* buffer = new uint8_t[1024 * 1024];
	memset(buffer, 0, 1024 * 1024);

	monitor_client::common_utility::ChangeItemInfo change_item_info;
	change_item_info.action = FILE_ACTION_MODIFIED;
	change_item_info.relative_path = L"C:\\Users\\ABO\\Desktop\\1\\1_1\\1_1_1";

	MakeNotify(buffer, change_item_info);

	handler->PushEvent(buffer);

	change_item_info.action = FILE_ACTION_MODIFIED;
	change_item_info.relative_path = L"C:\\Users\\ABO\\Desktop\\1\\1_1\\1_1_1\\1_1_1_1.txt";

	MakeNotify(buffer, change_item_info);

	handler->PushEvent(buffer);

	std::optional<monitor_client::common_utility::ChangeItemInfo> result = notify_queue->Front();

	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(FILE_ACTION_MODIFIED, result.value().action);
	ASSERT_EQ(L"C:\\Users\\ABO\\Desktop\\1\\1_1\\1_1_1\\1_1_1_1.txt", result.value().relative_path);

	delete[] buffer;
}