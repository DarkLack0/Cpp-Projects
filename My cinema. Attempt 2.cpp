/*
Name: My cinema,
Description: Code for Assignment "IY453",
Author: Mikhail Teslenko,
Version: 2.3;
Date: 6-7 March, 14-15, 21-24 April 2022.
*/

#include <iostream> // to input/output
#include <string> // to work with strings
#include <fstream> // to work with files
#include <vector> // to include vectors
#include <map> // to use dictionaries
#include <tuple> // to create an array with different types of data
#include <chrono> // to use time
#include <thread> // to use sleep command

// just to rename pair functions
#define timing first // timing like time
#define tick second // tick link tickets

// use default namespace
using namespace std;

// TIME to work with time
class TIME {
	time_t timeutc0;
	time_t timelocal;
	int localutc;
	char stringtime[26];
public:

	// use construtor to activate basic funtions by default
	TIME() {
		set_utc0_time();
		time_to_string();
		set_local_utc();
		set_local_time();
	}

	// set time
	void set_utc0_time() {

		// taken from https://stackoverflow.com/questions/62226043/how-to-get-the-current-time-in-c-in-a-safe-way
		using namespace std::chrono;

		auto time_now = system_clock::now();
		time_t time_as_number = system_clock::to_time_t(time_now);

		timeutc0 = time_as_number;
	}

	// turn time into a string
	void time_to_string() {
		ctime_s(stringtime, 26, &timeutc0);

		if (stringtime[8] == ' ') stringtime[8] = '0';
	}

	// use maths to set the localutc (using advantage that "stringtime" save correct time using timeutc0 as a value) 
	void set_local_utc() {
		int hoursutc0 = (timeutc0 % 86400) / 3600;
		int hourslocal = (stringtime[11] - '0') * 10 + stringtime[12] - '0';

		localutc = hourslocal - hoursutc0 + 24;
		while (localutc > 12) localutc -= 24;
	}

	void set_local_time() {
		timelocal = timeutc0 + localutc * 3600;
	}

	time_t get_time() {
		return timelocal;
	}

	int get_week_number() {
		int week_number;

		// get week number, where week from Thu Mar 24 to Wed Mar 30 is the start week (week #0)
		week_number = ((timelocal - (timelocal % 604800) - 1648080000) / 604800) % 1000;
		return week_number;
	}

	// function that return any time as a number to time as a string
	string any_string_time(time_t time) {
		time -= localutc * 3600;

		char buff[26];
		ctime_s(buff, 26, &time);
		if (buff[8] == ' ') buff[8] = '0';

		return buff;
	}

	// use full forms of weekdays
	string full_weekday(string abbreviation) {
		string full_day;

		// not 'switch-case' because paramentr must be a string
		if (abbreviation == "Mon") full_day = "Monday";
		else if (abbreviation == "Tue") full_day = "Tuesday";
		else if (abbreviation == "Wed") full_day = "Wednesday";
		else if (abbreviation == "Thu") full_day = "Thursday";
		else if (abbreviation == "Fri") full_day = "Friday";
		else if (abbreviation == "Sat") full_day = "Saturday";
		else full_day = "Sunday";

		return full_day;
	}

	// use full forms of months
	string full_month(string abbreviation) {
		string full_name;

		// not 'switch-case' because paramentr must be a string
		if (abbreviation == "Jan") full_name = "January";
		else if (abbreviation == "Feb") full_name = "February";
		else if (abbreviation == "Mar") full_name = "March";
		else if (abbreviation == "Apr") full_name = "April";
		else if (abbreviation == "May") full_name = "May";
		else if (abbreviation == "Jun") full_name = "June";
		else if (abbreviation == "Jul") full_name = "July";
		else if (abbreviation == "Aug") full_name = "August";
		else if (abbreviation == "Sep") full_name = "September";
		else if (abbreviation == "Oct") full_name = "October";
		else if (abbreviation == "Nov") full_name = "November";
		else full_name = "December";

		return full_name;
	}

