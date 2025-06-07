#include "Sheets.hpp"

#include <sha256/sha256.h>
#include <curl/include/curl/curl.h>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <map>
#include <sstream>
#include <iostream>
#include <fstream>

const std::string project_id = "sunwukong-5bd64";
bool isSigningUp = false;
static size_t write_cb(void* ptr, size_t size, size_t nmemb, void* userdata) {
    std::string& resp = *static_cast<std::string*>(userdata);
    resp.append(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

std::map<std::string, std::string> getUser(const std::string& name)
{
    std::string url = "https://firestore.googleapis.com/v1/projects/" +
                      project_id + "/databases/(default)/documents/players/" + name;

    CURL* curl = curl_easy_init();
    std::string response;
    if (!curl) { std::cerr << "curl init failed\n"; return {}; }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    std::map<std::string, std::string> result;
    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform failed: " << curl_easy_strerror(res) << "\n";
        return {};
    }

    try {
        auto j = nlohmann::json::parse(response);
        if (!j.contains("fields")) return {};

        for (auto& [key, value] : j["fields"].items()) {
            if (value.contains("stringValue"))
                result[key] = value["stringValue"];
            else if (value.contains("integerValue"))
                result[key] = value["integerValue"];
            // Add other types if needed (doubleValue, etc)
        }
    } catch (...) {
        std::cerr << "JSON parse error or missing fields\n";
        return {};
    }
    return result;
}

void createUser(const std::string& name,
                const std::string& password)
{
    std::string url = "https://firestore.googleapis.com/v1/projects/" +
                      project_id + "/databases/(default)/documents/players?documentId=" + name;
    std::string hash = sha256(password);

    std::string json = R"({
        "fields": {
            "name": {"stringValue": ")" + name + R"("},
            "pass": {"stringValue": ")" + hash + R"("},
            "stage": {"integerValue": 1},
            "px": {"integerValue": 0},
            "py": {"integerValue": 0},
            "coin": {"integerValue": 0},
            "hp": {"integerValue": 100},
            "friends": {
                "arrayValue": {
                    "values": []
                }
            },
            "friendscnt": {"integerValue": 0},
            "requests": {
                "arrayValue": {
                    "values": []
                }
            },
            "requestscnt": {"integerValue": 0}
        }
    })";

    CURL* curl = curl_easy_init();
    if (!curl) { std::cerr << "curl init failed\n"; return; }
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        std::cerr << "curl_easy_perform failed: " << curl_easy_strerror(res) << "\n";
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
}

void updateUser(const std::string& name,
                int px, int py, int coin, int hp)
{
    std::string url = "https://firestore.googleapis.com/v1/projects/" +
                      project_id + "/databases/(default)/documents/players/" + name;

    std::string json = R"({
        "fields": {
            "px": {"integerValue": )" + std::to_string(px) + R"(},
            "py": {"integerValue": )" + std::to_string(py) + R"(},
            "coin": {"integerValue": )" + std::to_string(coin) + R"(},
            "hp": {"integerValue": )" + std::to_string(hp) + R"(}
        }
    })";

    CURL* curl = curl_easy_init();
    if (!curl) { std::cerr << "curl init failed\n"; return; }
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        std::cerr << "curl_easy_perform failed: " << curl_easy_strerror(res) << "\n";
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
}

int authUser(const std::string& name, const std::string& password) {
    std::string hash = sha256(password);

    std::map<std::string, std::string> result = getUser(name);
    if (result.empty()) return -1;
    if (isSigningUp && !result.empty()) return 2; //found name in signingup
    if (result["pass"] == hash) {
        // Later
        std::ofstream ofs("Resource/account.txt");
        if (ofs) {
            ofs << result["name"] << " "
                << result["stage"] << " "
                << result["px"] << " "
                << result["py"] << " "
                << result["coin"] << " "
                << result["hp"] << "\n";
            ofs.close();
        }
        return 1; //ketemu passwordnya
    } else return 0; //salah password
}
