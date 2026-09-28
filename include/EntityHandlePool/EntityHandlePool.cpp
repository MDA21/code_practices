#include "EntityHandlePool.h"

EntityAllocator::EntityAllocator(std::uint32_t capacity)
	: capacity_(capacity), aliveCount_(0) {
	free_.reserve(capacity);
	alive_.reserve(capacity);
	generations_.reserve(capacity);

	for(uint32_t i = 0; i < capacity; ++i) {
		free_.push_back(i);
		alive_.push_back(false);
		generations_.push_back(0);
	}
}

std::optional<EntityHandle> EntityAllocator::create() {
	if (free_.empty()) {
		return std::nullopt;
	}

	auto index = free_.back();
	free_.pop_back();
	alive_[index] = true;
	EntityHandle handle{ index, generations_[index] };

	++aliveCount_;
	return handle;
}

bool EntityAllocator::destroy(EntityHandle handle) {
	if(!isAlive(handle)) {
		return false;
	}

	free_.push_back(handle.index);
    //可能会溢出
	generations_[handle.index]++;
	alive_[handle.index] = false;
	--aliveCount_;

	return true;
}

bool EntityAllocator::isAlive(EntityHandle handle) const {
	return handle.index < capacity_ && alive_[handle.index] && generations_[handle.index] == handle.generation;
}

std::size_t EntityAllocator::aliveCount() const noexcept {
	return aliveCount_;
}

void testEntityAllocator() {
    // 1. capacity == 0
    {
        EntityAllocator entities(0);

        assert(!entities.create().has_value());
        assert(entities.aliveCount() == 0);
        assert(!entities.isAlive({ 0, 0 }));
        assert(!entities.destroy({ 0, 0 }));
    }

    // 2. 分配直到耗尽
    {
        EntityAllocator entities(2);

        auto a = entities.create();
        auto b = entities.create();

        assert(a.has_value());
        assert(b.has_value());
        assert(!entities.create().has_value());

        assert(entities.aliveCount() == 2);
    }

    // 3~5. 复用、重复销毁、旧句柄
    {
        EntityAllocator entities(1);

        auto a = entities.create().value();

        assert(entities.isAlive(a));
        assert(entities.destroy(a));

        assert(!entities.isAlive(a));

        // 重复 destroy
        assert(!entities.destroy(a));

        auto b = entities.create().value();

        // index 被复用
        assert(a.index == b.index);

        // generation 必须不同
        assert(a.generation != b.generation);

        // 旧 handle 不能复活
        assert(!entities.isAlive(a));
        assert(!entities.destroy(a));

        assert(entities.isAlive(b));
    }

    // 6. index 越界
    {
        EntityAllocator entities(10);

        EntityHandle invalid{ 999, 0 };

        assert(!entities.isAlive(invalid));
        assert(!entities.destroy(invalid));
    }

    // 7. 交错 create / destroy
    {
        EntityAllocator entities(3);

        auto a = entities.create().value();
        auto b = entities.create().value();

        assert(entities.aliveCount() == 2);

        assert(entities.destroy(a));
        assert(entities.aliveCount() == 1);

        auto c = entities.create().value();
        assert(entities.aliveCount() == 2);

        auto d = entities.create().value();
        assert(entities.aliveCount() == 3);

        assert(entities.destroy(b));
        assert(entities.destroy(c));

        assert(entities.aliveCount() == 1);
        assert(entities.isAlive(d));
    }

    // stale handle
    {
        EntityAllocator entities(1);

        auto h0 = entities.create().value(); // {0, 0}
        assert(entities.destroy(h0));

        auto h1 = entities.create().value(); // {0, 1}
        assert(entities.destroy(h1));

        auto h2 = entities.create().value(); // {0, 2}

        assert(!entities.isAlive(h0));
        assert(!entities.isAlive(h1));
        assert(entities.isAlive(h2));

        assert(!entities.destroy(h0));
        assert(!entities.destroy(h1));

        assert(entities.aliveCount() == 1);
    }
}