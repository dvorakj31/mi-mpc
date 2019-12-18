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

		static_assert(N > 0);

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
				std::destroy(data, data + i); ::operator delete(data);
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
		small_vector() : data_(reinterpret_cast<T*>(data_buf_)), capacity_(N), size_(0){}

		small_vector(std::initializer_list<T> init) : small_vector()
		{
		    if (init.size() > N)
		        data_ = (T*)::operator new(sizeof(T) * init.size());
		    size_ = capacity_ = init.size();
            try
            {
                std::uninitialized_copy(init.begin(), init.end(), data_);
            }
            catch (...)
            {
                if (data_ != reinterpret_cast<T*>(data_buf_)) ::operator delete(data_);
                throw;
            }
		}

		small_vector(const small_vector& other)
		{
			if (other.size_ == 0)
				return;
			data_ = (T*)::operator new(other.capacity_ * sizeof(T));
			capacity_ = other.capacity_;
			try
			{
			    std::uninitialized_copy(other.data_, other.data_ + other.size_, data_);
			}
			catch (...)
			{
			    ::operator delete(data_); throw;
			}
			size_ = other.size_;
		}

		~small_vector()
		{
			clear();
			if (data_ != reinterpret_cast<T*>(data_buf_))
				::operator delete(data_);
		}

		small_vector(small_vector&& other) noexcept :
			data_(other.data_), capacity_(other.capacity_), size_(other.size_)
		{
		    if (other.data_ == reinterpret_cast<T*>(other.data_buf_))
		    {
                std::uninitialized_move(other.data_buf_, other.data_buf_ + other.size_, data_buf_);
                data_ = reinterpret_cast<T*>(data_buf_);
				other.clear();
            }
			other.data_ = reinterpret_cast<T*>(other.data_buf_);
			other.size_ = 0;
		}

		small_vector& operator=(const small_vector& other)
		{
            if (capacity() < other.size())
			{
                small_vector tmp(other);
                swap(tmp);
            }
			else if (this != &other)
			{
                clear();
                try { std::uninitialized_copy(other.data_, other.data_ + other.size_, data_); }
                catch (...) { ::operator delete(data_); throw; }
			}
            size_ = other.size_;
            capacity_ = other.capacity_;
            return *this;
		}

		small_vector& operator=(small_vector&& other) noexcept
		{
		    clear();
            swap(other);
			if (other.data_ == reinterpret_cast<T*>(other.data_buf_))
				other.clear();
            other.data_ = reinterpret_cast<T*>(other.data_buf_);
			return *this;
		}

		size_t size() const
		{
			return size_;
		}

        size_t capacity() const
		{
			return capacity_;
		}

		pointer data()
        {
		    return data_;
        }

        const_pointer data() const
        {
		    return data_;
        }

		void reserve(size_t new_size)
		{
			if (new_size <= capacity_)
				return;
			_resize(new_size);
		}

		void push_back(const value_type& value)
		{
			emplace_back(value);
		}

		void push_back(value_type&& value)
		{
			emplace_back(std::move(value));
		}

        value_type& operator[](size_t index)
		{
			return data_[index];
		}

		const value_type& operator[](size_t index) const
		{
			return data_[index];
		}

        template <typename... Ts>
        void emplace_back(Ts&&... vs)		{
			if (size_ == capacity_)
				reserve(capacity_ * 2);
			new (data_ + size_) T(std::forward<Ts>(vs)...);
			size_++;
		}

		void clear() noexcept
		{
			std::destroy(data_, data_ + size_);
			size_ = 0;
		}

		void swap(small_vector& rhs) noexcept
		{
		    if (data_ != reinterpret_cast<T*>(data_buf_) && rhs.data_ == reinterpret_cast<T*>(rhs.data_buf_))
            {
		        rhs.data_ = data_;
		        data_ = reinterpret_cast<T*>(data_buf_);
            }
		    else if (rhs.data_ != reinterpret_cast<T*>(rhs.data_buf_) && data_ == reinterpret_cast<T*>(data_buf_))
            {
		        data_ = rhs.data_;
		        rhs.data_ = reinterpret_cast<T*>(rhs.data_buf_);
            }
		    else if (data_ != reinterpret_cast<T*>(data_buf_) && rhs.data_ != reinterpret_cast<T*>(rhs.data_buf_))
		    {
                std::swap(data_, rhs.data_);
            }
            std::swap(data_buf_, rhs.data_buf_);
            std::swap(size_, rhs.size_);
			std::swap(capacity_, rhs.capacity_);
		}

        void resize(size_t size, const value_type& value = value_type())
        {
            if (size == size_) return;

            else if (size < size_) std::destroy(data_ + size, data_ + size_);

            else
            {
                _resize(size);
                std::uninitialized_fill_n(end(), size - size_, value);
            }
            size_ = size;
        }

		iterator begin() noexcept
        {
		    return data_;
        }

        iterator end() noexcept
        {
		    return data_ + size_;
        }

        const_iterator begin() const noexcept
        {
            return data_;
        }

        const_iterator end() const noexcept
        {
            return data_ + size_;
        }
    }; // class small vector

	template <typename T, size_t N>
	void swap(small_vector<T, N>& lhs, small_vector<T, N>& rhs) noexcept
	{
		lhs.swap(rhs);
	}

} // namespace mpc

#endif // MPC_SMALL_VECTOR
