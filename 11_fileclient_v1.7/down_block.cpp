#include <asio.hpp>
#include <iostream>
#include <fstream>
#include "down_block.h"
#include <string>
#include <filesystem>
#include "file_server.h"
#include "wget_c_file.h"

//down_json* g_down_json = nullptr;

down_block::down_block(asio::io_context& io_context,asio::ip::tcp::resolver::results_type& endpoints, filestruct::block& Files)
	:socket_(io_context)
	,blk(Files)
	,dj(io_context,endpoints)
{
	do_connect(endpoints);
	dj.client_list_profile(dj.list_json);
}

void down_block::do_connect(asio::ip::tcp::resolver::results_type& endpoints)
{
	asio::async_connect(socket_, endpoints,
		[this](std::error_code ec, tcp::endpoint)
		{
			if (!ec)
			{
				cout << "�ͻ��˶˿� 12314 ��������˿� 12314 ���ӳɹ�\n";
				do_send_filename();	
			}
			else//����ʱ���ӳ���
			{
				cout << "�ͻ��˶˿� 12314 ��������˿� 12314 ����ʧ��\n";
				cout << "��������������\n";
				asio::io_context ios_;
				asio::ip::tcp::resolver resolver_(ios_);
				auto endpoint_ = resolver_.resolve({ "127.0.0.1","12314" });
				down_block df(ios_, endpoint_, blk);
				ios_.run();
			}
		});
}



void down_block::do_send_filename()//������Ҫ���ص��ļ���
{
	if (downloadingIndex > blk.files.size())
		return;
	
	if (downloadingIndex == blk.files.size())
	{
		id_ = to_string(blk.id);
		client_to_server(downfile_path.port);		

	}

	std::string name = blk.files.at(downloadingIndex++);
	size_t name_len = name.size();
	File_name.resize(sizeof(size_t) + name_len);
	std::memcpy(File_name.data(), &name_len, sizeof(size_t));
	sprintf(&File_name[sizeof(size_t)], "%s", name.data());

	asio::async_write(socket_, asio::buffer(File_name.data(), File_name.size()),
		[this, name](std::error_code ec, std::size_t)
		{
			if (!ec)
			{
				cout << "\n�ͻ�����������  " << name << "�ļ�\n";
				does_the_folder_exist(name);				
			}
		});
}

void down_block::does_the_folder_exist(const string& list_name)//�ж��ļ����Ƿ���ڣ��������򴴽��ļ���
{
	fileName = downfile_path.path + "\\" + list_name;
	std::size_t found = fileName.find_last_of("\\");

	std::error_code ec;
	if (!std::filesystem::exists(fileName.substr(0, found)))//�����ڴ���
	{
		std::filesystem::path{ fileName.substr(0, found) }.parent_path();
		// std::boolalpha; std::filesystem::exists(path);
		std::filesystem::create_directories(fileName.substr(0, found), ec);
		std::cout << "�ļ��д����ɹ�\n";
	}
	do_recive_file_text(fileName,4096,list_name);
}



void down_block::do_recive_file_text(const string& fname,int recive_len, const string& no_path_added_name)//�����ļ�
{


	std::memset(recive_file_len, 0, 4096);//����ڴ�
	
	socket_.async_read_some(asio::buffer(recive_file_len, 4096), //һ���Խ���
		[this,fname, recive_len,no_path_added_name](std::error_code ec, std::size_t )
		{
			if (!ec)
			{
				
				char ch = recive_file_len[sizeof(size_t)];
				
				if (ch == '*')
				{
					
					std::memcpy(&filelen, recive_file_len, (sizeof(size_t)/*/2*/));//��file_len�������յĳ���
					
					if (filelen > 4096 )
					{
						 file_size = 4096-sizeof(size_t)-1;						 
					}
					else
					{					
						file_size =filelen;
					}
					
					std::string file_content(recive_file_len + sizeof(size_t) + 1);

					ofstream id_File(fname, ios::binary);

 					id_File.write(file_content.c_str(), file_size);

					id_File.close();
										
				//cout <<"���سɹ�\n";					
				}
				else {
					
					ofstream id_File(fname, ios::out|ios::binary|ios::app);	

					id_File.write(recive_file_len,recive_len);

					id_File.close();

				}
				
				if (filelen > 4096 || ch != '*')//�жϴ��ļ�������յ����ļ����ȴ���4096 �ͼ�������  ���߽��յ���sizeof(size_t)���λ�õ�ֵ���� * �ͼ��������ļ�
				{
					size_t num = (filelen - 4096 /*- sizeof(size_t) - 1*/) / 4096;
				
					if (count < num)   //�жϴ��ļ���û�������     ���������Ͼͼ�����������
					{
						do_recive_file_text(fname,4096,no_path_added_name);
						//Sleep(1000);//˯��1����
						printf("\r");
						printf("[ %.2lf%%   ", (count * 4096.0 / (filelen - 1)) * 100);  //��ӡ�����˶��ٵİٷֱ�						
						printf("] �����ļ�ing  %s ", fname.c_str());					
						count++;
					}
					else if (count == num)
					{
						int yushu = (filelen  % 4096)+sizeof(size_t)+1;
						do_recive_file_text(fname, yushu, no_path_added_name);
						
						count++;
					
					}
					else if (count>num)
					{
						printf("\r");
						printf("[ %.2lf%%   ", (count * 4096.0 / (filelen - 1)) * 100);  //��ӡ�����˶��ٵİٷֱ�						
						printf("] ���سɹ�  %s ", fname.c_str());
						save_location(fname,no_path_added_name);
						do_send_filename();

					}
				}
				else {
					printf("\r");
					printf("[  %.2d%%  ", (filelen / file_size )*100);  //��ӡ�����˶��ٵİٷֱ�						
					printf("] ���سɹ�  %s ", fname.c_str());
					cout << "\n";
					save_location(fname, no_path_added_name);
					do_send_filename();
				}
			}
			else
			{
			cout << "\n����ʧ��"<<endl;
			cout << "��������������\n"; 
		    
			wcfi=dj.wget_c_file_profile(dj.wget_c_file1);


			for (auto& iter : wcfi.wget_c_file_list)
			{
				wcf.wget_name = iter.wget_name;
				wcf.offset = iter.offset;
				wcfi1.wget_c_file_list.push_back(wcf);
			}
			save_location(fname, no_path_added_name);
			Breakpoint_location();
			 
			asio::io_context ios_;
			asio::ip::tcp::resolver resolver_(ios_);

			auto endpoint_ = resolver_.resolve({ "127.0.0.1","12313" });

			wget_c_file wcf(ios_,endpoint_);
			
			ios_.run();

			 }
		});
}
//�Ͽ�������ʱ     wcfi ���  �Ͽ�����ʱ�����浽һ���ļ��� ������ʱ���ȶ�����ļ�   �ٰ�������浽����ļ���
//���浽wget_c_file�ļ��� ������ɵ��ļ���  ��ƫ����
void down_block::save_location(const string& name,const string& no_path_add_name)
{
	

	ifstream id_File(name,ios::binary);
	id_File.seekg(0, ios_base::end);
	file_size = id_File.tellg();//�ı��Ĵ�С
	id_File.seekg(0, ios_base::beg);
	//cout << "\nfile_size_===" << file_size << endl;

	
	wcf.wget_name = no_path_add_name;
	wcf.offset = file_size;

	wcfi1.wget_c_file_list.push_back(wcf);
	save_wget_c_file_json(wcfi1,dj.wget_c_file1);
	
}

