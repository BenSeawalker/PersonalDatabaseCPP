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
void add_user(int &_records, char _profiles[][FIELDS][STRING_SIZE]);
void edit_user(int _records, char _profiles[][FIELDS][STRING_SIZE]);

void sort_profiles(int _records, char _profiles[][FIELDS][STRING_SIZE], bool _ascending);
bool name_compare(char _a[][STRING_SIZE], char _b[][STRING_SIZE], bool _ascending);

void get_input(int _stringsize, char _str[], bool _firsttoken);
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
				wait_for_input();
			break;
			case 2:
				add_user(records, profiles);
				wait_for_input();
			break;
			case 3:
				edit_user(records, profiles);
				wait_for_input();
			break;
			case 4:
				print_data(records, profiles);
				wait_for_input();
			break;
			case 5:
				cout << endl << "Goodbye!" << endl;
				done = true;
			break;
			default:
				cout << endl << "Invalid Choice!";
			break;
		}
	}
	save_data("profiles.dtb", records, profiles);

	wait_for_input();

	return 0;
}


// ** PROGRAM FUNCTIONS ** //

// ** print ** //
void print_data(int _records, char _profiles[][FIELDS][STRING_SIZE])
{
	
	cout << endl << "--------------------------------------------------------------" << endl;
	if (_records > 0)
	{
		for (int i = 0; i < _records; i++)
		{
			print_profile(_profiles[i]);
		}
	}
	else
		cout << "No records in database..." << endl;

	cout << endl << "--------------------------------------------------------------" << endl;
	cout << endl << "Total: " << _records << " records" << endl;
}

void print_menu()
{
	cout.flush();

	cout << endl << "--------------------------------------------------------------" << endl;
	cout << "\tPersonal Database" << endl << endl
		<< "1. Find a person's information" << endl
		<< "2. Add a person to the database" << endl
		<< "3. Edit a person's information" << endl
		<< "4. Display all profiles" << endl
		<< "5. Quit" << endl << endl
		<< "input: ";
}

//print profile to screen with endl
void print_profile(char _profile[FIELDS][STRING_SIZE])
{
	//+FIELDS for spaces in between...
	char pstring[STRING_SIZE*FIELDS + FIELDS] = { "" };
	profile_string(_profile, pstring);

	cout << pstring << endl;
}
//print profile to _file without endl
void print_profile(ofstream &_file, char _profile[FIELDS][STRING_SIZE])
{
	//+FIELDS for spaces in between...
	char pstring[STRING_SIZE*FIELDS + FIELDS] = { "" };
	profile_string(_profile, pstring);

	_file << pstring;
}

//stores the full profile string in _str
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

