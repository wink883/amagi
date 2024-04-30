#include <iostream>

#include "KVector.h"
#include "KVIterator.h"
#include "KAllocator.h"

int main()
{
    KVector<int> vec;

    std::cout << "size of vector: " << vec.size() << std::endl;

    // 1. push_back()
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    // 2. size () 和 capacity()
    std::cout << "size of vector: " << vec.size() << std::endl;
    std::cout << "capacity of vector: " << vec.capacity() << std::endl;

    // 3. 自定义迭代器测试
    for (KVIterator<int> it = vec.begin(); it != vec.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    // 4. insert()
    // 4.1 指定位置插入一个元素
    vec.insert(vec.begin() + 1, 8);
    // 4.2 向自定位置插入 count 个相同值
    vec.insert(vec.begin() + 1, 3, 0);

    std::cout << "size of vector after insert: " << vec.size() << std::endl;
    std::cout << "capacity of vector after insert: " << vec.capacity() << std::endl;

    for (auto it : vec)
    {
        std::cout << it << " ";
    }
    std::cout << std::endl;

    // 5. erase()
    // 5.1 指定位置删除一个元素
    vec.erase(vec.begin());

    std::cout << "size of vector after erase: " << vec.size() << std::endl;
    std::cout << "capacity of vector after erase: " << vec.capacity() << std::endl;

    for (auto it : vec)
    {
        std::cout << it << " ";
    }
    std::cout << std::endl;

    // 5.2 删除区间 [begin, end] 中的元素
    vec.erase(vec.begin(), vec.begin() + 3);

    std::cout << "size of vector after batch erase: " << vec.size() << std::endl;
    std::cout << "capacity of vector after batch erase: " << vec.capacity() << std::endl;

    for (auto it : vec)
    {
        std::cout << it << " ";
    }
    std::cout << std::endl;

    // 6. 拷贝构造
    KVector<int> v = vec;

    // clear()
    vec.clear();

    std::cout << "size of vector after clear: " << vec.size() << std::endl;
    std::cout << "capacity of vector after clear: " << vec.capacity() << std::endl;

    std::cout << "copied object: " << std::endl;

    for (auto it : v)
    {
        std::cout << it << " ";
    }
    std::cout << std::endl;

    // 7. at() / []
    std::cout << v.at(1) << std::endl;
    std::cout << v[2] << std::endl;

    // 8. isEmpty()
    if (!v.isEmpty())
    {
        for (auto it : v)
        {
            std::cout << it << " ";
        }
    }
    std::cout << std::endl;

    std::cout << "toString: " << std::endl;

    // 9. toString
    std::cout << v.toString() << std::endl;

    std::cout << "structure function test: " << std::endl;
    // 10. 测试五种构造方式
    // 10.1 普通构造
    KVector<int> v1;
    std::cout << "size of v1: " << v1.size() << std::endl;
    std::cout << "capacity of v1: " << v1.capacity() << std::endl;

    // 10.2 确定容量和初始值方式构造
    KVector<int> v2(10, 5);
    std::cout << "size of v2: " << v2.size() << std::endl;
    std::cout << "capacity of v2: " << v2.capacity() << std::endl;
    for (auto iter : v2)
    {
        std::cout << iter << " ";
    }
    std::cout << std::endl;


    // 10.3 拷贝构造
    KVector<int> v3(v2);
    std::cout << "size of v3: " << v3.size() << std::endl;
    std::cout << "capacity of v3: " << v3.capacity() << std::endl;
    for (auto iter : v3)
    {
        std::cout << iter << " ";
    }
    std::cout << std::endl;

    // 10.4 移动构造
    KVector<int> v4(std::move(v3));
    std::cout << v4.toString() << std::endl;

    // 10.5 列表初始化构造
    KVector<int> vector{ 1, 3, 5, 7, 9 };
    vector.push_back(11);

    // 自定义空间分配器的使用
    KVector<int, KAllocator<int>> vector1{ 2, 4, 6, 8, 10 };
    vector1.push_back(12);

    KVector<int, KAllocator<int>> vector2(15, 6);
    vector2.push_back(7);

    std::cout << vector.toString() << std::endl;
    std::cout << vector1.toString() << std::endl;
    std::cout << vector2.toString() << std::endl;

    std::cout << "size of vector: " << vector.size() << std::endl;
    std::cout << "capacity of vector: " << vector.capacity() << std::endl;

    std::cout << "size of vector1: " << vector1.size() << std::endl;
    std::cout << "capacity of vector1: " << vector1.capacity() << std::endl;

    std::cout << "size of vector2: " << vector2.size() << std::endl;
    std::cout << "capacity of vector2: " << vector2.capacity() << std::endl;

    // 11. 移动赋值运算符
    KVector<int, KAllocator<int>> vector3;
    vector3 = std::move(vector2);
    std::cout << vector2.toString() << std::endl;

    // 12. 其他类型容器的构造测试
    KVector<long> lv;
    KVector<double, KAllocator<double>> dv(6, 5.5);
    KVector<char, KAllocator<char>> cv{ 'a', 'b', 'c', 'd' };
    KVector<double, KAllocator<double>> dv1(dv);
    KVector<char, KAllocator<char>> cv1 = std::move(cv);

    return 0;
}