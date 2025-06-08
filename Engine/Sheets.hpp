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
void set_online(const std::string& name, bool on);
void addFriend(const std::string& player1, const std::string& player2);
void RemoveRequests(const std::string& player1, const std::string& player2);
std::vector<std::string> getAllPlayers(void);
extern bool isSigningUp;



#endif   // POINT_HPP