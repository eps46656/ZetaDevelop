#pragma once

template <typename Elem>
class Buffer {
public:
    Buffer() : buffer_{ nullptr }, size_{ 0 } {}

    ~Buffer() { std::free(this->buffer_); }

    Elem* GetBuffer(size_t size) {
        if (this->size_ < size) {
            std::free(this->buffer_);

            this->buffer_ = (Elem*)std::malloc(sizeof(Elem) * size);
            this->size_ = size;
        }

        return this->buffer_;
    }

private:
    Elem* buffer_;
    size_t size_;
};
