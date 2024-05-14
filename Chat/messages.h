#pragma once
#include <string>
#include<iostream>
using namespace std;

class Messages {
private:
	static const string messages_file;
	static const string mess_delim;
public:

	struct Message {
	public:
		string cli_login;
		string rec_login;
		string message;
		string time_stamp;

		Message() {
			this->message = "error"; //чтобы обрабатывать ошибки в методах get_ _messages
		}
				

		//конструктор с алгоритмом разбиения строки на поля структуры
		Message(string line) {
			int delim_pos = line.find(mess_delim);
			this->cli_login = line.substr(0, delim_pos);
			this->rec_login = line.substr(delim_pos + 7, (line.find(mess_delim, delim_pos + 1) - delim_pos - 7));

			int mess_offset = cli_login.length() + rec_login.length() + 14;
			this->message = line.substr(mess_offset, line.find(mess_delim, mess_offset) - mess_offset);
			this->time_stamp = line.substr(mess_offset + message.length() + 7);
		}
	};

	static Messages::Message* get_private_messages(string sender_login, string cli_login) {
		string line;
		Messages::Message* messages = new Messages::Message[20];
		int pos;
		int pos1;

		fstream file(messages_file);
		if (!file.is_open()) {
			Messages::Message null_struct = Messages::Message();
			return messages;
		}
		for (int i = 0; i < 20; ++i) {
			getline(file, line);
			if (line == "") {
				break;
			}
			pos = line.find(sender_login + mess_delim + cli_login);
			pos1 = line.find(cli_login + mess_delim + sender_login);
			if (pos > -1 || pos1 > -1) {
				Messages::Message message_struct = Messages::Message(line);
				messages[i] = message_struct;
			}
		}
		return messages;
	}

	static Messages::Message* get_public_messages() {
		string line;
		Messages::Message* messages = new Messages::Message[20];
		int pos;

		fstream file(messages_file);
		if (!file.is_open()) {
			Messages::Message null_struct = Messages::Message();
			return messages;
		}
		//в цикле читает 20 строк из файла
		for (int i = 0; i < 20; ++i){
			getline(file, line);
			if (line == "") {
				break;
			}
			//проверяет позицию нахождения паттерна в строке. Если паттерн найден, создает структуру и сует ее в массив
			pos = line.find(mess_delim + "Group" + mess_delim);
			if (pos > -1) {
				Messages::Message message_struct = Messages::Message(line);
				messages[i] = message_struct;
			}
		}
		return messages;
	}

	static void send_message(string cli_login, string message, string rec_login = "Group") {

		//создание метки времени
		time_t t1 = time(NULL);
		tm t;
		localtime_s(&t, &t1);
		string time_stamp = to_string(t.tm_hour) + ":" + to_string(t.tm_min) + ":" + to_string(t.tm_sec);

		fstream file(messages_file, ios::app);
		if (!file.is_open()) {
			return;
		}
		//запись строки с сообщением и метками в файл
		file << cli_login << mess_delim << rec_login << mess_delim << message << mess_delim << time_stamp <<endl;
		file.close();
		return;
	}

	static void draw_chat_history(Messages::Message* message_struct) {
		for (int i = 0; i < 20; ++i) {
			//выводит на экран поля структуры, если прошла проверка на "error" (см. конструктор структуры по умолчанию)
			if (message_struct[i].message != "error") {
				cout << endl << message_struct[i].cli_login << " " << message_struct[i].message << " " << message_struct[i].time_stamp << endl;
			}
		}
	}
};