	// get week as a string using format "from Thursday Month Date to Wednesday Month Date"
	string get_week_string(int number) {
		time_t time;
		string week;

		time = timelocal - (timelocal % 604800) + number * 604800;

		week = "from " + full_weekday(any_string_time(time).substr(0, 3)) +
			' ' + full_month(any_string_time(time).substr(4, 3)) + ' ' + any_string_time(time).substr(8, 2);

		time += 518400;

		week += " to " + full_weekday(any_string_time(time).substr(0, 3)) +
			' ' + full_month(any_string_time(time).substr(4, 3)) + ' ' + any_string_time(time).substr(8, 2);

		return week;
	}

};

// FILMS to save film details
class FILMS {
	// how to work with dictionaries was taken from https://java2blog.com/dictionary-cpp/
	map<string, string> film_details[25]; // use array of dictionaries to save movie details (each dictionary - data about one film)
	string keys[17];
public:

	// constructor to set the default keys
	FILMS() {

		string defaultvalues[17] = { "ID", "Poster Link", "Series Title", "Released Year", "Certificate", "Runtime", "Genre",
			"IMDB Rating", "Overview", "Meta Score", "Director", "Star1", "Star2", "Star3", "Star4", "No of Votes", "Gross" };

		copy(begin(defaultvalues), end(defaultvalues), begin(keys));
	}

	// create files to store 25 out of 1000 movie details there
	void save_films(const int startid) {

		string line;
		int id = 1;
		bool check = true;

		ifstream reader("FilmsALL.csv"); // open file with 1000 movies
		ofstream writer("FilmsAVAILABLE.txt"); // create new file (or clear if it is exist) 

		// skip first line
		getline(reader, line);

		// fill new file using data from big file
		while (getline(reader, line) && id <= startid + 24) {
			if (id >= startid) {
				writer << "\n" << "---------------\n" << id << "\n";

				for (char ch : line) {

					if (ch == '\"')
						check = !check;
					else if (ch == ',' && check)
						writer << '\n';
					else
						writer << ch;
				}
			}
			id++;
		}

		// close files after work with them
		reader.close();
		writer.close();
	}

	// use file with 25 films to set dictionaries
	void set_values() {

		// create string variables, line - for line to work with, line_skip for useless lines.
		string line, line_skip;

		// open file
		ifstream reader("FilmsAVAILABLE.txt");
		getline(reader, line_skip); // there is no check if the file is exist or not, because it will be created if it wasn't exist

		for (int i = 0; i < 25; i++) {
			getline(reader, line_skip);

			for (int j = 0; j < 17; j++) {
				getline(reader, line);

				if (line == "") line = "'DATA WAS NOT PROVIDED'"; // if nothing inside save that there is no provided data.

				film_details[i].insert(pair<string, string>(keys[j], line));
			}
		}
		reader.close(); // close the file
	}

	// display all film details by its number
	void see_film_details(int film_id) {

		cout << "\n---------------\n";
		for (int key = 1; key < 17; key++) {
			cout << keys[key] << ": " << film_details[film_id][keys[key]] << '\n';

			if (key == 1 || key == 8 || key % 7 == 0) cout << '\n'; // use some empty lines for better view
			else if (key == 16) cout << "---------------\n\n";
		}
	}

	// get value by number and key
	string get_value_by_n_k(int number, string key) {
		return film_details[number][key];
	}

	// sometimes share all details at once usinag just a pointer to it
	map<string, string>* getmap() {
		return film_details;
	}
};

// SCREENS to save screen details 
class SCREENS {
	tuple<int, string, int, string> screens[5]; // array of tuples to save each screen <id, film shown this week, max ticket amount, facility>
public:

	// all tuples content taken from https://www.geeksforgeeks.org/tuples-in-c/
	void set_values(map<string, string>* films, int* tickets, string* facilities) {
		for (int i = 0; i < 5; i++)
			screens[i] = make_tuple(i + 1, (*(films + i))["Series Title"], *(tickets + i), *(facilities + i));
	}

	// display the screen details
	void see_screen_details() {
		for (int i = 0; i < 5; i++) {
			cout << "\nScreen #" << get<0>(screens[i])
				<< "\nFilm shown this week: " << get<1>(screens[i])
				<< "\nMaximum number of seats - " << get<2>(screens[i])
				<< "\nScreen Facility: " << get<3>(screens[i]) << '\n';
		}
	}
};

