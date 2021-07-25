#include "pch.h"

#include <Windows.h>
#include <stdint.h>

#include <optional>
#include <memory>
#include <string>
#include <future>

#include "../monitor_client/event_queue.h"
#include "../monitor_client/common_utility.h"
#include "../monitor_client/common_struct.h"
#include "../monitor_client/item_dao.h"
#include "../monitor_client/common_struct.h"
#include "../monitor_client/base_event.h"
#include "../monitor_client/upload_event.h"
#include "../monitor_client/download_event.h"
#include "../monitor_client/conflict_event.h"
#include "../monitor_client/remove_event.h"
#include "../monitor_client/local_remove_event.h"
#include "../monitor_client/rename_event.h"

class EventQueueTest : public ::testing::Test {
public:
	monitor_client::ItemRequest* item_request;
	PROCESS_INFORMATION process_info;

	void SetUp() override {
		monitor_client::common_utility::NetworkInfo network_info{ L"localhost", 8080 };
		std::shared_ptr<monitor_client::ItemHttp> item_http = std::make_shared<monitor_client::ItemHttp>(network_info);
		std::shared_ptr<monitor_client::LocalDb> local_db = std::make_shared<monitor_client::LocalDb>(std::make_unique<monitor_client::ItemDaoDummy>());
		
		item_request = new monitor_client::ItemRequest(item_http, nullptr, local_db);

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
		delete item_request;
	}
};

TEST_F(EventQueueTest, Push) {
	monitor_client::common_utility::ItemInfo info;
	info.name = L"1_1/1_1_1/1_2_1_1.txt";
	info.size = 123;
	info.hash = L"hash";

	monitor_client::EventQueue queue;
	queue.Push(std::make_unique<monitor_client::UploadEvent>(info));

	ASSERT_NE(nullptr, queue.Front());
	queue.Pop();

	std::unique_ptr<monitor_client::BaseEvent> event = std::make_unique<monitor_client::RemoveEvent>(L"test");
	queue.Push(std::move(event));

	ASSERT_NE(nullptr, queue.Front());
	queue.Pop();
}

TEST_F(EventQueueTest, Break) {
	monitor_client::common_utility::ItemInfo info;
	info.name = L"1_1/1_1_1/1_2_1_1.txt";
	info.size = 123;
	info.hash = L"hash";

	monitor_client::EventQueue queue;
	queue.Push(std::make_unique<monitor_client::UploadEvent>(info));

	ASSERT_NE(nullptr, queue.Front());
	queue.Pop();

	std::unique_ptr<monitor_client::BaseEvent> event = std::make_unique<monitor_client::RemoveEvent>(L"test");
	queue.Push(std::move(event));

	ASSERT_NE(nullptr, queue.Front());
	queue.Pop();

	bool is_empty = false;
	std::future<void> result = std::async(std::launch::async, [&is_empty, &queue]() {
		if (queue.Front()) {
			is_empty = true;
		}
		else {
			is_empty = false;
		}
	});

	queue.Break();
	result.wait();

	ASSERT_FALSE(is_empty);
}

TEST_F(EventQueueTest, Execute) {
	monitor_client::common_utility::ItemInfo info;
	info.name = L"1_1/1_1_1/1_2_1_1.txt";
	info.size = 123;
	info.hash = L"hash";

	monitor_client::EventQueue queue;
	queue.Push(std::make_unique<monitor_client::UploadEvent>(info));

	ASSERT_NE(nullptr, queue.Front());
	ASSERT_TRUE(queue.Front()->Execute(*item_request));
	queue.Pop();

	std::unique_ptr<monitor_client::BaseEvent> event = std::make_unique<monitor_client::RemoveEvent>(L"test");
	queue.Push(std::move(event));

	ASSERT_NE(nullptr, queue.Front());
	ASSERT_TRUE(queue.Front()->Execute(*item_request));
	queue.Pop();
}