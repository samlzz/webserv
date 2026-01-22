#include "UserData.hpp"

#include <iostream>
#include <string>
#include <functional>
#include <sstream>
#include <map>
#include <fstream>
#include <cstdlib>


unsigned long hash_djb2(const std::string& str) {
    unsigned long hash = 5381;
    for (size_t i = 0; i < str.length(); i++) {
        // hash * 33 + caractère
        hash = ((hash << 5) + hash) + str[i];
    }
    return hash;
}

std::string hashPassword(const std::string &pPassword) {
	unsigned long hashed = hash_djb2(pPassword);
	std::ostringstream oss;
	oss << hashed;
	return oss.str();
}

unsigned long hash_sdbm(const std::string& str) {
    unsigned long hash = 0;
    for (size_t i = 0; i < str.length(); ++i) {
        // hash = c + (hash << 6) + (hash << 16) - hash;
        hash = static_cast<unsigned char>(str[i]) + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

std::string hashPasswordSDBM(const std::string &pPassword) {
	unsigned long hashed = hash_sdbm(pPassword);
	std::ostringstream oss;
	oss << hashed;
	return oss.str();
}

// CONSTRUCTOR / DESTRUCTOR
UserData::UserData() : asset_database("assets/users.db") {
	readDatabase(asset_database);
}

UserData::~UserData() {
}

UserData::UserData(const std::string &db_file) : asset_database(db_file) {
	readDatabase(asset_database);
}

// SETTERS
void UserData::setUserDatabase(const std::string &username, const userDB &db)
{
	userDatabase[username] = db;
}
// METHODS
std::string UserData::generateRandomSalt(void) {
	const char charset[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	const size_t max_index = (sizeof(charset) - 1);
	std::string salt;
	for (size_t i = 0; i < 16; ++i) {
		salt += charset[rand() % max_index];
	}
	return salt;
}

void UserData::addUser(const std::string &username, const std::string &password){
	if (userDatabase.find(username) != userDatabase.end())
	{
		std::cerr << "User already exists!" << std::endl;
		return;
	}
	std::string salt = generateRandomSalt();
	std::string saltedPassword = salt + password;
	std::string hashedPassword = hashPassword(saltedPassword);

	userDB newUser;
	newUser.salt = salt;
	newUser.hashedPassword = hashedPassword;
	userDatabase[username] = newUser;

	//write to file
	std::ofstream file(asset_database.c_str(), std::ios::app);
	if (!file.is_open()) {
		std::cerr << "Failed to open user database file: " << asset_database << std::endl;
		return;
	}
	file << username << ":" << salt << ":" << hashedPassword << std::endl;
	file.close();
}

void 	UserData::readDatabase(const std::string &filename)
{
	// Implementation to read user database from file
	std::cout << "Reading user database from: " << filename << std::endl;

	std::ifstream file(filename.c_str());
	if (!file.is_open()) {
		std::cerr << "Failed to open user database file: " << filename << std::endl;
		return;
	}
	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string username, salt, hashedPassword;

		//split line by ':'
		std::getline(iss, username, ':');
		std::getline(iss, salt, ':');
		std::getline(iss, hashedPassword, ':');
		// std::cout << "User: " << username << ", Salt: " << salt << ", Hashed Password: " << hashedPassword << std::endl;
		
		userDB newUser;
		newUser.salt = salt;
		newUser.hashedPassword = hashedPassword;
		UserData::setUserDatabase(username, newUser);
	}
}

void UserData::printDatabase(void)
{
	for (t_userDB::const_iterator it = userDatabase.begin(); it != userDatabase.end(); ++it)
	{
		std::cout << "Username: " << it->first << ", Salt: " << it->second.salt << ", Hashed Password: " << it->second.hashedPassword << std::endl;
	}
}


bool UserData::verifyUser(const std::string &username, const std::string &password)
{
	t_userDB::const_iterator it = userDatabase.find(username);
	if (it == userDatabase.end())
	{
		std::cerr << "User not found!" << std::endl;
		return false;
	}
	std::string salt = it->second.salt;
	std::string saltedPassword = salt + password;
	std::string hashedPassword = hashPassword(saltedPassword);

	if (hashedPassword == it->second.hashedPassword)
	{
		std::cout << "User verified successfully!" << std::endl;
		return true;
	}
	else
	{
		std::cerr << "Invalid password!" << std::endl;
		return false;
	}
}


void UserData::deleteUser(const std::string &username)
{
	t_userDB::iterator it = userDatabase.find(username);
	if (it == userDatabase.end())
	{
		std::cerr << "User not found!" << std::endl;
		return;
	}
	userDatabase.erase(it);

	// Delete line from file
	// deleteLineFromFile(asset_database, username + ":" + it->second.salt + ":" + it->second.hashedPassword);

	// Rewrite the database file
	std::ofstream file(asset_database.c_str());
	if (!file.is_open()) {
		std::cerr << "Failed to open user database file: " << asset_database << std::endl;
		return;
	}
	for (t_userDB::const_iterator it = userDatabase.begin(); it != userDatabase.end(); ++it)
	{
		file << it->first << ":" << it->second.salt << ":" << it->second.hashedPassword << std::endl;
	}
	file.close();
}


void UserData::deleteLineFromFile(const std::string &filename, const std::string &lineToDelete)
{
	std::ifstream fileIn(filename.c_str());
	if (!fileIn.is_open()) {
		std::cerr << "Failed to open file: " << filename << std::endl;
		return;
	}
	std::ofstream fileOut("temp.db");
	if (!fileOut.is_open()) {
		std::cerr << "Failed to open temporary file." << std::endl;
		return;
	}
	std::string line;
	while (std::getline(fileIn, line))
	{
		if (line != lineToDelete)
		{
			fileOut << line << std::endl;
		}
	}
	fileIn.close();
	fileOut.close();
	std::remove(filename.c_str());
	std::rename("temp.db", filename.c_str());
	std::cout << "Deleted user line from file: " << lineToDelete << std::endl;
}

int main() {

	UserData user("assets/users.db");
	// UserData.printDatabase();
	user.addUser("new_user", "new_password");
	// user.printDatabase();

	user.verifyUser("new_user", "new_password");

	user.deleteUser("new_user");
	user.printDatabase();

	user.addUser("new_user1", "new_password");
	user.addUser("new_user2", "new_passwore");
	user.addUser("new_user3", "new_passworc");
	user.addUser("new_user4", "new_password1");
	user.addUser("new_user5", "new_passwor0");

	user.verifyUser("new_user1", "new_password");
	user.verifyUser("new_user2", "new_passwore");
	user.verifyUser("new_user3", "new_passworc");
	user.verifyUser("new_user4", "new_password1");
	user.verifyUser("new_user5", "new_passwor0");

	// std::string filedata("assets/users.db");
	// user.readDatabase(filedata);
	// std::string pPassword = "zybmy_secure_password1";
	// std::string _hashedPassword = hashPassword(pPassword);
	// std::cout << "Original Password: " << pPassword << std::endl;
	// std::cout << "Hashed Password: " << _hashedPassword << std::endl;

	// std::string pPassword2 = "zybmy_secure_passwore2";
	// std::string _hashedPassword2 = hashPassword(pPassword2);
	// std::cout << "Original Password: " << pPassword2 << std::endl;
	// std::cout << "Hashed Password: " << _hashedPassword2 << std::endl;

	// std::string _hashedPasswordSDBM = hashPasswordSDBM(pPassword);
	// std::cout << "Hashed Password (SDBM): " << _hashedPasswordSDBM << std::endl;

	// std::string _hashedPasswordSDBM2 = hashPasswordSDBM(pPassword2);
	// std::cout << "Hashed Password (SDBM): " << _hashedPasswordSDBM2 << std::endl;

	return 0;
}