// DATA to store 
class DATA {
protected:				// how to work with vectors was taken from: https://en.cppreference.com/w/cpp/container/vector
	int ids[25]; // use array to save film ids (it is a primary key for each movie or just its line number in 1000 films file)
	vector<pair<time_t, int>> film_show_times[25]; // use array of vectors to save movie start time and tickets available for it
public:

	// return ids if needed
	int* get_ids() {
		return ids;
	}

	// return a pointer to film start time and tickets information 
	vector<pair<time_t, int>>* get_film_times() {
		return film_show_times;
	}

	// function that takes an id from a string
	int take_id(string str) {
		return stoi(str.substr(0, str.find('.')));
	}

	// function that takes a runtime from a string
	int take_runtime(string str) {
		return stoi(str.substr(0, str.find(' ')));
	}

	// function that takes time (as a number) from a string
	time_t take_time(string str) {
		return stoll(str.substr(0, str.find(' ')));
	}

	// function that takes ticket number from a string
	int take_tickets(string str) {
		return stoi(str.substr(str.find('!') + 1));
	}
};

// OLD DATA is class that only store data from a suitable file
class OLD_DATA : public DATA {
public:

	// use constructor to auto start other function
	OLD_DATA() {
		set_ids_and_times();
	}

	// take all data from the file
	void set_ids_and_times() {

		// use different string variables to separate the content
		string line, skip_line, id_line;
		int i = 0;

		// try to open the file to read
		ifstream reader("FilmsBOOKING.txt");

		if (reader) { // check if the file exist
			getline(reader, id_line);
			ids[i] = take_id(id_line);

			while (getline(reader, line)) {

				if (line == "") {
					i++;
					getline(reader, skip_line);
					getline(reader, skip_line);
					getline(reader, id_line);
					if (i < 25)	
						ids[i] = take_id(id_line);
				}
				else film_show_times[i].push_back(pair<time_t, int>(take_time(line), take_tickets(line)));
			}
			reader.close(); // close the file
		}

		else // if file is not exist
			ids[0] = -25;
	}
};

// NEW DATA needs to work with movie data (such as change the tickets amount and after save new data to the file)
class NEW_DATA : public DATA {
	int films_runtime[25]; // runtimes are needed to set the real movie start times
	vector<int> valid_films; // films where at least 1 ticket is available

public:

	// use constructor to run some functions by default
	NEW_DATA(map<string, string>* pointer_to_films) {
		set_ids(pointer_to_films);
		set_runtimes(pointer_to_films);
	}

	// set the movie ids using movie ids from films details
	void set_ids(map<string, string>* pointer_to_films) {
		for (int i = 0; i < 25; i++)
			ids[i] = stoi((*(pointer_to_films + i))["ID"]);
	}

	// set the movie runtimes using movie ids from films details
	void set_runtimes(map<string, string>* pointer_to_films) {
		for (int i = 0; i < 25; i++)
			films_runtime[i] = take_runtime((*(pointer_to_films + i))["Runtime"]);
	}

	// this function will set movie start times and ticket amount using real time and maximum ticket amount
	void set_times(time_t time, int* tickets) {
		time_t week_now;
		int start_weekday, minutes_today;
		vector<int> times;

		for (int i = 0; i < 25; i++) {

			week_now = time - (time % 604800) + 604800 * (i / 5);
			minutes_today = (time % 86400) / 60;
			start_weekday = (i < 5) ? (time % 604800) / 86400 : 0;

			for (int j = 0; j < 820 / films_runtime[i]; j++) // create an array that consist movie start time as minutes left from the day start
				times.push_back(615 + j * films_runtime[i]);

			for (int weekday = start_weekday; weekday < 7; weekday++)
				for (int minute : times)
					if (i > 4 || weekday != start_weekday || minute > minutes_today)
						film_show_times[i].push_back(pair<time_t, int>(week_now + weekday * 86400 + minute * 60, *(tickets + (i % 5))));

			times.clear();
		}
	}

	// use compare values function to set the tickets amount for film if this film will be available to book by user   
	void compare_values(vector<pair<time_t, int>>* old_data, int* old_start_id) {
		int old = *old_start_id;
		int modern = ids[0];
		int maxfirst = max(old, modern);

		old = maxfirst - old;
		modern = maxfirst - modern;

		for (int i = 0; i < 25 - (old + modern); i++)
			for (int j = 0; j < film_show_times[i + modern].size(); j++)
				for (auto target : *(old_data + i + old))
					if (film_show_times[i + modern][j].timing == target.timing)
						film_show_times[i + modern][j].tick = target.tick;
	}

