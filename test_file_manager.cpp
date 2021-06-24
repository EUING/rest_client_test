#include "pch.h"

#include "../rest_client/file_manager.h"
#include "../rest_client/notify_queue.h"

TEST(FileTest, NullCheck) {
	monitor_client::NotifyQueue notify_queue;
	monitor_client::FileManager manager(nullptr);

	bool result = manager.Run();
	
	ASSERT_FALSE(result);

	manager.Stop();

	result = manager.IsRunning();
	ASSERT_FALSE(result);
}

TEST(FileTest, NoRunAndNoStop) {
	monitor_client::NotifyQueue notify_queue;
	monitor_client::FileManager manager(&notify_queue);
}

TEST(FileTest, NoRunAndStop) {
	monitor_client::NotifyQueue notify_queue;
	monitor_client::FileManager manager(&notify_queue);

	manager.Stop();
}

TEST(FileTest, RunAndNoStop) {
	monitor_client::NotifyQueue notify_queue;
	monitor_client::FileManager manager(&notify_queue);

	bool result = manager.Run();

	ASSERT_TRUE(result);
}

TEST(FileTest, DuplicateRun) {
	monitor_client::NotifyQueue notify_queue;
	monitor_client::FileManager manager(&notify_queue);

	bool result = manager.Run();

	ASSERT_TRUE(result);

	result = manager.Run();
	ASSERT_FALSE(result);
}

TEST(FileTest, DuplicateStop) {
	monitor_client::NotifyQueue notify_queue;
	monitor_client::FileManager manager(&notify_queue);

	bool result = manager.Run();

	ASSERT_TRUE(result);

	manager.Stop();
	manager.Stop();
}

TEST(FileTest, Stop) {
	monitor_client::NotifyQueue notify_queue;
	monitor_client::FileManager manager(&notify_queue);

	bool result = manager.Run();

	ASSERT_TRUE(result);

	result = manager.IsRunning();
	ASSERT_TRUE(result);

	manager.Stop();
	result = manager.IsRunning();
	ASSERT_FALSE(result);
}

TEST(FileTest, ReRunning) {
	monitor_client::NotifyQueue notify_queue;
	monitor_client::FileManager manager(&notify_queue);

	bool result = manager.Run();

	ASSERT_TRUE(result);

	manager.Stop();

	result = manager.Run();

	ASSERT_TRUE(result);
}