/*��¼��ͣ����ʱ��  �ļ����Լ�ƫ����  */
void down_block::Breakpoint_location()
{
	/*�Ѷϵ����ļ���Ҳ������wget_c_file��*/
	for (auto& iter : files_inclient.file_list)
	{

		//�ڱ���list.json�ı����ҵ��ͷ������ͬ������
		auto it_client = std::find_if(wcfi1.wget_c_file_list.begin(), wcfi1.wget_c_file_list.end(), [&](auto file) {return file.wget_name == iter.path; });
		if (it_client == wcfi1.wget_c_file_list.end())//���û���ҵ����֣��Ͱ��ļ�����ӵ�wget_c_file����ļ���
		{
			if (iter.path.empty())
				continue;

			wcf.wget_name = iter.path;
			wcf.offset = 0;
			wcfi1.wget_c_file_list.push_back(wcf);
		}
	}
	save_wget_c_file_json(wcfi1,dj.wget_c_file);

}

void down_block::save_wget_c_file_json(filestruct::wget_c_file_info wcfi,const string& name)
{
	string text = RapidjsonToString(wcfi.serializeToJSON());
	gsh(text);
	
	auto file = fopen(name.c_str(), "wb");

	const char* t = text.c_str();
	size_t length = text.length();
	fwrite(t, length, 1, file);
	fflush(file);
	fclose(file);

	
}

void down_block::client_to_server(string profile_port)//��һ���̣߳��ͻ���ת���ɷ����
{

	thread t(std::bind(&down_block::server, this, profile_port));
	/*	t.join();*/
	cout << "�ͻ���: id: " << id_ << "  �˿ں�: " << profile_port << " ��ɷ����\n";

	t.detach();


}

void down_block::server(const std::string& server_port)//�ͻ���ת���ɷ����
{
	int port = atoi(server_port.c_str());
	asio::io_context io_context;//����������������������
	asio::ip::tcp::endpoint _endpoint(asio::ip::tcp::v4(), port);
	auto fs = std::make_shared<file_server>(io_context, _endpoint);

	io_context.run();
}



 void down_block::gsh(std::string& strtxt)//���ո�ʽд��id.json �ļ�
        {
            unsigned int dzkh = 0; //���ŵļ�����
            bool isy = false; //�ǲ�������
            for (int i = 0; i < strtxt.length(); ++i) {
                if (isy || strtxt[i] == '"') // "ǰ���� "������
                {
                    if (strtxt[i] == '"')
                        isy = !isy;
                    continue;
                }
                std::string tn = "";

#define ADD_CHANGE                          \
    for (unsigned int j = 0; j < dzkh; ++j) \
        tn += "\t";

                if (strtxt[i] == '{' || strtxt[i] == '[') {
                    dzkh++;
                    ADD_CHANGE
                        strtxt = strtxt.substr(0, i + 1) + "\n" + tn + strtxt.substr(i + 1);
                    i += dzkh + 1;
                }
                else if (strtxt[i] == '}' || strtxt[i] == ']') {
                    dzkh--;
                    ADD_CHANGE
                        strtxt = strtxt.substr(0, i) + "\n" + tn + strtxt.substr(i);
                    i += dzkh + 1;
                }
                else if (strtxt[i] == ',') {
                    ADD_CHANGE
                        strtxt = strtxt.substr(0, i + 1) + "\n" + tn + strtxt.substr(i + 1);
                    i += dzkh + 1;
                }
            }
        }

