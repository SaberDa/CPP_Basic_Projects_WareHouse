#内存泄漏检查器的设计与实现


## **设计**：

要实现内存泄露的检查，可以从以下两点来考虑：

- 内存泄露产生于new操作进行后没有执行delete
- 最先被创建的对象，其析构函数永远都是最后执行的

对应以上两点，我们可以：

- 重载new运算符
- 创建一个静态对象，用于在原始程序退出的时候才调用这个静态对象的析构函数

这样两个步骤的好处在于：

在无需修改原始代码的前提下，就能进行内存检查。

## **实现**：

可以使用双向链表来实现内存泄漏检查。

原因在于，对于内存检查器来说，并不知道实际代码在什么时候会需要申请内存空间，所以使用线性结构并不够合理，一个动态的结构（链表）是非常便捷的。

而我们在删除内存检查器中的对象时，需要更新整个结构，对于单向链表来说，也是不够便捷的。

---
---

# Memory Leak Checker Design and Implementation


## **Design**:

To achieve memory leak detection, you can consider the following two points:

- The memory leak is caused by no delete after the new operation is performed
- The object created first, its destructor is always executed last

Corresponding to the above two points, we can:

- Overload the new operator
- Create a static object to call the destructor of the static object when the original program exits

The advantages of these two steps are:

The memory check can be performed without modifying the original code.

## **Implementation**:

You can use a doubly linked list to implement memory leak checking.

The reason is that for the memory checker, it is not known when the actual code will need to apply for memory space, so it is not reasonable to use a linear structure. A dynamic structure (linked list) is very convenient.

When we delete the object in the memory checker, we need to update the entire structure, which is not convenient enough for a singly linked list.