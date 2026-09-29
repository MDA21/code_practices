#include "SparseSet.h"

#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

struct Tracked {
    static inline int alive = 0;

    int value = 0;

    explicit Tracked(int v = 0)
        : value(v) {
        ++alive;
    }

    ~Tracked() {
        --alive;
    }

    Tracked(const Tracked&) = delete;
    Tracked& operator=(const Tracked&) = delete;

    Tracked(Tracked&& other) noexcept
        : value(other.value) {
        ++alive;
        other.value = -1;
    }

    Tracked& operator=(Tracked&& other) noexcept {
        if (this != &other) {
            value = other.value;
            other.value = -1;
        }
        return *this;
    }
};

static std::vector<std::uint32_t> collectEntities(const SparseSet<int>& set) {
    std::vector<std::uint32_t> entities;
    entities.reserve(set.size());
    for (std::size_t i = 0; i < set.size(); ++i) {
        entities.push_back(set.entityAt(i));
    }
    return entities;
}

void testSparseSet() {
    // 1. maxEntity == 0
    {
        SparseSet<int> set(0);

        assert(set.maxEntity() == 0);
        assert(set.size() == 0);
        assert(!set.contains(0));
        assert(set.get(0) == nullptr);
        assert(!set.insert(0, 1));
        assert(!set.remove(0));
    }

    // 2. 越界 entity
    {
        SparseSet<int> set(4);

        assert(!set.insert(4, 10));
        assert(!set.insert(100, 10));
        assert(!set.contains(4));
        assert(set.get(4) == nullptr);
        assert(!set.remove(4));
        assert(set.size() == 0);
    }

    // 3. 插入、点查、dense 插入顺序
    {
        SparseSet<int> set(8);

        assert(set.insert(2, 20));
        assert(set.insert(5, 50));
        assert(set.insert(1, 10));

        assert(set.size() == 3);
        assert(set.contains(2));
        assert(set.contains(5));
        assert(set.contains(1));
        assert(!set.contains(0));
        assert(!set.contains(3));

        assert(set.get(2) != nullptr);
        assert(*set.get(2) == 20);
        assert(*set.get(5) == 50);
        assert(*set.get(1) == 10);
        assert(set.get(0) == nullptr);

        assert(set.entityAt(0) == 2);
        assert(set.entityAt(1) == 5);
        assert(set.entityAt(2) == 1);
        assert(set.dataAt(0) == 20);
        assert(set.dataAt(1) == 50);
        assert(set.dataAt(2) == 10);
    }

    // 4. 覆盖已存在的值
    {
        SparseSet<int> set(4);

        assert(set.insert(1, 10));
        assert(set.insert(1, 99));

        assert(set.size() == 1);
        assert(*set.get(1) == 99);
        assert(set.entityAt(0) == 1);
        assert(set.dataAt(0) == 99);
    }

    // 5. 删除不存在 / 重复删除
    {
        SparseSet<int> set(4);

        assert(!set.remove(1));
        assert(set.insert(1, 10));
        assert(set.remove(1));
        assert(!set.contains(1));
        assert(set.get(1) == nullptr);
        assert(set.size() == 0);
        assert(!set.remove(1));
    }

    // 6. 删除后其余 entity 仍能点查，dense 无空洞
    {
        SparseSet<int> set(8);

        assert(set.insert(1, 10));
        assert(set.insert(3, 30));
        assert(set.insert(6, 60));
        assert(set.insert(2, 20));

        assert(set.remove(3));
        assert(!set.contains(3));
        assert(set.get(3) == nullptr);
        assert(set.size() == 3);

        assert(set.contains(1));
        assert(set.contains(6));
        assert(set.contains(2));
        assert(*set.get(1) == 10);
        assert(*set.get(6) == 60);
        assert(*set.get(2) == 20);

        auto entities = collectEntities(set);
        assert(entities.size() == 3);

        bool seen1 = false;
        bool seen6 = false;
        bool seen2 = false;
        for (auto entity : entities) {
            assert(entity != 3);
            if (entity == 1) {
                seen1 = true;
                assert(*set.get(entity) == 10);
            } else if (entity == 6) {
                seen6 = true;
                assert(*set.get(entity) == 60);
            } else if (entity == 2) {
                seen2 = true;
                assert(*set.get(entity) == 20);
            } else {
                assert(false);
            }
        }
        assert(seen1 && seen6 && seen2);

        for (std::size_t i = 0; i < set.size(); ++i) {
            const auto entity = set.entityAt(i);
            assert(set.contains(entity));
            assert(set.get(entity) == &set.dataAt(i));
        }
    }

    // 7. 删最后一个剩下的元素
    {
        SparseSet<int> set(3);

        assert(set.insert(2, 20));
        assert(set.remove(2));
        assert(set.size() == 0);
        assert(!set.contains(2));
        assert(set.get(2) == nullptr);
    }

    // 8. const 查询
    {
        SparseSet<int> set(4);
        assert(set.insert(0, 7));

        const SparseSet<int>& constSet = set;
        assert(constSet.contains(0));
        assert(constSet.get(0) != nullptr);
        assert(*constSet.get(0) == 7);
        assert(constSet.dataAt(0) == 7);
        assert(constSet.entityAt(0) == 0);
    }

    // 9. 只移动类型
    {
        SparseSet<std::string> set(4);

        assert(set.insert(2, std::string("shadow")));
        assert(set.insert(0, std::string("gbuffer")));
        assert(*set.get(2) == "shadow");

        assert(set.remove(2));
        assert(set.get(2) == nullptr);
        assert(*set.get(0) == "gbuffer");
        assert(set.size() == 1);
    }

    // 10. remove 后对象离开集合
    {
        assert(Tracked::alive == 0);

        {
            SparseSet<Tracked> set(4);
            assert(set.insert(1, Tracked{ 42 }));
            assert(Tracked::alive == 1);

            assert(set.remove(1));
            assert(Tracked::alive == 0);
            assert(set.size() == 0);
        }

        assert(Tracked::alive == 0);
    }

    std::cout << "All SparseSet tests passed.\n";
}
