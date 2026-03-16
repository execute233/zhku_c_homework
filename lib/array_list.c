#include "array_list.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

ArrayList createAListDefault() {
    return createAList(10);
}
ArrayList createAList(int capacity) {
    ArrayList list = malloc(sizeof(ArrayList));
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
static void checkSizeOrResize(ArrayList alist) {
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
int addAList(ArrayList alist, void * element) {
    checkSizeOrResize(alist);
    int addIndex = alist->size;
    *(alist->array + addIndex) = element;
    alist->size++;
    return addIndex;
}
bool insertAList(ArrayList alist,void * element, int index) {
    if (index < 0 || index > alist->size) return false;
    checkSizeOrResize(alist);
    for (int i = alist->size; i > index; i--) {
        *(alist->array + i) = *(alist->array + i - 1);
    }
    alist->size++;
    *(alist->array + index) = element;
    return true;
}
bool removeAList(ArrayList alist, int index) {
    if (alist == NULL || index < 0 || index >= alist->size) return false;
    for (int i = index + 1; i < alist->size; i++, index++) {
        *(alist->array + index) = *(alist->array + i);
    }
    alist->size--;
    return true;
}
_Bool removeAListRls(ArrayList alist, int index) {
    if (alist == NULL || index < 0 || index >= alist->size) return false;
    free(*(alist->array + index));
    for (int i = index + 1; i < alist->size; i++, index++) {
        *(alist->array + index) = *(alist->array + i);
    }
    alist->size--;
    return true;
}
bool removeAListByPtr(ArrayList alist, void * element) {
    if (alist == NULL || element == NULL) return false;
    int index = findAListByPtr(alist, element);
    return index < 0 ? false: removeAList(alist, index);
}
void* replaceAList(ArrayList alist, int index, void* newElement) {
    if (index < 0 || index >= alist->size) return NULL;
    void* oldPtr = *(alist->array + index);
    *(alist->array + index) = newElement;
    return oldPtr;
}
bool replaceAListRls(ArrayList alist, int index, void * newElement) {
    void* oldPtr = NULL;
    if ((oldPtr = *(alist->array + index)) == NULL) {
        return false;
    }
    free(oldPtr);
    *(alist->array + index) = newElement;
    return true;
}
int findAListByPtr(ArrayList alist, void * element) {
    int index = -1;
    for (int i = 0; i < alist->size; i++) {
        if (*(alist->array + i) == element) {
            index = i;
            break;
        }
    }
    return index;
}
int findAListByCmp(ArrayList alist, _Bool compare(void*, void*), void * element) {
    int index = -1;
    for (int i = 0; i < alist->size; i++) {
        if (compare(*(alist->array + i), element)) {
            index = i;
            break;
        }
    }
    return index;
}
void* getAList(ArrayList alist, int index) {
    if (alist == NULL || index < 0 || index >= alist->size) return NULL;
    return *(alist->array + index);
}
void clearAList(ArrayList alist) {
    if (alist == NULL) return;
    alist->size = 0;
}
void clearAListRls(ArrayList alist) {
    if (alist == NULL) return;
    for (int i = 0; i < alist->size; i++) {
        free(*(alist->array + i));
    }
    alist->size = 0;
}
void destroyAList(ArrayList alist) {
    if (alist == NULL) return;
    free(alist->array);
    free(alist);
}
void destroyAListRls(ArrayList alist) {
    if (alist == NULL) return;
    for (int i = 0; i < alist->size; i++) {
        free(*(alist->array + i));
    }
    destroyAList(alist);
}
void forEachAList(ArrayList alist, void forEach(void * element)) {
    if (alist == NULL) return;
    for (int i = 0; i < alist->size; i++) {
        forEach(*(alist->array + i));
    }
}
void sort(ArrayList alist, _Bool esc, int compare(void*, void*)) {
    if (alist == NULL || alist->size <= 1 || compare == NULL) return;
    // 使用冒泡排序
    for (int i = 0; i < alist->size - 1; i++) {
        _Bool isSorted = false;
        for (int j = i + 1; j < alist->size; j++) {
            if (esc ? compare(alist->array[i], alist->array[j]) > 0 : compare(alist->array[i], alist->array[j]) < 0) {
                void* tmp = alist->array[i];
                alist->array[i] = alist->array[j];
                alist->array[j] = tmp;
                isSorted = true;
            }
        }
        if (!isSorted) break;
    }
}
void* max(ArrayList alist, int compare(void*, void*)) {
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