#pragma once

//-----------------------------------------------------------------------------
#include "WindowsIncludes.h"
#define far
#include <d3dx9.h>
#undef DrawText

//-----------------------------------------------------------------------------
#ifdef _DEBUG
#	define V(x) assert(SUCCEEDED(x))
#else
#	define V(x) (x)
#endif
//-----------------------------------------------------------------------------
// template pointer-type check
template<typename T>
struct is_pointer { static const bool value = false; };

template<typename T>
struct is_pointer<T*> { static const bool value = true; };

//-----------------------------------------------------------------------------
template<typename T>
inline void SafeRelease(T& x)
{
	if((is_pointer<T>::value) && (x != nullptr))
	{
		x->Release();
		x = nullptr;
	}
}
