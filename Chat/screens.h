#pragma once
#include"users.h"
#include <string>
#include<iostream>
#include "messages.h"
using namespace std;

class Screen {
public:

	static Users::Data Registration() {
		string login;
		string pwd;
		string name;
		Users::Data udata;

		
		while (udata.token == "") {
			while (true) {
				cin.ignore();
				cin.clear();
				// логин с проверкой валидности
				cout << "Enter login: ";
				getline(cin, login);

				if (login.find(Users::delim) != string::npos || login == "Group" || login == "") {
					cout << "Login must not contain <delim> or Group or be an empty line!" << endl;
					continue;
				}
				break;
			}
			// не проверяю валидность, потому что дальше используется хэш пароля
			cout << "Enter pwd: ";
			getline(cin, pwd);

			while (true)
			{
				// имя с проверкой валидности
				cout << "Enter your name: ";
				getline(cin, name);

				if (name.find(Users::delim) != string::npos || name == "") {
					cout << "Name must not contain <delim> or Group or be an empty line!" << endl;
					continue;
				}
				break;
			}
			udata = Users::registration(login, pwd, name);
		}

		return udata;
	}

		static Users::Data Authorization() {
			cin.ignore();
			cin.clear(); // очистка буфера потока ввода перед getline
			string login;
			string pwd;
			Users::Data udata;

			while (udata.token == "") {
				cout << endl << "Enter login: ";
				getline(cin, login);
				cout << endl << "Enter pwd: ";
				getline(cin, pwd);

				udata = Users::authorization(login, pwd);
			}

			return udata;
		}

		static string SelectChat() {

			// получение карты пользователей для последующего вывода на экран
			map <string, string> users_map = Users::get_users_logins_map(); 
			string input = "";
			system("cls"); // очистка консоли перед выводом экрана

			cout << "Choose chat: " << endl;
			cout << "q - Exit" << endl;
			cout << "0 - Group" << endl;

			// вывод доступных логинов на экран
			for (int i = 1; i <= users_map.size(); ++i) {
				cout << i << " - " << users_map[to_string(i)] << endl;
			}
			cin >> input;
			if (input == "q") {

				exit(0);
			}

			if (input == "0") {

				return "Group";
			}

			return users_map[input];
		}

		static void Chat(string login, string client_login) {
			cin.clear();
			string input = "";
			while (true) {
				system("cls"); // очистка экрана
				cout << "q - Exit" << endl;
				cout << "Chat with " << login << endl;

				// пробую тернарный оператор в качестве одного из аргументов функции:
				Messages::draw_chat_history( 
					login == "Group" ? // если логин == Group - true
						Messages::get_public_messages() // выполняется get_public_messages
						: Messages::get_private_messages(login, client_login), // если false, выполняется get_private_messages
					client_login // второй аргумент
				);

				cout << endl << "Enter message: ";
				getline(cin, input);
				if (input == "q") {
					break;
				}
				Messages::send_message(client_login, input, login);
			}
		}	
};