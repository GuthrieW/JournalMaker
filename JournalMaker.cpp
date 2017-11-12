#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <vector>
#include <sstream>
#include <algorithm>
#include <fstream>

#define TRUE_INT 1
#define FALSE_INT 0
#define COMPARE_TRUE 0
#define COMPARE_FALSE 1
#define QUIT 100
#define HELP 101
#define NEW_ENTRY 102
#define EDIT_ENTRY 103
#define SHOW_ENTRIES 104
#define INPUT_ERROR 105

using namespace std;

tm* getCurrentTime();
string getTimeString(tm* time);
void greeting();
int getInput();
void help();
void newEntry();
void editEntry();
vector<string> getEntries();
void quit(int exit_status, string exit_message);
void printEntries(vector<string> entries);
void askForCommand();
void addSeparation();
void waitForEnter();

int main() {
  greeting();
  while(1) {
    int input = getInput();
    addSeparation();
    if (input == HELP) {
      help();
    } else if (input == NEW_ENTRY) {
      newEntry();
    } else if (input == EDIT_ENTRY) {
      editEntry();
    } else if (input == SHOW_ENTRIES) {
      printEntries(getEntries());
    } else if (input == QUIT) {
      quit(EXIT_SUCCESS, "Closing the journal entry program, thank you!\n");
    } else {
      cout << "ERROR";
    }
    waitForEnter();
    addSeparation();
  }
  return 0;
}

tm* getCurrentTime() {
  time_t t = time(0);
  struct tm* now = localtime(&t);
  return now;
}

string getTimeString(tm *time) {
  int year = time->tm_year + 1900;
  int mon =  time->tm_mon + 1;
  int day = time->tm_mday;
  string strYear = to_string(year);
  string strMon = to_string(mon);
  string strDay = to_string(day);
  string dateString = strMon + "-" + strDay + "-" + strYear;
  return dateString;
}

void greeting() {
  cout << "Welcome to the journal entry program!\nIf this is your first time using the journal entry program enter \'help\' for a list of possible commans.\n";
  return;
}

int getInput() {
  while(1) {
    askForCommand();
    string input = "";
    getline(cin, input);
    if (input.compare("help") == COMPARE_TRUE) {
      return HELP;
    } else if (input.compare("new_entry") == COMPARE_TRUE) {
      return NEW_ENTRY;
    } else if (input.compare("edit_entry") == COMPARE_TRUE) {
      return EDIT_ENTRY;
    } else if (input.compare("show_entries") == COMPARE_TRUE) {
      return SHOW_ENTRIES;
    } else if (input.compare("quit") == COMPARE_TRUE) {
      return QUIT;
    } else {
      cout << "You have entered invalid input. If you do not know the possible commands, type \'help\'.\n";
    }
  }
  return INPUT_ERROR;
}

void help() {
  cout << "Possible commands:" << endl;
  cout << "help - lists the possible commands" << endl;
  cout << "new_entry - creates a new journal entry and opens the entry editor" << endl;
  cout << "show_entries - shows all of the journal entries that have been written" << endl;
  cout << "edit_entries - allows for editing a specific entry that has already been written" << endl;
  cout << "quit - closes the journal entry program" << endl;
  return;
}

void newEntry() {
  string entryText = "";
  string confirm = "";
  while (TRUE_INT) {
    cout << "What would you like to put in this journal entry? Remember to keep the entry to one paragraph." << endl;
    getline(cin, entryText);
    cout << "Are you sure this is what you want to enter [y/n]? " << endl;
    getline(cin, confirm);
    if (confirm.compare("Y") == COMPARE_TRUE || confirm.compare("y") == COMPARE_TRUE) {
      break;
    }
    cout << "\n\n\n";
  }

  tm* timeStruct = getCurrentTime();
  string currentTime = getTimeString(timeStruct);
  cout << "STUPID: " << currentTime << endl;
  vector<string> v;
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir("./entries")) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      ostringstream buffer;
      for (int i = 0; i < strlen(ent->d_name); i++) {
        buffer << ent->d_name[i];
      }
      string entry = buffer.str();
      if (entry.find(currentTime) != -1) {
        v.push_back(entry);
      }
    }

    int entrySpotFound = FALSE_INT;
    int firstRun = TRUE_INT;
    int count = 0;

    string entrySpot = "";
    if (v.size() == 0) {
      entrySpotFound = TRUE_INT;
      cout << "CurrentTime: " << currentTime << endl;
      entrySpot = currentTime;
    } else {
      cout << "SKIPPED" << endl;
    }
    while (entrySpotFound == FALSE_INT) {
      if (firstRun == TRUE_INT) {
        // TODO: THIS LINE ISN'T WORKING
        if (find(v.begin(), v.end(), currentTime) != v.end()) {
          cout << "found a thing 1" << endl;
          entrySpot = currentTime;
          break;
        }

        firstRun = FALSE_INT;
      } else {
        string checkString = currentTime + "_" + to_string(count);
        if (find(v.begin(), v.end(), checkString) != v.end()) {
          cout << "found a thing 2" << endl;
          entrySpot = checkString;
          break;
        }
      }
      count++;
    }

    cout << entryText << endl;
    cout << entrySpot << endl;

    ofstream file("./entries/" + entrySpot);
    file.open(entrySpot.c_str());
    if (file.is_open()) {
      cout << "File is open";
    }
    file << "I'm writing this to a file";
    file.close();
    closedir(dir);
  } else {
    quit(EXIT_FAILURE, "Failed to open entries directory\n");
  }

}

void editEntry() {
  while(1) {
    cout << "Current journal entries:" << endl;
    getEntries();
    cout << "Which entry would you like to edit?\n";
  }
  return;
}

vector<string> getEntries() {
  DIR *dir;
  struct dirent *ent;
  vector<string> entries;
  if ((dir = opendir("./entries")) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      ostringstream buffer;
      for (int i = 0; i < strlen(ent->d_name); i++) {
        buffer << ent->d_name[i];
      }
      string entry = buffer.str();
      if (entry.compare(".") != COMPARE_TRUE && entry.compare("..") != COMPARE_TRUE) {
        entries.push_back(entry);
      }
      buffer.clear();
      buffer.str("");
    }
  } else {
    cout << "ERROR: Could not read entry directory.\n";
  }
  return entries;
}

void quit(int exit_status, string exit_message) {
  if (exit_status == EXIT_SUCCESS) {
    cout << exit_message;
    exit(EXIT_SUCCESS);
  } else if (exit_status == EXIT_FAILURE) {
    perror(exit_message.c_str());
    exit(EXIT_FAILURE);
  }
}

void printEntries(vector<string> entries) {
  cout << "Previous journal entries" << endl;
  for (int i = 0; i < entries.size(); i++) {
    cout << entries[i] << endl;
  }
  return;
}

void askForCommand() {
  cout << "Please enter a command: ";
  return;
}

void addSeparation() {
  cout << "---------------------------------------------------------------------------------------\n\n\n";
}

void waitForEnter() {
  cout << "\n\n\nPress enter to continue...";
  getchar();
  return;
}