	// set the valid films vector
	void set_valid_films() {
		for (int i = 0; i < 25; i++)
			for (auto element : film_show_times[i])
				if (element.tick) {
					valid_films.push_back(i);
					break;
				}
	}

	// save new data to the file 
	// useng blocks with films in between empty lines and inside this block information has the following format:
	// "time as a number | time as a string !tickets available (for provided START movie time)"
	void save_new_data(FILMS* films, TIME* time) {
		ofstream writer("FilmsBOOKING.txt");
		string timestr;

		for (int i = 0; i < 25; i++) {
			writer << films->get_value_by_n_k(i, "ID") << ". " << films->get_value_by_n_k(i, "Series Title") << '\n';

			for (auto element : film_show_times[i]) {
				timestr = (time->any_string_time(element.timing)).substr(0, 24);
				writer << element.timing << " | " << timestr << " !" << element.tick << '\n';
			}
			writer << '\n' << "------End of the Film--------" << "\n\n";
		}
		writer.close();
	}

	// return true if film is valid or false if not
	bool if_film_is_valid(int film_number) {
		for (int value : valid_films)
			if (value == film_number)
				return true;
		return false;
	}

	// return a pointer to tickets amount of the film by film and its time index
	int* choose_tickets(int film, int time_index) {
		return &film_show_times[film][time_index].tick;
	}

	// return a time as a number of the film by film and its time index
	time_t choose_time(int film, int time_index) {
		return film_show_times[film][time_index].timing;
	}
};

// USER_CHOICE class is used to check the user input and save its choice 
class USER_CHOICE {
	string choice_str;
public:
	// take choice
	void take_choice() {
		getline(cin, choice_str);
	}

	// check if string is a number
	bool is_number(const string str) {
		for (int i = 0; i < str.size(); i++)
			if (str[i] - '0' < 0 || str[i] - '0' > 9)
				return 0;

		return (str.size() != 0);
	}

	// test if user's input a number (less than 10000)
	bool test_choice() {
		return is_number(choice_str) && choice_str.size() < 5;
	}

	// take an input from user, valid input is only any number (integer) between min and max values (including them)
	int make_choice_between(int min_value, int max_value) {
		int value = -1;

		take_choice();
		if (test_choice())
			value = stoi(choice_str);

		while (!test_choice() || value < min_value || value > max_value) {
			cout << "\nInvalid input, please enter a number between " << min_value << " and " << max_value << ".\n";
			take_choice();
			if (test_choice())
				value = stoi(choice_str);
		}

		return value; // return the value
	}

	// only card input, valid input is only a 16-digit value with the first digit is not 0
	void card_input() {
		take_choice();

		while (choice_str.size() != 16 || !is_number(choice_str) || choice_str[0] == '0') {
			cout << "\nInvalid input, please enter a valid card number using \"1234567812345678\" format.\n";
			take_choice();
		}
	}

	// expiration data input, valid input is only a two 2-digit numbers with '/' between them (first value cannot be more than 12 or less than 1)
	void expiration_date_input() {
		take_choice();

		while (choice_str.size() != 5 || !is_number(choice_str.substr(0, 2)) || stoi(choice_str.substr(0, 2)) > 12
			|| stoi(choice_str.substr(0, 2)) < 1 || choice_str[2] != '/' || !is_number(choice_str.substr(3))) {

			cout << "\nInvalid input, please enter a expiration date using mm/yy format (for example, '11/22').\n";
			take_choice();
		}
	}

	// turn value into index by subtracting 1
	int get_index_choice(int value) {
		return value - 1;
	}

	// input anything if there at least 3 characters 
	void get_any_name_choice() {
		take_choice();
		bool invalid = false;

		for (char ch : choice_str)
			if (int(ch) < 65 || (int(ch) > 90 && int(ch) < 97) || int(ch) > 122)
				invalid = true;

		while (choice_str.size() < 3 || invalid) {
			invalid = false;
			cout << "\nInvalid input, please enter at least 3 letters and use only alphabet.\n";
			take_choice();

			for (char ch : choice_str)
				if (int(ch) < 65 || (int(ch) > 90 && int(ch) < 97) || int(ch) > 122)
					invalid = true;
		}
	}

