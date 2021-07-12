#include "pch.h"

#include <memory>

#include "../monitor_client/item_dao.h"
#include "../monitor_client/event_consumer.h"
#include "../monitor_client/event_queue.h"

class EventConsumerTest : public ::testing::Test {
public:
	monitor_client::EventConsumer* consumer;

	void SetUp() override {
		consumer = new monitor_client::EventConsumer(std::make_shared<monitor_client::EventQueue>(),
			monitor_client::common_utility::NetworkInfo{ L"localhost", 8080 },
			std::make_unique<monitor_client::ItemDaoDummy>());
	}

	void TearDown() override {
		delete consumer;
	}
};

TEST_F(EventConsumerTest, NoRunAndNoStop) {
}
TEST_F(EventConsumerTest, NullCheck) {
	delete consumer;
	consumer = new monitor_client::EventConsumer(nullptr,
		monitor_client::common_utility::NetworkInfo{ L"localhost", 8080 },
		std::make_unique<monitor_client::ItemDaoDummy>());

	bool result = consumer->Run();
	
	ASSERT_FALSE(result);

	consumer->Stop();

	result = consumer->IsRunning();
	ASSERT_FALSE(result);
}

TEST_F(EventConsumerTest, NoRunAndStop) {
	consumer->Stop();
}

TEST_F(EventConsumerTest, RunAndNoStop) {
	bool result = consumer->Run();

	ASSERT_TRUE(result);
}

TEST_F(EventConsumerTest, DuplicateRun) {
	bool result = consumer->Run();

	ASSERT_TRUE(result);

	result = consumer->Run();
	ASSERT_FALSE(result);
}

TEST_F(EventConsumerTest, DuplicateStop) {
	bool result = consumer->Run();

	ASSERT_TRUE(result);

	consumer->Stop();
	consumer->Stop();
}

TEST_F(EventConsumerTest, Stop) {
	bool result = consumer->Run();

	ASSERT_TRUE(result);

	result = consumer->IsRunning();
	ASSERT_TRUE(result);

	consumer->Stop();
	result = consumer->IsRunning();
	ASSERT_FALSE(result);
}

TEST_F(EventConsumerTest, ReRunning) {
	bool result = consumer->Run();

	ASSERT_TRUE(result);

	consumer->Stop();

	result = consumer->Run();

	ASSERT_TRUE(result);
}