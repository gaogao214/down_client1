		#include <algorithm>
		#include "down_json.h"
		#include <sstream>
		#include "down_block.h"
		#include "file_server.h"
		#include <string>
		using namespace std;
		using namespace asio::ip;
		using namespace rapidjson;
		//down_json* g_down_json = nullptr;
		filestruct::profile downfile_path; 
		filestruct::files_info files_inclient;//解析客户端本地的json文本
		down_json::down_json(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints)
			: io_context_(io_context)
			, socket_(io_context)
			, pool(2)	
		{
			//g_down_json = this;
			do_connect(endpoints);
		}

		void down_json::do_connect(const asio::ip::tcp::resolver::results_type& endpoints)
		{
			asio::async_connect(socket_, endpoints,
				[this](std::error_code ec, asio::ip::tcp::endpoint)
				{
					if (!ec)
					{
						cout << "客户端端口 12312 与服务器端口 12312 连接成功\n";
						open_profile(open);
						recive_list();//接收list.json文件名字
					}	
					else {
						cout << "客户端端口 12312 与服务器端口 12312 连接失败\n";
					}
				});
		}

		void  down_json::open_profile(string& name)//打开配置文件，并找到配置文件中的路径,查看路径下的文件或文件名   解析json文件
		{
			string readbuffer = open_json_file(name);
			downfile_path.deserializeFromJSON(readbuffer.c_str());
		}

		void  down_json::client_list_profile(string& name)//打开list_json   json文件  解析json文件
		{
			string readbuffer = open_json_file(name);
			files_inclient.deserializeFromJSON(readbuffer.c_str());
		}

		void  down_json::server_list_profile(string& text_json)//打开list_json   json文件  解析json文件
		{
			files_inserver.deserializeFromJSON(text_json.c_str());
		}

		void  down_json::block_list_profile(string& text_json)//打开list_json   json文件  解析json文件
		{
			blks_.deserializeFromJSON(text_json.c_str());
		}

		filestruct::wget_c_file_info  down_json::wget_c_file_profile(string& name)//打开断点续传文件  解析json文件
		{
			string readbuffer = open_json_file(name);

			wcfi.deserializeFromJSON(readbuffer.c_str());

			return wcfi;
		}

		std::string down_json::open_json_file(const std::string& json_name)//打开指定名称的json文本
		{
			std::string content{};
			std::string tmp{};
			std::fill(content.begin(), content.end(), 0);//清空

			fstream ifs(json_name, std::ios::in | std::ios::binary);
			if (!ifs.is_open())
				return {};

			while (std::getline(ifs, tmp))
			{
				content.append(tmp);
			}

			return content;

		}


		void down_json::recive_list()//接收list.json文件名字
		{
			asio::async_read(socket_, asio::buffer(list_len, sizeof(size_t)),
				[this](std::error_code ec, std::size_t)
				{
					if (!ec)
					{
				
						std::memcpy(&list_name_len, list_len, sizeof(size_t));
						//int list_name_len = atoi(list_len);
						list_name.resize(list_name_len);
						asio::async_read(socket_, asio::buffer(list_name, list_name_len),
							[this](std::error_code ec, std::size_t)
							{
								if (!ec)
								{

									auto pos = list_name.find_first_of("*");
									auto name = list_name.substr(0, pos);
									auto text = list_name.substr(pos + 1);

									cout << "下载 " << list_name << " 文本" << endl;
									server_list_profile(text);
									isfile_exist(text, text.size());//判断list.json文件是否存在,存在就解析json文本与server的json进行比较，不存在就保存文件
									recive_id();//接收id的名字
								}
							});
					}
				});
		}

		void down_json::isfile_exist(const string file_buf, int buf_len)//判断list.json文件是否存在,存在就解析json文本与server的json进行比较，不存在就保存文件
		{

			std::fstream list("list.json", std::ios::binary | std::ios::out | std::ios::app);
			if (!list.is_open())
			{
				while (file_buf.c_str() != nullptr)
				{
					list.write(file_buf.c_str(), buf_len);
				}
				list.flush();
				list.close();
			}
			else {
				client_list_profile(list_json);
				save_file(list_json, file_buf);//保存内容
			}
		}


		void down_json::save_file(const std::string& name, const std::string& file_buf)//保存内容
		{

			ofstream save_file(name,ios::out|ios::binary);
	
			save_file.write(file_buf.c_str(),strlen(file_buf.c_str())-len);
	
			save_file.close();
			cout << name << " 文件保存成功\n";
		}

		void down_json::recive_id()//接收id文件的名字
		{
			asio::async_read(socket_, asio::buffer(id_len, sizeof(size_t)),
				[this](std::error_code ec, std::size_t)
				{
					if (!ec)
					{

						std::memcpy(&id_name_len, id_len,sizeof(size_t));
						id_name_text.resize(id_name_len);
						asio::async_read(socket_, asio::buffer(id_name_text.data(), id_name_len),
							[this](std::error_code ec, std::size_t)
							{
								if (!ec)
								{

									auto pos = id_name_text.find_first_of("*");
									auto name = id_name_text.substr(0,pos);
									auto text = id_name_text.substr(pos+1);

									save_file(name, text);//保存内容
									block_list_profile(text);
									down_load();//把任务放在线程池里向服务器请求下载
								}
							});
					}
				});
		}



		void down_json::down_json_run(filestruct::block Files, string& loadip, string& loadport, const string& comePort)//连接下载文件的端口
		{	
			try {
			asio::io_context ios;
			asio::ip::tcp::resolver resolver_(ios);
			auto endpoint = resolver_.resolve({ loadip,loadport });
			down_block db(ios, endpoint, Files);
	
			ios.run();
			}
			catch (...)
			{
				open_profile(open);
				/*while (!*/send_id_port(comePort + "," + downfile_path.port);//)
				//	continue;
			}
		}

		void down_json::down_load()//把任务放在线程池里向服务器请求下载
		{
			for (auto i : files_inserver.file_list)//每个id(id:1 id:2)的总数量
			{
				id_index[i.blockid] += 1;
			}
	
			//ThreadPool pool(2);
	
			for (auto& iter : files_inserver.file_list)
			{
				//在本地list.json文本里找到和服务端相同的名字
				auto it_client = std::find_if(files_inclient.file_list.begin(), files_inclient.file_list.end(), [&](auto file) {return file.path == iter.path; });
				if (it_client == files_inclient.file_list.end() || it_client->version < iter.version)//如果没有找到名字，或者版本号没有服务器的高，就下载
				{
			
			
					blks.blocks_[iter.blockid].id = iter.blockid;//遍历list.json文件里的 id 存到 blks.id(下载时就把一个文件的id  名字存起来)

					blks.blocks_[iter.blockid].files.push_back(iter.path);


					index[iter.blockid] += 1;          //下载一个文件   就让下载的数量+1


					if (id_index[iter.blockid] == index[iter.blockid])//（id:1  id:2）每个id的总数量 == 存到的数量     （数量相等时开始请求下载）
					{
						auto it = blks_.blocks.find(blks.blocks_[iter.blockid].id);//找到要下载文件的id 
						//auto it = std::find_if(blks.blocks_.begin(), blks.blocks_.end(), [&](auto file) {return file == blks.blocks_[iter.blockid].id; });

						//auto it = blks_.blocks.find(blks.blocks_.begin(), blks.blocks_.end(), blks.blocks_[iter.blockid].id);//找到要下载文件的id 
						if (it == blks_.blocks.end())//没有找到
							continue;

						pool.enqueue(bind(&down_json::down_json_run, this, blks.blocks_[iter.blockid], it->second.server.back().ip, it->second.server.back().port, to_string(iter.blockid)));
				

					}
				}
			}
		}





		void down_json::send_id_port(const string id_port)//发送成为服务器的id ip port 
		{
			std::size_t id_port_len = id_port.size();//id ip port字符串大小                       
			id_port_buf.resize(sizeof(size_t) + id_port_len);//给id_port_buf分配sizeof(size_t) + id_port_len的长度
			std::memcpy(id_port_buf.data(), &id_port_len, sizeof(size_t));
			sprintf(&id_port_buf[sizeof(size_t)], "%s", id_port.c_str());//把文件名赋给&Id_IP_Port_buf[10]

			asio::async_write(socket_, asio::buffer(id_port_buf.data(), id_port_buf.size()),	//一次传输文件名长度和文件名
				[this, id_port_len, id_port](std::error_code ec, std::size_t)
				{
					if (!ec)
					{
						std::cout << "发送给服务器  id ip port:  " << id_port_buf << std::endl;
					}
				});
		}


