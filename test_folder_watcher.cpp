#include "pch.h"

#include <Windows.h>
#include <direct.h>

#include <fstream>
#include <iostream>
#include <string>

#include "../monitor_client/event_filter_dummy.h"
#include "../monitor_client/folder_watcher.h"
#include "../monitor_client/event_queue.h"
#include "../monitor_client/upload_event.h"
#include "../monitor_client/remove_event.h"
#include "../monitor_client/rename_event.h"

const wchar_t* const kTestPath = L"C:\\Users\\ABO\\Desktop";
class FolderTest : public ::testing::Test {
public:
	monitor_client::FolderWatcher* watcher;
	monitor_client::EventProducer* event_producer;
	std::shared_ptr<monitor_client::EventQueue> event_queue;
	std::shared_ptr<monitor_client::EventFilterDummy> event_filter_dummy;

	void SetUp() override {
		event_queue = std::make_shared<monitor_client::EventQueue>();
		event_filter_dummy = std::make_shared<monitor_client::EventFilterDummy>();

		event_producer = new monitor_client::EventProducer(event_filter_dummy, event_queue);
		watcher = new monitor_client::FolderWatcher(*event_producer, kTestPath);
	}

	void TearDown() override {
		delete watcher;
		delete event_producer;
	}
};

TEST_F(FolderTest, WrongPath) {
	std::wstring wrong_path = L"WRONG PATH";
	watcher->SetWatchFolder(wrong_path);

	bool result = watcher->StartWatching();		

	ASSERT_FALSE(result);
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

	ASSERT_NE(event_queue->Front(), nullptr);
	monitor_client::BaseEvent* event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::UploadEvent*>(event), nullptr);
	event_queue->Pop();
}

TEST_F(FolderTest, DeleteFolder) {
	std::wstring new_folder = L"C:\\Users\\ABO\\Desktop\\test";
	_wmkdir(new_folder.c_str());

	watcher->StartWatching();

	RemoveDirectory(new_folder.c_str());

	watcher->StopWatching();

	ASSERT_NE(event_queue->Front(), nullptr);
	monitor_client::BaseEvent* event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::RemoveEvent*>(event), nullptr);
	event_queue->Pop();
}

TEST_F(FolderTest, ChangeName) {
	std::wstring old_folder = L"C:\\Users\\ABO\\Desktop\\old";
	_wmkdir(old_folder.c_str());

	watcher->StartWatching();

	std::wstring new_folder = L"C:\\Users\\ABO\\Desktop\\new";
	_wrename(old_folder.c_str(), new_folder.c_str());

	watcher->StopWatching();

	ASSERT_NE(event_queue->Front(), nullptr);
	monitor_client::BaseEvent* event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::RenameEvent*>(event), nullptr);
	event_queue->Pop();

	_wrmdir(new_folder.c_str());
}

TEST_F(FolderTest, CreateText) {
	watcher->StartWatching();

	std::wstring file_name = L"C:\\Users\\ABO\\Desktop\\test.txt";
	std::ofstream file{ file_name };
	file.close();
	
	watcher->StopWatching();

	ASSERT_NE(event_queue->Front(), nullptr);
	monitor_client::BaseEvent* event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::UploadEvent*>(event), nullptr);
	event_queue->Pop();
	
	_wremove(file_name.c_str());
}

TEST_F(FolderTest, ModifyText) {
	std::wstring file_name = L"C:\\Users\\ABO\\Desktop\\test.txt";
	std::ofstream temp{ file_name };
	temp.close();

	watcher->StartWatching();

	std::ofstream file{ file_name };
	file.close();

	ASSERT_NE(event_queue->Front(), nullptr);
	monitor_client::BaseEvent* event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::UploadEvent*>(event), nullptr);
	event_queue->Pop();

	_wremove(file_name.c_str());
}