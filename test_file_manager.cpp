#include "pch.h"

#include "../monitor_client/file_manager.h"
#include "../monitor_client/notify_queue.h"

TEST(FileTest, NullCheck) {
	monitor_client::NotifyQueue notify_queue;
	monitor_client::FileManager manager(nullptr, nullptr);

	bool result = manager.Run();
	
	ASSERT_FALSE(result);

	manager.Stop();

	result = manager.IsRunning();
	ASSERT_FALSE(result);
}

TEST(FileTest, NoRunAndNoStop) {
	monitor_client::NotifyQueue notify_queue;
	monitor_client::FileManager manager(&notify_queue, nullptr);
}

TEST(FileTest, NoRunAndStop) {
	monitor_client::NotifyQueue notify_queue;
	monitor_client::FileManager manager(&notify_queue, nullptr);

	manager.Stop();
}

TEST(FileTest, RunAndNoStop) {
	monitor_client::NotifyQueue notify_queue;
	monitor_client::FileManager manager(&notify_queue, nullptr);

	bool result = manager.Run();

	ASSERT_TRUE(result);
}

TEST(FileTest, DuplicateRun) {
	monitor_client::NotifyQueue notify_queue;
	monitor_client::FileManager manager(&notify_queue, nullptr);

	bool result = manager.Run();

	ASSERT_TRUE(result);

	result = manager.Run();
	ASSERT_FALSE(result);
}

TEST(FileTest, DuplicateStop) {
	monitor_client::NotifyQueue notify_queue;
	monitor_client::FileManager manager(&notify_queue, nullptr);

	bool result = manager.Run();

	ASSERT_TRUE(result);

	manager.Stop();
	manager.Stop();
}

TEST(FileTest, Stop) {
	monitor_client::NotifyQueue notify_queue;
	monitor_client::FileManager manager(&notify_queue, nullptr);

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
	monitor_client::FileManager manager(&notify_queue, nullptr);

	bool result = manager.Run();

	ASSERT_TRUE(result);

	manager.Stop();

	result = manager.Run();

	ASSERT_TRUE(result);
}