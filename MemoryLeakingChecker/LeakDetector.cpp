#include <iostream>
#include <cstring>

// Define the _DEBUG_NEW_ macro here
// So that the new operator is no longer overloaded in
// this implement file
// So as to prevent compilation conflicts
#define __NEW_OVERLOAD_IMPLEMENTATION__
#include "LeakDetector.hpp"

typedef struct _MemoryList {
    struct  _MemoryList *next, *prev;
    size_t  size;           // Size for applying memory
    bool    isArray;        // Is or not applying array
    char    *file;          // Store the current file
    unsigned int line;      // Store the current line
} _MemoryList;

static unsigned long _memory_allocated = 0;     // Store the size of unreleasing memory

static _MemoryList _root = {
    &_root, &_root,     // The first element's next and prev pointers are pointing itself
    0,                  
    false,
    NULL,
    0
}; 

unsigned int _leak_detector::callCount = 0;

/*
 * Allocate the memory from the head of _MemoryList
*/
void* AllocateMemory(size_t _size, bool _array, char *_file, unsigned _line) {
    // Calculate the new memory size
    size_t newSize = sizeof(_MemoryList) + _size;

    // We use the malloc to allocate the memory due to the new has been overloaded
    _MemoryList *newElem = (_MemoryList*)malloc(newSize);

    newElem->next = _root.next;
    newElem->prev = &_root;
    newElem->size = _size;
    newElem->isArray = _array;
    newElem->file = NULL;

    // Store the file if it exists
    if (_file) {
        newElem->file = (char *)malloc(strlen(_file) + 1);
        strcpy(newElem->file, _file);
    }

    // Store line
    newElem->line = _line;

    // Update list
    _root.next->prev = newElem;
    _root.next = newElem;

    // Recode the unreleasing memory number
    _memory_allocated += _size;

    // Return the allocated memory
    // Transform the newElem to char* to control the pointer move 1 byte one time
    return (char*)newElem + sizeof(_MemoryList);
}

/*
 * Delete
*/
void DeleteMemory(void *_ptr, bool _array) {
    // Return the begin of MemoryList
    _MemoryList *currentElem = (_MemoryList*)((char*)_ptr - sizeof(_MemoryList));

    if (currentElem->isArray != _array) return;

    // Update list
    currentElem->prev->next = currentElem->next;
    currentElem->next->prev = currentElem->prev;
    _memory_allocated -= currentElem->size;

    // Release the memory for storing file 
    if (currentElem->file) free(currentElem->file);
    free(currentElem);
}

/*
 * Overloaded new and delete operation
*/
void* operator new(size_t _size) {
    return AllocateMemory(_size, false, NULL, 0);
}
void* operator new[](size_t _size) {
    return AllocateMemory(_size, true, NULL, 0);
}
void* operator new(size_t _size, char *_file, unsigned int _line) {
    return AllocateMemory(_size, false, _file, _line);
}
void* operator new[](size_t _size, char *_file, unsigned int _line) {
    return AllocateMemory(_size, true, _file, _line);
}
void operator delete(void *_ptr) noexcept {
    DeleteMemory(_ptr, false);
}
void operator delete[](void *_ptr) noexcept {
    DeleteMemory(_ptr, true);
}

/*
 * '_leak_detector::LeakDetector()' will be called when destruct the 
 * 'static _leak_detector _exit_counter'. At this moment, all of the other
 * objects will be released. If there is mempry lefting in 'static _MemoryList _root',
 * which means the memory leaking is occuring. Then we just traverse the '_root' and
 * we will get the result
*/
unsigned int _leak_detector::LeakDetector(void) noexcept {
    unsigned int count = 0;
    // Traverse the whole list. If there exists the memory leaking, then
    // '_LeakRoot' will always not point to itself
    _MemoryList *ptr = _root.next;
    while (ptr && ptr != &_root) {
        // Print the message of the memory leaking, such as size or position
        if (ptr->isArray) std::cout << "leak[] ";
        else std::cout << "leak ";
        std::cout << ptr << " size " << ptr->size;
        if (ptr->file) std::cout << " (located in " << ptr->file << " line " << ptr->line << ")";
        else std::cout << " (Cannot find position)";
        std::cout << std::endl;
        ++count;
        ptr = ptr->next;
    }
    if (count) {
        std::cout << "Total " << count << " leaks, size is " << _memory_allocated << " bytes." << std::endl;
    }
    return count;
}