#ifndef INCLUDE_PEOPLE_H_
#define INCLUDE_PEOPLE_H_

#include "common/json2object.h"
using namespace std;

namespace common {
//作用: 将两个token(可以是宏),连接在一起
#define __macro_cat__(a, b) a##b
#define MACRO_CAT(a, b) __macro_cat__(a, b)

// 宏展开实现伪循环
/*
 * __func_3(func,a,b,c) 具体展开过程：
 * 第一次： __func_1(func,a) __func_2(func,b,c) 
 * 第二次： func(a)          __func_1(func,b)    __func_1(func,c)
 * 第三次： func(a)          func(b)             func(c)
 * 最终在a,b,c上都调用了一次传入的func函数
 */
#define __func_1(func, member) func(member);
#define __func_2(func, member, ...) __func_1(func, member) __func_1(func, __VA_ARGS__)
#define __func_3(func, member, ...) __func_1(func, member) __func_2(func, __VA_ARGS__)
#define __func_4(func, member, ...) __func_1(func, member) __func_3(func, __VA_ARGS__)
#define __func_5(func, member, ...) __func_1(func, member) __func_4(func, __VA_ARGS__)
//###### COUNT 宏实现#######
//作用: 返回传入参数个数. eg: COUNT(a,b,c)返回3
#define COUNT(...) __count__(0, ##__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define __count__(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N

//###### FOR_EACH 实现#######
//作用：传入一个函数func和一个list，把func用在list的每个元素上
//#define FOR_EACH(func, ...) MACRO_CAT(__func_, COUNT(__VA_ARGS__))(func, __VA_ARGS__)

#define FOR_EACH_(func, ...) MACRO_CAT(__func_, COUNT(__VA_ARGS__))(func, __VA_ARGS__)
#define FOR_EACH(func, ...) FOR_EACH_(func, __VA_ARGS__)

#define __unmarshal_obj_each_field__(field)      \
  if (!common::Unmarshal(field, root[#field])) { \
    ret = false;                                 \
  }

#define UNMARSHAL_OBJ(...)                              \
  bool unmarshal(const Json::Value& root) {             \
    bool ret = true;                                    \
    FOR_EACH(__unmarshal_obj_each_field__, __VA_ARGS__) \
    return ret;                                         \
  }

// JSON_HELP 是一个变参宏
#define JSON_HELP(...) UNMARSHAL_OBJ(__VA_ARGS__)  //这里生成unmarshal函数

//MARSHAL_OBJ(__VA_ARGS__)

struct Student {
  int id;
  bool sex;
  double score;
  string name;

  JSON_HELP(id, sex, score, name)
};
struct Teacher {
  string name;
  int subject;
  vector<Student> stus;

  JSON_HELP(name, subject, stus)
};

}  // namespace common

#endif
