#pragma once
#include"users.h"
#include <string>
#include<iostream>
#include "messages.h"
using namespace std;

class Screen {
public:
	static string Registration() {
		string login;
		string pwd;
		string name;
		string token = "";

		while (token == "") {
			cout << "Enter login: " << endl;
			cin >> login;
			cout << "Enter pwd: " << endl;
			cin >> pwd;
			cout << "Enter your name: " << endl;
			cin >> name;

			token = Users::registration(login, pwd, name);
		}

		return token;
	}

	static string Authorization() {
		string login;
		string pwd;
		string token = "";

		while (token == "") {
			cout << "Enter login: " << endl;
			cin >> login;
			cout << "Enter pwd: " << endl;
			cin >> pwd;

			token = Users::authorization(login, pwd);
		}

		return token;
	}

	static string SelectChat() {

		map <string, string> users_map = Users::get_users_logins_map();
		string input = "";


		cout << "Choose chat: " << endl;
		cout << "q - Exit" << endl;
		cout << "0 - Group" << endl;
		for (int i = 1; i < users_map.size(); ++i) {
			cout << i << " - " << users_map[to_string(i)] << endl;
		}
		cin >> input;
		if (input == "q") {
			exit(0);
		}

		return users_map[input];
	}

	static void Chat(string login, string client_login) {
		string input = "";
		while (input != "q") {

			if (login == "Group") {
				Messages::draw_chat_history(Messages::get_public_messages());
			}

			Messages::draw_chat_history(Messages::get_private_messages(login, client_login));
			cout << endl << "Enter message:" << endl;
			cin >> input;
			Messages::send_message(client_login, input, login);
		}
	}
};