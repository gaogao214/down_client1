#pragma once
#include <asio.hpp>
#include "common.h"
#include "GxJsonUtility.h"
#include "rapidjson/filereadstream.h"
#include "down_json.h"

using namespace asio::ip;
using namespace std;
//
//namespace filestruct {
//	struct blocks_for_download;
//	struct block;
//}

class down_block:public enable_shared_from_this<down_block>
{
public:
	down_block(asio::io_context& io_context, asio::ip::tcp::resolver::results_type& endpoints, filestruct::block& Files);

	void do_connect( asio::ip::tcp::resolver::results_type& endpoints);

	void do_send_filename();//��������

	void does_the_folder_exist(const string& list_name);//�ж��ļ����Ƿ���ڣ��������򴴽��ļ���

	void do_recive_file_text(const string& fname,int recive_len,const string& no_path_added_name);//�����ļ�����

	void gsh(std::string& strtxt);//���ո�ʽд��id.json �ļ�

	void client_to_server(string profile_port);    //��һ���̣߳��ͻ���ת���ɷ����

	void server(const std::string& server_port);      //�ͻ���ת���ɷ����

	void save_location(const string& name,const string& no_path_add_name);    /*��¼��ͣ����ʱ��  �ļ����Լ�ƫ����  */

	void save_wget_c_file_json(filestruct::wget_c_file_info wcfi,const string& name);

	void Breakpoint_location();    /*��¼��ͣ����ʱ��  �ļ����Լ�ƫ����  */

	void save_file(const string& fname);

	string id_ip_port_;

private:
	filestruct::block& blk;//feige
	int downloadingIndex=0;//������ĸ���
	asio::ip::tcp::socket socket_;
	string File_name;//���͵������Լ����ֳ���
	char recive_file_len[4096] = { 0 };//�����ļ��ĳ���	
	char* count_file_buf;//�������ַ�
	string recive_file;//�����ı�
	std::string fileName;   //·��+�ļ���
	std::size_t filelen = 0;//���ճ���  ���ͳ��ȣ����ĳ���
	size_t file_size;   //�ļ��Ĵ�С
	int count=0 ;   //����
	string id_;   //id ��
	string wget_name;   //�ϵ�ʱ������
	filestruct::wget_c_file wcf;
	filestruct::wget_c_file_info wcfi;  //����һ���ṹ��
	filestruct::wget_c_file_info wcfi1;  //����һ���ṹ��
	down_json dj;       //����һ��down_json��
};