#include "pch.h"

#include <windows.h>

#include <chrono>
#include <thread>
#include <iostream>
#include <future>
#include <optional>
#include <string>

#include "../rest_client/my_client.h"
#include "../rest_client/folder_watcher.h"

TEST(PingTest, ReturnPong) {
  bool result = my_rest_client::Ping().has_value();
  ASSERT_TRUE(result);

  std::wstring message = my_rest_client::Ping().value();
  EXPECT_EQ(message, L"pong");
}

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