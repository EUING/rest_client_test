#include "pch.h"

#include <Windows.h>
#include <direct.h>

#include <fstream>
#include <iostream>
#include <string>

#include "../monitor_client/common_utility.h"
#include "../monitor_client/folder_watcher.h"
#include "../monitor_client/notify_queue.h"

const wchar_t* const kTestPath = L"C:\\Users\\ABO\\Desktop";
class FolderTest : public ::testing::Test {
public:
	monitor_client::FolderWatcher* watcher;
	std::shared_ptr<monitor_client::NotifyQueue> notify_queue;
	void SetUp() override {
		notify_queue = std::make_shared<monitor_client::NotifyQueue>();
		watcher = new monitor_client::FolderWatcher(notify_queue, kTestPath);
	}

	void TearDown() override {
		delete watcher;
	}
};

TEST_F(FolderTest, WrongPath) {
	std::wstring wrong_path = L"WRONG PATH";
	watcher->SetWatchFolder(wrong_path);

	bool result = watcher->StartWatching();		

	ASSERT_FALSE(result);
}

TEST_F(FolderTest, NullCheck) {
	std::wstring wrong_path = L"WRONG PATH";
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	delete watcher;
	watcher = new monitor_client::FolderWatcher(nullptr, wrong_path);

	bool result = watcher->StartWatching();

	ASSERT_FALSE(result);
	watcher->SetWatchFolder(path);
	result = watcher->StartWatching();

	ASSERT_TRUE(result);
}

TEST_F(FolderTest, NoStartAndNoStop) {
}

TEST_F(FolderTest, NoStartAndStop) {
	watcher->StopWatching();
}

TEST_F(FolderTest, StartAndNoStop) {
	bool result = watcher->StartWatching();
	ASSERT_TRUE(result);
}

TEST_F(FolderTest, DuplicateStart) {
	bool result = watcher->StartWatching();
	ASSERT_TRUE(result);

	result = watcher->StartWatching();
	ASSERT_FALSE(result);
}

TEST_F(FolderTest, DuplicateStop) {
	bool result = watcher->StartWatching();
	ASSERT_TRUE(result);

	watcher->StopWatching();
	watcher->StopWatching();
}

TEST_F(FolderTest, Stop) {
	bool result = watcher->StartWatching();
	ASSERT_TRUE(result);

	result = watcher->IsRunning();
	ASSERT_TRUE(result);

	watcher->StopWatching();
	result = watcher->IsRunning();
	ASSERT_FALSE(result);
}

TEST_F(FolderTest, ReRunning) {
	bool result = watcher->StartWatching();
	ASSERT_TRUE(result);

	watcher->StopWatching();
	result = watcher->StartWatching();
	ASSERT_TRUE(result);

	result = watcher->IsRunning();
	ASSERT_TRUE(result);
}

TEST_F(FolderTest, GetFolder) {
	bool result = watcher->StartWatching();
	ASSERT_TRUE(result);

	std::wstring current_path = watcher->GetWatchFolder();
	ASSERT_EQ(kTestPath, current_path);
}

TEST_F(FolderTest, ChangeFolder) {
	std::wstring new_path = L"C:\\";
	bool result = watcher->StartWatching();
	ASSERT_TRUE(result);

	watcher->StopWatching();

	watcher->SetWatchFolder(new_path);
	result = watcher->StartWatching();
	ASSERT_TRUE(result);

	std::wstring current_path = watcher->GetWatchFolder();
	ASSERT_EQ(new_path, current_path);
}

TEST_F(FolderTest, CreateFolder) {
	watcher->StartWatching();

	std::wstring new_folder = L"C:\\Users\\ABO\\Desktop\\test";
	_wmkdir(new_folder.c_str());

	watcher->StopWatching();
	RemoveDirectory(new_folder.c_str());

	auto result = notify_queue->Front();
	ASSERT_TRUE(result.has_value());
	notify_queue->Pop();

	monitor_client::common_utility::ChangeItemInfo result_info = result.value();

	monitor_client::common_utility::ChangeItemInfo info;
	info.action = FILE_ACTION_ADDED;
	info.relative_path = L"test";

	ASSERT_EQ(result_info.action, info.action);
	ASSERT_EQ(result_info.relative_path, info.relative_path);
}

TEST_F(FolderTest, DeleteFolder) {
	std::wstring new_folder = L"C:\\Users\\ABO\\Desktop\\test";
	_wmkdir(new_folder.c_str());

	watcher->StartWatching();

	RemoveDirectory(new_folder.c_str());

	watcher->StopWatching();

	auto result = notify_queue->Front();
	ASSERT_TRUE(result.has_value());
	notify_queue->Pop();

	monitor_client::common_utility::ChangeItemInfo result_info = result.value();

	monitor_client::common_utility::ChangeItemInfo info;
	info.action = FILE_ACTION_REMOVED;
	info.relative_path = L"test";

	ASSERT_EQ(result_info.action, info.action);
	ASSERT_EQ(result_info.relative_path, info.relative_path);
}

TEST_F(FolderTest, ChangeName) {
	std::wstring old_folder = L"C:\\Users\\ABO\\Desktop\\old";
	_wmkdir(old_folder.c_str());

	watcher->StartWatching();

	std::wstring new_folder = L"C:\\Users\\ABO\\Desktop\\new";
	_wrename(old_folder.c_str(), new_folder.c_str());

	watcher->StopWatching();

	auto result = notify_queue->Front();
	ASSERT_TRUE(result.has_value());
	notify_queue->Pop();

	monitor_client::common_utility::ChangeItemInfo result_info = result.value();

	monitor_client::common_utility::ChangeItemInfo info;
	info.action = FILE_ACTION_RENAMED_NEW_NAME;
	info.relative_path = L"old?new";

	ASSERT_EQ(result_info.action, info.action);
	ASSERT_EQ(result_info.relative_path, info.relative_path);

	_wrmdir(new_folder.c_str());
}

TEST_F(FolderTest, CreateText) {
	watcher->StartWatching();

	std::wstring file_name = L"C:\\Users\\ABO\\Desktop\\test.txt";
	std::ofstream file{ file_name };
	file.close();
	
	watcher->StopWatching();

	auto result = notify_queue->Front();
	ASSERT_TRUE(result.has_value());
	notify_queue->Pop();

	monitor_client::common_utility::ChangeItemInfo result_info = result.value();

	monitor_client::common_utility::ChangeItemInfo info;
	info.action = FILE_ACTION_ADDED;
	info.relative_path = L"test.txt";

	ASSERT_EQ(result_info.action, info.action);
	ASSERT_EQ(result_info.relative_path, info.relative_path);
	
	_wremove(file_name.c_str());
}

TEST_F(FolderTest, ModifyText) {
	std::wstring file_name = L"C:\\Users\\ABO\\Desktop\\test.txt";
	std::ofstream temp{ file_name };
	temp.close();

	watcher->StartWatching();

	std::ofstream file{ file_name };
	file.close();

	auto result = notify_queue->Front();
	ASSERT_TRUE(result.has_value());
	notify_queue->Pop();

	monitor_client::common_utility::ChangeItemInfo result_info = result.value();

	monitor_client::common_utility::ChangeItemInfo info;
	info.action = FILE_ACTION_MODIFIED;
	info.relative_path = L"test.txt";

	ASSERT_EQ(result_info.action, info.action);
	ASSERT_EQ(result_info.relative_path, info.relative_path);

	_wremove(file_name.c_str());
}