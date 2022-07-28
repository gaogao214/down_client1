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

	void do_send_wget_file_name();//��������


	void do_send_wget_file_name_offset();//����ƫ�Ƴ���


	void do_recive_wget_file();

	inline size_t send_file_len(const std::string& filename)
	{
		ifstream infile(filename.c_str());
		infile.seekg(0, ios_base::end);
		size_t fsize = infile.tellg();//list.json�ı��Ĵ�С
		infile.close();

		return fsize;
	}

	inline string send_file_context(const std::string& filename)//�ı�������
	{

		ifstream File(filename.c_str());
		char file_buf = '0';//list.json�ļ�
		string buf;//һ��һ����֮��������list.json�ı�

		while (File.get(file_buf))
		{
			buf.push_back(file_buf);
		}
		File.close();

		return buf;
	}
private:
	
	
	asio::ip::tcp::socket socket_;
	down_json dj;  //�� down_json ʵ����

	string wget_text;						//���Ͷϵ������ļ�������
	string wget_c_name = "wget_c_file.json";
	string send_name;						//���Ͷϵ�����������
	string recive_wget_name;               //���նϵ��������ļ��� ƫ���� ���µĳ��� ����
	char recive_wget_len[sizeof(size_t)];  //���նϵ�ʱ���ļ�������

	
	string offset;
	

	
	string file_name;
	size_t num=0;
};