	// return a boolean if user enter 'yes' ( if any of the letters is capital but the word still 'yes' function will return true)
	bool get_yes_or_notyes() {
		take_choice();
		for (int i = 0; i < choice_str.size(); i++) {
			choice_str[i] = tolower(choice_str[i]);
		}

		return (choice_str == "yes");
	}

	// use a function here to smoothly output the text
	void one_by_one_cout(string str, int output_speed_milliseconds = 30) {
		using namespace std::this_thread; using namespace std::chrono; // to make this part of the program clearer

		for (char ch : str) {
			cout << ch;

			// sleep command taken from https://www.cplusplus.com/reference/thread/this_thread/sleep_for/
			if (ch == '.') sleep_for(milliseconds(200));
			else sleep_for(milliseconds(output_speed_milliseconds));
		}
	}
};

// PAYMENT to get user's tickets order and take payment 
class PAYMENT {
	int tickets_amount[4];
	int tickets_before_ordering;
public:

	// use constructor to set the default values
	PAYMENT(int ticket_number) {
		int full_zeros[4] = { 0, 0, 0, 0 };
		copy(begin(full_zeros), end(full_zeros), begin(tickets_amount));
		tickets_before_ordering = ticket_number;
	}

	// get order functon
	string get_ticket_order(USER_CHOICE* user_choice) {

		int amount_of_tickets;
		int choice;
		int remaining_tickets;
		bool go_again;
		string groups[4] = { "adult", "children", "senior citizen", "student" };

		// a loop in case of mistakes
		do {
			go_again = false; int full_zeros[4] = { 0, 0, 0, 0 };
			copy(begin(full_zeros), end(full_zeros), begin(tickets_amount));

			cout << "\nHow many tickets do you want to buy?\n";
			amount_of_tickets = user_choice->make_choice_between(1, tickets_before_ordering);
			remaining_tickets = amount_of_tickets;

			// if user trying to buy more than 1 ticket
			if (amount_of_tickets > 1) {										// '\x9C' is a pound sign
				cout << "\nThe cinema has tickets of 4 formats: \nfor adults - \t\t\x9C" << "9.50, \nfor children - \t\t\x9C" << "5.50, " 
					<< "\nfor senior citizens - \t\x9C" << "6.50, \nfor students - \t\t\x9C" << "7.00.\n"
					<< "\nDo you want to buy tickets of the same format (type '1') or tickets of different formats (type '2')?\n";
				// choice 1 - all tickets have the same format
				if (user_choice->make_choice_between(1, 2) == 1) {
					cout << "\nChoose your format: \nWrite 1 to choose 'adult', \n      2 for 'children', "
						<< "\n      3 for 'senior citizens', \n   or 4 to choose 'students'.\n";

					choice = user_choice->make_choice_between(1, 4);
					tickets_amount[choice - 1] = amount_of_tickets;
				}

				// choice 2 - buy tickets of the different formats
				else {
					// use for loop to check remaining tickets
					for (int i = 0; i < 4; i++)
						if (remaining_tickets != 0) {
							cout << "\nHow many " << groups[i] << " tickets do you want? "
								<< "Please enter '0' if you don't need any tickets of this format.\n";

							tickets_amount[i] = user_choice->make_choice_between(0, remaining_tickets);
							remaining_tickets -= tickets_amount[i];
						}
						else break;
					// if any tickets remaining go again
					if (remaining_tickets != 0) {
						cout << "\nWe apologize but the total amount of tickets (" << amount_of_tickets - remaining_tickets
							<< ") does not match the indicated amount (" << amount_of_tickets << ").\nPlease try again.\n";
						go_again = true;
					}
				}
			}

			// user try to buy only 1 ticket:
			else {															// '\x9C' is a pound sign
				cout << "\nThe cinema has tickets of 4 formats: for adults (\x9C" << "9.50), children (\x9C" << "5.50), "
					<< "senior citizens (\x9C" << "6.50) and students (\x9C" << "7.00).\nPlease, choose your format: "
					<< "\nWrite 1 to choose 'adult', \n      2 for 'children', "
					<< "\n      3 for 'senior citizens', \n   or 4 to choose 'students'.\n";

				choice = user_choice->make_choice_between(1, 4);
				tickets_amount[choice - 1] = amount_of_tickets;
			}
		} while (go_again);

		// create an order depends on how many tickets of every format user selected
		string order;
		order = "\nYour order is:";

		for (int i = 0; i < 4; i++)
			if (tickets_amount[i] != 0) {
				order += ' ' + (to_string(tickets_amount[i])) + ' ' + groups[i] + " ticket";

				if (tickets_amount[i] > 1) order += 's';

				order += ',';
			}

		order[order.size() - 1] = '.'; // put a dot at the end
		return order; // return the order
	}

