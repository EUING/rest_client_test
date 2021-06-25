#include "pch.h"

#include <optional>
#include <vector>

#include "../monitor_client/common_utility.h"
#include "../monitor_client/file_http.h"

TEST(HttpTest, GetTotalFile) {
	monitor_client::FileHttp file_http(L"ggulmo.iptime.org", 56380);

	std::vector<monitor_client::common_utility::FileInfo> current_info_list;
	current_info_list.push_back({ L"비트맵.bmp", 2239542, L"2021-06-25 12:35:20", L"2021-06-25 12:35:34" });
	current_info_list.push_back({ L"새 텍스트 문서.txt", 9, L"2021-06-25 12:35:16", L"2021-06-25 12:35:53" });

	std::optional<std::vector<monitor_client::common_utility::FileInfo>> result = file_http.GetTotalFile();

	ASSERT_TRUE(result.has_value());
	
	auto file_info_list = result.value();
	for (int i = 0; i < current_info_list.size(); ++i) {
		ASSERT_EQ(file_info_list[i].name, current_info_list[i].name);
		ASSERT_EQ(file_info_list[i].size, current_info_list[i].size);
		ASSERT_EQ(file_info_list[i].creation_time, current_info_list[i].creation_time);
		ASSERT_EQ(file_info_list[i].last_modified_time, current_info_list[i].last_modified_time);
	}
}

TEST(HttpTest, GetFile) {
	monitor_client::FileHttp file_http(L"ggulmo.iptime.org", 56380);

	monitor_client::common_utility::FileInfo current_file_info;
	current_file_info.name = L"비트맵.bmp";
	current_file_info.size = 2239542;
	current_file_info.creation_time = L"2021-06-25 12:35:20";
	current_file_info.last_modified_time = L"2021-06-25 12:35:34";

	std::optional<monitor_client::common_utility::FileInfo> result = file_http.GetFile(current_file_info.name);

	ASSERT_TRUE(result.has_value());

	auto file_info = result.value();
	ASSERT_EQ(file_info.name, current_file_info.name);
	ASSERT_EQ(file_info.size, current_file_info.size);
	ASSERT_EQ(file_info.creation_time, current_file_info.creation_time);
	ASSERT_EQ(file_info.last_modified_time, current_file_info.last_modified_time);
}

TEST(HttpTest, AddFile) {
	monitor_client::FileHttp file_http(L"ggulmo.iptime.org", 56380);

	monitor_client::common_utility::FileInfo file_info;
	file_info.name = L"새 텍스트 문서.txt";
	file_info.size = 10;
	file_info.creation_time = L"2021-06-22T22:07:27";
	file_info.last_modified_time = L"2021-06-22T22:07:27";

	bool result = file_http.AddFile(file_info);
	ASSERT_TRUE(result);
}

TEST(HttpTest, ModifyFile) {
	monitor_client::FileHttp file_http(L"ggulmo.iptime.org", 56380);

	monitor_client::common_utility::FileInfo file_info;
	file_info.name = L"새 텍스트 문서.txt";
	file_info.size = 10;
	file_info.creation_time = L"2021-06-22T22:07:27";
	file_info.last_modified_time = L"2021-06-22T22:07:27";

	bool result = file_http.ModifyFile(file_info);
	ASSERT_TRUE(result);
}

TEST(HttpTest, RemovedFile) {
	monitor_client::FileHttp file_http(L"ggulmo.iptime.org", 56380);

	std::wstring file_name = L"새 텍스트 문서.txt";

	bool result = file_http.RemoveFile(file_name);
	ASSERT_TRUE(result);
}

TEST(HttpTest, RenamedFile) {
	monitor_client::FileHttp file_http(L"ggulmo.iptime.org", 56380);

	monitor_client::common_utility::ChangeNameInfo info;
	info.old_name = L"새 텍스트 문서.txt";
	info.new_name = L"새 텍스트 문서2.txt";

	bool result = file_http.RenameFile(info);
	ASSERT_TRUE(result);
}