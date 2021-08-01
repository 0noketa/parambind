#pragma once
#ifndef parambind__hpp
#define parambind__hpp


#include <memory>
#include <type_traits>
#include "parambind.h"


namespace parambind
{
	using namespace std;

	template<class T>
	struct is_valid_value
		: bool_constant<
			is_pointer<T>::value
			|| is_integral<T>::value
			&& sizeof(T) == sizeof(void*)>
	{};

	template<class T>
	struct parambind_delete : default_delete<T>
	{
		void operator()(T *p) const noexcept
		{
			static_assert(is_pointer<T>::value);

			parambind_free(reinterpret_cast<void*>(p));
		}
	};


#ifndef _WIN64
	template<typename T1, typename T2>
	unique_ptr<T1(*__cdecl)(void), parambind_delete<T1(*__cdecl)(void)>>
		bind(T1(*__cdecl f)(T2), T2 x)
	{
		static_assert(
			is_valid_value<T1>::value
			&& is_valid_value<T2>::value);

		void *p = reinterpret_cast<void*>(f);
		void *q = reinterpret_cast<void*>(x);
		void *r = parambind_bind_u_cdecl(p, q);

		return reinterpret_cast<T1(*__cdecl)(void)>(r);
	}

	template<typename T1, typename T2, typename T3>
	unique_ptr<T1(*__cdecl)(T3), parambind_delete<T1(*__cdecl)(T3)>>
		bind(T1(*__cdecl f)(T2, T3), T2 x)
	{
		static_assert(
			is_valid_value<T1>::value
			&& is_valid_value<T2>::value
			&& is_valid_value<T3>::value);

		void *p = reinterpret_cast<void*>(f);
		void *q = reinterpret_cast<void*>(x);
		void *r = parambind_bind_l_cdecl(p, q);

		return reinterpret_cast<T1(*__cdecl)(T3)>(r);
	}
#endif

#ifdef _WIN64
	template<typename T1, typename T2>
	unique_ptr<T1(*)(void), parambind_delete<T1(*)(void)>>
		bind_u(T1(*f)(T2), T2 x)
	{
		static_assert(
			is_valid_value<T1>::value
			&& is_valid_value<T2>::value);

		void *p = reinterpret_cast<void*>(f);
		void *q = reinterpret_cast<void*>(x);
		void *r = parambind_bind_u_fastcall64(p, q);
		auto s = reinterpret_cast<T1(*)(void)>(r);
		unique_ptr<T1(*)(void), parambind_delete<T1(*)(void)>> t = nullptr;

		t.reset(s);
		return t;
	}
/**/
	template<typename T1, typename T2, typename T3>
	unique_ptr<T1(*)(T3), parambind_delete<T1(*)(T3)>>
		bind_l(T1(*f)(T2, T3), T2 x)
	{
		static_assert(
			is_valid_value<T1>::value
			&& is_valid_value<T2>::value
			&& is_valid_value<T3>::value);

		void *p = reinterpret_cast<void*>(f);
		void *q = reinterpret_cast<void*>(x);
		void *r = parambind_bind_l_fastcall64(p, q);
		auto s = reinterpret_cast<T1(*)(T3)>(r);
		unique_ptr<T1(*)(T3), parambind_delete<T1(*)(void)>> t = nullptr;

		t.reset(s);
		return t;
	}
#endif


}


#endif