	// make payment function
	void make_payment(USER_CHOICE* user_choice) {
		cout << "\nWhat payment method do you prefer? Please enter '1' for card or '2' for cash.\n";

		// choice 1 - card
		if (user_choice->make_choice_between(1, 2) == 1) {
			cout << "\nThank you for choosing card method.\n";

			cout << "Please write your FIRST name: "; // first name input
			user_choice->get_any_name_choice();

			cout << "\nPlease write your LAST name: "; // last name input
			user_choice->get_any_name_choice();

			cout << "\nPlease type your CARD number (without spaces): "; // card number input
			user_choice->card_input();

			cout << "\nPlease write the EXPIRATION date (using mm/yy format): "; // expiration date input
			user_choice->expiration_date_input();

			cout << "\nPlease type your CVV: "; // CVV input
			user_choice->make_choice_between(100, 9999);

			// nothing stored because it is one of the requirements //

			cout << "\nThank you so much for making payment online!\n";
		}

		// choice 2 - cash
		else
			cout << "\nThank you for choosing cash.\nPlease go to the checkout and pay for the purchase."
				<< "\nOr, do so at any time before the movie starts, otherwise your ticket will be invalid.\n";

		this_thread::sleep_for(chrono::seconds(2)); // use sleep_for to make program more realistic
	}

	// get total cost
	double get_total_cost() {
		return tickets_amount[0] * 9.5 + tickets_amount[1] * 5.5 + tickets_amount[2] * 6.5 + tickets_amount[3] * 7;
	}

	// get amount of tickets
	int get_amount_of_tickets() {
		int amount_of_tickets = 0;
		for (int tickets : tickets_amount) amount_of_tickets += tickets;

		return amount_of_tickets;
	}
};

