#include "array_list.h"
#include <stdlib.h>
#include <stdbool.h>
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
static void checkSizeOrResize(ArrayList alist) {
    if (alist->size >= alist->capacity) {
        int newCapacity = (alist->capacity << 1) - alist->capacity;
        void** newArr = realloc(alist->array, sizeof(void*) * newCapacity);
        if (newArr != alist->array) {
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
    if (index < 0 || index >= alist->size) return false;
    for (int i = index + 1; i < alist->size; i++, index++) {
        *(alist->array + index) = *(alist->array + i);
    }
    alist->size--;
    return true;
}
bool removeAListByPtr(ArrayList alist, void * element) {
    int index = findAListByPtr(alist, element);
    return index < 0? false: removeAList(alist, index);
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
    if (index < 0 || index >= alist->size) return NULL;
    return *(alist->array + index);
}
void clearAList(ArrayList alist) {
    alist->size = 0;
}
void clearAListRls(ArrayList alist) {
    for (int i = 0; i < alist->size; i++) {
        free(*(alist->array + i));
    }
    alist->size = 0;
}
void destroyAList(ArrayList alist) {
    free(alist->array);
    free(alist);
}
void destroyAListRls(ArrayList alist) {
    for (int i = 0; i < alist->size; i++) {
        free(*(alist->array + i));
    }
    destroyAList(alist);
}
void forEachAList(ArrayList alist, void forEach(void * element)) {
    for (int i = 0; i < alist->size; i++) {
        forEach(*(alist->array + i));
    }
}