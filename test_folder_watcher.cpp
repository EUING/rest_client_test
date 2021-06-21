#include "pch.h"

#include <Windows.h>
#include <direct.h>

#include <fstream>
#include <iostream>
#include <string>
#include <queue>
#include <utility>

#include "../rest_client/folder_watcher.h"

TEST(FolderTest, WrongPath) {
	std::wstring wrong_path = L"WRONG PATH";
	std::queue<std::pair<DWORD, std::wstring>> change_info;

	my_rest_client::FolderWatcher watcher(&change_info, wrong_path);
	bool result = watcher.StartWatching();		

	ASSERT_FALSE(result);
}

TEST(FolderTest, NoStartAndNoStop) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	std::queue<std::pair<DWORD, std::wstring>> change_info;

	my_rest_client::FolderWatcher watcher(&change_info, path);
}

TEST(FolderTest, NoStartAndStop) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	std::queue<std::pair<DWORD, std::wstring>> change_info;

	my_rest_client::FolderWatcher watcher(&change_info, path);
	watcher.StopWatching();
}

TEST(FolderTest, StartAndNoStop) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	std::queue<std::pair<DWORD, std::wstring>> change_info;

	my_rest_client::FolderWatcher watcher(&change_info, path);
	bool result = watcher.StartWatching();
	ASSERT_TRUE(result);
}

TEST(FolderTest, DuplicateStart) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	std::queue<std::pair<DWORD, std::wstring>> change_info;

	my_rest_client::FolderWatcher watcher(&change_info, path);
	bool result = watcher.StartWatching();
	ASSERT_TRUE(result);

	result = watcher.StartWatching();
	ASSERT_FALSE(result);
}

TEST(FolderTest, DuplicateStop) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	std::queue<std::pair<DWORD, std::wstring>> change_info;

	my_rest_client::FolderWatcher watcher(&change_info, path);
	bool result = watcher.StartWatching();
	ASSERT_TRUE(result);

	watcher.StopWatching();
	watcher.StopWatching();
}

TEST(FolderTest, Running) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	std::queue<std::pair<DWORD, std::wstring>> change_info;

	my_rest_client::FolderWatcher watcher(&change_info, path);
	bool result = watcher.StartWatching();
	ASSERT_TRUE(result);

	result = watcher.IsRunning();
	ASSERT_TRUE(result);
}

TEST(FolderTest, Stop) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	std::queue<std::pair<DWORD, std::wstring>> change_info;

	my_rest_client::FolderWatcher watcher(&change_info, path);
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
	std::queue<std::pair<DWORD, std::wstring>> change_info;

	my_rest_client::FolderWatcher watcher(&change_info, path);
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
	std::queue<std::pair<DWORD, std::wstring>> change_info;

	my_rest_client::FolderWatcher watcher(&change_info, path);
	bool result = watcher.StartWatching();
	ASSERT_TRUE(result);

	std::wstring current_path = watcher.GetWatchFolder();
	ASSERT_EQ(path, current_path);
}

TEST(FolderTest, ChangeFolder) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	std::wstring new_path = L"C:\\";
	std::queue<std::pair<DWORD, std::wstring>> change_info;

	my_rest_client::FolderWatcher watcher(&change_info, path);
	bool result = watcher.StartWatching();
	ASSERT_TRUE(result);

	watcher.StopWatching();

	watcher.SetWatchFolder(new_path);
	result = watcher.StartWatching();
	ASSERT_TRUE(result);

	std::wstring current_path = watcher.GetWatchFolder();
	ASSERT_EQ(new_path, current_path);
}

TEST(FolderTest, Move) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	std::queue<std::pair<DWORD, std::wstring>> change_info;

	my_rest_client::FolderWatcher watcher(&change_info, path);;
	bool result = watcher.StartWatching();
	ASSERT_TRUE(result);

	my_rest_client::FolderWatcher wathcer2(std::move(watcher));

	result = wathcer2.StartWatching();
	ASSERT_TRUE(result);
}

TEST(FolderTest, CreateFolder) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	std::queue<std::pair<DWORD, std::wstring>> change_info;

	my_rest_client::FolderWatcher watcher(&change_info, path);;
	watcher.StartWatching();

	std::string new_folder = "C:\\Users\\ABO\\Desktop\\test";
	_mkdir(new_folder.c_str());

	watcher.StopWatching();
	_rmdir(new_folder.c_str());

	std::queue<std::pair<DWORD, std::wstring>> result_info;
	result_info.push({ FILE_ACTION_ADDED, L"C:\\Users\\ABO\\Desktop\\test" });

	ASSERT_EQ(result_info.size(), change_info.size());

	auto p1 = result_info.front();
	auto p2 = change_info.front();

	ASSERT_EQ(p1, p2);
}

TEST(FolderTest, DeleteFolder) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	std::queue<std::pair<DWORD, std::wstring>> change_info;

	std::string new_folder = "C:\\Users\\ABO\\Desktop\\test";
	_mkdir(new_folder.c_str());

	my_rest_client::FolderWatcher watcher(&change_info, path);;
	watcher.StartWatching();

	_rmdir(new_folder.c_str());

	std::queue<std::pair<DWORD, std::wstring>> result_info;
	result_info.push({ FILE_ACTION_REMOVED, L"C:\\Users\\ABO\\Desktop\\test" });
	ASSERT_EQ(result_info.size(), change_info.size());

	auto p1 = result_info.front();
	auto p2 = change_info.front();

	ASSERT_EQ(p1, p2);
}

TEST(FolderTest, ChangeName) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	std::queue<std::pair<DWORD, std::wstring>> change_info;

	std::string old_folder = "C:\\Users\\ABO\\Desktop\\old";
	_mkdir(old_folder.c_str());

	my_rest_client::FolderWatcher watcher(&change_info, path);;
	watcher.StartWatching();

	std::string new_folder = "C:\\Users\\ABO\\Desktop\\new";
	std::rename(old_folder.c_str(), new_folder.c_str());

	watcher.StopWatching();

	std::queue<std::pair<DWORD, std::wstring>> result_info;
	result_info.push({ FILE_ACTION_RENAMED_NEW_NAME, L"C:\\Users\\ABO\\Desktop\\old;C:\\Users\\ABO\\Desktop\\new" });
	ASSERT_EQ(result_info.size(), change_info.size());

	auto p1 = result_info.front();
	auto p2 = change_info.front();

	ASSERT_EQ(p1, p2);

	_rmdir(new_folder.c_str());
}

TEST(FolderTest, CreateText) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	std::queue<std::pair<DWORD, std::wstring>> change_info;

	my_rest_client::FolderWatcher watcher(&change_info, path);;
	watcher.StartWatching();

	std::string file_name = "C:\\Users\\ABO\\Desktop\\test.txt";
	std::ofstream file{ file_name };
	file.close();	
	
	watcher.StopWatching();

	std::queue<std::pair<DWORD, std::wstring>> result_info;
	result_info.push({ FILE_ACTION_ADDED, L"C:\\Users\\ABO\\Desktop\\test.txt" });
	ASSERT_EQ(result_info.size(), change_info.size());

	auto p1 = result_info.front();
	auto p2 = change_info.front();

	ASSERT_EQ(p1, p2);
	
	std::remove(file_name.c_str());
}