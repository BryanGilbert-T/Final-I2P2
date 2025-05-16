#include <sha256/sha256.h>
#include <curl/include/curl/curl.h>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <sstream>
#include <iostream>

const std::string API = "https://script.google.com/macros/s/AKfycbxEOi5h8_zZB6kgFtiCrdjurJzgCq5FwfHPuO_XiJF3e-o-G_Ml1NldINYh_GPPGtm11A/exec";

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

void createUser(std::string name, std::string pass) {
    auto createReq = nlohmann::json{
          {"action",        "createUser"},
          {"username",      name},
          {"password_hash", sha256(pass)},
          {"stage",         1},
          {"px",            0},
          {"py",            0},
          {"coin",          0},
          {"hp",            100}
    };
    auto r = http_json(API, &createReq);
    std::cout<<"createUser → "<<r.dump()<<"\n";
}

void findUser(std::string name) {
    std::string url2 = API
     + "?action=getUser&username="
     + curl_easy_escape(nullptr, name.c_str(), 0);
    auto r = http_json(url2, nullptr);
    std::cout<<"getUser → "<< r.dump(2) << "\n";
}

void updateUser(std::string name,
    int stage, int px, int py, int coin, int hp) {
    auto updReq = nlohmann::json{
          {"action",   "updateUser"},
          {"username", name},
          {"stage",    stage},
          {"px",       px},
          {"py",       py},
          {"coin",     coin},
          {"hp",       hp}
    };
    auto r = http_json(API, &updReq);
    std::cout<<"updateUser → "<<r.dump()<<"\n";
}

void authUser(std::string name, std::string pass) {
    auto authReq = nlohmann::json{
          {"action",        "auth"},
          {"username",      name},
          {"password_hash", sha256(pass)}
    };
    auto r = http_json(API, &authReq);
    std::cout<<"auth → "<<r.dump()<<"\n";
}
