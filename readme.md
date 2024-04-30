# KVector

## 1. 概述

KVector 是一个 C++ 模板类，实现了类似于 std::vector 的动态数组。它提供了动态内存分配、大小调整、插入、删除等常见操作的功能。其底层数据结构非常简单，就是使用 3 个迭代器（可以理解成指针）来表示的：

```C++
template<typename T, typename Allocator = std::allocator<T>>
class KVector
{
private:
    T* m_data;
    T* m_avail;
    T* m_limit;
    allocator_type m_alloc; // 空间（内存）分配器
}
```

其中,`m_data` 指向的是 `KVctor` 容器对象的起始字节位置；`m_avail` 指向当前最后一个元素的末尾字节；`m_limit` 指向整个 `KVector` 容器所占用内存空间的末尾字节。下图 演示了以上这 3 个迭代器分别指向的位置。

![image-20240427095746913](https://chery-lx.oss-cn-hangzhou.aliyuncs.com/66aac9bb-9f70-43c0-9170-b5d40aa8810e.png)

通过这 3 个迭代器，就可以表示出一个已容纳 2 个元素，容量为 5 的 vector 容器。通过灵活运用这 3 个迭代器，vector 容器可以轻松的实现诸如首尾标识、大小、容器、空容器判断等几乎所有的功能。

## 2. 特性

- **动态数组**：KVector 提供动态内存分配，允许在添加或删除元素时自动调整大小。它使用指针管理动态分配的内存，当元素数量超出当前容量时，会重新分配更大的内存空间，并将元素复制到新的内存位置。
- **迭代器支持**：它支持基于迭代器的元素访问，允许遍历和操作容器中的元素。通过实现 `begin()` 和 `end()` 方法，KVector 返回指向容器第一个元素和最后一个元素之后位置的迭代器。
- **移动语义**：KVector 实现了移动构造函数和移动赋值运算符，以便高效移动元素。移动构造函数允许将已有的资源（如内存）转移给新对象，而不是进行深度复制。移动赋值运算符允许将右值引用参数的内容转移到左值引用参数。
- **自定义分配器**：KVector 允许您指定自定义分配器以进行内存管理，提供了灵活性和定制选项。您可以在实例化 KVector 时指定自定义的分配器类型，以便更好地控制内存分配和释放。
- **错误处理**：它包括错误处理机制，以优雅地处理内存分配错误。当内存分配失败时，KVector 抛出 `std::bad_alloc` 异常，以便在运行时捕获并处理错误。
- **字符串表示**：提供 `toString()` 方法，将 KVector 对象转换为字符串表示形式。这使得可以方便地打印、输出或调试 KVector 对象的内容。

## 3. 主要支持操作

| 函数                                                         | 功能描述                                                     |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| iterator begin()                                             | 返回一个迭代器，指向 `KVector` 对象的第一个元素              |
| iterator end()                                               | 返回一个迭代器，指向 `KVector` 对象的最后一个元素            |
| size_type size()                                             | 返回`KVector`对象的大小                                      |
| size_type capacity()                                         | 返回`KVector`对象的容量                                      |
| void push_back(const T & value)                              | 向容器末尾插入一个值为`value`的元素                          |
| T& at(size_t idx)                                            | 返回`KVector`对象下标为`idx`的元素                           |
| bool isEmpty()                                               | 判断`KVector`对象是否为空                                    |
| void clear()                                                 | 清空容器，但并不释放`KVector`的内存                          |
| iterator erase(iterator position)                            | 从指定位置删除一个元素，并返回指向删除元素之后的位置的迭代器。 |
| iterator erase(iterator first, iterator last)                | 从指定范围内删除元素，并返回最后一个被删除元素之后的位置的迭代器 |
| iterator insert(iterator position, const T& value)           | 将 `value` 插入到 `position` 所指向的位置之前，并返回指向新插入元素的迭代器 |
| iterator insert(iterator position, size_type count, const T& value) | 将`count`个值为`value`的元素插入到`position`所指向的位置之前，并返回指向第一个新插入元素的迭代器 |
| std::string toString()                                       | 遍历 `KVector` 中的所有元素，将每个元素转换为字符串，并使用空格分隔，最后返回表示整个 `KVector` 的字符串。 |

## 4. 实现细节

### 4.1 自动扩展容量

另外需要指明的是，当 `KVector` 的大小和容量相等（`size==capacity`）也就是满载时，如果再向其添加元素，那么 `KVector` 就需要扩容。`KVector` 容器扩容的过程需要经历以下 3 步：

1. 完全弃用现有的内存空间，重新申请更大的内存空间，每次总是申请比所需要更多的容量，即每次扩展到原来容量的2倍，以避免频繁申请内存。
2. 将旧内存空间中的数据，按原有顺序移动到新的内存空间中；
3. 最后将旧的内存空间释放。

![image-20240427103509159](https://chery-lx.oss-cn-hangzhou.aliyuncs.com/62b53fd7-ef22-4d9e-abb3-78b2b397579d.png)

具体实现如下：

```C++
template<class T, class Allocator>
void KVector<T, Allocator>::grow()
{
    size_type newCapacity = std::max(static_cast<ptrdiff_t>(2 * (m_limit - m_data)), static_cast<ptrdiff_t>(1));
    // 1. 申请新的空间
    T* newData = m_alloc.allocate(newCapacity);
    // 2. 将旧内存空间中的数据，按原有顺序移动到新的内存空间中；
    T* newAvail = std::uninitialized_copy(m_data, m_avail, newData);
    // 3. 将旧的内存空间释放
    uncreate();

    m_data = newData;
    m_avail = newAvail;
    m_limit = m_data + newCapacity;
}
```



### 4.2 自动缩小容量

自动缩小容量的实现和扩展容量的逻辑相似，即当容器中`size() < capacity() / 2` 时，自动缩小容量到原来容量的一半，需要注意的是，缩小过程中至少保留一个容量大小。

![image-20240429143604716](https://chery-lx.oss-cn-hangzhou.aliyuncs.com/e83eac79-2c80-468e-b40f-1b10ca4acf24.png)



### 4.3自定义容器迭代器

实现了 `* ++ -- +=` 等操作符的重载，使其能像`STL`中容器迭代器一样实现对自定义容器`KVector`对象的遍历。

```C++
template<typename T>
class KVIterator
{
public:
    typedef T value_type;
    typedef T& reference;
    typedef T* pointer;
    typedef std::ptrdiff_t difference_type;
    typedef std::random_access_iterator_tag iterator_category;

    explicit KVIterator(pointer m_ptr) : m_ptr(m_ptr)
    {

    }

    reference operator*() const
    {
        return *m_ptr;
    }

    pointer operator->() const
    {
        return m_ptr;
    }

    KVIterator& operator++()
    {
        ++m_ptr;
        return *this;
    }
    
    ......
        
private:
    pointer m_ptr;
}
```



### 4.4 自定义空间分配器

自定义空间分配器`KVAllocator`是根据`SGI-STL中二级空间配置器`的思想来设计的，其中以`128bytes`作为大小内存块的界限，即当需要申请的内存 `>= 128bytes` 时定义为大内存，否则定义为小内存。其分配思想如下：

- 一级空间分配器：简单的对`molloc()` 和`free()`的封装，负责分配大内存块
- 二级空间分配器：使用内存池实现，负责分配小内存块，避免频繁的向系统申请小内存块，导致内存碎片化

![image-20240429151558738](https://chery-lx.oss-cn-hangzhou.aliyuncs.com/49a4c4c0-4801-4909-940a-51aa51aa19c5.png)

#### 二级分配器实现:

- 申请：二级空间分配器使用内存池来管理碎片化内存，每次首先根据申请的内存大小向`freeList`相应桶中查找该桶下面是否挂有内存块，有则直接返回；没有则向内存池申请内存，向用户返回一小块内存后将申请的余量内存填充到该桶下面，方便下次需要时直接从该桶中返回，而不需要频繁的向内存池申请内存。当内存池中内存枯竭时，则需要向系统中申请内存存到内存池中。
- 释放：向谁申请的就归还给谁，方便下次直接从桶中返回内存。

![image-20240429230644834](https://chery-lx.oss-cn-hangzhou.aliyuncs.com/3f212f1b-5c08-4054-ab82-dc69a3491069.png)

![image-20240429230738624](https://chery-lx.oss-cn-hangzhou.aliyuncs.com/61a8d235-16c1-4a0f-9bc5-8de7298fd45b.png)



二级分配器流程图:

![image-20240429175930658](https://chery-lx.oss-cn-hangzhou.aliyuncs.com/bf0effc4-7552-49fb-ad7c-53d8c543571b.png)

```C++
class Allocator
{
public:
    void *allocate(size_t size);  //申请 size 字节内存
    void deallocate(void *p, size_t size); //释放 p 类型的 size 字节内存
    void *refill(size_t nBytes); //将内存块添加到连接的链表里
    char *chunkAlloc(size_t size, int &nObjs); //从系统中申请内存，并且将超出部分添加到内存池之中
    static size_t roundUp(size_t n); //将 n 向上对齐为8bytes的整数倍
    
private:
    static const int kMaxBytes = 128; //大小内存块分界线
    static const int kAlign = 8; //对齐字节数
    static const int kNumFreeLists = kMaxBytes / kAlign; //维护的freeList桶数量

    union obj
    {
        union obj *next;
        char clientData[1];
    };

    obj *volatile freeList[kNumFreeLists] = {nullptr};

    static char *startFree; // 内存池的起始地址
    static char *endFree; //内存池的结束地址
    static size_t heapSize; // 内存池总共的大小
}
```

#### 空间分配器的二次封装：

```C++
template<typename T>
class KAllocator {
public:
    typedef size_t size_type;
    typedef T* pointer;
public:
    static pointer allocate(size_type n); // 申请内存n个T类型的内存

    static pointer allocate(); // 申请一个T类型的内存

    static void deallocate(pointer p, size_type n); // 释放n个T类型的内存

    static void deallocate(pointer p); // 释放一个T类型的内存

    void construct(pointer p, const T& val); // 在内存中构造一个T类型对象

    void destroy(pointer p); // 销毁内存
```



## 5. 使用方法

要在您的项目中使用 `KVector`，请包含 `KVector.h` 头文件，并使用所需的元素类型和（可选）自定义分配器类型实例化 `KVector` 模板类。然后，您可以使用提供的方法进行元素操作和管理。

```cpp
#include "KVector.h"

int main() 
{
    // 实例化一个整数类型的 KVector
    KVector<int> Vec;
    
    // 使用自定义的空间分配器来构造对象
    KVector<int, KAllocator<int>> vec1;
    
    // 确定容量和初始值构造对象
    KVector<int, KAllocator<int>> vec2(3, 0);
    
    // 初始化列表构造
    KVector<int, KAllocator<int>> vec3{1, 2, 3, 4,5};

    // 向容器中添加元素
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    
    // 向容器插入元素
    vec.insert(vec.begin(), 4);
    vec.insert(vec.begin(), 4, 9);
    
   // 删除容器中的元素
    vec.erase(vec.begin());
    vec.erase(vec.begin(), vec.begin() + 2);
    
    // 使用索引访问元素
    vec.at(2);
    vec[3];
    
    // 容器大小、容量
    vec.size();
    vec.capacity();

    // 使用迭代器遍历元素
    for (auto it = myVector.begin(); it != myVector.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    return 0;
}
```
