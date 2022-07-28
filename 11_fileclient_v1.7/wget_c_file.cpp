#include "wget_c_file.h"


wget_c_file::wget_c_file(asio::io_context& io_context, asio::ip::tcp::resolver::results_type& endpoints)
	:socket_(io_context)
	,dj(io_context,endpoints)
{
	do_connect(endpoints);
}

void wget_c_file::do_connect(asio::ip::tcp::resolver::results_type& endpoints)
{
	asio::async_connect(socket_, endpoints,
		[this](std::error_code ec, tcp::endpoint)
		{
			if (!ec)
			{
				cout << "�ͻ��˶˿� 12313 ��������˿� 12313 ���ӳɹ�\n";
				do_send_wget_file_name();
			}
			else {			
				cout << "�ͻ��˶˿� 12313 ��������˿� 12313 ����ʧ��\n";
			}
		});
}

/*���Ͷϵ�����������*/
void wget_c_file::do_send_wget_file_name()
{
	size_t list_name_len = wget_c_name.size();//������ϵ�����.json���ֵĴ�С
	send_name.resize(sizeof(size_t) + list_name_len);
	//sprintf(&send_name[0], "%d", static_cast<int>(list_name_len));
	memcpy(send_name.data(),&list_name_len,sizeof(size_t));
	sprintf(&send_name[sizeof(size_t)], "%s", wget_c_name.c_str());

	asio::async_write(socket_, asio::buffer(send_name.data(), send_name.size()),
		[this](std::error_code ec, std::size_t sz)
		{
			if (!ec)
			{
				do_send_wget_file_name_offset();//�����ļ�������
			}
		});
}

/*���Ͷϵ�����������*/
void wget_c_file::do_send_wget_file_name_offset()
{

	size_t fsize = send_file_len(wget_c_name);
	std::string list_buf = send_file_context(wget_c_name);
	wget_text.resize(sizeof(size_t)+ fsize);
	memcpy(wget_text.data(), &fsize, sizeof(size_t));
	sprintf(&wget_text[sizeof(size_t)],"%s",list_buf.c_str());

 	asio::async_write(socket_, asio::buffer(wget_text.c_str(), wget_text.size()),
		[this](std::error_code ec, std::size_t)
		{
			if (!ec)
			{
				cout <<wget_text << "���ͳɹ�\n";
				do_recive_wget_file();
			}
		});
}

/*���նϵ����������ֺ�����*/
void wget_c_file::do_recive_wget_file()
{
	asio::async_read(socket_,asio::buffer(recive_wget_len,sizeof(size_t)),
 	//socket_.async_read_some(asio::buffer(recive_wget_len, sizeof(size_t)),  /*�����ļ����ƺ�ƫ����*/
		[this](std::error_code ec, std::size_t)
		{
			if (!ec) 
			{			
				memcpy(&num,recive_wget_len,sizeof(size_t));	
				std::memset(recive_wget_len, 0, sizeof(size_t));//����ڴ�
			
				recive_wget_name.resize(num);
				asio::async_read(socket_, asio::buffer(recive_wget_name.data(), num),
					[this](std::error_code ec, std::size_t)
					{
						if (!ec)
						{			    
								auto pos = recive_wget_name.find_first_of("!");
								auto remaining_total = recive_wget_name.substr(0, pos);		//���յ��ַ����ܳ���
								cout << "total >:" << remaining_total << endl;

								auto str = recive_wget_name.substr(pos + 1);	//�ϵ�ʱ �ļ��� ƫ����  �Լ����µ�����
								auto pos1 = str.find_first_of(',');
								auto name = str.substr(0, pos1);             
								cout << "name >: " << name << endl;  //ƫ����

								auto  str1 = str.substr(pos1 + 1); //���µ�����
								auto pos2 = str1.find_first_of('*');
								offset = str1.substr(0,pos2);
								wget_text = str1.substr(pos2 + 1);
								


					
								file_name = downfile_path.path + "\\" + name;

								int offset_len = atoi(offset.data());
								int total = atoi(remaining_total.data());
								ofstream file(file_name.data(), ios::out | ios::binary | ios::app);
								file.seekp(offset_len, ios::beg);

								file.write(wget_text.c_str(), total);
				
								file.close();
								cout << file_name << "�ļ����ճɹ�\n";
			

							do_recive_wget_file();
						}
					});
			}
		});			
}