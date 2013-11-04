#ifndef SECUREALLOCATOR_H
#define SECUREALLOCATOR_H

namespace i2pcpp {
	template<typename T>
		class secure_allocator : public std::allocator<T>
		{
			typedef typename std::allocator<T>::pointer pointer;
			typedef typename std::allocator<T>::size_type size_type;

			public:
				static void deallocate(pointer p, size_type s) noexcept
				{
					for(size_t i = 0; i != s; ++i)
						p[i] = 0;

					::operator delete((void*)p);
				}
		};
}

#endif
