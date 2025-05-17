#ifndef SHEETS_HPP
#define SHEETS_HPP

#include <string>

extern void createUser(const std::string& username, const std::string& password);
extern void getUser(const std::string& username);
extern void updateUser(const std::string& name, int stage, int px, int py, int coin, int hp);
extern void authUser(const std::string& name,const std::string& pass);


#endif   // POINT_HPP