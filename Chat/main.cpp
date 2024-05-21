#include<iostream>
#include<map>
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
	Users::Data udata;
	string screen_choice;


	while (true) {

		while (udata.token == "") {
			cout << "1 - Registration" << endl << "2 - Authorization" << endl << "3 - Exit" << endl;
			cin >> screen_choice;

			if (screen_choice == "1") {
				udata = Screen::Registration();
				break;
			}

			if (screen_choice == "2") {
				udata = Screen::Authorization();
				break;
			}

			if (screen_choice == "3") {
				cout << "Exit" << endl;
				return 0;
			}

			system("cls");
			cout << "Enter 1-3 value or gtfo" << endl;
		}

		while (Users::verify_token(udata.token)) {
			Screen::Chat(Screen::SelectChat(), udata.login);
		}
	}

	return 0;
}
