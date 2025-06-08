#ifndef SHEETS_HPP
#define SHEETS_HPP

#include <string>
#include <map>
#include <vector>

std::map<std::string, std::string> getUser(const std::string& name);
void createUser(const std::string& name, const std::string& password);
void updateUser(const std::string& name, int px, int py, int coin, int hp);
int authUser(const std::string& name, const std::string& password);
std::vector<std::string> getFriends(const std::string& name);
std::vector<std::string> getRequests(const std::string& name);
std::map<std::string, bool> find_online();
extern bool isSigningUp;



#endif   // POINT_HPP