#pragma once
#include"users.h"
#include <time.h>
#include <fstream> 
#include <limits>
#include <string>
#include <unordered_map>
#include <map>

using namespace std;

class Users {
private:
	static const string users_file;

public:
	static const string delim;

	struct Data {
	public:
		string login;
		string pwd_hash;
		string token;
		string name;
		int cursor_pos = 0;
		int line_length = 0;

		Data() {
			this->cursor_pos = -1;
		};

		Data(string line, int cursor_pos) {
			// delim size
			int ds = Users::delim.length();

			int first_delim_pos = int(line.find(Users::delim));
			this->login = line.substr(0, first_delim_pos);

			this->pwd_hash = line.substr(
				first_delim_pos + ds, 
				line.find(Users::delim, first_delim_pos + 1) - first_delim_pos - ds
			);

			int token_offset = login.length() + ds * 2 + pwd_hash.length();
			this->token = line.substr(token_offset, line.find(Users::delim, token_offset) - token_offset);
			this->name = line.substr(token_offset + token.length() + ds);
			this->cursor_pos = cursor_pos;
		}
	};

	static string gen_token(bool zero_token = false) {
		if (zero_token) {
			return "00000000000000000000000000000000";
		}
		string token;
		srand(static_cast<unsigned int>(time(nullptr)));
		for (int i = 0; i < 32; ++i) {
			token.append(to_string(rand() % 10));
		}
		
		return token;
	}

	// проверяет актуальность текущего токена
	static bool verify_token(string token) {
		return token != "" && search(token, "token").token == token;
	}
	
	static Data search(string data, string by = "login") { // data - логин или токен
		string pattern = data + delim; // паттерн поиска
		string line; // текущая строка
		int pattern_pos = -1;
		int pattern_pos_max = 0; // ограничение позиции вхождения паттерна в строку

		fstream file(users_file);
		if (!file.is_open()) {
			return Data();
		}

		if (by == "token") {
			pattern = delim + data + delim;
			pattern_pos_max = INT_MAX;
		}

		while (getline(file, line)) {
			pattern_pos = line.find(pattern);

			if (pattern_pos > pattern_pos_max || pattern_pos == -1) {
				continue;
			}

			break;
		}
		int cursor_pos = int(file.tellg());
		file.close();

		//если паттерн не найден, возвращается конструктор пустой даты с офсетом -1
		if (pattern_pos == -1) {
			return Data();
		}

		return Data(line, cursor_pos);
	}

	static Data registration(string login, string pwd, string name) {
		
		// проверка на логин == пустая строка или содержание пробелов
		if (login == "" || login.find(" ") > -1) { 
			return Data();
		}

		// проверка наличия логина в базе
		if (search(login).cursor_pos > -1) {
			return Data();
		}

		fstream file(users_file, ios::app);
		if (!file.is_open()) {
			return Data();
		}

		// запись строки нового пользователя в файл
		hash<string> hasher;
		file << login << delim << hasher(pwd) << delim << gen_token(true)//zero token gen
			<< delim << name << endl;
		file.close();

		// сразу его авторизуем
		return authorization(login, pwd);
	}

	static Data authorization(string login, string pwd) {
		// пишем в поля структуры разделенную строку 
		Data user_data = search(login);

		// если логин в файле не найден (в офсет записано find(login) == -1),
		// возвращает пустую структуру (см конструктор по умолчанию)
		if (user_data.cursor_pos == -1) {
			return Data();
		}

		hash<string> hasher;

		// сравнение хэшей пароля
		if (to_string(hasher(pwd)) != user_data.pwd_hash) {
			return Data();
		}

		fstream file(users_file);
		if (!file.is_open()) {
			return Data();
		}

		// ставим курсор на позицию начала токена в строке
		file.seekp(user_data.cursor_pos - user_data.name.length() - 41);

		// создаем новый токен и пишем его в файл
		user_data.token = gen_token();
		file << user_data.token;
		file.close();

		return user_data;
	}

	static map <string, string> get_users_logins_map() {
		// создает список логинов в тип map
		map <string, string> logins;
		string line;
		int i = 0;

		fstream file(users_file);
		if (!file.is_open()) {
			return logins;
		}

		while (getline(file, line)) {
			++i;
			logins.emplace(
				to_string(i), 
				line.substr(0, line.find(delim))
			);
		}
		file.close();

		return logins;
	}
};