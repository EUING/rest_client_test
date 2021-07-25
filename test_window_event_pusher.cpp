#include "pch.h"

#include <Windows.h>
#include <stdint.h>

#include <optional>
#include <memory>
#include <string>

#include "../monitor_client/event_filter_dummy.h"
#include "../monitor_client/window_event_pusher.h"
#include "../monitor_client/common_utility.h"
#include "../monitor_client/common_struct.h"
#include "../monitor_client/base_event.h"
#include "../monitor_client/upload_event.h"
#include "../monitor_client/remove_event.h"
#include "../monitor_client/rename_event.h"

class WindowEventPusherTest : public ::testing::Test {
public:
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
	}

	int MakeNotify(uint8_t* buffer, const monitor_client::common_utility::ChangeItemInfo& change_item_info, bool is_last) {
		FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&buffer[0]);

		fni->Action = change_item_info.action;
		fni->FileNameLength = change_item_info.relative_path.size() * sizeof(wchar_t);
		memcpy(fni->FileName, change_item_info.relative_path.c_str(), change_item_info.relative_path.size() * sizeof(wchar_t));
		int align = fni->FileNameLength / 4 * 4;
		fni->NextEntryOffset = is_last ? 0 : sizeof(FILE_NOTIFY_INFORMATION) + align;;

		return fni->NextEntryOffset;
	}
};

TEST_F(WindowEventPusherTest, PushEvent) {
	std::wstring target_folder = L"C:\\Users\\ABO\\Desktop\\1";
	ASSERT_TRUE(SetCurrentDirectory(target_folder.c_str()));

	std::shared_ptr<uint8_t[]> buffer(new uint8_t[1024 * 1024], std::default_delete<uint8_t[]>());
	memset(buffer.get(), 0, 1024 * 1024);
	uint8_t* pos = buffer.get();

	monitor_client::common_utility::ChangeItemInfo info;
	info.action = FILE_ACTION_ADDED;
	info.relative_path = L"1_1/1_1_1/1_1_1_1.txt";
	int next = MakeNotify(pos, info, false);
	pos += next;

	info.action = FILE_ACTION_ADDED;
	info.relative_path = L"1_2/1_2_1/1_2_1_1.txt";
	next = MakeNotify(pos, info, false);
	pos += next;

	info.action = FILE_ACTION_RENAMED_OLD_NAME;
	info.relative_path = L"1_3/1_3_1/1_3_1_1.txt";
	next = MakeNotify(pos, info, false);
	pos += next;

	info.action = FILE_ACTION_RENAMED_NEW_NAME;
	info.relative_path = L"1_3/1_3_1/1_3_1_1.jpg";
	next = MakeNotify(pos, info, false);
	pos += next;

	info.action = FILE_ACTION_MODIFIED;
	info.relative_path = L"1_4/1_4_1/1_4_1_1.txt";
	next = MakeNotify(pos, info, false);
	pos += next;

	info.action = FILE_ACTION_REMOVED;
	info.relative_path = L"1_1/1_1_1/1_1_1_1.txt";
	MakeNotify(pos, info, true);
	
	monitor_client::WindowEventPusher event_pusher(buffer);
	std::shared_ptr<monitor_client::EventFilterDummy> event_filter_dummy = std::make_shared<monitor_client::EventFilterDummy>();
	event_pusher.PushEvent(event_filter_dummy, event_queue);

	monitor_client::BaseEvent* event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::UploadEvent*>(event), nullptr);
	event_queue->Pop();

	event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::UploadEvent*>(event), nullptr);
	event_queue->Pop();

	event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::RenameEvent*>(event), nullptr);
	event_queue->Pop();

	event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::UploadEvent*>(event), nullptr);
	event_queue->Pop();

	event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::RemoveEvent*>(event), nullptr);
	event_queue->Pop();
}