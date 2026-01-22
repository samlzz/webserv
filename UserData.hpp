
#pragma once
#include <string>
#include <map>

class UserData
{
private:
	struct userDB
	{
		std::string salt;
		std::string hashedPassword;
	};
	typedef std::map<std::string, userDB>	t_userDB;
	t_userDB userDatabase;

	std::string asset_database;

public:
	UserData();
	~UserData();
	UserData(const std::string &db_file);

	void readDatabase(const std::string &filename);

	void setUserDatabase(const std::string &username, const userDB &db);
	void addUser(const std::string &username, const std::string &password);

	std::string generateRandomSalt(void);
	
	bool verifyUser(const std::string &username, const std::string &password);
	void deleteUser(const std::string &username);
	void deleteLineFromFile(const std::string &filename, const std::string &lineToDelete);

	void printDatabase(void);
};

