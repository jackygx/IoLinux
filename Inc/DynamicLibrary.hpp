/*
 * Copyright (c) 2018 Guo Xiang
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __DYNAMIC_LIBRARY_HPP__
#define __DYNAMIC_LIBRARY_HPP__

#ifdef DEBUG_DYNAMIC_LIBRARY
#define DLIB_DEBUG(...) TRACE_INFO(__VA_ARGS__, EOS)
#else
#define DLIB_DEBUG(...)
#endif

#include <dlfcn.h>
#include <EasyCpp.hpp>

namespace Platform {

DEFINE_CLASS(DynamicLibrary);
DEFINE_SYNC_PROMISE(DLib, CDynamicLibraryPtr);

class CDynamicLibrary
{
public:
	static inline CDLibPromisePtr Create(const CConstStringPtr &path);

	inline ~CDynamicLibrary(void);

	template <class Fn>
	inline decltype(auto) GetFunc(const CConstStringPtr &funcName) const;

	friend CDynamicLibraryPtr
		MakeShared<CDynamicLibrary>(CVoidPointerWrapper &&);
private:
	inline CDynamicLibrary(CVoidPointerWrapper &&handle);

private:
	void *mHandle;
};

inline CDLibPromisePtr CDynamicLibrary::Create(const CConstStringPtr &path)
{
	void *handle = dlopen(path->ToCStr(), RTLD_LAZY);

	if (NULL == handle) {
		return CDLibPromisePtr();
	} else {
		DLIB_DEBUG("Library ", path, " is loaded");
		return CDLibPromisePtr(MakeShared<CDynamicLibrary>(CVoidPointerWrapper(handle)));
	}
}

inline CDynamicLibrary::~CDynamicLibrary(void)
{
	dlclose(mHandle);
	mHandle = NULL;
}

template <class Fn>
inline decltype(auto) CDynamicLibrary::GetFunc(const CConstStringPtr &funcName) const
{
	DEFINE_SYNC_PROMISE(DLib, Fn);

	dlerror();    /* Clear any existing error */

	Fn func((void *)dlsym(mHandle, funcName->ToCStr()));

	if ((dlerror() != NULL) || (NULL == func)) {
		throw E("Fail to dlsym the function: ", funcName);
	}

	return CDLibPromisePtr(func);
}

inline CDynamicLibrary::CDynamicLibrary(CVoidPointerWrapper &&handle) :
	mHandle(handle.Get())
{
	/* Does nothing */
}

}

#endif /* __DYNAMIC_LIBRARY_HPP__ */

