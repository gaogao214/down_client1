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
	char refilename_len[10];//接收文件名的大小
	string  refile_name;//接收文件名
	char refile_len[10];
	string refile;
	char filename_length[4096] = {0};
	char flen[10];
	File_ Down_path;
	string str_;
	char file_buf;//把配置文件里的字符一个一个读出来
	string count_file_buf;//拼在一个字符串里
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
			[this](std::error_code ec, asio::ip::tcp::socket socket) //接受连接
			{
				if (!ec)
				{
					//std::cout << "已连接\n";
					auto fs = std::make_shared<file_sesson>(std::move(socket));//转到session类
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
