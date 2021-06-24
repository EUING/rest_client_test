#include "pch.h"

#include <Windows.h>
#include <direct.h>

#include <fstream>
#include <iostream>
#include <string>

#include "../rest_client/common_utility.h"
#include "../rest_client/folder_watcher.h"
#include "../rest_client/notify_queue.h"

TEST(FolderTest, WrongPath) {
	std::wstring wrong_path = L"WRONG PATH";
	monitor_client::NotifyQueue notify_queue;

	monitor_client::FolderWatcher watcher(&notify_queue, wrong_path);
	bool result = watcher.StartWatching();		

	ASSERT_FALSE(result);
}

TEST(FolderTest, NullCheck) {
	std::wstring wrong_path = L"WRONG PATH";
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	monitor_client::NotifyQueue notify_queue;
	monitor_client::FolderWatcher watcher(nullptr, wrong_path);

	bool result = watcher.StartWatching();

	ASSERT_FALSE(result);
	watcher.SetWatchFolder(path);
	result = watcher.StartWatching();

	ASSERT_TRUE(result);
}

TEST(FolderTest, NoStartAndNoStop) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	monitor_client::NotifyQueue notify_queue;

	monitor_client::FolderWatcher watcher(&notify_queue, path);
}

TEST(FolderTest, NoStartAndStop) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	monitor_client::NotifyQueue notify_queue;

	monitor_client::FolderWatcher watcher(&notify_queue, path);
	watcher.StopWatching();
}

TEST(FolderTest, StartAndNoStop) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	monitor_client::NotifyQueue notify_queue;

	monitor_client::FolderWatcher watcher(&notify_queue, path);
	bool result = watcher.StartWatching();
	ASSERT_TRUE(result);
}

TEST(FolderTest, DuplicateStart) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	monitor_client::NotifyQueue notify_queue;

	monitor_client::FolderWatcher watcher(&notify_queue, path);
	bool result = watcher.StartWatching();
	ASSERT_TRUE(result);

	result = watcher.StartWatching();
	ASSERT_FALSE(result);
}

TEST(FolderTest, DuplicateStop) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	monitor_client::NotifyQueue notify_queue;

	monitor_client::FolderWatcher watcher(&notify_queue, path);
	bool result = watcher.StartWatching();
	ASSERT_TRUE(result);

	watcher.StopWatching();
	watcher.StopWatching();
}

TEST(FolderTest, Stop) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	monitor_client::NotifyQueue notify_queue;

	monitor_client::FolderWatcher watcher(&notify_queue, path);
	bool result = watcher.StartWatching();
	ASSERT_TRUE(result);

	result = watcher.IsRunning();
	ASSERT_TRUE(result);

	watcher.StopWatching();
	result = watcher.IsRunning();
	ASSERT_FALSE(result);
}

TEST(FolderTest, ReRunning) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	monitor_client::NotifyQueue notify_queue;

	monitor_client::FolderWatcher watcher(&notify_queue, path);
	bool result = watcher.StartWatching();
	ASSERT_TRUE(result);

	watcher.StopWatching();
	result = watcher.StartWatching();
	ASSERT_TRUE(result);

	result = watcher.IsRunning();
	ASSERT_TRUE(result);
}

TEST(FolderTest, GetFolder) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	monitor_client::NotifyQueue notify_queue;

	monitor_client::FolderWatcher watcher(&notify_queue, path);
	bool result = watcher.StartWatching();
	ASSERT_TRUE(result);

	std::wstring current_path = watcher.GetWatchFolder();
	ASSERT_EQ(path, current_path);
}

TEST(FolderTest, ChangeFolder) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	std::wstring new_path = L"C:\\";
	monitor_client::NotifyQueue notify_queue;

	monitor_client::FolderWatcher watcher(&notify_queue, path);
	bool result = watcher.StartWatching();
	ASSERT_TRUE(result);

	watcher.StopWatching();

	watcher.SetWatchFolder(new_path);
	result = watcher.StartWatching();
	ASSERT_TRUE(result);

	std::wstring current_path = watcher.GetWatchFolder();
	ASSERT_EQ(new_path, current_path);
}

