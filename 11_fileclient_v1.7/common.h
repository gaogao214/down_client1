#pragma once
/**
 * @file common.h
 * @author 李新
 * @brief json utility common def
 * @version 0.1
 * @date 2021-12-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#define PRIVATE_ARGS_GLUE(x, y) x y
#define PRIVATE_MACRO_VAR_ARGS_IMPL_COUNT(_1,_2,_3,_4,_5,_6,_7,_8,_9, _10, N, ...) N
#define PRIVATE_MACRO_VAR_ARGS_IMPL(args) PRIVATE_MACRO_VAR_ARGS_IMPL_COUNT args
#define COUNT_MACRO_VAR_ARGS(...) PRIVATE_MACRO_VAR_ARGS_IMPL((__VA_ARGS__,10, 9,8,7,6,5,4,3,2,1,0))
#define PRIVATE_MACRO_CHOOSE_HELPER2(M,count)  M##count
#define PRIVATE_MACRO_CHOOSE_HELPER1(M,count) PRIVATE_MACRO_CHOOSE_HELPER2(M,count)
#define PRIVATE_MACRO_CHOOSE_HELPER(M,count)   PRIVATE_MACRO_CHOOSE_HELPER1(M,count)
#define INVOKE_VAR_MACRO(M,...) PRIVATE_ARGS_GLUE(PRIVATE_MACRO_CHOOSE_HELPER(M,COUNT_MACRO_VAR_ARGS(__VA_ARGS__)), (__VA_ARGS__))

#include <type_traits>
#define BoolE typename std::enable_if<std::is_enum<T>::value, bool>::type
#define BoolNE typename std::enable_if<!std::is_enum<T>::value, bool>::type

#include <vector>
#include <unordered_map>
#include <set>
#include <string>
#include <cstdint>
#include <bitset>
// rapidjson impl
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>

using JsonItem = rapidjson::Value;
using JsonAllocator = rapidjson::Document::AllocatorType;

