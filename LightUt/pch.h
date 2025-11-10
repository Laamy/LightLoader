#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <NativeCore.h>

template <typename R, typename... Args>
R CallFunc(void* func, Args... args) { return ((R(*)(Args...))func)(args...); }

#endif //PCH_H
