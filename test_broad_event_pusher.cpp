#include "pch.h"

#include <Windows.h>
#include <stdint.h>

#include <optional>
#include <memory>
#include <string>

#include "../monitor_client/event_filter_dummy.h"
#include "../monitor_client/broadcast_event_pusher.h"
#include "../monitor_client/common_utility.h"
#include "../monitor_client/common_struct.h"
#include "../monitor_client/base_event.h"
#include "../monitor_client/download_event.h"
#include "../monitor_client/local_rename_event.h"
#include "../monitor_client/local_remove_event.h"

class BroadcastEventPusherTest : public ::testing::Test {
public:
	std::shared_ptr<monitor_client::EventQueue> event_queue;
	void SetUp() override {
		event_queue = std::make_shared<monitor_client::EventQueue>();
	}

	void TearDown() override {
	}
};

TEST_F(BroadcastEventPusherTest, PushEvent) {
	std::string json = "{\"event\":\"rename\",\"old_name\":\"old\",\"new_name\":\"new\"}";
	monitor_client::BroadcastEventPusher* event_producer = new monitor_client::BroadcastEventPusher(json);
	std::shared_ptr<monitor_client::EventFilterDummy> event_filter_dummy = std::make_shared<monitor_client::EventFilterDummy>();
	event_producer->PushEvent(event_filter_dummy, event_queue);
	delete event_producer;

	monitor_client::BaseEvent* event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::LocalRenameEvent*>(event), nullptr);
	event_queue->Pop();	

	json = "{\"event\":\"download\",\"name\":\"down\",\"hash\":\"hash\",\"size\":123}";
	event_producer = new monitor_client::BroadcastEventPusher(json);
	event_producer->PushEvent(event_filter_dummy, event_queue);
	delete event_producer;

	event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::DownloadEvent*>(event), nullptr);
	event_queue->Pop();

	json = "{\"event\":\"remove\",\"name\":\"remove\"}";
	event_producer = new monitor_client::BroadcastEventPusher(json);
	event_producer->PushEvent(event_filter_dummy, event_queue);
	delete event_producer;

	event = const_cast<monitor_client::BaseEvent*>(event_queue->Front().get());
	ASSERT_NE(dynamic_cast<monitor_client::LocalRemoveEvent*>(event), nullptr);
	event_queue->Pop();

	json = "{\"event\":\"wrong\",\"name\":\"wrong\"}";
	event_producer = new monitor_client::BroadcastEventPusher(json);
	event_producer->PushEvent(event_filter_dummy, event_queue);
	delete event_producer;

	ASSERT_EQ(event_queue->Size(), 0);
}