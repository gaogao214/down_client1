#pragma once
#include <asio.hpp>
#include "common.h"
#include "GxJsonUtility.h"
#include "rapidjson/filereadstream.h"
#include "down_json.h"

class wget_c_file :public enable_shared_from_this<wget_c_file>
{
public:
	wget_c_file(asio::io_context& io_context, asio::ip::tcp::resolver::results_type& endpoints);

	void do_connect(asio::ip::tcp::resolver::results_type& endpoints);

	void do_send_wget_file_name();//发送名字


	void do_send_wget_file_name_offset();//发送偏移长度


	void do_recive_wget_file();

	inline size_t send_file_len(const std::string& filename)
	{
		ifstream infile(filename.c_str());
		infile.seekg(0, ios_base::end);
		size_t fsize = infile.tellg();//list.json文本的大小
		infile.close();

		return fsize;
	}

	inline string send_file_context(const std::string& filename)//文本的内容
	{

		ifstream File(filename.c_str());
		char file_buf = '0';//list.json文件
		string buf;//一个一个读之后存在这里，list.json文本

		while (File.get(file_buf))
		{
			buf.push_back(file_buf);
		}
		File.close();

		return buf;
	}
private:
	
	
	asio::ip::tcp::socket socket_;
	down_json dj;  //给 down_json 实例化

	string wget_text;						//发送断点续传文件的内容
	string wget_c_name = "wget_c_file.json";
	string send_name;						//发送断点续传的名字
	string recive_wget_name;               //接收断点续传的文件名 偏移量 余下的长度 内容
	char recive_wget_len[sizeof(size_t)];  //接收断点时的文件名长度

	
	string offset;
	

	
	string file_name;
	size_t num=0;
};
