#pragma once

template <typename R, typename... Args>
R CallFunc(void* func, Args... args) { return ((R(*)(Args...))func)(args...); }
