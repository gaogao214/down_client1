//发生异常原因之一：是因为list.json id.json 文件没有删除 


#include <iostream>
#include <asio.hpp>
#include "down_json.h"
#include <memory>



int main()
{
	system("del wget_c_file.json");
	system("del wget_c_file1.json");
	system("del list.json");
	system("del id.json");
	system("rmdir /s .\\down");


	asio::io_context io_context;

	asio::ip::tcp::resolver resolver(io_context);

	auto endpoints = resolver.resolve("127.0.0.1", "12312");

	std::shared_ptr<down_json> p = std::make_shared<down_json>(io_context, endpoints);

	
	io_context.run();
	
	std::cin.get();
	return 0;
}