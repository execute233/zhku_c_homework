#include "array_list.h"
#include <stdlib.h>
#include <string.h>

struct ArrayList* createAListDefault() {
    return createAList(10);
}
struct ArrayList* createAList(int capacity) {
    struct ArrayList* list = malloc(sizeof(struct ArrayList));
    list->capacity = capacity;
    list->size = 0;
    list->array = malloc(sizeof(void*) * capacity);
    return list;
}
/**
* 缩容没实现
* 检查ArrayList是否需要扩容，如果需要就扩容
* 能跑就别乱动，乱搞内存会炸Boom！！！
**/
static void checkSizeOrResize(struct ArrayList* alist) {
    if (alist->size >= alist->capacity) {
        int newCapacity = (alist->capacity >> 1) + alist->capacity;
        // 为什么不用realloc?你会你可以用
        void** newArr = malloc( sizeof(void*) * newCapacity);
        if (newArr != alist->array) {
            memcpy(newArr, alist->array, sizeof(void*) * alist->size);
            free(alist->array);
            alist->array = newArr;
        }
        alist->capacity = newCapacity;
    }
}
int addAList(struct ArrayList* alist, void * element) {
    checkSizeOrResize(alist);
    int addIndex = alist->size;
    *(alist->array + addIndex) = element;
    alist->size++;
    return addIndex;
}
bool insertAList(struct ArrayList* alist,void * element, int index) {
    if (index < 0 || index > alist->size) return false;
    checkSizeOrResize(alist);
    for (int i = alist->size; i > index; i--) {
        *(alist->array + i) = *(alist->array + i - 1);
    }
    alist->size++;
    *(alist->array + index) = element;
    return true;
}
bool removeAList(struct ArrayList* alist, int index) {
    if (alist == NULL || index < 0 || index >= alist->size) return false;
    for (int i = index + 1; i < alist->size; i++, index++) {
        *(alist->array + index) = *(alist->array + i);
    }
    alist->size--;
    return true;
}
_Bool removeAListRls(struct ArrayList* alist, int index) {
    if (alist == NULL || index < 0 || index >= alist->size) return false;
    free(*(alist->array + index));
    for (int i = index + 1; i < alist->size; i++, index++) {
        *(alist->array + index) = *(alist->array + i);
    }
    alist->size--;
    return true;
}
bool removeAListByPtr(struct ArrayList* alist, void * element) {
    if (alist == NULL || element == NULL) return false;
    int index = findAListByPtr(alist, element);
    return index < 0 ? false: removeAList(alist, index);
}
void* replaceAList(struct ArrayList* alist, int index, void* newElement) {
    if (index < 0 || index >= alist->size) return NULL;
    void* oldPtr = *(alist->array + index);
    *(alist->array + index) = newElement;
    return oldPtr;
}
bool replaceAListRls(struct ArrayList* alist, int index, void * newElement) {
    void* oldPtr = NULL;
    if ((oldPtr = *(alist->array + index)) == NULL) {
        return false;
    }
    free(oldPtr);
    *(alist->array + index) = newElement;
    return true;
}
int findAListByPtr(struct ArrayList* alist, void * element) {
    int index = -1;
    for (int i = 0; i < alist->size; i++) {
        if (*(alist->array + i) == element) {
            index = i;
            break;
        }
    }
    return index;
}
int findAListByCmp(struct ArrayList* alist, _Bool compare(void*, void*), void * element) {
    int index = -1;
    for (int i = 0; i < alist->size; i++) {
        if (compare(*(alist->array + i), element)) {
            index = i;
            break;
        }
    }
    return index;
}
void* getAList(struct ArrayList* alist, int index) {
    if (alist == NULL || index < 0 || index >= alist->size) return NULL;
    return *(alist->array + index);
}
void clearAList(struct ArrayList* alist) {
    if (alist == NULL) return;
    alist->size = 0;
}
void clearAListRls(struct ArrayList* alist) {
    if (alist == NULL) return;
    for (int i = 0; i < alist->size; i++) {
        free(*(alist->array + i));
    }
    alist->size = 0;
}
void destroyAList(struct ArrayList* alist) {
    if (alist == NULL) return;
    free(alist->array);
    free(alist);
}
void destroyAListRls(struct ArrayList* alist) {
    if (alist == NULL) return;
    for (int i = 0; i < alist->size; i++) {
        free(*(alist->array + i));
    }
    destroyAList(alist);
}
void forEachAList(struct ArrayList* alist, void forEach(void * element)) {
    if (alist == NULL) return;
    for (int i = 0; i < alist->size; i++) {
        forEach(*(alist->array + i));
    }
}
// 合并两个有序数组
static void merge(void** array, void** temp, int left, int mid, int right, _Bool esc, int compare(void*, void*)) {
    int i = left;      // 左半部分起始索引
    int j = mid + 1;   // 右半部分起始索引
    int k = left;      // 临时数组索引
    // 比较两个部分，将较小的元素放入临时数组
    while (i <= mid && j <= right) {
        int cmpResult = compare(array[i], array[j]);
        if (esc ? cmpResult <= 0 : cmpResult >= 0) {
            temp[k++] = array[i++];
        } else {
            temp[k++] = array[j++];
        }
    }
    // 复制左半部分剩余元素
    while (i <= mid) {
        temp[k++] = array[i++];
    }
    // 复制右半部分剩余元素
    while (j <= right) {
        temp[k++] = array[j++];
    }
    // 将临时数组的元素复制回原数组
    for (i = left; i <= right; i++) {
        array[i] = temp[i];
    }
}
// 归并排序递归函数
static void mergeSort(void** array, void** temp, int left, int right, _Bool esc, int compare(void*, void*)) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    // 左半部分
    mergeSort(array, temp, left, mid, esc, compare);
    // 右半部分
    mergeSort(array, temp, mid + 1, right, esc, compare);
    // 合并两个有序数组
    merge(array, temp, left, mid, right, esc, compare);
}
void sort(struct ArrayList* alist, _Bool esc, int compare(void*, void*)) {
    if (alist == NULL || alist->size <= 1 || compare == NULL) return;
    // 由于数据量较大，使用归并排序
    // 分配临时数组
    void** temp = malloc(sizeof(void*) * alist->capacity);
    if (!temp) return;
    // 归并排序
    mergeSort(alist->array, temp, 0, alist->size - 1, esc, compare);
    free(temp);
}
void* getMax(struct ArrayList* alist, int compare(void*, void*)) {
    if (alist == NULL || alist->size < 1 || compare == NULL) return NULL;
    if (alist->size == 1) return *(alist->array);
    int maxIndex = 0;
    for (int i = 1; i < alist->size; i++) {
        if (compare(alist->array[i], alist->array[maxIndex]) > 0) {
            maxIndex = i;
        }
    }
    return *(alist->array + maxIndex);
}