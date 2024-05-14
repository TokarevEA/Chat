#pragma once
#include"users.h"
#include <time.h>
#include <fstream> 
#include <limits>
#include <string>
#include <unordered_map>

using namespace std;

class Users {
private:
	static const string users_file;
	static const string delim;

public:

	struct Data {
	public:
		string login;
		string pwd_hash;
		string token;
		string name;
		int offset = 0;
		int line_length = 0;

		Data() {
			this->offset = -1;
			cout << endl << "new struct offset: " << this->offset << endl;
		};


		Data(string line, int offset) {
			string delim = "<delim>";
			int delim_pos = int(line.find(delim));
			this->login = line.substr(0, delim_pos);
			this->pwd_hash = line.substr(delim_pos + 7, (line.find(delim, delim_pos + 1) - delim_pos - 7));

			int token_offset = login.length() + 7 * 2 + pwd_hash.length();
			this->token = line.substr(token_offset, line.find(delim, token_offset) - token_offset);
			this->name = line.substr(token_offset + token.length() + 7);
			this->offset = offset;
		}
	};


	static string gen_token(bool zero_t = false) {
		string token;
		if (zero_t) {
			for (int i = 0; i < 32; ++i) {
				token.append("0");
			}
		}
		else {
			srand(static_cast<unsigned int>(time(nullptr)));
			for (int i = 0; i < 32; ++i) {
				token.append(to_string(rand() % 10));
			}
		}
		return token;
	}


	static bool verify_token(string token) {

		return Users::search(token, "token").token == token;
	}



	static Users::Data search(string str, string by = "login") { // str - логин или токен
		string pattern = str + Users::delim; // паттерн поиска
		string line; // текущая строка
		int out = -1;
		int offset = 0; // финальная позиция нахождения паттерна в строке

		fstream file(users_file);
		if (!file.is_open()) {
			Users::Data data = Users::Data();
			return data;
		}

		if (by == "token") {
			pattern = delim + str + delim;
			offset = numeric_limits<int>::max();
		}

		while (getline(file, line)) {
			out = line.find(pattern);

			if (out >= offset || out > 0) {
				break;
			}

		}

		offset = int(file.tellg());	//где остановился курсор в файле после чтения
		file.close();

		//если паттерн не найден, возвращается конструктор пустой даты с оффсетом -1
		if (out == -1) {
			Users::Data data = Users::Data();

			return data;
		}

		Users::Data data = Users::Data(line, offset);
		return data;
	}


	static string registration(string login, string pwd, string name) {
		if (login == "" || login.find(" ") > -1) {
			return "";
		}
		fstream file(users_file, ios::app);
		if (!file.is_open()) {
			return "";
		}
		if (Users::search(login).offset > -1) {
			return "";
		}
		hash<string> hasher;
		file << login << delim << hasher(pwd) << delim << gen_token(true)//zero token gen
			<< delim << name << endl;
		file.close();
		return authorization(login, pwd);
	}


	static string authorization(string login, string pwd) {
		Data user_data = search(login);
		hash<string> hasher;
		if (user_data.offset == -1) {
			return "";
		}

		if (to_string(hasher(pwd)) != user_data.pwd_hash) {
			return "";
		}

		else {
			fstream file(users_file);
			if (!file.is_open()) {
				return "";
			}

			file.seekp(user_data.offset - user_data.name.length() - 41);
			file << gen_token();
			file.close();
			user_data.token = gen_token();
			return user_data.token;
		}
	}


	static string get_users_logins() {
		string logins;
		string line;
		fstream file(users_file);
		if (!file.is_open()) {
			return "";
		}

		while (getline(file, line)) {
			logins.append(line.substr(0, line.find(Users::delim)) + "\n");
		}

		return logins;
	}
};