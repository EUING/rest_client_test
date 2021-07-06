#include "pch.h"

#include <memory>

#include "../monitor_client/item_dao.h"
#include "../monitor_client/item_manager.h"
#include "../monitor_client/notify_queue.h"

class FileTest : public ::testing::Test {
public:
	monitor_client::ItemManager* manager;

	void SetUp() override {
		manager = new monitor_client::ItemManager(std::make_shared<monitor_client::NotifyQueue>(),
			monitor_client::common_utility::NetworkInfo{ L"localhost", 8080 },
			std::make_unique<monitor_client::ItemDaoDummy>());
	}

	void TearDown() override {
		delete manager;
	}
};

TEST_F(FileTest, NoRunAndNoStop) {
}
TEST_F(FileTest, NullCheck) {
	delete manager;
	manager = new monitor_client::ItemManager(nullptr,
		monitor_client::common_utility::NetworkInfo{ L"localhost", 8080 },
		std::make_unique<monitor_client::ItemDaoDummy>());

	bool result = manager->Run();
	
	ASSERT_FALSE(result);

	manager->Stop();

	result = manager->IsRunning();
	ASSERT_FALSE(result);
}

TEST_F(FileTest, NoRunAndStop) {
	manager->Stop();
}

TEST_F(FileTest, RunAndNoStop) {
	bool result = manager->Run();

	ASSERT_TRUE(result);
}

TEST_F(FileTest, DuplicateRun) {
	bool result = manager->Run();

	ASSERT_TRUE(result);

	result = manager->Run();
	ASSERT_FALSE(result);
}

TEST_F(FileTest, DuplicateStop) {
	bool result = manager->Run();

	ASSERT_TRUE(result);

	manager->Stop();
	manager->Stop();
}

TEST_F(FileTest, Stop) {
	bool result = manager->Run();

	ASSERT_TRUE(result);

	result = manager->IsRunning();
	ASSERT_TRUE(result);

	manager->Stop();
	result = manager->IsRunning();
	ASSERT_FALSE(result);
}

TEST_F(FileTest, ReRunning) {
	bool result = manager->Run();

	ASSERT_TRUE(result);

	manager->Stop();

	result = manager->Run();

	ASSERT_TRUE(result);
}