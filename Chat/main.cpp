#include<iostream>
#include"users.h"
using namespace std;

string const Users::delim = "<delim>";
string const Users::users_file = "users.txt";

int main() {
	Users::registration("kuku", "qwerty", "Lalka");
	string auth = Users::authorization("kuku", "qwerty");
	cout << auth << endl;
	cout << Users::verify_token(auth);


	return 0;
}
