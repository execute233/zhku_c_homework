#ifndef COLLECTION_TYPE_PTR_H_
#define COLLECTION_TYPE_PTR_H_
/**
* 使用malloc分配给指定了的int类型数据空间，返回存储该元素的地址
* @param value 指定的数据
*/
inline int * int_ptr(int value);
/**
* 使用malloc分配给指定了的unsigned int类型数据空间，返回存储该元素的地址
* @param value 指定的数据
*/
inline unsigned int * uint_ptr(unsigned int value);
/**
* 使用malloc分配给指定了的long类型数据空间，返回存储该元素的地址
* @param value 指定的数据
*/
inline long long * long_ptr(long long value);
/**
* 使用malloc分配给指定了的unsigned long类型数据空间，返回存储该元素的地址
* @param value 指定的数据
*/
inline unsigned long long * ulong_ptr(unsigned long long value);
/**
* 使用malloc分配给指定了的float类型数据空间，返回存储该元素的地址
* @param value 指定的数据
*/
inline float * float_ptr(float value);
/**
* 使用malloc分配给指定了的double类型数据空间，返回存储该元素的地址
* @param value 指定的数据
*/
inline double * double_ptr(double value);
/**
* 使用malloc分配给指定了的long double类型数据空间，返回存储该元素的地址
* @param value 指定的数据
*/
inline long double * long_double_ptr(long double value);
/**
* 使用malloc分配给指定了的char类型数据空间，返回存储该元素的地址
* @param value 指定的数据
*/
inline char * char_ptr(char value);
/**
* 使用malloc分配给指定了的char *类型数据空间，返回存储该元素的地址
* @param value 指定的数据
*/
inline char * string_ptr(char * value);
#endif //COLLECTION_TYPE_PTR_H_