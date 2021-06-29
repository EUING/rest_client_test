#include "pch.h"

#include <optional>
#include <vector>

#include "../monitor_client/common_utility.h"
#include "../monitor_client/item_http.h"

TEST(HttpTest, RenamedFile) {
	monitor_client::ItemHttp item_http(L"ggulmo.iptime.org", 56380);

	monitor_client::common_utility::ChangeNameInfo info;
	info.old_name = L"새 텍스트 문서.txt";
	info.new_name = L"새 텍스트 문서2.txt";

	bool result = item_http.RenameItem(info);
	ASSERT_TRUE(result);
}

TEST(HttpTest, AddFile) {
	monitor_client::ItemHttp item_http(L"ggulmo.iptime.org", 56380);

	monitor_client::common_utility::FileInfo file_info;
	file_info.name = L"새 텍스트 문서.txt";
	file_info.size = 10;
	file_info.creation_time = L"2021-06-22T22:07:27";
	file_info.last_modified_time = L"2021-06-22T22:07:27";

	bool result = item_http.AddFile(file_info);
	ASSERT_TRUE(result);
}

TEST(HttpTest, ModifyFile) {
	monitor_client::ItemHttp item_http(L"ggulmo.iptime.org", 56380);

	monitor_client::common_utility::FileInfo file_info;
	file_info.name = L"새 텍스트 문서.txt";
	file_info.size = 10;
	file_info.creation_time = L"2021-06-22T22:07:27";
	file_info.last_modified_time = L"2021-06-22T22:07:27";

	bool result = item_http.ModifyFile(file_info);
	ASSERT_TRUE(result);
}

TEST(HttpTest, AddFolder) {
	monitor_client::ItemHttp item_http(L"ggulmo.iptime.org", 56380);

	monitor_client::common_utility::FolderInfo folder_info;
	folder_info.name = L"new folder";
	folder_info.creation_time = L"2021-06-29 11:30:00";

	bool result = item_http.AddFolder(folder_info);
	ASSERT_TRUE(result);
}