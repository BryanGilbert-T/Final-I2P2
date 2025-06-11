#include "Sheets.hpp"

#include <sha256/sha256.h>
#include <curl/include/curl/curl.h>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <map>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>

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
            "px": {"integerValue": 860},
            "py": {"integerValue": 1372},
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
            "pending": {
                "arrayValue": {
                    "values": []
                }
            }
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

void set_online(const std::string& name, bool on) {
    // Build the document URL, including an update mask so only 'online' changes
    std::string url =
        "https://firestore.googleapis.com/v1/projects/" +
        project_id +
        "/databases/(default)/documents/players/" + name +
        "?updateMask.fieldPaths=online";

    // Prepare the JSON payload
    std::string json = R"({
        "fields": {
            "online": { "booleanValue": )"
        + std::string(on ? "true" : "false")
        + R"( }
        }
    })";

    // Initialize curl
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "curl init failed\n";
        return;
    }

    // Set headers
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // Response buffer
    std::string response;

    // Configure curl for PATCH
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // (Optional) disable peer verification if you did so elsewhere
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    // Execute
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "set_online failed: "
                  << curl_easy_strerror(res) << "\n";
    }

    // Clean up
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
                int px, int py, int coin, int hp, int stage)
{
    // 1) Build URL with updateMask for each field we’re updating:
    std::string url =
        "https://firestore.googleapis.com/v1/projects/" +
        project_id +
        "/databases/(default)/documents/players/" + name +
        "?updateMask.fieldPaths=px"
        "&updateMask.fieldPaths=py"
        "&updateMask.fieldPaths=coin"
        "&updateMask.fieldPaths=hp"
        "&updateMask.fieldPaths=stage";

    // 2) JSON payload for just those five fields:
    std::string json = R"({
      "fields": {
        "px":    { "integerValue": )" + std::to_string(px)    + R"( },
        "py":    { "integerValue": )" + std::to_string(py)    + R"( },
        "coin":  { "integerValue": )" + std::to_string(coin)  + R"( },
        "hp":    { "integerValue": )" + std::to_string(hp)    + R"( },
        "stage": { "integerValue": )" + std::to_string(stage) + R"( }
      }
    })";

    // 3) cURL setup
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "curl init failed\n";
        return;
    }
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // 4) Configure PATCH
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);

    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // (optional) if you’ve disabled SSL checks elsewhere:
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    // 5) Perform & clean up
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "updateUser failed: "
                  << curl_easy_strerror(res) << "\n"
                  << "response: " << response << "\n";
    }
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
}

int authUser(const std::string& name, const std::string& password) {
    std::string hash = sha256(password);

    std::map<std::string, std::string> result = getUser(name);
    if (result.empty()) return -1;
    // if (isSigningUp && !result.empty()) return 2; //found name in signingup
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

static void patchArrayField(
    const std::string& docId,
    const std::string& fieldName,
    const std::vector<std::string>& values)
{
    // build the URL with updateMask so only this field is touched
    std::string url =
        "https://firestore.googleapis.com/v1/projects/" +
        project_id +
        "/databases/(default)/documents/players/" + docId +
        "?updateMask.fieldPaths=" + fieldName;

    // build JSON: { "fields": { "friends": { "arrayValue": { "values": [ … ] } } } }
    std::ostringstream js;
    js << R"({ "fields": { ")"
       << fieldName
       << R"(": { "arrayValue": { "values": [)";

    for (size_t i = 0; i < values.size(); ++i) {
        if (i) js << ',';
        js << R"({ "stringValue": ")" << values[i] << R"(" })";
    }
    js << R"(] } } } })";

    std::string json = js.str();

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "curl init failed\n";
        return;
    }
    struct curl_slist* hdrs = nullptr;
    hdrs = curl_slist_append(hdrs, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdrs);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &json /*or a real response buffer*/);

    // if you’ve disabled SSL verify elsewhere, repeat here
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "patchArrayField failed: "
                  << curl_easy_strerror(res) << "\n";
    }
    curl_easy_cleanup(curl);
    curl_slist_free_all(hdrs);
}

void addFriend(const std::string& player1, const std::string& player2) {
    auto f1 = getFriends(player1);
    auto f2 = getFriends(player2);

    // 2) Append if missing
    if (std::find(f1.begin(), f1.end(), player2) == f1.end()) {
        f1.push_back(player2);
        patchArrayField(player1, "friends", f1);
    }
    if (std::find(f2.begin(), f2.end(), player1) == f2.end()) {
        f2.push_back(player1);
        patchArrayField(player2, "friends", f2);
    }
}

void RemoveRequests(const std::string& player1, const std::string& player2) {
    // 1) Load current requests
    auto reqs = getRequests(player1);

    // 2) Remove player2 if present
    auto it = std::find(reqs.begin(), reqs.end(), player2);
    if (it == reqs.end()) return;  // nothing to do

    reqs.erase(it);

    // 3) Push the updated array back to Firestore
    patchArrayField(player1, "requests", reqs);
}

