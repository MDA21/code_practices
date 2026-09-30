#include "DeferredRemove.h"

#include <cassert>
#include <cstdint>
#include <iostream>

void testDeferredRemove() {
    // 1. maxEntity == 0
    {
        DeferredRemove deferred(0);
        SparseSet<int> set(0);

        assert(deferred.maxEntity() == 0);
        assert(deferred.pendingCount() == 0);
        assert(!deferred.mark(0));
        assert(!deferred.isMarked(0));
        assert(deferred.flush(set) == 0);
        assert(deferred.pendingCount() == 0);
    }

    // 2. 越界 entity
    {
        DeferredRemove deferred(4);
        SparseSet<int> set(4);
        assert(set.insert(1, 10));

        assert(!deferred.mark(4));
        assert(!deferred.mark(100));
        assert(!deferred.isMarked(4));
        assert(deferred.pendingCount() == 0);
        assert(deferred.flush(set) == 0);
        assert(set.contains(1));
        assert(set.size() == 1);
    }

    // 3. mark 不改集合，dense 下标和指针保持有效
    {
        SparseSet<int> set(8);
        assert(set.insert(1, 10));
        assert(set.insert(2, 20));
        assert(set.insert(3, 30));

        const std::uint32_t entity0 = set.entityAt(0);
        const std::uint32_t entity1 = set.entityAt(1);
        const std::uint32_t entity2 = set.entityAt(2);
        const int* component1 = set.get(1);

        DeferredRemove deferred(8);
        assert(deferred.mark(2));
        assert(deferred.isMarked(2));
        assert(!deferred.isMarked(1));
        assert(deferred.pendingCount() == 1);

        assert(set.size() == 3);
        assert(set.entityAt(0) == entity0);
        assert(set.entityAt(1) == entity1);
        assert(set.entityAt(2) == entity2);
        assert(set.get(1) == component1);
        assert(set.get(2) != nullptr);
        assert(*set.get(2) == 20);
    }

    // 4. 重复 mark
    {
        SparseSet<int> set(4);
        assert(set.insert(1, 10));
        assert(set.insert(2, 20));

        DeferredRemove deferred(4);
        assert(deferred.mark(1));
        assert(deferred.mark(1));
        assert(deferred.pendingCount() == 1);
        assert(deferred.isMarked(1));

        assert(deferred.flush(set) == 1);
        assert(!set.contains(1));
        assert(set.contains(2));
        assert(*set.get(2) == 20);
        assert(set.size() == 1);
        assert(deferred.pendingCount() == 0);
        assert(!deferred.isMarked(1));
    }

    // 5. 只删除标记过的，未标记的留下
    {
        SparseSet<int> set(8);
        assert(set.insert(1, 10));
        assert(set.insert(4, 40));
        assert(set.insert(6, 60));

        DeferredRemove deferred(8);
        assert(deferred.mark(4));
        assert(deferred.mark(6));
        assert(deferred.pendingCount() == 2);

        assert(deferred.flush(set) == 2);
        assert(set.contains(1));
        assert(*set.get(1) == 10);
        assert(!set.contains(4));
        assert(!set.contains(6));
        assert(set.size() == 1);
        assert(deferred.pendingCount() == 0);
        assert(!deferred.isMarked(4));
        assert(!deferred.isMarked(6));

        for (std::size_t i = 0; i < set.size(); ++i) {
            const std::uint32_t entity = set.entityAt(i);
            assert(set.contains(entity));
            assert(set.get(entity) == &set.dataAt(i));
        }
    }

    // 6. 空标记 flush
    {
        SparseSet<int> set(4);
        assert(set.insert(0, 7));

        DeferredRemove deferred(4);
        assert(deferred.flush(set) == 0);
        assert(set.contains(0));
        assert(set.size() == 1);
        assert(deferred.flush(set) == 0);
    }

    // 7. 标记了集合里不存在的实体
    {
        SparseSet<int> set(4);
        assert(set.insert(0, 1));

        DeferredRemove deferred(4);
        assert(deferred.mark(2));
        assert(deferred.flush(set) == 0);
        assert(set.contains(0));
        assert(*set.get(0) == 1);
        assert(set.size() == 1);
        assert(deferred.pendingCount() == 0);
        assert(!deferred.isMarked(2));
    }

    // 8. 先标记，flush 前才插入
    {
        SparseSet<int> set(4);
        DeferredRemove deferred(4);

        assert(deferred.mark(1));
        assert(set.insert(1, 5));
        assert(set.contains(1));

        assert(deferred.flush(set) == 1);
        assert(!set.contains(1));
        assert(set.size() == 0);
        assert(deferred.pendingCount() == 0);
    }

    // 9. flush 前已经被手动 remove
    {
        SparseSet<int> set(4);
        assert(set.insert(1, 10));
        assert(set.insert(2, 20));

        DeferredRemove deferred(4);
        assert(deferred.mark(1));
        assert(deferred.mark(2));
        assert(set.remove(1));

        assert(deferred.flush(set) == 1);
        assert(!set.contains(1));
        assert(!set.contains(2));
        assert(set.size() == 0);
        assert(deferred.pendingCount() == 0);
        assert(!deferred.isMarked(1));
        assert(!deferred.isMarked(2));
    }

    // 10. flush 之后可以再次标记同一实体
    {
        SparseSet<int> set(4);
        DeferredRemove deferred(4);

        assert(set.insert(1, 7));
        assert(deferred.mark(1));
        assert(deferred.flush(set) == 1);
        assert(!deferred.isMarked(1));

        assert(set.insert(1, 8));
        assert(deferred.mark(1));
        assert(deferred.isMarked(1));
        assert(deferred.pendingCount() == 1);
        assert(set.contains(1));
        assert(*set.get(1) == 8);

        assert(deferred.flush(set) == 1);
        assert(!set.contains(1));
        assert(deferred.pendingCount() == 0);
    }

    // 11. 标记里夹着不存在的实体
    {
        SparseSet<int> set(8);
        assert(set.insert(0, 1));
        assert(set.insert(3, 3));

        DeferredRemove deferred(8);
        assert(deferred.mark(0));
        assert(deferred.mark(1));
        assert(deferred.mark(3));
        assert(deferred.pendingCount() == 3);

        assert(deferred.flush(set) == 2);
        assert(set.size() == 0);
        assert(!set.contains(0));
        assert(!set.contains(3));
        assert(deferred.pendingCount() == 0);
    }

    std::cout << "All DeferredRemove tests passed.\n";
}
