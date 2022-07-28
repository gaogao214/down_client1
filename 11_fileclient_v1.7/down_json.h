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
	struct profile {                  //�����ļ�
		std::string path;
		std::string port;
		GX_JSON(path, port);
	};

	struct profile_info               //�����ļ�
	{
		vector<profile> file_list;
		GX_JSON(file_list);
	};

	struct block                      //��һ����id���ļ���
	{
		int id;
		std::vector<std::string> files;
	};

	struct blocks_for_download
	{
		std::unordered_map<int, block> blocks_;
	};
	struct wget_c_file                 //�ϵ������ļ�
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
extern  filestruct::profile downfile_path;      //�����ļ�
extern  filestruct::files_info files_inclient;	//�����ͻ��˱��ص�json�ı�

class down_json :public enable_shared_from_this<down_json>
{
public:
	down_json(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints);

	void open_profile(string& name);			//��open.json�����ļ�

	void client_list_profile(string& name);		//��list.json�����ļ�

	void server_list_profile(string& text_json);//��list.json�����ļ�
	
	void block_list_profile(string& text_json); //��id.json�����ļ�

	void send_id_port(const string id_port);	//���ͳ�Ϊ��������id   port

	void down_json_run(filestruct::block Files, string& loadip, string& loadport, const string& comePort);//����

	std::string open_json_file(const std::string& json_name);  //��json�ļ�
	
	filestruct::wget_c_file_info wget_c_file_profile(string& name);//��wget_c_file.json �ϵ����������ļ�


	filestruct::files_info files_inserver;		//������������json�ı�
	
	filestruct::blocks blks_;					//����id.json�ı�
	filestruct:: blocks_for_download blks;		//��һ����id���ļ���
	std::string list_json = "list.json";
	std::string id_json = "id.json";
	std::string open = "down.json";
	std::string wget_c_file = "wget_c_file.json";
	std::string wget_c_file1 = "wget_c_file1.json";
	filestruct::wget_c_file_info wcfi;			//����һ���ṹ��


private:

	void do_connect(const asio::ip::tcp::resolver::results_type& endpoints);
	
	void recive_list();							//����list.json�ļ���������
	
	void recive_id();							//����id.json�ļ��� ������

	void isfile_exist( const string file_buf,int buf_len);//�ж��ļ����Ƿ����
	
	void down_load();							//��һ���̳߳�

	void save_file(const std::string& name , const std::string& file_buf);//�����ļ�

	asio::io_context& io_context_;
	asio::ip::tcp::socket socket_;

	char list_len[sizeof(size_t)];			//����list.json�ļ����ĳ���
	string list_name;						//����llist.json�ļ���
	int id_name_len ;
	int list_name_len ;
	char id_len[sizeof(size_t)];			//����id.json�ļ����ĳ���
	string id_name_text;					//����id.json�ļ���
	unordered_map<int, int> index;			//���صĴ���
	unordered_map<int, int> id_index;		//һ����id  ����
	string id_port_buf;						//���͵�id��port�ĳ��Ⱥ�����
	ThreadPool pool;
	volatile int len = 0;
};
