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
		string author_login;
		string rec_login;
		string message;
		string time_string;

		Message() {
			time_t t1 = time(NULL);
			tm t;
			localtime_s(&t, &t1);
			this->author_login = "system";
			this->message = "error"; //чтобы обрабатывать ошибки в методах get_ _messages
			this->time_string = to_string(t.tm_hour) + ":" + to_string(t.tm_min) + ":" + to_string(t.tm_sec);
		}
				

		//конструктор с алгоритмом разбиения строки на поля структуры
		Message(string line) {
			if (line[0] == '\n') {
				line = line.substr(1);
			}
			int delim_pos = line.find(mess_delim);
			this->author_login = line.substr(0, delim_pos);
			this->rec_login = line.substr(delim_pos + 7, (line.find(mess_delim, delim_pos + 1) - delim_pos - 7));

			int mess_offset = author_login.length() + rec_login.length() + 14;
			this->message = line.substr(mess_offset, line.find(mess_delim, mess_offset) - mess_offset);
			this->time_string = line.substr(mess_offset + message.length() + 7);
		}
	};

	static Messages::Message* get_private_messages(string sender_login, string cli_login) {
		string line = "";
		char symbol;
		Messages::Message* messages = new Messages::Message[20];
		int pos;
		int pos1;
		int saved_messages = 0;


		fstream file(messages_file, ios_base::in | ios_base::ate);
		if (!file.is_open()) {
			Messages::Message null_struct = Messages::Message();
			return messages;
		}

		while (saved_messages < 20 && file.tellg() > -1) {

			file.seekg(-1, std::ios::cur);
			do {
				file.seekg(-2, std::ios::cur); // двигаем указатель на два символа назад
				file.get(symbol); // считываем символ
				line = symbol + line;
			} while (symbol != '\n' && file.tellg() > -1);

			pos = line.find(sender_login + mess_delim + cli_login);
			pos1 = line.find(cli_login + mess_delim + sender_login);

			if (pos > -1 || pos1 > -1) {
				Messages::Message message_struct = Messages::Message(line);
				messages[19 - saved_messages] = message_struct;
				++saved_messages;
			}

			line = "";
		}

		return messages;
	}
		
	static Messages::Message* get_public_messages() {
		string line = "";
		char symbol;
		Messages::Message* messages = new Messages::Message[20];
		int pos;
		int saved_messages = 0;

		fstream file(messages_file, ios_base::in | ios_base::ate);
		if (!file.is_open()) {
			Messages::Message null_struct = Messages::Message();
			return messages;
		}
		while (saved_messages < 20 && file.tellg() > -1) {
			file.seekg(-1, std::ios::cur);
			do {
				file.seekg(-2, std::ios::cur); // двигаем указатель на два символа назад
				file.get(symbol); // считываем символ
				line = symbol + line;
			} while (symbol != '\n' && file.tellg() > -1);

			pos = line.find(mess_delim + "Group" + mess_delim);

			if (pos > -1) {
				Messages::Message message_struct = Messages::Message(line);
				messages[19 - saved_messages] = message_struct;
				++saved_messages;
			}
			line = "";			
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
			//выводит на экран поля структуры, если прошла проверка на "error" 
			// (см. конструктор структуры по умолчанию)
			if (message_struct[i].message != "error") {
				cout << endl << message_struct[i].author_login 
					<< " " << message_struct[i].message << " " 
					<< message_struct[i].time_string;
			}
		}
	}
};