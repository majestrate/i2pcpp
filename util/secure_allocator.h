#ifndef SECUREALLOCATOR_H
#define SECUREALLOCATOR_H

namespace i2pcpp {
	template<typename T>
		class secure_allocator
		{
			public:
				typedef T value_type;
				typedef T* pointer;
				typedef std::size_t size_type;

				pointer allocate(size_type n)
				{
					return static_cast<pointer>(::operator new(n * sizeof(value_type)));
				}

				void deallocate(pointer p, size_type s) noexcept
				{
					for(size_t i = 0; i != s; ++i)
						p[i] = 0;

					::operator delete((void*)p);
				}
		};
}

#endif
