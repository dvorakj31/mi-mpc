#ifndef MPC_SMALL_VECTOR
#define MPC_SMALL_VECTOR

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

	public:
		using value_type = T;
		using reference = T&;
		using const_reference = const T&;
		using pointer = T*;
		using const_pointer = const T*;
		using iterator = T*;
		using const_iterator = const T*;


	private:
		T* data_;
		typename std::aligned_storage<sizeof(T), alignof(T)>::type data_buf_[N];
		size_t capacity_;
		size_t size_;

		void _resize(size_t new_size)
		{
			T* data = (T*)::operator new(new_size * sizeof(T));
			size_t i = 0;
			try
			{
				for (; i < size_; i++)
					new (data + i) T(std::move_if_noexcept(data_[i]));
			}
			catch (...)
			{
				std::destroy(data_, data_ + i);
				throw;
			}
			clear();
			if (data_ != reinterpret_cast<T*>(data_buf_))
				::operator delete(data_);
			data_ = data;
			capacity_ = new_size;
			size_ = i;
		}

    public:
		constexpr small_vector() : data_(reinterpret_cast<T*>(data_buf_)), capacity_(N), size_(0) {}
		constexpr small_vector(std::initializer_list<T> l)
		{
			for (auto& item : l)
				push_back(item);
		}

		small_vector(const small_vector& other)
		{
			if (other.size_ == 0)
				return;
			data_ = (T*)::operator new(other.capacity_ * sizeof(T));
			capacity_ = other.capacity_;
			try { std::uninitialized_copy(other.data_, other.data_ + other.size_, data_); }
			catch (...) { ::operator delete(data_); throw; }
			size_ = other.size_;
		}

		~small_vector() {
			clear();
			if (data_ != reinterpret_cast<T*>(data_buf_))
				::operator delete(data_);
		}

		small_vector(small_vector&& other) noexcept :
			data_(other.data_), capacity_(other.capacity_), size_(other.size_)
		{			other.data_ = nullptr; other.capacity_ = other.size_ = 0;		}		small_vector& operator=(const small_vector& other)
		{
			if (capacity() < other.size())
			{
				small_vector temp(other);
				swap(temp);
			}
			else if (this != &other)
			{
				clear(); std::uninitialized_copy(other.data_, other.data_ + other.size_, data_);
				size_ = other.size_;
			}
			return *this;
		}		small_vector& operator=(small_vector&& other) noexcept
		{ 
			swap(other);
			other.clear(); 
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
			emplace_back(std::move(value));
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
			std::destroy(data_, data_ + size_);
			size_ = 0;
		}

		void swap(small_vector& rhs) noexcept
		{
			std::swap(data_, rhs.data_);
			std::swap(size_, rhs.size_);
			std::swap(capacity_, rhs.capacity_);
		}
    }; // class small vector

	template <typename T, size_t N>
	void swap(small_vector<T, N>& lhs, small_vector<T, N>& rhs) noexcept
	{
		lhs.swap(rhs);
	}

} // namespace mpc

#endif // MPC_SMALL_VECTOR
