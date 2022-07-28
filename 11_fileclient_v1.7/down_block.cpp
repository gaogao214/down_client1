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
				cout << "客户端端口 12314 与服务器端口 12314 连接成功\n";
				do_send_filename();	
			}
			else//下载时连接出错
			{
				cout << "客户端端口 12314 与服务器端口 12314 连接失败\n";
				cout << "向主服务器连接\n";
				asio::io_context ios_;
				asio::ip::tcp::resolver resolver_(ios_);
				auto endpoint_ = resolver_.resolve({ "127.0.0.1","12314" });
				down_block df(ios_, endpoint_, blk);
				ios_.run();
			}
		});
}



void down_block::do_send_filename()//发送需要下载的文件名
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
				cout << "\n客户端请求下载  " << name << "文件\n";
				does_the_folder_exist(name);				
			}
		});
}

void down_block::does_the_folder_exist(const string& list_name)//判断文件夹是否存在，不存在则创建文件夹
{
	fileName = downfile_path.path + "\\" + list_name;
	std::size_t found = fileName.find_last_of("\\");

	std::error_code ec;
	if (!std::filesystem::exists(fileName.substr(0, found)))//不存在创建
	{
		std::filesystem::path{ fileName.substr(0, found) }.parent_path();
		// std::boolalpha; std::filesystem::exists(path);
		std::filesystem::create_directories(fileName.substr(0, found), ec);
		std::cout << "文件夹创建成功\n";
	}
	do_recive_file_text(fileName,4096,list_name);
}



void down_block::do_recive_file_text(const string& fname,int recive_len, const string& no_path_added_name)//接收文件
{


	std::memset(recive_file_len, 0, 4096);//清空内存
	
	socket_.async_read_some(asio::buffer(recive_file_len, 4096), //一次性接收
		[this,fname, recive_len,no_path_added_name](std::error_code ec, std::size_t )
		{
			if (!ec)
			{
				
				char ch = recive_file_len[sizeof(size_t)];
				
				if (ch == '*')
				{
					
					std::memcpy(&filelen, recive_file_len, (sizeof(size_t)/*/2*/));//给file_len赋给接收的长度
					
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
										
				//cout <<"下载成功\n";					
				}
				else {
					
					ofstream id_File(fname, ios::out|ios::binary|ios::app);	

					id_File.write(recive_file_len,recive_len);

					id_File.close();

				}
				
				if (filelen > 4096 || ch != '*')//判断大文件，如果收到的文件长度大于4096 就继续接收  或者接收到的sizeof(size_t)这个位置的值不是 * 就继续接收文件
				{
					size_t num = (filelen - 4096 /*- sizeof(size_t) - 1*/) / 4096;
				
					if (count < num)   //判断大文件接没接收完毕     如果接收完毕就继续发送名字
					{
						do_recive_file_text(fname,4096,no_path_added_name);
						//Sleep(1000);//睡眠1毫秒
						printf("\r");
						printf("[ %.2lf%%   ", (count * 4096.0 / (filelen - 1)) * 100);  //打印下载了多少的百分比						
						printf("] 下载文件ing  %s ", fname.c_str());					
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
						printf("[ %.2lf%%   ", (count * 4096.0 / (filelen - 1)) * 100);  //打印下载了多少的百分比						
						printf("] 下载成功  %s ", fname.c_str());
						save_location(fname,no_path_added_name);
						do_send_filename();

					}
				}
				else {
					printf("\r");
					printf("[  %.2d%%  ", (filelen / file_size )*100);  //打印下载了多少的百分比						
					printf("] 下载成功  %s ", fname.c_str());
					cout << "\n";
					save_location(fname, no_path_added_name);
					do_send_filename();
				}
			}
			else
			{
			cout << "\n下载失败"<<endl;
			cout << "向主服务器连接\n"; 
		    
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
//断开再连接时     wcfi 清空  断开连接时，保存到一个文件中 ，连接时，先读这个文件   再把这个保存到别的文件中
//保存到wget_c_file文件中 下载完成的文件名  和偏移量
void down_block::save_location(const string& name,const string& no_path_add_name)
{
	

	ifstream id_File(name,ios::binary);
	id_File.seekg(0, ios_base::end);
	file_size = id_File.tellg();//文本的大小
	id_File.seekg(0, ios_base::beg);
	//cout << "\nfile_size_===" << file_size << endl;

	
	wcf.wget_name = no_path_add_name;
	wcf.offset = file_size;

	wcfi1.wget_c_file_list.push_back(wcf);
	save_wget_c_file_json(wcfi1,dj.wget_c_file1);
	
}

/*记录暂停下载时的  文件名以及偏移量  */
void down_block::Breakpoint_location()
{
	/*把断点后的文件名也保存在wget_c_file中*/
	for (auto& iter : files_inclient.file_list)
	{

		//在本地list.json文本里找到和服务端相同的名字
		auto it_client = std::find_if(wcfi1.wget_c_file_list.begin(), wcfi1.wget_c_file_list.end(), [&](auto file) {return file.wget_name == iter.path; });
		if (it_client == wcfi1.wget_c_file_list.end())//如果没有找到名字，就把文件名添加到wget_c_file这个文件中
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

void down_block::client_to_server(string profile_port)//开一个线程，客户端转换成服务端
{

	thread t(std::bind(&down_block::server, this, profile_port));
	/*	t.join();*/
	cout << "客户端: id: " << id_ << "  端口号: " << profile_port << " 变成服务端\n";

	t.detach();


}

void down_block::server(const std::string& server_port)//客户端转换成服务端
{
	int port = atoi(server_port.c_str());
	asio::io_context io_context;//创建对象，用来连接上下文
	asio::ip::tcp::endpoint _endpoint(asio::ip::tcp::v4(), port);
	auto fs = std::make_shared<file_server>(io_context, _endpoint);

	io_context.run();
}



 void down_block::gsh(std::string& strtxt)//按照格式写入id.json 文件
        {
            unsigned int dzkh = 0; //括号的计数器
            bool isy = false; //是不是引号
            for (int i = 0; i < strtxt.length(); ++i) {
                if (isy || strtxt[i] == '"') // "前引号 "后引号
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