// main
int main() {

	// check if there is master file in the system
	ifstream mainfile("FilmsALL.csv");

	if (mainfile) // if yes just close it and continue program work
		mainfile.close();

	else { // if not - provide suitable ending
		cout << "\nThe system has lost the Master file. \nPlease, return it and restart the program to restore the system.\n";
		return 0;
	}

	// set default max tickets amount and facilities
	int tickets_max[5] = { 250, 200, 150, 100, 100 };
	string facilities[5] = { "IMAX", "Couple seats", "Standard", "Outdoor", "IMAX" };

	// create objects
	TIME time;
	FILMS films;
	films.save_films(time.get_week_number() * 7 + 1);
	films.set_values();

	SCREENS screens;
	screens.set_values(films.getmap(), tickets_max, facilities);

	OLD_DATA old_data;
	NEW_DATA new_data(films.getmap());
	new_data.set_times(time.get_time(), tickets_max);
	new_data.compare_values(old_data.get_film_times(), old_data.get_ids());

	USER_CHOICE user_choice;

	cout << "Welcome to the cinema!\n";

	bool agree_with_film_choice = false;

	// use a loop in case of mistakes
	while (!agree_with_film_choice) {
		int choice;

		cout << "\nWhat would you like?\n";
		do { // use 'do-while' loop until the user choose to buy tickets
			cout << "Please select between:\n1. To see the screen details\n2. To see the movie details"
				<< "\n3. To buy tickets.\n4. To end the session.\n";

			choice = user_choice.make_choice_between(1, 4);
			if (choice == 1) { // selection 1. - see the screen details
				screens.see_screen_details();
				cout << '\n';
			}

			else if (choice == 2) { // selection 2. - see the movie details
				cout << "\n0. ALL\n";
				for (int i = 0; i < 25; i++)
					cout << i + 1 << ". " << films.get_value_by_n_k(i, "Series Title") << '\n';

				cout << "\nPlease, select which movie details you would like to see using its number\n"
					<< "(enter '0' if you want to see all of them).\n";
				choice = user_choice.make_choice_between(0, 25);

				if (choice == 0) // if user has selected to see all movie details
					for (int i = 0; i < 25; i++)
						films.see_film_details(i);
				else // user has selected 1 movie to see its details
					films.see_film_details(choice - 1);

				if (choice == 3) // in case user choose third movie change choice because otherwise we will go out of this 'do-while' loop
					choice = -1;
			}

			else if (choice == 4) { // user choose to end the session
				cout << "\nIt was a pleasure doing business with you.\nAll the best.\n";
				return 0; // program finish without errors
			}

		} while (choice != 3);

		// user choose to buy tickets
		int page = 0;
		int chosen_film = -1;

		new_data.set_valid_films();

		do { // show films until user select a valid movie
			if (chosen_film == page * 5) page--; // previous page choice (each page has movies that will be shown during a week)
			else if (chosen_film == page * 5 + 6) page++; // next page choice
			page = (page + 5) % 5;

			cout << "\nPage " << page + 1 << ".\nFilms available " << time.get_week_string(page)
				<< ":\n\n" << page * 5 << ". " << "<- See the previous Page <-\n";

			for (int i = page * 5; i < page * 5 + 5; i++) { // show films on the current page

				cout << i + 1 << ". " << films.get_value_by_n_k(i, "Series Title") << ", " << films.get_value_by_n_k(i, "Runtime");

				if (new_data.if_film_is_valid(i))
					cout << " (Screen #" << i % 5 + 1 << ")\n";
				else
					cout << " (ALL TICKETS SOLD OUT)\n";
			}

			cout << page * 5 + 6 << ". " << "-> Look at next Page ->" << '\n'
				<< "\nPlease select a film or action by writting its number.\n";

			chosen_film = user_choice.make_choice_between(page * 5, page * 5 + 6); // choose a film

			if (chosen_film % 26 != 0 && !new_data.if_film_is_valid(chosen_film - 1)) // if film is not valid
				cout << "\nThere are no tickets for chosing film. Try something else\n";

		} while ((chosen_film) % 6 == (6 - page) % 6 || !new_data.if_film_is_valid(chosen_film - 1));

		// user make a choice //

		// turn user's choice into an index
		chosen_film = user_choice.get_index_choice(chosen_film);

		cout << "\nYour selection is: " << films.get_value_by_n_k(chosen_film, "Series Title") 
			<< "\nThe film will be shown in the following days:\n";

		vector<string> available_days;
		string weekday;

		// select available days for the movie
		for (int i = 0; i < (*(new_data.get_film_times() + chosen_film)).size(); i++) {
			weekday = time.any_string_time((*(new_data.get_film_times() + chosen_film))[i].timing).substr(0, 10);
			weekday = time.full_weekday(weekday.substr(0, 3)) + ' ' + time.full_month(weekday.substr(4, 3)) + ' ' + weekday.substr(8);

			if ((*(new_data.get_film_times() + chosen_film))[i].tick != 0 &&
				(available_days.empty() || available_days.back().substr(0, 3) != weekday.substr(0, 3))) available_days.push_back(weekday);
		}

		// show available days for the film
		for (int i = 0; i < available_days.size(); i++)
			cout << i + 1 << ". " << available_days[i] << '\n';

		// user selects a day 
		cout << "\nPlease select a day by entering its number.\n";
		int chosen_day = user_choice.get_index_choice(user_choice.make_choice_between(1, available_days.size()));

		cout << "\nYour choice is: " << available_days[chosen_day] << "\nMovie START times for the chosen day:\n";

		vector<string> available_times;
		string day_time;
		choice = -1;

		// select the times available for the chosen day
		for (int i = 0; i < (*(new_data.get_film_times() + chosen_film)).size(); i++) {
			time_t time_as_number = (*(new_data.get_film_times() + chosen_film))[i].timing;

			if (time.any_string_time(time_as_number).substr(0, 3) == available_days[chosen_day].substr(0, 3)) {
				if (choice == -1) choice = i;
				day_time = time.any_string_time(time_as_number).substr(11, 8);
				available_times.push_back(day_time);
			}
		}

		bool are_there_any_tickets;
		int chosen_time;

		// use 'do-while' loop in case user make a choice when there are no tickets
		do {

			// show the times available for the chosen day
			for (int i = 0; i < available_times.size(); i++) {
				string tickets_amount = to_string(*(new_data.choose_tickets(chosen_film, choice + i)));
				if (tickets_amount == "0") tickets_amount = "Sold Out";

				cout << i + 1 << ". " << available_times[i] << " (tickets available - " << tickets_amount << ")\n";
			}

			// user selects a time 
			cout << "\nPlease select time using its index in the list.\n";
			chosen_time = user_choice.get_index_choice(user_choice.make_choice_between(1, available_times.size()));

			if (*(new_data.choose_tickets(chosen_film, choice + chosen_time)) == 0) { // user choose invalid time
				cout << "\nThere are no more tickets available for that time,\nplease choose another time.\n\n";
				are_there_any_tickets = false;
			}

			else { // user choose valid time
				choice += chosen_time;
				are_there_any_tickets = true;
			}

		} while (!are_there_any_tickets);

		bool agree_with_ticket_choice = false;

		// use 'while' loop in case of mistakes
		while (!agree_with_ticket_choice) {

			// user needs to confirm the provided information
			cout << "\nPlease confirm the information provided.\n" << "Film: " << films.get_value_by_n_k(chosen_film, "Series Title")
				<< "\nDay: " << available_days[chosen_day] << "\nTime: " << available_times[chosen_time] 
				<< "\nRuntime: " << films.get_value_by_n_k(chosen_film, "Runtime") 
				<< "\n\nIs everything correct? (please enter 'yes' to continue)\n";

			if (user_choice.get_yes_or_notyes()) { // if user selects to continue

				agree_with_film_choice = true;

				// create a payment object
				PAYMENT payment(*new_data.choose_tickets(chosen_film, choice));

				// get user order and total cost
				string order = payment.get_ticket_order(&user_choice);
				double total_cost = payment.get_total_cost();

				// user needs to confirm the provided information									// '\x9C' is a pound sign
				cout << "\nPlease confirm the information provided.\n" << order << "\nTotal cost is: \x9C" << total_cost
					<< ".\nIs everything correct? (please enter 'yes' to continue)\n";

				if (user_choice.get_yes_or_notyes()) { // if user selects to continue 

					agree_with_ticket_choice = true;
					payment.make_payment(&user_choice); // make payment

					int amount_of_tickets = payment.get_amount_of_tickets();

					// update the file
					*new_data.choose_tickets(chosen_film, choice) -= amount_of_tickets;
					new_data.save_new_data(&films, &time);

					string receipt; // create a receipt that consist of tickets

					string ticket_first_part = films.get_value_by_n_k(chosen_film, "ID");
					string ticket_middle_part = to_string(new_data.choose_time(chosen_film, choice) % 900000 + 100000);
					string ticket_last_part;

					receipt = "\nPlease, get your ticket";
					if (amount_of_tickets > 1) receipt += 's';
					receipt += ':';

					for (int i = 1; i <= amount_of_tickets; i++) {

						if (i % 5 == 1) receipt += '\n';

						ticket_last_part = to_string(*(new_data.choose_tickets(chosen_film, choice)) + i);
						if (ticket_last_part.size() == 2) ticket_last_part = '0' + ticket_last_part;
						else if (ticket_last_part.size() == 1) ticket_last_part = "00" + ticket_last_part;

						receipt += " #" + ticket_first_part + ticket_middle_part + ticket_last_part;
						if (i == amount_of_tickets) receipt += ".\n";
						else receipt += ',';
					}

					int output_speed = 50 / amount_of_tickets * 2;
					user_choice.one_by_one_cout(receipt, output_speed); // print the tickets for the user
				}
				else {  // user did not confirm ticket choice
					string try_again_text = "...\nPlease, Try Again :)\n...";
					cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

					user_choice.one_by_one_cout(try_again_text);

					cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
				}
			}

			else { // user did not confirm movie choice
				agree_with_film_choice = false;
				agree_with_ticket_choice = true;

				string try_again_text = "...\nPlease, Try Once More :)\n...";
				cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

				user_choice.one_by_one_cout(try_again_text);

				cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
			}
		}
	}
	return 0; // program finish without errors
}