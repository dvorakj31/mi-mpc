#ifndef MPC_SMALL_VECTOR
#define MPC_SMALL_VECTOR

#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace mpc
{

	template <typename T, size_t N>
	class small_vector
	{
		T* data_;
		T  data_buf_[N];
		size_t capacity_;
		size_t size_;

		void _resize(size_t new_size = 0)
		{
			T* tmp = new T[new_size == 0 ? capacity_ *= 2 : capacity_ = new_size];
			memcpy(tmp, data_, size_ * sizeof(T));
			if (data_ != data_buf_)
				delete[] data_;
			data_ = tmp;
		}

	public:
		small_vector() : data_(data_buf_), capacity_(N), size_(0) {}

		~small_vector()
		{
			if (data_ != data_buf_)
				delete[] data_;
		}

		size_t size() const
		{
			return size_;
		}

		size_t capacity() const
		{
			return capacity_;
		}

		void reserve(size_t new_size)
		{
			_resize(new_size);
		}

		void push_back(const T& val)
		{
			if (size_ == capacity_)
				_resize();
			data_[size_++] = val;
		}

		T& operator[](size_t index)
		{
			return data_[index];
		}

		T operator[](size_t index) const
		{
			return data_[index];
		}

		void clear()
		{
			size_ = 0;
		}
	}; // class small vector

} // namespace mpc

#endif // MPC_SMALL_VECTOR