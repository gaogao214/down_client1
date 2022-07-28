#pragma once
/**
 * @file rapidjson_serialize.h
 * @author 李新
 * @brief json utility rapidjson 序列化实现
 * @version 0.1
 * @date 2021-12-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */

namespace rapidjson_impl {

template<class T>
bool serializeValue( JsonItem& item, const T& value, JsonAllocator& allocator) {
    return value.serializeToJsonItem(item, allocator);
}
template<>
bool serializeValue( JsonItem& item, const int32_t& value, JsonAllocator& allocator);
template<>
bool serializeValue( JsonItem& item, const uint32_t& value, JsonAllocator& allocator);
template<>
bool serializeValue( JsonItem& item, const float& value, JsonAllocator& allocator);
template<>
bool serializeValue( JsonItem& item, const double& value, JsonAllocator& allocator);
template<>
bool serializeValue( JsonItem& item, const std::string& value, JsonAllocator& allocator);
template<>
bool serializeValue( JsonItem& item, char const* const& value, JsonAllocator& allocator);
template<class T, size_t SIZE>
bool serializeValue( JsonItem& item, const T(&arr)[SIZE], JsonAllocator& allocator) {
    item.SetArray();
    auto jarr = item.GetArray();
    for( const auto& val: arr) {
        rapidjson::Value it;
        serializeValue(it, val, allocator);
        jarr.PushBack(it, allocator);
    }
    return true;
}
template<class T>
bool serializeValue( JsonItem& item, const std::vector<T>& vec, JsonAllocator& allocator) {
    item.SetArray();
    auto jarr = item.GetArray();
    for(const auto& val: vec) {
        rapidjson::Value it;
        serializeValue(it, val, allocator);
        jarr.PushBack(it, allocator);
    }
    return true;
}

template<class T>
bool serializeValue(JsonItem& item, const std::set<T>& s, JsonAllocator& allocator) {
    item.SetArray();
    auto jarr = item.GetArray();
    for (const auto& val : s) {
        rapidjson::Value it;
        serializeValue(it, val, allocator);
        jarr.PushBack(it, allocator);
    }
    return true;
}

template<class K,class V>
bool serializeValue(JsonItem& item, const std::unordered_map<K,V>& mp, JsonAllocator& allocator) {
    item.SetArray();
    auto jarr = item.GetArray();
    for (const auto& val : mp) {
        rapidjson::Value it1;
        serializeValue(it1, val.first, allocator);
        jarr.PushBack(it1, allocator);
        rapidjson::Value it2;
        serializeValue(it2, val.second, allocator);
        jarr.PushBack(it2, allocator);
    }
    return true;
}

template<size_t SIZE>
bool serializeValue( JsonItem& item, const char(&arr)[SIZE], JsonAllocator& allocator) {
    item.SetString(rapidjson::StringRef<char>(&arr[0]));
    return true;
}
/**
 * @brief 
 *      T is enum type 
 */
template<class T>
BoolE serializeMember(JsonItem& item, const T& type, JsonAllocator& allocator) {
    return false;
}
/**
 * @brief 
 *      T is not enum type 
 */
template<class T>
BoolNE serializeMember( JsonItem& item, const T& type, JsonAllocator& allocator) {
    return serializeValue(item, type, allocator);
}

/**
 * @brigh
 *
 */



}