TEST(FolderTest, CreateFolder) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	monitor_client::NotifyQueue notify_queue;

	monitor_client::FolderWatcher watcher(&notify_queue, path);
	watcher.StartWatching();

	std::string new_folder = "C:\\Users\\ABO\\Desktop\\test";
	_mkdir(new_folder.c_str());

	watcher.StopWatching();
	_rmdir(new_folder.c_str());

	auto result = notify_queue.Pop();
	ASSERT_TRUE(result.has_value());

	monitor_client::common_utility::ChangeObjectInfo result_info = result.value();

	monitor_client::common_utility::ChangeObjectInfo info;
	info.action = FILE_ACTION_ADDED;
	info.full_path = L"C:\\Users\\ABO\\Desktop\\test";

	ASSERT_EQ(result_info.action, info.action);
	ASSERT_EQ(result_info.full_path, info.full_path);
}

TEST(FolderTest, DeleteFolder) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	monitor_client::NotifyQueue notify_queue;

	std::string new_folder = "C:\\Users\\ABO\\Desktop\\test";
	_mkdir(new_folder.c_str());

	monitor_client::FolderWatcher watcher(&notify_queue, path);
	watcher.StartWatching();

	_rmdir(new_folder.c_str());

	auto result = notify_queue.Pop();
	ASSERT_TRUE(result.has_value());

	monitor_client::common_utility::ChangeObjectInfo result_info = result.value();

	monitor_client::common_utility::ChangeObjectInfo info;
	info.action = FILE_ACTION_REMOVED;
	info.full_path = L"C:\\Users\\ABO\\Desktop\\test";

	ASSERT_EQ(result_info.action, info.action);
	ASSERT_EQ(result_info.full_path, info.full_path);
}

TEST(FolderTest, ChangeName) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	monitor_client::NotifyQueue notify_queue;

	std::string old_folder = "C:\\Users\\ABO\\Desktop\\old";
	_mkdir(old_folder.c_str());

	monitor_client::FolderWatcher watcher(&notify_queue, path);
	watcher.StartWatching();

	std::string new_folder = "C:\\Users\\ABO\\Desktop\\new";
	std::rename(old_folder.c_str(), new_folder.c_str());

	watcher.StopWatching();

	auto result = notify_queue.Pop();
	ASSERT_TRUE(result.has_value());

	monitor_client::common_utility::ChangeObjectInfo result_info = result.value();

	monitor_client::common_utility::ChangeObjectInfo info;
	info.action = FILE_ACTION_RENAMED_NEW_NAME;
	info.full_path = L"C:\\Users\\ABO\\Desktop\\old?C:\\Users\\ABO\\Desktop\\new";

	ASSERT_EQ(result_info.action, info.action);
	ASSERT_EQ(result_info.full_path, info.full_path);

	_rmdir(new_folder.c_str());
}

TEST(FolderTest, CreateText) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	monitor_client::NotifyQueue notify_queue;

	monitor_client::FolderWatcher watcher(&notify_queue, path);
	watcher.StartWatching();

	std::string file_name = "C:\\Users\\ABO\\Desktop\\test.txt";
	std::ofstream file{ file_name };
	file.close();
	
	watcher.StopWatching();

	auto result = notify_queue.Pop();
	ASSERT_TRUE(result.has_value());

	monitor_client::common_utility::ChangeObjectInfo result_info = result.value();

	monitor_client::common_utility::ChangeObjectInfo info;
	info.action = FILE_ACTION_ADDED;
	info.full_path = L"C:\\Users\\ABO\\Desktop\\test.txt";

	ASSERT_EQ(result_info.action, info.action);
	ASSERT_EQ(result_info.full_path, info.full_path);
	
	std::remove(file_name.c_str());
}

TEST(FolderTest, ModifyText) {	
	std::string file_name = "C:\\Users\\ABO\\Desktop\\test.txt";
	std::ofstream temp{ file_name };
	temp.close();

	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	monitor_client::NotifyQueue notify_queue;

	monitor_client::FolderWatcher watcher(&notify_queue, path);
	watcher.StartWatching();

	std::ofstream file{ file_name };
	file.close();

	auto result = notify_queue.Pop();
	ASSERT_TRUE(result.has_value());

	monitor_client::common_utility::ChangeObjectInfo result_info = result.value();

	monitor_client::common_utility::ChangeObjectInfo info;
	info.action = FILE_ACTION_MODIFIED;
	info.full_path = L"C:\\Users\\ABO\\Desktop\\test.txt";

	ASSERT_EQ(result_info.action, info.action);
	ASSERT_EQ(result_info.full_path, info.full_path);

	std::remove(file_name.c_str());
}