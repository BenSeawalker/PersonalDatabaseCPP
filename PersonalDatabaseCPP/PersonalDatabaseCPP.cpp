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




// ** CONSTANTS ** //
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
void print_profile(char _profile[FIELDS][STRING_SIZE]);
void print_profile(ofstream &_file, char _profile[FIELDS][STRING_SIZE]);

int read_data(char _filename[], int _maxsize, char _profiles[][FIELDS][STRING_SIZE]);
void save_data(char _filename[], int _records, char _profiles[][FIELDS][STRING_SIZE]);

int find_user_data(int _records, char _profiles[][FIELDS][STRING_SIZE]);

void sort_profiles(int _records, char _profiles[][FIELDS][STRING_SIZE], bool _ascending);
bool name_compare(char _a[][STRING_SIZE], char _b[][STRING_SIZE], bool _ascending);

void get_input(int _stringsize, char _str[]);
void wait_for_input();
void clean_input();

bool string_match(char _str1[], char _str2[]);

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
				find_user_data(records, profiles);
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
		print_profile(_profiles[i]);
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

void print_profile(char _profile[FIELDS][STRING_SIZE])
{
	cout << _profile[LNAME] << " "
		<< _profile[FNAME] << " "
		<< _profile[PHONE] << " "
		<< _profile[BDATE] << endl;
}
void print_profile(ofstream &_file, char _profile[FIELDS][STRING_SIZE])
{
	_file << _profile[LNAME] << " "
		<< _profile[FNAME] << " "
		<< _profile[PHONE] << " "
		<< _profile[BDATE];
}

void profile_string(char _profile[FIELDS][STRING_SIZE], char _str[])
{
	strcpy(_str, _profile[LNAME]);
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
			}
			//ignore blank lines
			if(strlen(_profiles[records][0]) > 0) records++;
		} while (!file.eof());
	}
	//for priming read...
	records = (records == 1) ? 0 : records;

	sort_profiles(records, _profiles, true);
	
	return records;
}

void save_data(char _filename[], int _records, char _profiles[][FIELDS][STRING_SIZE])
{
	ofstream file = file_open_write(_filename);

	if (file.is_open())
	{
		for (int i = 0; i < _records; i++)
		{
			print_profile(file, _profiles[i]);
			if (i < _records - 1) file << endl;
		}
	}

	file_close(file);
}


//menu choices
int find_user_data(int _records, char _profiles[][FIELDS][STRING_SIZE])
{
	cout << endl << "Search For: ";
	char search_name[STRING_SIZE * 2] = { "" };
	get_input(STRING_SIZE * 2, search_name);
	
	int results[DATABASE_SIZE];
	int rsize = 0;

	char *token;
	int num_tokens = 0;
	char tsearch[STRING_SIZE * 2] = { "" };

	for (int i = 0; i < _records; i++)
	{
		char *fname = _profiles[i][FNAME];
		char *lname = _profiles[i][LNAME];
		
		//because strtok inserts \0 chars...
		strcpy(tsearch, search_name);

		bool lfound = false;
		bool ffound = false;
		
		num_tokens = 0;
		token = strtok(tsearch, " ");

		while (token != NULL)
		{
			num_tokens++;
			bool lft = false;
			if (!lfound)
			{
				lfound = (string_match(lname, token));
				lft = lfound;
			}
			if (!ffound && !lft) ffound = (string_match(fname, token));

			token = strtok(NULL, " ");
		}


		if (num_tokens > 1)
		{
			if (lfound && ffound) results[rsize++] = i;
		}
		else if (lfound || ffound)
		{
			results[rsize++] = i;
		}
	}

	if (rsize > 1)
	{
		cout << endl << "did you mean?" << endl;
		for (int i = 0; i < rsize; i++)
		{
			cout << i + 1 << ". " << _profiles[results[i]][LNAME] << " " << _profiles[results[i]][FNAME] << endl;
		}
		cout << endl << "input: ";

		int choice;
		cin >> choice;
		choice = max(min(choice, rsize), 1) - 1;

		cout << endl;
		print_profile(_profiles[results[choice]]);

		return results[choice];
	}
	else if (rsize == 1)
	{
		cout << endl;
		print_profile(_profiles[results[0]]);

		return results[0];
	}
	else
	{
		cout << endl << "No matches found." << endl;
		return -1;
	}
}


// profile manipulation
void sort_profiles(int _records, char _profiles[][FIELDS][STRING_SIZE], bool _ascending)
{
	bool sorted = false;
	for (int i = 0; i < _records - 1; i++)
	{
		sorted = true;
		for (int j = 0; j < _records - 1; j++)
		{
			if (name_compare(_profiles[j], _profiles[j + 1], _ascending))
			{
				sorted = false;
				swap(_profiles[j], _profiles[j + 1]);
			}
		}
		if (sorted) break;
	}
}
bool name_compare(char _a[][STRING_SIZE], char _b[][STRING_SIZE], bool _ascending)
{
	char n1[STRING_SIZE * 2] = {""};
	strcat(n1, _a[LNAME]);
	strcat(n1, _a[FNAME]);
	char n2[STRING_SIZE * 2] = {""};
	strcat(n2, _b[LNAME]);
	strcat(n2, _b[FNAME]);

	if (_ascending)
		return (_stricmp(n1,n2) > 0);
	else
		return (_stricmp(n1, n2) < 0);
}


//cstring
bool string_match(char _str1[], char _str2[])
{
	int len = min(strlen(_str1), strlen(_str2));
	if (len == 0)
		return false;

	bool match_found = true;
	for (int i = 0; i < len; i++)
	{
		if (tolower(_str1[i]) != tolower(_str2[i]))
		{
			match_found = false;
			break;
		}
	}

	return match_found;
}



// ** UTILITY FUNCTIONS ** //

//input
void get_input(int _stringsize, char _str[])
{
	cin.ignore(cin.rdbuf()->in_avail());
	cin.getline(_str,_stringsize);
	clean_input();
}
void wait_for_input()
{
	cout << "Press \"Enter\" to continue...";
	char str[1];
	cin.getline(str,1);
	clean_input();

	cout << endl;
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