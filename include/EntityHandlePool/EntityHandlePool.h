#pragma once
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include <cassert>

struct EntityHandle {
    std::uint32_t index;
    std::uint32_t generation;

    bool operator==(const EntityHandle&) const = default;
};

class EntityAllocator {
public:
    explicit EntityAllocator(std::uint32_t capacity);

    // 成功时返回新句柄；容量耗尽时返回 nullopt。
    [[nodiscard]]
    std::optional<EntityHandle> create();

    // 仅当 handle 当前有效时才销毁并返回 true。
    // 重复销毁、越界句柄和过期句柄均返回 false。
    bool destroy(EntityHandle handle);

    [[nodiscard]]
    bool isAlive(EntityHandle handle) const;

    [[nodiscard]]
    std::size_t aliveCount() const noexcept;

private:
    // 自行设计成员变量
	std::uint32_t capacity_;
    std::size_t aliveCount_{ 0 };
    std::vector<std::uint32_t> free_;
	std::vector<std::uint32_t> generations_;
    std::vector<std::uint8_t> alive_;
};

void testEntityAllocator();