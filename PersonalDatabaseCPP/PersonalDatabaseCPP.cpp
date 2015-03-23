// PersonalDatabaseCPP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <cstring>
using namespace std;




// ** CONTSTANTS ** //
const int STRING_SIZE = 51;
const int DATABASE_SIZE = 100;
const int FIELDS = 4;
//field indexes
const int LNAME = 0;
const int FNAME = 1;
const int PHONE = 2;
const int BDATE = 3;


// ** FORWARD DECLARATION ** //
void profile_string(char _profile[FIELDS][STRING_SIZE], char _str[]);
void print_data(int _records, char _profiles[][FIELDS][STRING_SIZE]);
void print_menu();

int read_data(char _filename[], int _maxsize, char _profiles[][FIELDS][STRING_SIZE]);
void save_data(char _filename[], int _records, char _profiles[][FIELDS][STRING_SIZE]);

void sort_profiles(int _records, char _profiles[][FIELDS][STRING_SIZE]);
bool name_compare(char _a[][STRING_SIZE], char _b[][STRING_SIZE]);

void get_input(int _stringsize, char _str[]);
void clean_input();

ifstream file_open_read(char _filepath[]);
ofstream file_open_write(char _filepath[]);
ofstream file_open_append(char _filepath[]);
bool file_close(ofstream &_file);

// ** BEGIN ** //

int _tmain(int argc, _TCHAR* argv[])
{
	char profiles[DATABASE_SIZE][FIELDS][STRING_SIZE];

	int records = read_data("profiles.dtb", DATABASE_SIZE, profiles);

	int input = 0;
	bool done = false;
	while (!done)
	{
		print_menu();
		cin >> input;
		clean_input();

		switch (input)
		{
			case 1:

			break;
			case 2:

			break;
			case 3:

			break;
			case 4:
				print_data(records, profiles);
			break;
			case 5:
				cout << "Goodbye!";
				done = true;
			break;
			default:
				cout << "Invalid Choice";
			break;
		}
	}

	save_data("profiles.dtb", records, profiles);

	return 0;
}


// ** PROGRAM FUNCTIONS ** //

// print
void print_data(int _records, char _profiles[][FIELDS][STRING_SIZE])
{
	cout << "--------------------------------------------------------------" << endl;
	for (int i = 0; i < _records; i++)
	{
		char profile[FIELDS*STRING_SIZE] = {""};
		profile_string(_profiles[i], profile);
		cout << profile << endl;
	}
}

void print_menu()
{
	cout << endl << "--------------------------------------------------------------" << endl;
	cout << "\tPersonal Database" << endl << endl
		<< "1. Find a person's information" << endl
		<< "2. Add a person to the database" << endl
		<< "3. Edit a person's information" << endl
		<< "4. Display all profiles" << endl
		<< "5. Quit" << endl << endl
		<< "input: ";
}

void profile_string(char _profile[FIELDS][STRING_SIZE], char _str[])
{
	strcat(_str, _profile[LNAME]);
	strcat(_str, " ");
	strcat(_str, _profile[FNAME]);
	strcat(_str, " ");
	strcat(_str, _profile[PHONE]);
	strcat(_str, " ");
	strcat(_str, _profile[BDATE]);
}

// file data
int read_data(char _filename[], int _maxsize, char _profiles[][FIELDS][STRING_SIZE])
{
	int records = 0;

	ifstream file = file_open_read(_filename);
	if (file.is_open())
	{
		do
		{
			for (int i = 0; i < FIELDS; i++)
			{
				file >> _profiles[records][i];
				//int slen = strlen(_profiles[datasize][i]);
				//_profiles[datasize][i][slen] = '\0';
			}
			records++;
		} while (!file.eof());
	}
	//for priming read...
	records = (records == 1) ? 0 : records;

	sort_profiles(records, _profiles);
	
	return records;
}

void save_data(char _filename[], int _records, char _profiles[][FIELDS][STRING_SIZE])
{
	ofstream file = file_open_write(_filename);

	if (file.is_open())
	{
		for (int i = 0; i < _records; i++)
		{
			char profile[FIELDS*STRING_SIZE] = { "" };
			profile_string(_profiles[i], profile);
			file << profile << endl;
		}
	}

	file_close(file);
}



// profile manipulation
void sort_profiles(int _records, char _profiles[][FIELDS][STRING_SIZE])
{
	bool sorted = false;
	for (int i = 0; i < _records - 1; i++)
	{
		sorted = true;
		for (int j = 0; j < _records - 1; j++)
		{
			if (name_compare(_profiles[j], _profiles[j + 1]))
			{
				sorted = false;
				swap(_profiles[j], _profiles[j + 1]);
			}
		}
		if (sorted) break;
	}
}
bool name_compare(char _a[][STRING_SIZE], char _b[][STRING_SIZE])
{
	char n1[STRING_SIZE * 2] = {""};
	strcat(n1, _a[LNAME]);
	strcat(n1, _a[FNAME]);
	char n2[STRING_SIZE * 2] = {""};
	strcat(n2, _b[LNAME]);
	strcat(n2, _b[FNAME]);

	//if (_stricmp(n1,n2) < 0)
	//	cout << "Swapping: " << n1 << " & " << n2 << endl;

	return (_stricmp(n1,n2) > 0);
}





// ** UTILITY FUNCTIONS ** //

//input
void get_input(int _stringsize, char _str[])
{
	cin.ignore(cin.rdbuf()->in_avail());
	cin.getline(_str,_stringsize);
	clean_input();
}
string get_input()
{
	string str;

	cin.ignore(cin.rdbuf()->in_avail());
	getline(cin, str);
	clean_input();

	return str;
}

void clean_input()
{
	cin.clear();
	cin.ignore(cin.rdbuf()->in_avail());
}

//file manipulation
ifstream file_open_read(char _filepath[])
{
	ifstream fdata(_filepath);

	if (fdata.fail())
		cout << "Failed to open file for reading";

	return fdata;
}

ofstream file_open_write(char _filepath[])
{
	ofstream fdata(_filepath);

	if (fdata.fail())
		cout << "Failed to open file for writing";

	return fdata;
}

ofstream file_open_append(char _filepath[])
{
	ofstream fdata(_filepath, ios::app);

	if (fdata.fail())
		cout << "Failed to open file for appending";

	return fdata;
}

bool file_close(ofstream &_file)
{
	if (_file.is_open())
	{
		_file.close();
		return true;
	}

	return false;
}
bool file_close(ifstream &_file)
{
	if (_file.is_open())
	{
		_file.close();
		return true;
	}

	return false;
}