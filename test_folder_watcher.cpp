#include "pch.h"

#include <windows.h>

#include <iostream>
#include <string>
#include "../rest_client/folder_watcher.h"

TEST(FolderTest, WrongPath) {
	std::wstring wrong_path = L"WRONG PATH";
	my_rest_client::FolderWatcher watcher;
	bool result = watcher.StartWatching(wrong_path);		

	ASSERT_FALSE(result);
}

TEST(FolderTest, NoStartAndNoStop) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";

	my_rest_client::FolderWatcher watcher;
}

TEST(FolderTest, NoStartAndStop) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";

	my_rest_client::FolderWatcher watcher;
	watcher.StopWatching();
}

TEST(FolderTest, StartAndNoStop) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";

	my_rest_client::FolderWatcher watcher;
	bool result = watcher.StartWatching(path);
	ASSERT_TRUE(result);
}

TEST(FolderTest, DuplicateStart) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";

	my_rest_client::FolderWatcher watcher;
	bool result = watcher.StartWatching(path);
	ASSERT_TRUE(result);

	result = watcher.StartWatching(path);
	ASSERT_FALSE(result);
}

TEST(FolderTest, DuplicateStop) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";

	my_rest_client::FolderWatcher watcher;
	bool result = watcher.StartWatching(path);
	ASSERT_TRUE(result);

	watcher.StopWatching();
	watcher.StopWatching();
}

TEST(FolderTest, Running) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";

	my_rest_client::FolderWatcher watcher;
	bool result = watcher.StartWatching(path);
	ASSERT_TRUE(result);

	result = watcher.IsRunning();
	ASSERT_TRUE(result);
}

TEST(FolderTest, Stop) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";

	my_rest_client::FolderWatcher watcher;
	bool result = watcher.StartWatching(path);
	ASSERT_TRUE(result);

	result = watcher.IsRunning();
	ASSERT_TRUE(result);

	watcher.StopWatching();
	result = watcher.IsRunning();
	ASSERT_FALSE(result);
}

TEST(FolderTest, ReRunning) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";

	my_rest_client::FolderWatcher watcher;
	bool result = watcher.StartWatching(path);
	ASSERT_TRUE(result);

	watcher.StopWatching();
	result = watcher.StartWatching(path);
	ASSERT_TRUE(result);

	result = watcher.IsRunning();
	ASSERT_TRUE(result);
}

TEST(FolderTest, GetFolder) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";

	my_rest_client::FolderWatcher watcher;
	bool result = watcher.StartWatching(path);
	ASSERT_TRUE(result);

	std::wstring current_path = watcher.GetWatchFolder();
	ASSERT_EQ(path, current_path);

	watcher.StopWatching();
	current_path = watcher.GetWatchFolder();
	ASSERT_TRUE(current_path.empty());
}

TEST(FolderTest, ChangeFolder) {
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	std::wstring new_path = L"C:\\";

	my_rest_client::FolderWatcher watcher;
	bool result = watcher.StartWatching(path);
	ASSERT_TRUE(result);

	watcher.StopWatching();

	result = watcher.StartWatching(new_path);
	ASSERT_TRUE(result);

	std::wstring current_path = watcher.GetWatchFolder();
	ASSERT_EQ(new_path, current_path);
}