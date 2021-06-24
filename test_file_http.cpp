#include "pch.h"

#include "../monitor_client/common_utility.h"
#include "../monitor_client/file_http.h"

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