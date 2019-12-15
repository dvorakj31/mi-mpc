#ifndef MPC_SMALL_VECTOR
#define MPC_SMALL_VECTOR

#include <vector>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <memory>


namespace mpc
{

    template <typename T, size_t N>
    class small_vector
    {
        T* data_;
        alignas(T) T data_buf_[N * sizeof(T)];
        size_t capacity_;
        size_t size_;

        void _resize(size_t new_size)
        {
            T* data = (T*)::operator new(new_size * sizeof(T));
            std::uninitialized_move(data_, data_ + size_, data);
            std::destroy(data_, data_ + size_);
            ::operator delete(data_);
            data_ = data;
            capacity_ = new_size;
        }

    public:
        constexpr small_vector() : data_(data_buf_), capacity_(N), size_(0) {}
        constexpr small_vector(std::initializer_list<T> l) {}

        ~small_vector()
        {
            if (data_ != data_buf_)
                delete[] data_;
        }

        constexpr size_t size() const
        {
            return size_;
        }

        constexpr size_t capacity() const
        {
            return capacity_;
        }

        constexpr void reserve(size_t new_size)
        {
            if (new_size <= capacity_)
                return;
            _resize(new_size);
        }

        void push_back(const T& value)
        {
            emplace_back(value);
        }

        void push_back(T&& value)
        {
            emplace_back(value);
        }

        T& operator[](size_t index)
        {
            return data_[index];
        }

        const T& operator[](size_t index) const
        {
            return data_[index];
        }

        template <class... _Args>
        void emplace_back(_Args&&... args)
        {
            if (size_ == capacity_)
                reserve(capacity_ == 0 ? 1 : 2 * capacity_);
            new (data_ + size_) T(std::forward<_Args>(args)...);
            size_++;
        }

        void clear() noexcept
        {
            if (data_ != data_buf_)
            {
                std::destroy(data_, data_ + size_);
                ::operator delete(data_);
            }
            size_ = 0;
        }
    }; // class small vector

} // namespace mpc

#endif // MPC_SMALL_VECTOR
