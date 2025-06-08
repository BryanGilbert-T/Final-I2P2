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
            "requestscnt": {"integerValue": 0},
            "online": {"booleanValue": true},
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

std::map<std::string, bool> find_online() {
        // 1) Build the “list documents” URL for your collection:
    std::string url =
        "https://firestore.googleapis.com/v1/projects/" +
        project_id +
        "/databases/(default)/documents/players";

    // 2) Perform the GET
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "curl init failed\n";
        return {};
    }
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform failed: "
                  << curl_easy_strerror(res) << "\n";
        return {};
    }

    // 3) Parse JSON and build the map
    std::map<std::string,bool> statusMap;
    try {
        auto j = nlohmann::json::parse(response);
        if (!j.contains("documents")) return {};

        for (const auto& doc : j["documents"]) {
            // Firestore returns a full resource name like:
            // projects/…/databases/(default)/documents/players/<docID>
            std::string fullName = doc.value("name", "");
            // extract the trailing ID (after the last '/')
            auto pos = fullName.find_last_of('/');
            std::string playerID = (pos == std::string::npos)
                                   ? fullName
                                   : fullName.substr(pos + 1);

            bool online = false;
            if (doc.contains("fields")
             && doc["fields"].contains("online")
             && doc["fields"]["online"].contains("booleanValue"))
            {
                online = doc["fields"]["online"]["booleanValue"].get<bool>();
            }
            statusMap[playerID] = online;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "JSON parse error in getAllPlayersOnlineStatus: "
                  << e.what() << "\n";
        return {};
    }

    return statusMap;
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

std::vector<std::string> getFriends(const std::string& name) {
    // build the URL just like getUser()
    std::string url =
      "https://firestore.googleapis.com/v1/projects/" +
      project_id + "/databases/(default)/documents/players/" + name;

    CURL* curl = curl_easy_init();
    std::string response;
    if (!curl) {
        std::cerr << "curl init failed\n";
        return {};
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform failed: "
                  << curl_easy_strerror(res) << "\n";
        return {};
    }

    // parse JSON and extract friends array
    try {
        auto j = nlohmann::json::parse(response);
        if (!j.contains("fields")
         || !j["fields"].contains("friends")
         || !j["fields"]["friends"].contains("arrayValue")
         || !j["fields"]["friends"]["arrayValue"].contains("values"))
        {
            return {};
        }

        const auto& vals = j["fields"]
                              ["friends"]
                              ["arrayValue"]
                              ["values"];
        std::vector<std::string> friends;
        for (const auto& v : vals) {
            if (v.contains("stringValue"))
                friends.push_back(v["stringValue"].get<std::string>());
        }
        return friends;
    } catch (const std::exception& e) {
        std::cerr << "JSON error in getFriends: " << e.what() << "\n";
        return {};
    }
}

std::vector<std::string> getRequests(const std::string& name) {
    // Build the same URL
    std::string url =
      "https://firestore.googleapis.com/v1/projects/" +
      project_id + "/databases/(default)/documents/players/" + name;

    CURL* curl = curl_easy_init();
    std::string response;
    if (!curl) {
        std::cerr << "curl init failed\n";
        return {};
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    if (curl_easy_perform(curl) != CURLE_OK) {
        std::cerr << "curl failed: "
                  << curl_easy_strerror(curl_easy_perform(curl))
                  << "\n";
        curl_easy_cleanup(curl);
        return {};
    }
    curl_easy_cleanup(curl);

    // Parse out the requests array
    try {
        auto j = nlohmann::json::parse(response);
        auto& f = j["fields"];
        if (!f.contains("requests")
         || !f["requests"].contains("arrayValue")
         || !f["requests"]["arrayValue"].contains("values"))
        {
            return {};
        }

        std::vector<std::string> reqs;
        for (auto& v : f["requests"]["arrayValue"]["values"]) {
            if (v.contains("stringValue"))
                reqs.push_back(v["stringValue"].get<std::string>());
        }
        return reqs;
    } catch (const std::exception& e) {
        std::cerr << "JSON error in getRequests: " << e.what() << "\n";
        return {};
    }
}

