#include "IniBuilder.h"

template<typename T>
T GetOrDefault(const std::string& key, T def);

// so they expand when exporting
template int GetOrDefault<int>(const std::string& key, int def);
template float GetOrDefault<float>(const std::string& key, float def);
template double GetOrDefault<double>(const std::string& key, double def);
template std::string GetOrDefault<std::string>(const std::string& key, std::string def);
