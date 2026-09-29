#pragma once

#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

template <typename T>
class RingQueue {
public:
    explicit RingQueue(std::size_t capacity) : capacity_(capacity), head_(0), tail_(0), buffer_(capacity) {

    }

    [[nodiscard]]
    bool push(const T& value) {
        if(full()) {
            return false;
		}
        buffer_[tail_] = value;
        tail_ = (tail_ + 1) % capacity_;
		return true;
    }

    [[nodiscard]]
    bool push(T&& value) {
        if (full()) {
            return false;
        }
		buffer_[tail_] = std::move(value);
		tail_ = (tail_ + 1) % capacity_;
        return true;
    }

    // 队列为空时返回 nullopt。
    [[nodiscard]]
    std::optional<T> pop() {
        if (empty()) {
            return std::nullopt;
        }
        std::optional<T> value = std::move(buffer_[head_]);
        buffer_[head_] = std::nullopt;
        head_ = (head_ + 1) % capacity_;
		return value;
    }

    [[nodiscard]]
    const T* front() const noexcept {
        if (empty()) return nullptr;
        return &buffer_[head_].value();
    }

    [[nodiscard]]
    bool empty() const noexcept {
		if (capacity_ == 0) return true;
		return head_ == tail_ && !buffer_[head_].has_value();
    }

    [[nodiscard]]
    bool full() const noexcept {
        if (capacity_ == 0) return true;
        return head_ == tail_ && buffer_[head_].has_value();
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        if (full()) return capacity_;
		if (empty()) return 0;
        if(tail_ > head_) {
            return tail_ - head_;
        } else {
            return capacity_ - head_ + tail_;
		}
    }

    [[nodiscard]]
    std::size_t capacity() const noexcept {
        return capacity_;
    }

private:
    // 自行设计成员变量
    std::size_t capacity_;
    std::size_t head_;
    std::size_t tail_;
	std::vector<std::optional<T>> buffer_;
};

void testRingQueue();
