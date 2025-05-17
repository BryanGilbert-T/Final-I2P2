#include <sha256/sha256.h>
#include <curl/include/curl/curl.h>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <sstream>
#include <iostream>

const std::string API = "https://script.google.com/macros/s/AKfycbxEOi5h8_zZB6kgFtiCrdjurJzgCq5FwfHPuO_XiJF3e-o-G_Ml1NldINYh_GPPGtm11A/exec";

static size_t write_cb(void* ptr, size_t sz, size_t nm, void* usr) {
    std::string& resp = *static_cast<std::string*>(usr);
    resp.append(reinterpret_cast<char*>(ptr), sz * nm);
    return sz * nm;
}

static nlohmann::json http_post_form(const std::string& url, const std::string& formBody) {
    CURL* curl = curl_easy_init();
    if (!curl) { std::cerr << "curl init failed\n"; return {}; }

    std::string resp;
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, formBody.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform failed: " << curl_easy_strerror(res) << "\n";
        curl_easy_cleanup(curl);
        return {};
    }

    curl_easy_cleanup(curl);

    if (resp.empty()) {
        std::cerr << "empty response\n";
        return {};
    }

    try {
        return nlohmann::json::parse(resp);
    } catch (nlohmann::json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << "\n";
        std::cerr << "Response was not JSON, response:\n" << resp << "\n";
        return {};
    }
}

static nlohmann::json http_get(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) { std::cerr << "curl init failed\n"; return {}; }

    std::string resp;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform failed: " << curl_easy_strerror(res) << "\n";
        curl_easy_cleanup(curl);
        return {};
    }

    curl_easy_cleanup(curl);

    if (resp.empty()) {
        std::cerr << "empty response\n";
        return {};
    }

    try {
        return nlohmann::json::parse(resp);
    } catch (nlohmann::json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << "\n";
        std::cerr << "Response was not JSON, response:\n" << resp << "\n";
        return {};
    }
}

// Create a new user
void createUser(const std::string& name, const std::string& pass) {
    CURL* esc = curl_easy_init();
    char* u = curl_easy_escape(esc, name.c_str(), 0);
    char* p = curl_easy_escape(esc, sha256(pass).c_str(), 0);

    std::ostringstream form;
    form << "action=createUser"
         << "&username=" << (u ? u : "")
         << "&password_hash=" << (p ? p : "")
         << "&stage=1&px=0&py=0&coin=0&hp=100";

    if (u) curl_free(u);
    if (p) curl_free(p);
    curl_easy_cleanup(esc);

    auto r = http_post_form(API, form.str());
    std::cout << "createUser → " << r.dump(2) << "\n";
}

// Authenticate user
void authUser(const std::string& name, const std::string& pass) {
    CURL* esc = curl_easy_init();
    char* u = curl_easy_escape(esc, name.c_str(), 0);
    char* p = curl_easy_escape(esc, sha256(pass).c_str(), 0);

    std::ostringstream form;
    form << "action=auth"
         << "&username=" << (u ? u : "")
         << "&password_hash=" << (p ? p : "");

    if (u) curl_free(u);
    if (p) curl_free(p);
    curl_easy_cleanup(esc);

    auto r = http_post_form(API, form.str());
    std::cout << "authUser → " << r.dump(2) << "\n";
}

// Update user
void updateUser(const std::string& name, int stage, int px, int py, int coin, int hp) {
    CURL* esc = curl_easy_init();
    char* u = curl_easy_escape(esc, name.c_str(), 0);

    std::ostringstream form;
    form << "action=updateUser"
         << "&username=" << (u ? u : "")
         << "&stage=" << stage
         << "&px=" << px << "&py=" << py
         << "&coin=" << coin << "&hp=" << hp;

    if (u) curl_free(u);
    curl_easy_cleanup(esc);

    auto r = http_post_form(API, form.str());
    std::cout << "updateUser → " << r.dump(2) << "\n";
}

void getUser(const std::string& name) {
    CURL* esc = curl_easy_init();
    char* u = curl_easy_escape(esc, name.c_str(), 0);

    std::ostringstream url;
    url << API << "?action=getUser&username=" << (u ? u : "");

    if (u) curl_free(u);
    curl_easy_cleanup(esc);

    auto r = http_get(url.str());
    std::cout << "getUser → " << r.dump(2) << "\n";
}
