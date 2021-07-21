#include "pch.h"

#include <fstream>

#include "../monitor_client/item_s3.h"

class S3Test : public ::testing::Test {
public:
	monitor_client::ItemS3* item_s3;

	void SetUp() override {
		std::wstring file_path = L"C:\\Users\\ABO\\Desktop\\1";
		_wmkdir(file_path.c_str());

		monitor_client::common_utility::S3Info s3_info;
		s3_info.id = L"chlguddnjs";
		s3_info.password = L"Rladmdgur!";
		s3_info.bucket = L"test-bucket";

		monitor_client::common_utility::NetworkInfo network_info;
		network_info.host = L"ggulmo.iptime.org";
		network_info.port = 56390;

		item_s3 = new monitor_client::ItemS3(network_info, s3_info);	
	}

	void TearDown() override {
		delete item_s3;
		_wsystem(L"rmdir /s /q C:\\Users\\ABO\\Desktop\\1");
	}
};

TEST_F(S3Test, GetItem) {
	std::wstring target_folder = L"C:\\Users\\ABO\\Desktop\\1";
	ASSERT_TRUE(SetCurrentDirectory(target_folder.c_str()));
	monitor_client::common_utility::ItemInfo info;
	info.name = L"test.txt";
	info.hash = L"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
	info.size = 0;

	ASSERT_TRUE(item_s3->GetItem(info));
	target_folder.append(L"\\").append(info.name);
	std::ifstream is(target_folder, std::ifstream::binary);
	ASSERT_TRUE(is);
}

TEST_F(S3Test, PutItem) {
	std::wstring target_folder = L"C:\\Users\\ABO\\Desktop\\1";
	ASSERT_TRUE(SetCurrentDirectory(target_folder.c_str()));

	std::wstring file_name = L"C:\\Users\\ABO\\Desktop\\1\\temp.txt";
	std::ofstream temp{ file_name };
	temp << "Hello World!";
	temp.close();

	monitor_client::common_utility::ItemInfo info;
	info.name = L"temp.txt";
	info.hash = L"hello";
	info.size = 0;

	ASSERT_TRUE(item_s3->PutItem(info));
}