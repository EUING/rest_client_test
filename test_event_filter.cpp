#include "pch.h"

#include <memory>

#include "../monitor_client/common_struct.h"
#include "../monitor_client/local_db.h"
#include "../monitor_client/item_dao_sqlite.h"
#include "../monitor_client/event_filter.h"
#include "../monitor_client/event_queue.h"
#include "../monitor_client/upload_event.h"
#include "../monitor_client/rename_event.h"

class EventFilterTest : public ::testing::Test {
public:
	monitor_client::EventFilter* filter;
	std::shared_ptr<monitor_client::EventQueue> event_queue;
		
	void SetUp() override {
		event_queue = std::make_shared<monitor_client::EventQueue>();
		std::unique_ptr<monitor_client::ItemDaoSqlite> item_dao = std::make_unique<monitor_client::ItemDaoSqlite>();
		item_dao->OpenDatabase(L":memory:");

		monitor_client::common_utility::ItemInfo item_info;
		item_info.name = L"새 폴더 (2)";
		item_info.size = -1;
		item_info.hash.clear();
		item_dao->UpdateItemInfo(item_info, 0);

		item_info.name = L"test.txt";
		item_info.size = 0;
		item_info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";

		item_dao->UpdateItemInfo(item_info, 1);
		filter = new monitor_client::EventFilter(std::move(item_dao));

		std::wstring file_path = L"C:\\Users\\ABO\\Desktop\\새 폴더 (2)";
		_wmkdir(file_path.c_str());

		std::wstring file_name = L"C:\\Users\\ABO\\Desktop\\새 폴더 (2)\\test.txt";
		std::ofstream temp{ file_name };
		temp.close();
	}

	void TearDown() override {
		delete filter;

		_wsystem(L"rmdir /s /q C:\\Users\\ABO\\Desktop\\새 폴더 (2)");
	}
};

TEST_F(EventFilterTest, UploadFilter) {
	std::wstring target_folder = L"C:\\Users\\ABO\\Desktop";
	ASSERT_TRUE(SetCurrentDirectory(target_folder.c_str()));

	monitor_client::common_utility::ItemInfo item_info;
	item_info.name = L"새 폴더 (2)/test.txt";
	item_info.size = 0;
	item_info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";

	filter->UploadFilter(event_queue, item_info);
	
	item_info.name = L".ignore/test.txt";
	item_info.size = 0;
	item_info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";

	filter->UploadFilter(event_queue, item_info);

	item_info.name = L".test.txt.conflict";
	item_info.size = 0;
	item_info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";

	filter->UploadFilter(event_queue, item_info);

	item_info.name = L"새 폴더 (2)/new.txt";
	item_info.size = 0;
	item_info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";

	filter->UploadFilter(event_queue, item_info);

	ASSERT_EQ(event_queue->Size(), 1);
}

TEST_F(EventFilterTest, RenameFilter) {
	std::wstring target_folder = L"C:\\Users\\ABO\\Desktop";
	ASSERT_TRUE(SetCurrentDirectory(target_folder.c_str()));

	monitor_client::common_utility::ChangeNameInfo change_name_info;
	change_name_info.old_name = L"새 폴더 (2)/test.txt";
	change_name_info.new_name = L"새 폴더 (2)/test.txt.conflict";

	filter->RenameFilter(event_queue, change_name_info);

	change_name_info.old_name = L"새 폴더 (2)/test.txt.conflict";
	change_name_info.new_name = L"새 폴더 (2)/new.txt.conflict";

	filter->RenameFilter(event_queue, change_name_info);

	change_name_info.old_name = L"새 폴더 (2)/test.txt";
	change_name_info.new_name = L"새 폴더 (2)/test2.txt";

	filter->RenameFilter(event_queue, change_name_info);

	monitor_client::BaseEvent* event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::RenameEvent*>(event), nullptr);
	event_queue->Pop();

	change_name_info.old_name = L"새 폴더 (2)/test.txt.conflict";
	change_name_info.new_name = L"새 폴더 (2)/test.txt";

	filter->RenameFilter(event_queue, change_name_info);
	
	event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::UploadEvent*>(event), nullptr);
	event_queue->Pop();

	ASSERT_EQ(event_queue->Size(), 0);
}