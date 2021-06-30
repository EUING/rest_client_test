#include "pch.h"

#include <optional>

#include "../monitor_client/common_utility.h"
#include "../monitor_client/item_dao.h"

monitor_client::ItemDao item_dao;
int parent_id = -1;

class TestEnvironment : public ::testing::Environment {
	public:
	void SetUp() override {
		item_dao.OpenDatabase(L"C:\\Users\\ABO\\Desktop\\새 폴더\\test_items.db");
		monitor_client::common_utility::FileInfo info;
		info.name = L"새 텍스트 파일.txt";
		info.size = 10;

		item_dao.InsertFileInfo(info, 0);

		info.name = L"새 폴더";
		info.size = -1;
		item_dao.InsertFileInfo(info, 0);

		std::optional<int> result = item_dao.GetItemId(info.name, 0);
		if (result.has_value()) {
			parent_id = result.value();
		}

		info.name = L"test.jpg";
		info.size = 5000;
		item_dao.InsertFileInfo(info, parent_id);

		info.name = L"새 텍스트 파일.txt";
		info.size = 100;
		item_dao.InsertFileInfo(info, parent_id);

		info.name = L"새 폴더";
		info.size = -1;
		item_dao.InsertFileInfo(info, parent_id);
	}

	void TearDown() override {
		item_dao.DeleteItemInfo(L"새 텍스트 파일.txt", 0);
		item_dao.DeleteItemInfo(L"새 폴더", 0);
		item_dao.DeleteItemInfo(L"test.jpg", parent_id);
		item_dao.DeleteItemInfo(L"새 텍스트 파일.txt", parent_id);
		item_dao.DeleteItemInfo(L"새 폴더", parent_id);
	}
};

TEST(ItemDao, OpenMemory) {
	monitor_client::ItemDao item_dao;
	bool result = item_dao.OpenDatabase(L":memory:");
	ASSERT_TRUE(result);
}

TEST(ItemDao, GetFileInfo) {
	std::optional<monitor_client::common_utility::FileInfo> result = item_dao.GetFileInfo(L"새 텍스트 파일.txt", -1);
	ASSERT_FALSE(result.has_value());

	result = item_dao.GetFileInfo(L"새 텍스트 파일.txt", 0);
	ASSERT_TRUE(result.has_value());

	auto info = result.value();
	ASSERT_EQ(info.name, L"새 텍스트 파일.txt");
	ASSERT_EQ(info.size, 10);

	result = item_dao.GetFileInfo(L"새 텍스트 파일.txt", parent_id);
	ASSERT_TRUE(result.has_value());

	info = result.value();
	ASSERT_EQ(info.name, L"새 텍스트 파일.txt");
	ASSERT_EQ(info.size, 100);
}

int main(int argc, char* argv[]) {
	::testing::InitGoogleTest(&argc, argv);

	::testing::AddGlobalTestEnvironment(new TestEnvironment);

	return RUN_ALL_TESTS();
}