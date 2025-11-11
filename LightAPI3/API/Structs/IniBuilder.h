#pragma once

#include <map>
#include <string>
#include <fstream>
#include <optional>
#include <filesystem>

namespace fs = std::filesystem;

class IniBuilder {
    std::map<std::string, std::string> data;
    std::string configPath;
    std::string clientPath;

public:
    IniBuilder() = default;
    IniBuilder(std::string clientPath, std::string _configPath) {
        this->clientPath = clientPath;
        configPath = _configPath;
        if (fs::exists(clientPath + configPath)) {
            //Console::Log("ZoomMod", "Loading config from %s", configPath.c_str());
            std::ifstream _file(clientPath + configPath);
            std::string content;
            if (_file.is_open()) {
                std::string line;
                while (std::getline(_file, line)) {
                    if (content.size() > 1)
                        content += "\r\n";

                    content += line;
                }
                _file.close();
            }
            fetchIni(content);
        }
        else {
            //Console::Log("ZoomMod", "Config file %s not found, creating new one", configPath.c_str());
            std::ofstream _file(clientPath + configPath);
            if (_file.is_open()) {
                _file << str();
                _file.close();
            }
        }
    }

public:
    void add(const std::string& key, const std::string& value) {
        data[key] = value;
        std::ofstream _file(clientPath + configPath);
        if (_file.is_open()) {
            _file << str();
            _file.close();
        }
    }

    std::optional<std::string> Get(const std::string& key) const {
        auto it = data.find(key);
        if (it == data.end()) return std::nullopt;
        return it->second;
    }

    // Supports int, float, double, std::string
    template<typename T>
    T GetOrDefault(const std::string& key, T def) {
        auto val = Get(key);
        if (!val.has_value()) {
            if constexpr (std::is_same_v<T, std::string>)
                data[key] = def;
            else
                data[key] = std::to_string(def);
            std::ofstream _file(clientPath + configPath);
            if (_file.is_open()) {
                _file << str();
                _file.close();
            }
            return def;
        }
        if constexpr (std::is_same_v<T, std::string>) {
            return val.value();
        }
        else {
            std::istringstream ss(val.value());
            T out;
            ss >> out;
            if (ss.fail()) {
                data[key] = std::to_string(def);
                std::ofstream _file(clientPath + configPath);
                if (_file.is_open()) {
                    _file << str();
                    _file.close();
                }
                return def;
            }
            return out;
        }
    }

private:
    std::string str() const {
        std::ostringstream out;
        for (const auto& [key, value] : data)
            out << key << "=" << value << "\n";
        return out.str();
    }

    void fetchIni(const std::string& content) {
        data.clear();
        std::istringstream in(content);
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty() || line[0] == ';' || line[0] == '#' || (line.front() == '[' && line.back() == ']')) continue;
            auto eq = line.find('=');
            if (eq == std::string::npos) continue;
            std::string key = line.substr(0, eq);
            std::string value = line.substr(eq + 1);
            data[key] = value;
        }
    }
};
