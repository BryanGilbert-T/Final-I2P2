#ifndef SHEETS_HPP
#define SHEETS_HPP

#include <string>
#include <map>

std::map<std::string, std::string> getUser(const std::string& name);
void createUser(const std::string& name, const std::string& password);
void updateUser(const std::string& name, int px, int py, int coin, int hp);
int authUser(const std::string& name, const std::string& password);



#endif   // POINT_HPP