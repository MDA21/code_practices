#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include <optional>

// 实体编号是 [0, maxEntity) 内的稠密 id。
// 本题不处理 generation，只训练 sparse / dense 映射。
template <typename T>
class SparseSet {
public:
    explicit SparseSet(std::uint32_t maxEntity) :maxEntity_(maxEntity), sparse_(maxEntity,std::nullopt){
        
    }

    // entity >= maxEntity 时返回 false。
    // 不存在则插入；已存在则覆盖值。成功返回 true。
    [[nodiscard]]
    bool insert(std::uint32_t entity, const T& value) {
        if (contains(entity)) {
            T* component = get(entity);
            *component = value;
            return true;
        }
        if (entity >= maxEntity_) return false;

        dense_.push_back(std::pair<std::uint32_t, T>(entity, value));
        sparse_[entity] = static_cast<std::uint32_t>(dense_.size() - 1);
        return true;
    }

    [[nodiscard]]
    bool insert(std::uint32_t entity, T&& value) {
        if (contains(entity)) {
            T* component = get(entity);
            *component = std::move(value);
            return true;
        }
        if (entity >= maxEntity_) return false;

        dense_.push_back(std::pair<std::uint32_t, T>(entity, std::move(value)));
        sparse_[entity] = static_cast<std::uint32_t>(dense_.size() - 1);
        return true;
    }

    // 仅当 entity 当前存在时删除并返回 true。
    bool remove(std::uint32_t entity) {
        if (!contains(entity)) return false;
        const std::size_t denseIndex = sparse_[entity].value();
        const std::uint32_t movedEntity = dense_.back().first;
        std::swap(dense_[denseIndex], dense_.back());
        dense_.pop_back();
        sparse_[entity] = std::nullopt;
        if (entity != movedEntity) {
            sparse_[movedEntity] = static_cast<std::uint32_t>(denseIndex);
        }
        return true;
    }

    [[nodiscard]]
    bool contains(std::uint32_t entity) const {
        if (entity >= maxEntity_) return false;
        return sparse_[entity].has_value();
    }

    [[nodiscard]]
    T* get(std::uint32_t entity) {
        if (entity >= maxEntity_ || !sparse_[entity].has_value()) return nullptr;
        return &dense_[sparse_[entity].value()].second;
    }

    [[nodiscard]]
    const T* get(std::uint32_t entity) const {
        if (entity >= maxEntity_ || !sparse_[entity].has_value()) return nullptr;
        return &dense_[sparse_[entity].value()].second;
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return dense_.size();
    }

    [[nodiscard]]
    std::uint32_t maxEntity() const noexcept {
        return maxEntity_;
    }

    // dense 区：下标范围 [0, size())，无空洞。
    // 仅插入、从未删除时，顺序与插入顺序一致。
    // 发生过 remove 后，顺序可以变化，但每个现存 entity 恰好出现一次。
    [[nodiscard]]
    std::uint32_t entityAt(std::size_t denseIndex) const {
        return dense_[denseIndex].first;
    }

    [[nodiscard]]
    T& dataAt(std::size_t denseIndex) {
        return dense_[denseIndex].second;
    }

    [[nodiscard]]
    const T& dataAt(std::size_t denseIndex) const {
        return dense_[denseIndex].second;
    }

private:
    // 自行设计成员变量
    std::uint32_t maxEntity_;
    std::vector<std::pair<std::uint32_t, T>> dense_;
    std::vector<std::optional<std::uint32_t>> sparse_;
};

void testSparseSet();
