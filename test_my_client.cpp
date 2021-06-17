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

extern bool my_rest_client::g_watching;

TEST(PingTest, ReturnPong) {
  bool result = my_rest_client::Ping().has_value();
  ASSERT_TRUE(result);

  std::wstring message = my_rest_client::Ping().value();
  EXPECT_EQ(message, L"pong");
}

TEST(FolderTest, WrongPath) {
	my_rest_client::g_watching = true;
	std::wstring wrong_path = L"WRONG PATH";

	std::future<std::optional<bool>> fuction_fail = std::async(std::launch::async, my_rest_client::WatchDirectory, std::cref(wrong_path));
	auto result = fuction_fail.get();

	ASSERT_FALSE(result.has_value());
}

TEST(FolderTest, NoChange) {
	my_rest_client::g_watching = true;
	std::wstring path = L"C:\\Users\\ABO\\Desktop";
	std::future<std::optional<bool>> no_change = std::async(std::launch::async, my_rest_client::WatchDirectory, std::cref(path));

	my_rest_client::g_watching = false;

	auto result = no_change.get();
	ASSERT_TRUE(result.has_value());
	ASSERT_FALSE(result.value());
}

TEST(FolderTest, NewFolder) {
	my_rest_client::g_watching = true;
	std::wstring path = L"C:\\Users\\ABO\\Desktop";

	std::future<std::optional<bool>> new_folder = std::async(std::launch::async, my_rest_client::WatchDirectory, std::cref(path));

	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1s);

	std::wstring new_folder_path = path + L"\\test";
	_wmkdir(new_folder_path.c_str());
	
	my_rest_client::g_watching = false;

	auto result = new_folder.get();
	RemoveDirectory(new_folder_path.c_str());

	ASSERT_TRUE(result.has_value());
	ASSERT_TRUE(result.value());	
}

TEST(FolderTest, NewFile) {
	my_rest_client::g_watching = true;
	std::wstring path = L"C:\\Users\\ABO\\Desktop";

	std::future<std::optional<bool>> new_file = std::async(std::launch::async, my_rest_client::WatchDirectory, std::cref(path));

	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1s);

	std::wstring new_file_path = path + L"\\test.txt";
	std::ofstream file(new_file_path);
	file << "test";
	file.close();

	my_rest_client::g_watching = false;

	auto result = new_file.get();
	DeleteFile(new_file_path.c_str());

	ASSERT_TRUE(result.has_value());
	ASSERT_TRUE(result.value());	
}