// ** file data ** //
int read_data(char _filename[], int _maxsize, char _profiles[][FIELDS][STRING_SIZE])
{
	int records = 0;

	ifstream file = file_open_read(_filename);
	if (file.is_open())
	{
		// do:while loop for simplified priming read
		do
		{
			for (int i = 0; i < FIELDS; i++)
			{
				file >> _profiles[records][i];
			}
			//ignore blank lines
			if(strlen(_profiles[records][LNAME]) > 0) records++;
		} while (!file.eof() && records < _maxsize);
	}

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


// ** menu choices ** //
int find_user_data(int _records, char _profiles[][FIELDS][STRING_SIZE])
{
	cout << endl << "Search For: ";
	char search_name[STRING_SIZE * 2] = { "" };
	char tsearch[STRING_SIZE * 2] = { "" };
	get_input(STRING_SIZE * 2, search_name, false);
	
	int results[DATABASE_SIZE];
	int rsize = 0;
	

	//	loop through all profiles and find names that "match" the input.
	for (int i = 0; i < _records; i++)
	{
		//pointers to the first and last names in a profile.
		char *lname = _profiles[i][LNAME];
		char *fname = _profiles[i][FNAME];
		bool lfound = false;
		bool ffound = false;

		//because strtok inserts null chars, we must use a separate cstring (tsearch)
		strcpy(tsearch, search_name);
		char *token = strtok(tsearch, " ");

		/*	loop through all input tokens for the current profile
		*	"token_found" ensures that the same token is not used for both first and last names.
		*/
		int num_tokens = 0;
		while (token != NULL)
		{
			num_tokens++;
			bool token_found = false;
			if (!lfound)
			{
				lfound = (string_match(lname, token));
				token_found = lfound;
			}
			if (!ffound && !token_found) ffound = (string_match(fname, token));

			token = strtok(NULL, " ");
		}

		/*	
		*	if a name begins with one of the input tokens, add its profile index to the results list.
		*	if there is more than one token, both fname and lname must match the input.
		*/
		if (num_tokens > 1)
		{
			if (lfound && ffound) results[rsize++] = i;
		}
		else if (lfound || ffound)
		{
			results[rsize++] = i;
		}
	}

	//	if there is more than one result, question the user for the profile they meant to choose.
	if (rsize > 1)
	{
		cout << endl << endl << "did you mean?" << endl;
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
	//otherwise print the first result
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

//returns true if _str2 matches exactly the FIRST characters in _str1.
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


void add_user(int &_records, char _profiles[][FIELDS][STRING_SIZE])
{
	if (_records >= DATABASE_SIZE)
	{
		cout << "Database Full!" << endl
			<< "Do you wish to edit a profile instead?" << endl << endl
			<< "(Y/N) : ";

		char input[STRING_SIZE] = { "" };
		get_input(STRING_SIZE, input, true);
		if (toupper(input[0]) == 'Y') edit_user(_records, _profiles);
	}
	else
	{
		clean_input();
		cout << "Enter last name: ";
		get_input(STRING_SIZE, _profiles[_records][LNAME], true);
		clean_input();

		cout << "Enter first name: ";
		get_input(STRING_SIZE, _profiles[_records][FNAME], true);
		clean_input();

		cout << "Enter phone number: ";
		get_input(STRING_SIZE, _profiles[_records][PHONE], true);
		clean_input();

		cout << "Enter Birth Date (mm/dd/yyyy): ";
		get_input(STRING_SIZE, _profiles[_records][BDATE], true);
		clean_input();

		cout << endl << "New Profile: ";
		print_profile(_profiles[_records]);

		sort_profiles(++_records, _profiles, true);
	}
}


void edit_user(int _records, char _profiles[][FIELDS][STRING_SIZE])
{
	int index = find_user_data(_records, _profiles);

	if (index > -1)
	{
		char *lname = _profiles[index][LNAME];
		char *fname = _profiles[index][FNAME];
		char *phone = _profiles[index][PHONE];
		char *bdate = _profiles[index][BDATE];

		char input[STRING_SIZE];
		cout << endl << "Edit Data or press enter to leave as is:" << endl << endl
			<< "Last Name (" << lname << ") : ";
		get_input(STRING_SIZE, input, true);
		if (strlen(input) > 0)
			lname = input;

		cout << endl << "First Name (" << fname << ") : ";
		get_input(STRING_SIZE, input, true);
		if (strlen(input) > 0)
			fname = input;

		cout << endl << "Phone Number (" << phone << ") : ";
		get_input(STRING_SIZE, input, true);
		if (strlen(input) > 0)
			phone = input;

		cout << endl << "Birth Date (" << bdate << ") : ";
		get_input(STRING_SIZE, input, true);
		if (strlen(input) > 0)
			bdate = input;

		cout << endl;

		strcpy(_profiles[index][LNAME], lname);
		strcpy(_profiles[index][FNAME], fname);
		strcpy(_profiles[index][PHONE], phone);
		strcpy(_profiles[index][BDATE], bdate);

		cout << "Updated Info: ";
		print_profile(_profiles[index]);
		cout << endl;

		sort_profiles(_records, _profiles, true);
	}
}

// ** profile array manipulation ** //

//flagged bubble sort based on names (last,first) in _ascending order
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

//returns true if the last and first names are in _ascending order
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




// ** UTILITY FUNCTIONS ** //

// ** input ** //
void get_input(int _stringsize, char _str[], bool _firsttoken)
{
	cin.ignore(cin.rdbuf()->in_avail());
	cin.getline(_str,_stringsize);
	if (_firsttoken) _str = strtok(_str, " ");
	clean_input();
}
void wait_for_input()
{
	clean_input();

	cout << endl << "Press \"Enter\" to continue...";
	char str[1];
	cin.getline(str,1);
	cout << endl;

	clean_input();
}

void clean_input()
{
	cin.clear();
	cin.ignore(cin.rdbuf()->in_avail());
}

// ** file manipulation ** //
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