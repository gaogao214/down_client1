#pragma once
#pragma once
#include <asio.hpp>
#include <iostream>
#include "common.h"
#include "GxJsonUtility.h"
#include "rapidjson/filereadstream.h"
using namespace asio::ip;
using namespace std;


struct File_ {
	std::string path;

	GX_JSON(path);
};

struct Files_info_
{
	vector<File_> file_list;
	GX_JSON(file_list);
};

class file_sesson : public std::enable_shared_from_this<file_sesson>
{
public:
	file_sesson(tcp::socket socket);


	void start();
private:

	void open_path_();
	void do_recive_filename();
	void do_send_filename(string& fname);



	asio::ip::tcp::socket socket_;
	char refilename_len[10];//�����ļ����Ĵ�С
	string  refile_name;//�����ļ���
	char refile_len[10];
	string refile;
	char filename_length[4096] = {0};
	char flen[10];
	File_ Down_path;
	string str_;
	char file_buf;//�������ļ�����ַ�һ��һ��������
	string count_file_buf;//ƴ��һ���ַ�����
};


class file_server
{
public:
	file_server(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint)
		:acceptorf(io_context, endpoint)
	{
		do_accept();
	}
	void do_accept()
	{
		acceptorf.async_accept(
			[this](std::error_code ec, asio::ip::tcp::socket socket) //��������
			{
				if (!ec)
				{
					//std::cout << "������\n";
					auto fs = std::make_shared<file_sesson>(std::move(socket));//ת��session��
					fs->start();
					sessions.push_back(fs);
				}
				do_accept();
			});
	}

private:
	std::vector<std::shared_ptr<file_sesson>> sessions;
	asio::ip::tcp::acceptor acceptorf;

};
