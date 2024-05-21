#pragma once
#include <string>
#include<iostream>
#include <chrono>
#include <ctime>
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
			this->author_login = "";
			this->message = "";
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
	
	static Messages::Message* get_private_messages(string login1, string login2) {
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
		//файл читается от более новых строк к более старым (с конца в начало)
		while (saved_messages < 20 && file.tellg() > -1) {
			// посимвольное чтение строки с конца в начало
			file.seekg(-1, std::ios::cur);
			do {
				file.seekg(-2, std::ios::cur); // двигаем указатель на два символа назад
				file.get(symbol); // считываем символ

				if (symbol == '\n') {
					break;
				}

				line = symbol + line;
			} while (file.tellg() > -1);
			// записываю позицию обоих вариантов расположения логинов в строке,
			// чтобы найти и отправленные, и полученные сообщения
			pos = line.find(login1 + mess_delim + login2);
			pos1 = line.find(login2 + mess_delim + login1);

			if (pos == 0 || pos1 == 0) {
				Messages::Message message_struct = Messages::Message(line);
				messages[19 - saved_messages] = message_struct;
				++saved_messages;
			}

			line = "";
		}

		file.close();
		return messages;
	}

	static Messages::Message* get_public_messages(string login) {
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
				
				if (symbol == '\n') {
					break;
				}

				line = symbol + line;
			} while (file.tellg() > -1);

			pos = line.find(login + mess_delim + "Group" + mess_delim);

			if (pos > -1) {
				Messages::Message message_struct = Messages::Message(line);
				messages[19 - saved_messages] = message_struct;
				++saved_messages;
			}
			line = "";
		}

		file.close();
		return messages;
	}


	static void send_message(string cli_login, string message, string rec_login = "Group") {

		// Ограничение размера сообщения и ввода запретных паттернов
		if (message.length() > 2000 || message.length() < 1 || message.find(Users::delim) != string::npos) {
			return;
		}

		// Получение текущего времени и форматирование в строку
		auto now = chrono::system_clock::now();
		time_t now_time = chrono::system_clock::to_time_t(now);

		struct tm timeinfo;
		localtime_s(&timeinfo, &now_time);

		char time_string[80];
		strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", &timeinfo);

		fstream file(messages_file, ios::app);
		if (!file.is_open()) {
			return;
		}

		//запись строки с сообщением и метками в файл
		file << cli_login << mess_delim << rec_login << mess_delim << message << mess_delim << time_string << endl;
		file.close();
		return;
	}

	static void draw_chat_history(Message* ms, string login) {
		for (int i = 0; i < 20; ++i) {

			//выводит на экран поля структуры, если прошла проверка на "" 
			// (см. конструктор структуры по умолчанию)
			if (ms[i].message == "") {
				continue;
			}
			// формирование строки и наведение консольной красоты
			cout << endl
				<< "\033[32m[" << ms[i].time_string << "]\033[0m "
				<< (ms[i].author_login == login ? "\033[33m" : "")
				<< ms[i].author_login << " >>> "
				<< ms[i].rec_login << ": "
				<< ms[i].message << "\033[0m ";

			
		}
	}
};