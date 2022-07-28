#include <asio.hpp>
#include <iostream>
#include <fstream>
#include "file_server.h"

file_sesson::file_sesson(asio::ip::tcp::socket socket)
	:socket_(std::move(socket))
{

}
void  file_sesson::start()
{
	open_path_();
}

void file_sesson::open_path_()
{

	FILE* fp = fopen("down.json", "rb");
	char readBuffer[4996];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));//��fp����ı��������readBuffer���ֽ���������readBuffer��
	Down_path.deserializeFromJSON((const char*)readBuffer);

	fclose(fp);
	
	str_ = Down_path.path;
	do_recive_filename();
}


void file_sesson::do_recive_filename()//��������
{
	socket_.async_read_some(asio::buffer(filename_length, 16384),//һ���Խ���
		[this](std::error_code ec, std::size_t sz)
		{
			if (!ec)
			{
				std::size_t filelen = 0;//���ճ���  ���ͳ��ȣ����ĳ���
				std::memcpy(&filelen, filename_length, sizeof(size_t));//��file_len�������յĳ���

				std::string file_content(filename_length + sizeof(size_t));//���Ⱥ�����ļ�����

				string PAth_ = str_ + "\\" + file_content;
				FILE* fp = fopen(PAth_.c_str(), "rb");
				if (fp == nullptr)
				{
					std::cout << "û��" << PAth_ << "����ļ�\n";
					return;
				}
				do_send_filename(PAth_);



			}
		});
}


void file_sesson::do_send_filename(string& fname)//��������
{

	
//	fread(&refile[0], 1, flens, fp);
	count_file_buf.clear();

	ifstream file(fname.c_str());
	if (!file.is_open())
		return;

	file.seekg(0, ios_base::end);
	size_t flens = file.tellg();//list.json�ı��Ĵ�С
	file.close();

	ifstream File(fname.c_str());
	if (!File.is_open())
		return;

	while (File.get(file_buf))
	{

		count_file_buf.push_back(file_buf);
	}
	File.close();

	size_t file_len = flens;

	refile.resize(sizeof(size_t) + file_len);
	std::memcpy(refile.data(), &file_len, sizeof(size_t));
	//sprintf(&file_string[0], "%d", static_cast<int>(file_size));

	sprintf(&refile[sizeof(size_t)], "%s", count_file_buf.data());

	asio::async_write(socket_, asio::buffer(refile.data(), refile.size()),
		[this,fname](std::error_code ec, std::size_t sz)
		{
			if (!ec)
			{
				cout << fname << "�ļ����ͳɹ�\n";
				do_recive_filename();


			}
		});
	
}
