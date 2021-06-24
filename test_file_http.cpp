#include "pch.h"

#include "../rest_client/common_utility.h"
#include "../rest_client/file_http.h"

TEST(HttpTest, AddFile) {
	monitor_client::FileHttp file_http(L"ggulmo.iptime.org", 56380);

	monitor_client::common_utility::FileInfo file_info;
	file_info.file_name = L"새 텍스트 문서.txt";
	file_info.file_size = L"10";
	file_info.creation_iso_time = L"2021-06-22T22:07:27";
	file_info.last_modified_iso_time = L"2021-06-22T22:07:27";

	bool result = file_http.AddFile(file_info);
	ASSERT_TRUE(result);
}

TEST(HttpTest, ModifyFile) {
	monitor_client::FileHttp file_http(L"ggulmo.iptime.org", 56380);

	monitor_client::common_utility::FileInfo file_info;
	file_info.file_name = L"새 텍스트 문서.txt";
	file_info.file_size = L"10";
	file_info.creation_iso_time = L"2021-06-22T22:07:27";
	file_info.last_modified_iso_time = L"2021-06-22T22:07:27";

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