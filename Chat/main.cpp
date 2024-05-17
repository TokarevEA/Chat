#include<iostream>
#include <map>
#include<ctime>
#include"screens.h"
#include"users.h"
#include"messages.h"
using namespace std;

string const Users::delim = "<delim>";
string const Users::users_file = "users.txt";
string const Messages::messages_file = "messages.txt";
string const Messages::mess_delim = "<delim>";



int main() {
	string client_login = "Lol1";
	string token = "";
	string name;
	int choice;

	while (true) {

		cout << "1 - Registration" << endl << "2 - Authorization" << endl << "3 - Exit" << endl;
		cin >> choice;
		switch (choice)
		{
		case 1:
			token = Screen::Registration();
			break;

		case 2:
			token = Screen::Authorization();
			break;

		case 3:
			cout << "Exit" << endl;
			return 0;

		default:
			cout << "Enter 1-3 value or gtfo" << endl;
			break;
		}

		while (Users::verify_token(token)) {

			Screen::Chat(Screen::SelectChat(), client_login);
		}

		return 0;
	}
}
