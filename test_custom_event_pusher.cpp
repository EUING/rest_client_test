#include "pch.h"

#include <Windows.h>
#include <stdint.h>

#include <optional>
#include <memory>
#include <string>

#include "../monitor_client/event_filter_dummy.h"
#include "../monitor_client/custom_event_pusher.h"
#include "../monitor_client/event_queue.h"
#include "../monitor_client/common_utility.h"
#include "../monitor_client/common_struct.h"
#include "../monitor_client/item_dao.h"
#include "../monitor_client/base_event.h"
#include "../monitor_client/upload_event.h"
#include "../monitor_client/download_event.h"
#include "../monitor_client/local_remove_event.h"
#include "../monitor_client/conflict_event.h"

class CustomEventPusherTest : public ::testing::Test {
public:
	monitor_client::ItemRequest* item_request;
	std::shared_ptr<monitor_client::EventQueue> event_queue;
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

		event_queue = std::make_shared<monitor_client::EventQueue>();
		monitor_client::common_utility::NetworkInfo network_info{ L"localhost", 8080 };
		item_request = new monitor_client::ItemRequest(network_info, std::make_unique<monitor_client::ItemDaoDummy>());

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
		delete item_request;
	}
};

TEST_F(CustomEventPusherTest, PushEvent) {
	std::wstring target_folder = L"C:\\Users\\ABO\\Desktop\\1";
	ASSERT_TRUE(SetCurrentDirectory(target_folder.c_str()));

	monitor_client::diff_check::ServerDiffList list;

	monitor_client::common_utility::ItemInfo info;
	info.name = L"1_1/1_1_1/1_1_1_1.txt";
	info.size = 100;
	info.hash = L"hash";
	list.upload_request_list.push_back(info);

	info.name = L"1_2/1_2_1/1_2_1_1.txt";
	info.size = 1000;
	info.hash = L"hash";
	list.upload_request_list.push_back(info);

	list.download_request_list.push_back(L"download1");
	list.download_request_list.push_back(L"download2");

	list.remove_list.push_back(L"remove");

	list.conflict_list.push_back(L"1_3/1_3_1/1_3_1_1.txt");
	list.conflict_list.push_back(L"1_4/1_4_1/1_4_1_1.txt");


	monitor_client::CustomEventPusher event_producer(list);
	std::shared_ptr<monitor_client::EventFilterDummy> event_filter_dummy = std::make_shared<monitor_client::EventFilterDummy>();
	event_producer.PushEvent(event_filter_dummy, event_queue);

	monitor_client::BaseEvent* event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::UploadEvent*>(event), nullptr);
	event_queue->Pop();

	event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::UploadEvent*>(event), nullptr);
	event_queue->Pop();

	event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::DownloadEvent*>(event), nullptr);
	event_queue->Pop();

	event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::DownloadEvent*>(event), nullptr);
	event_queue->Pop();

	event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::LocalRemoveEvent*>(event), nullptr);
	event_queue->Pop();

	event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::ConflictEvent*>(event), nullptr);
	event_queue->Pop();

	event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::ConflictEvent*>(event), nullptr);
	event_queue->Pop();
}