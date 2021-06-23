#include "pch.h"

#include "../rest_client/file_manager.h"
#include "../rest_client/notify_queue.h"

TEST(FileTest, NullCheck) {
	my_rest_client::NotifyQueue notify_queue;
	my_rest_client::FileManager manager(nullptr);

	bool result = manager.Run();
	
	ASSERT_FALSE(result);

	manager.Stop();

	result = manager.IsRunning();
	ASSERT_FALSE(result);
}

TEST(FileTest, NoRunAndNoStop) {
	my_rest_client::NotifyQueue notify_queue;
	my_rest_client::FileManager manager(&notify_queue);
}

TEST(FileTest, NoRunAndStop) {
	my_rest_client::NotifyQueue notify_queue;
	my_rest_client::FileManager manager(&notify_queue);

	manager.Stop();
}

TEST(FileTest, RunAndNoStop) {
	my_rest_client::NotifyQueue notify_queue;
	my_rest_client::FileManager manager(&notify_queue);

	bool result = manager.Run();

	ASSERT_TRUE(result);
}

TEST(FileTest, DuplicateRun) {
	my_rest_client::NotifyQueue notify_queue;
	my_rest_client::FileManager manager(&notify_queue);

	bool result = manager.Run();

	ASSERT_TRUE(result);

	result = manager.Run();
	ASSERT_FALSE(result);
}

TEST(FileTest, DuplicateStop) {
	my_rest_client::NotifyQueue notify_queue;
	my_rest_client::FileManager manager(&notify_queue);

	bool result = manager.Run();

	ASSERT_TRUE(result);

	manager.Stop();
	manager.Stop();
}

TEST(FileTest, Stop) {
	my_rest_client::NotifyQueue notify_queue;
	my_rest_client::FileManager manager(&notify_queue);

	bool result = manager.Run();

	ASSERT_TRUE(result);

	result = manager.IsRunning();
	ASSERT_TRUE(result);

	manager.Stop();
	result = manager.IsRunning();
	ASSERT_FALSE(result);
}

TEST(FileTest, ReRunning) {
	my_rest_client::NotifyQueue notify_queue;
	my_rest_client::FileManager manager(&notify_queue);

	bool result = manager.Run();

	ASSERT_TRUE(result);

	manager.Stop();

	result = manager.Run();

	ASSERT_TRUE(result);
}