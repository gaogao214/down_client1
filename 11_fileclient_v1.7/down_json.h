#pragma once

#include <asio.hpp>
#include <fstream>
#include "common.h"
#include "rapidjson/filereadstream.h"
#include "GxJsonUtility.h"
#include <memory>
#include <unordered_map>
#include <iostream>
#include <filesystem>
#include "ThreadPool.h"


using namespace std;
using namespace asio::ip;

namespace filestruct {
	struct list_json {                 //list.json
		std::string path;
		uint32_t version;
		uint32_t blockid;
		GX_JSON(path, version, blockid);
	};
	
	struct files_info                  //list.json
	{
		vector<list_json> file_list;
		GX_JSON(file_list);
	};

	struct id_json {                   //id.json
		std::string ip;
		std::string port;
		GX_JSON(ip, port)
	};
	struct block_info {                //id.json
		int id;
		vector<id_json> server; 
		GX_JSON(id, server)
	};
	struct blocks {                    //id.json
		unordered_map<int, block_info> blocks;
		//vector<block_info> blocks;
		GX_JSON(blocks);
	};
	struct profile {                  //配置文件
		std::string path;
		std::string port;
		GX_JSON(path, port);
	};

	struct profile_info               //配置文件
	{
		vector<profile> file_list;
		GX_JSON(file_list);
	};

	struct block                      //存一个块id的文件名
	{
		int id;
		std::vector<std::string> files;
	};

	struct blocks_for_download
	{
		std::unordered_map<int, block> blocks_;
	};
	struct wget_c_file                 //断点续传文件
	{
		std::string wget_name;
		uint32_t offset;
		GX_JSON(wget_name,offset);
	};
	struct wget_c_file_info
	{
		vector<wget_c_file> wget_c_file_list;
		GX_JSON(wget_c_file_list);
	};
	

}


class down_json;
//
//extern down_json* g_down_json;
extern  filestruct::profile downfile_path;      //配置文件
extern  filestruct::files_info files_inclient;	//解析客户端本地的json文本

class down_json :public enable_shared_from_this<down_json>
{
public:
	down_json(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints);

	void open_profile(string& name);			//打开open.json配置文件

	void client_list_profile(string& name);		//打开list.json配置文件

	void server_list_profile(string& text_json);//打开list.json配置文件
	
	void block_list_profile(string& text_json); //打开id.json配置文件

	void send_id_port(const string id_port);	//发送成为服务器的id   port

	void down_json_run(filestruct::block Files, string& loadip, string& loadport, const string& comePort);//运行

	std::string open_json_file(const std::string& json_name);  //读json文件
	
	filestruct::wget_c_file_info wget_c_file_profile(string& name);//打开wget_c_file.json 断点续传配置文件


	filestruct::files_info files_inserver;		//解析服务器的json文本
	
	filestruct::blocks blks_;					//解析id.json文本
	filestruct:: blocks_for_download blks;		//存一个块id的文件名
	std::string list_json = "list.json";
	std::string id_json = "id.json";
	std::string open = "down.json";
	std::string wget_c_file = "wget_c_file.json";
	std::string wget_c_file1 = "wget_c_file1.json";
	filestruct::wget_c_file_info wcfi;			//声明一个结构体


private:

	void do_connect(const asio::ip::tcp::resolver::results_type& endpoints);
	
	void recive_list();							//接收list.json文件名和内容
	
	void recive_id();							//接收id.json文件名 和内容

	void isfile_exist( const string file_buf,int buf_len);//判断文件夹是否存在
	
	void down_load();							//开一个线程池

	void save_file(const std::string& name , const std::string& file_buf);//保存文件

	asio::io_context& io_context_;
	asio::ip::tcp::socket socket_;

	char list_len[sizeof(size_t)];			//接收list.json文件名的长度
	string list_name;						//接收llist.json文件名
	int id_name_len ;
	int list_name_len ;
	char id_len[sizeof(size_t)];			//接收id.json文件名的长度
	string id_name_text;					//接收id.json文件名
	unordered_map<int, int> index;			//下载的次数
	unordered_map<int, int> id_index;		//一共的id  数量
	string id_port_buf;						//发送的id和port的长度和内容
	ThreadPool pool;
	volatile int len = 0;
};
