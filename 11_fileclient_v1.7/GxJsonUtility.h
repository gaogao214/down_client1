#pragma once
/**
 * @file GxJsonUtility.h
 * @author 李新
 * @brief json utility header file
 * @version 0.1
 * @date 2021-12-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */


/**
 * @brief 
 * 这里注意，enum也是可以支持的，但是！需要开启RTTI机制，所以暂时就没给加上，有需要再加这个功能。
 */
#pragma once

#include "common.h"



#include "rapidjson/rapidjson_serialize.h"
#include "rapidjson/rapidjson_deserialize.h"
#include "rapidjson/rapidjson_macro.h"
// bson impl
std::string RapidjsonToString(const rapidjson::Document& doc);


// bson 用了windows.h，暂时没看为什么，反正非常离谱，有可能是用的了内存分配的东西，离谱。
#ifdef GetObject
    #undef GetObject
#endif