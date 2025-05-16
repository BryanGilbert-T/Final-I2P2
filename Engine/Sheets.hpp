#ifndef SHEETS_HPP
#define SHEETS_HPP

#include <string>

extern void createUser(std::string username, std::string password);
extern void findUser(std::string username);
extern void updateUser(std::string name, int stage, int px, int py, int coin, int hp);
extern void authUser(std::string name, std::string pass);


#endif   // POINT_HPP