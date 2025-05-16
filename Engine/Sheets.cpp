#include <sha256/sha256.h>
#include <curl/include/curl.h>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <sstream>
#include <iostream>


// -------------- HTTP helper --------------
size_t write_cb(void* ptr, size_t sz, size_t nm, void* usr) {
    std::string& resp = *static_cast<std::string*>(usr);
    resp.append((char*)ptr, sz*nm);
    return sz*nm;
}

nlohmann::json http_json(const std::string& url,
                         const nlohmann::json* postBody=nullptr)
{
    CURL* curl = curl_easy_init();
    std::string resp;
    curl_slist* hdrs = nullptr;
    hdrs = curl_slist_append(hdrs, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdrs);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);

    if (postBody) {
        auto s = postBody->dump();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, s.c_str());
    }

    if (curl_easy_perform(curl)!=CURLE_OK) {
        std::cerr<<"CURL request failed\n";
    }
    curl_slist_free_all(hdrs);
    curl_easy_cleanup(curl);
    return nlohmann::json::parse(resp);
}

// -------------- Usage example --------------
int main(){
    const std::string API = "https://script.google.com/macros/s/…/exec";

    // 1) Create a user with extra fields
    auto createReq = nlohmann::json{
      {"action",        "createUser"},
      {"username",      "alice"},
      {"password_hash", sha256("s3cr3t")},
      {"stage",         1},
      {"px",            100},
      {"py",            200},
      {"coin",          50},
      {"hp",            100}
    };
    auto r1 = http_json(API, &createReq);
    std::cout<<"createUser → "<<r1.dump()<<"\n";

    // 2) Fetch that user
    std::string url2 = API
      + "?action=getUser&username="
      + curl_easy_escape(nullptr, "alice", 0);
    auto r2 = http_json(url2, nullptr);
    std::cout<<"getUser → "<< r2.dump(2) << "\n";

    // 3) Update their game state later:
    auto updReq = nlohmann::json{
      {"action",   "updateUser"},
      {"username", "alice"},
      {"stage",    2},
      {"px",       150},
      {"py",       180},
      {"coin",     75},
      {"hp",       90}
    };
    auto r3 = http_json(API, &updReq);
    std::cout<<"updateUser → "<<r3.dump()<<"\n";

    // 4) Authenticate
    auto authReq = nlohmann::json{
      {"action",        "auth"},
      {"username",      "alice"},
      {"password_hash", sha256("s3cr3t")}
    };
    auto r4 = http_json(API, &authReq);
    std::cout<<"auth → "<<r4.dump()<<"\n";

    return 0;
}