std::vector<std::string> getAllPlayers() {
    std::vector<std::string> players;

    // 1) Firestore “list documents” endpoint for your collection
    std::string url =
        "https://firestore.googleapis.com/v1/projects/" +
        project_id +
        "/databases/(default)/documents/players";

    // 2) Perform the GET
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "curl init failed\n";
        return players;
    }
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    if (curl_easy_perform(curl) != CURLE_OK) {
        std::cerr << "curl_easy_perform failed: "
                  << curl_easy_strerror(curl_easy_perform(curl))
                  << "\n";
        curl_easy_cleanup(curl);
        return players;
    }
    curl_easy_cleanup(curl);

    // 3) Parse JSON, extract the trailing ID from each document’s `name`
    try {
        auto j = nlohmann::json::parse(response);
        if (!j.contains("documents")) return players;
        for (auto& doc : j["documents"]) {
            std::string fullName = doc.value("name", "");
            auto pos = fullName.find_last_of('/');
            std::string id = (pos == std::string::npos)
                             ? fullName
                             : fullName.substr(pos + 1);
            players.push_back(id);
        }
    } catch (const std::exception& e) {
        std::cerr << "JSON parse error in getAllPlayers: " << e.what() << "\n";
    }

    return players;
}

void setRequests(const std::string& player1, const std::string& player2) {
    auto reqs = getRequests(player2);

    if (std::find(reqs.begin(), reqs.end(), player1) != reqs.end()) {
        return;
    }

    // 2) append if missing (so you don’t send duplicates)
    if (std::find(reqs.begin(), reqs.end(), player1) == reqs.end()) {
        reqs.push_back(player1);

        // 3) PATCH just that field back to Firestore
        patchArrayField(
            /*docId:*/   player2,
            /*fieldName:*/"requests",
            /*values:*/   reqs
        );
    }
}

std::vector<std::string> getPendings(const std::string& name) {
    // Build the document URL
    std::string url =
        "https://firestore.googleapis.com/v1/projects/" +
        project_id +
        "/databases/(default)/documents/players/" + name;

    // Fetch via cURL
    CURL* curl = curl_easy_init();
    std::vector<std::string> pendings;
    if (!curl) {
        std::cerr << "curl init failed\n";
        return pendings;
    }
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    if (curl_easy_perform(curl) != CURLE_OK) {
        std::cerr << "curl_easy_perform failed: "
                  << curl_easy_strerror(curl_easy_perform(curl))
                  << "\n";
        curl_easy_cleanup(curl);
        return pendings;
    }
    curl_easy_cleanup(curl);

    // Parse out the pending array
    try {
        auto j = nlohmann::json::parse(response);
        if (!j.contains("fields")
         || !j["fields"].contains("pending")
         || !j["fields"]["pending"].contains("arrayValue")
         || !j["fields"]["pending"]["arrayValue"].contains("values"))
        {
            return pendings;
        }

        for (auto& v : j["fields"]["pending"]["arrayValue"]["values"]) {
            if (v.contains("stringValue"))
                pendings.push_back(v["stringValue"].get<std::string>());
        }
    } catch (const std::exception& e) {
        std::cerr << "JSON error in getPendings: " << e.what() << "\n";
    }

    return pendings;
}

void addPending(const std::string& player1, const std::string& player2) {
    // 1) Pull the current `pending` array for player1
    auto pendings = getPendings(player1);

    // 2) If player2 is already pending, do nothing
    if (std::find(pendings.begin(), pendings.end(), player2) != pendings.end()) {
        return;
    }

    // 3) Otherwise append and PATCH back to Firestore
    pendings.push_back(player2);
    patchArrayField(
        /* docId:    */ player1,
        /* fieldName:*/ "pending",
        /* values:   */ pendings
    );
}
void removePending(const std::string& player1, const std::string& player2) {
    // player1.pending.remove(player2)
    // 1) Load the current pending list
    auto pendings = getPendings(player1);

    // 2) Find and erase player2, if present
    auto it = std::find(pendings.begin(), pendings.end(), player2);
    if (it == pendings.end()) {
        // nothing to do
        return;
    }
    pendings.erase(it);

    // 3) PATCH the updated array back to Firestore
    patchArrayField(
        /* docId:    */ player1,
        /* fieldName:*/ "pending",
        /* values:   */ pendings
    );
}

UserData getUserData(const std::string& name) {
    UserData ud;
    // 1) build URL
    std::string url =
        "https://firestore.googleapis.com/v1/projects/" +
        project_id +
        "/databases/(default)/documents/players/" + name;

    // 2) do one cURL GET
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "curl init failed\n";
        return ud;
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
        std::cerr << "curl failed: " << curl_easy_strerror(res) << "\n";
        return ud;
    }

    // 3) parse JSON once
    try {
        auto j = nlohmann::json::parse(response);
        if (!j.contains("fields")) return ud;
        auto &f = j["fields"];

        auto extractArray = [&](const std::string &fieldName,
                                std::vector<std::string> &out) {
            if (f.contains(fieldName)
             && f[fieldName].contains("arrayValue")
             && f[fieldName]["arrayValue"].contains("values")) {
                for (auto &v : f[fieldName]["arrayValue"]["values"]) {
                    if (v.contains("stringValue"))
                        out.push_back(v["stringValue"].get<std::string>());
                }
             }
        };

        extractArray("friends",  ud.friends);
        extractArray("requests", ud.requests);
        extractArray("pending",  ud.pending);
    }
    catch (const std::exception &e) {
        std::cerr << "JSON parse error in getUserData: " << e.what() << "\n";
    }

    return ud;
}