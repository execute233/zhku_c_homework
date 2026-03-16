#ifndef COLLECTION_ARRAY_LIST_H_
#define COLLECTION_ARRAY_LIST_H_
#include "../proj/data.h"
/**
* ArrayList的定义，存储指针数组
* 当前文件中的方法使用非预期的值都会返回无效数据
*/
typedef struct ArrayList {
    int size;
    int capacity;
    void** array;
} * ArrayList;
/**
* 创建一个ArrayList，默认初始容量为10
*/
ArrayList createAListDefault();
/**
* 创建一个拥有指定初始容量的ArrayList
* @param capacity 指定容量
*/
ArrayList createAList(int capacity);
/**
* 添加指针到ArrayList末尾中
* @param alist 指定的ArrayList结构体
* @param element 元素的地址，可在collection_type_tool.h中构造基本数据类型的地址
* @return 添加到的元素的下标，插入失败则返回-1
*/
int addAList(ArrayList alist, void * element);
/**
* 插入元素到指定的索引上，原本的指针及其后面的所有指针都会向后移
* @param alist 指定的ArrayList结构体
* @param element 元素的引用，可在collection_type_tool.h中构造基本数据类型的地址
* @param index 指定的下标
* @return 插入是否成功
*/
_Bool insertAList(ArrayList alist,void * element, int index);
/**
* 删除指定下标的指针，但不会释放其指向的内容
* @param alist 指定的ArrayList结构体
* @param index 指定下标
* @return 是否删除成功
*/
_Bool removeAList(ArrayList alist, int index);
/**
* 删除指定下标的指针，但会释放其指向的内容
* @param alist 指定的ArrayList结构体
* @param index 指定下标
* @return 是否删除成功
*/
_Bool removeAListRls(ArrayList alist, int index);
/**
* 删除ArrayList中的指定指针
* @param alist 指定的ArrayList结构体
* @param element 元素的引用，可在collection_type_tool.h中构造基本数据类型的地址
* @return 删除的指针下标，没有时返回-1
*/
_Bool removeAListByPtr(ArrayList alist, void * element);
/**
* 替换指定下标处的指针
* @param alist 指定的ArrayList结构体
* @param newElement 新的指针
* @param index 指定的下标
* @return 被替换的指针，如果失败返回NULL
*/
void* replaceAList(ArrayList alist, int index, void * newElement);
/**
* 替换指定下标处的指针，并且被替换的指针指向的内存会被释放
* @param alist 指定的ArrayList结构体
* @param newElement 新的指针
* @param index 指定的下标
* @return 替换是否成功
*/
_Bool replaceAListRls(ArrayList alist, int index, void * newElement);
/**
* 查找指定的指针
* @param alist 指定的ArrayList结构体
* @param element 要查找的指针
* @return 指定的指针下标，没有返回-1
*/
int findAListByPtr(ArrayList alist, void * element);
/**
* 查找指定的元素，通过传入的compare函数实现比较指定值
* @param alist 指定的ArrayList结构体
* @param element 要进行比较的指针
* @return 指定的指针下标，没有返回-1
*/
int findAListByCmp(ArrayList alist, _Bool compare(void*, void*), void * element);
/**
* 获取指定下标的指针，非法数据则返回NULL
* @param alist 指定的ArrayList结构体
* @param index 指定下标
*/
void* getAList(ArrayList alist, int index);
/**
* 清空ArrayList的所有指针，但是不会释放其指向的内存
* 同时该ArrayList分配的array内存不会释放，意味着该ArrayList仍然可以当做新的使用
* @param alist 指定的ArrayList结构体
*/
void clearAList(ArrayList alist);
/**
* 清空ArrayList的所有指针，但是会释放其指向的内存!
* 同时该ArrayList分配的array内存不会释放，意味着该ArrayList仍然可以当做新的使用
* @param alist 指定的ArrayList结构体
*/
void clearAListRls(ArrayList alist);
/**
* 摧毁该ArrayList，变得不可用，但是其指针指向的内容是不会被释放的
* @param alist 指定的ArrayList结构体
*/
void destroyAList(ArrayList alist);
/**
* 摧毁该ArrayList，变得不可用，但是其指针指向的内容是会被释放的!
* @param alist 指定的ArrayList结构体
*/
void destroyAListRls(ArrayList alist);
/**
* 遍历每个指针
* @param alist 指定的ArrayList结构体
* @param forEach 遍历函数
*/
void forEachAList(ArrayList alist, void forEach(void * element));
/**
 * 通过指定的比较方法排序集合，返回的数值>0表示前者元素大于后者元素，<0表示前者元素小于后者元素，=0表示相等
 * @param alist 指定的ArrayList结构体
 * @param esc true表示升序，false表示降序
 * @param compare 比较函数
 ***/
void sort(ArrayList alist, _Bool esc, int compare(void*, void*));
/**
 * 获取集合中最大的元素，返回的数值>0表示前者元素大于后者元素，<0表示前者元素小于后者元素，=0表示相等
 * @param alist 指定的ArrayList结构体
 * @param compare 比较函数
 ***/
void* max(ArrayList alist, int compare(void*, void*));


#endif