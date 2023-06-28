#ifndef INCLUDE_COMMON_JSON2OBJECT_H_
#define INCLUDE_COMMON_JSON2OBJECT_H_

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <json/json.h>  // NOLINT
#include "common_def.h"
#include "utils.h"
using namespace std;

namespace common {

//解析基本类型 int bool float double string 的一组模板
/*
 * 其实可声明为重载函数,声明为模板只是为了可以放在头文件中
 * 不能声明为 template <typename T> bool Unmarshal(int& obj,const Json::Value &root);是因为 
 * 在编译时编译器不能推断出T的类型，导致编译失败.所以将模板类型列表设置为template <int = 0> (Nontype 
 * Parameters) 此处int并无实际意义
 */
template <int = 0>
inline bool Unmarshal(int& obj, const Json::Value& root) {
  if (!root.isIntegral()) return false;
  obj = root.asInt();
  return true;
}

template <int = 0>
inline bool Unmarshal(bool& obj, const Json::Value& root) {
  if (!root.isBool()) return false;
  obj = root.asBool();
  return true;
}

template <int = 0>
inline bool Unmarshal(float& obj, const Json::Value& root) {
  if (!root.isString()) return false;
  std::string js_str = root.asString();
  utils::lexical_cast<float>(js_str, obj, 0.0);
  return true;
}

template <int = 0>
inline bool Unmarshal(double& obj, const Json::Value& root) {
  if (!root.isDouble()) return false;
  obj = root.asDouble();
  return true;
}

template <int = 0>
inline bool Unmarshal(std::string& obj, const Json::Value& root) {
  if (!root.isString()) return false;
  obj = root.asString();
  return true;
}

//解析基本类型容器类的一组模板
//只能匹配 vector<T> map<string,T> map<long,T> map<int,T> 的一组模板
template <typename T>
bool Unmarshal(vector<T>& obj, const Json::Value& root) {
  if (!root.isArray()) return false;
  obj.clear();
  bool ret = true;
  for (int i = 0; i < root.size(); ++i) {
    T tmp;                         //类型T要含有T()构造函数
    if (!Unmarshal(tmp, root[i]))  //递归调用Unmarshal函数
      ret = false;
    obj.push_back(tmp);
  }
  return ret;
}

//map key:string
template <typename T>
bool Unmarshal(map<string, T>& obj, const Json::Value& root) {
  if (!root.isObject()) return false;
  obj.clear();
  bool ret = true;
  Json::Value::Members mem = root.getMemberNames();
  for (auto iter = mem.begin(); iter != mem.end(); iter++) {
    T tmp;  //类型T要含有T()构造函数
    auto key_item = (*iter).c_str();
    if (!Unmarshal(tmp, root[key_item]))  //递归调用Unmarshal函数
      ret = false;
    obj[key_item] = tmp;
  }

  return ret;
}

// 检测一个类 是否含有非静态非重载的unmarshal方法
template <typename T>
struct TestUnmarshalFunc {
  //版本1
  template <typename TT>
  static char func(decltype(&TT::unmarshal));

  //版本2
  template <typename TT>
  static int func(...);

  /*
     * 如果类型T没有unmarshal方法,func<T>(NULL)匹配版本1时会产生错误,由于SFINAE准则,只能匹配版本2 
     * 的func,此时返回值4个字节,has变量为false.反之 has变量为true
     */
  const static bool has = (sizeof(func<T>(NULL)) == sizeof(char));
};

//如果对象自身含有 unmarshal 方法,则调用对象的unmarshal.否则会因SFINAE准则跳过这个版本的Unamrshal
template <typename T, typename enable_if<TestUnmarshalFunc<T>::has, int>::type = 0>
inline bool Unmarshal(T& obj, const Json::Value& root) {
  return obj.unmarshal(root);
}

template <typename T>
bool Unmarshal(T& obj, const string& s) {
  Json::Reader reader;
  Json::Value root;
  if (!reader.parse(s, root)) return false;
  return Unmarshal(obj, root);
}

}  // namespace common

#endif
