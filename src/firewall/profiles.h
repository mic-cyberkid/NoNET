#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <fstream>

struct Profile {
    std::string name;
    std::vector<std::wstring> allowedApps;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Profile, name)
};

// Custom serialization for std::wstring
namespace nlohmann {
    template <>
    struct adl_serializer<std::wstring> {
        static void to_json(json& j, const std::wstring& s) {
            std::string utf8;
            // Simplified conversion for this context
            for (wchar_t wc : s) utf8 += (char)wc;
            j = utf8;
        }
        static void from_json(const json& j, std::wstring& s) {
            std::string utf8 = j.get<std::string>();
            s.clear();
            for (char c : utf8) s += (wchar_t)c;
        }
    };
}

class ProfileManager {
public:
    void saveProfiles(const std::vector<Profile>& profiles, const std::string& filename) {
        nlohmann::json j = profiles;
        std::ofstream file(filename);
        file << j.dump(4);
    }

    std::vector<Profile> loadProfiles(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return {};
        nlohmann::json j;
        file >> j;
        return j.get<std::vector<Profile>>();
    }
};
