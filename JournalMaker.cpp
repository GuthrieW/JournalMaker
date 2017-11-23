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

#define EMPTY 0
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
#define DELETE_ENTRY 106

using namespace std;

tm* getCurrentTime();
string getTimeString(tm* time);
void greeting();
int getInput();
void help();
void newEntry();
void editEntry();
string chooseEntryToEdit();
vector<string> getEntries();
void quit(int exit_status, string exit_message);
void printVector(vector<string> entries);
void askForCommand();
void addSeparation();
void waitForEnter();
int vectorContainsString(vector<string> v, string find);
void deleteEntry();
string chooseEntryToDelete();
vector<string> sortDatesVector(vector<string> v);

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
      printVector(getEntries());
    } else if (input == DELETE_ENTRY) {
      deleteEntry();
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

/**
* getCurrentTime - returns a tm struct
*/
tm* getCurrentTime() {
  time_t t = time(0);
  struct tm* now = localtime(&t);
  return now;
}

/**
* getTimeString - returns a string form of the date given as input
* in the format: MM-DD-YYYY
*/
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

/**
* greeting - print a greeting to the user
*/
void greeting() {
  cout << "Welcome to the journal entry program!\nIf this is your first time using the journal entry program enter \'help\' for a list of possible commans.\n";
  return;
}

/**
* getInput - ask the user to enter a command and continue looping until they
* give valid input
*/
int getInput() {
  while(TRUE_INT) {
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
    } else if (input.compare("delete_entry") == COMPARE_TRUE) {
      return DELETE_ENTRY;
    } else if (input.compare("quit") == COMPARE_TRUE) {
      return QUIT;
    } else {
      cout << "You have entered invalid input. If you do not know the possible commands, type \'help\'.\n";
    }
  }
  return INPUT_ERROR;
}

/**
* help - prints the possible commands a user can enter
*/
void help() {
  cout << "Possible commands:" << endl;
  cout << "help - lists the possible commands" << endl;
  cout << "new_entry - creates a new journal entry and opens the entry editor" << endl;
  cout << "show_entries - shows all of the journal entries that have been written" << endl;
  cout << "edit_entry - allows for editing a specific entry that has already been written" << endl;
  cout << "delete_entry - allows for deleting a specific entry" << endl;
  cout << "quit - closes the journal entry program" << endl;
  return;
}

/**
* getEntryText - get the text the user wants to put in their jounral entry
*/
string getEntryText() {
  string entryText = "";
  string confirm = "";
  while (TRUE_INT) {
    cout << "What would you like to put in this journal entry?\nRemember to keep the entry to one paragraph." << endl;
    getline(cin, entryText);
    cout << "Are you sure this is what you want to enter [y/n]? " << endl;
    getline(cin, confirm);
    if (confirm.compare("Y") == COMPARE_TRUE || confirm.compare("y") == COMPARE_TRUE) {
      break;
    }
    cout << "\n\n\n";
  }
  return entryText;
}

/**
* newEntry - create a new jounral entry in the entry directory and write the
* text the user wants to it
*/
void newEntry() {
  string entryText = getEntryText();
  tm* timeStruct = getCurrentTime();
  string currentTime = getTimeString(timeStruct);
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
    int count = 1;

    string entrySpot = "";

    if (v.size() == EMPTY) {
      entrySpotFound = TRUE_INT;
      entrySpot = currentTime + "_" + to_string(count);
    }

    while (entrySpotFound == FALSE_INT) {
      string checkString = currentTime + "_" + to_string(count);
      if (vectorContainsString(v, checkString) == FALSE_INT) {
        entrySpot = checkString;
        break;
      }
      count++;
    }

    ofstream outputFile;
    string filename = "./entries/" + entrySpot;
    outputFile.open(filename);
    outputFile << entryText << endl;
    outputFile.close();
    closedir(dir);
  } else {
    quit(EXIT_FAILURE, "Failed to open entries directory\n");
  }
}

/**
* editEntry - edit a journal entry that already exists
*/
void editEntry() {
  string entryToEdit = chooseEntryToEdit();
  if (entryToEdit.compare("NO_ENTRIES") == COMPARE_TRUE) {
    cout << "There are no entries to edit" << endl;
    return;
  }

  vector<string> entries = getEntries();
  if (vectorContainsString(entries, entryToEdit) == FALSE_INT) {
    cout << "That entry does not exist and therefore cannot be edited" << endl;
    return;
  }

  string line;
  ifstream inFile;

  inFile.open("./entries/" + entryToEdit);

  if (inFile.is_open() == TRUE_INT) {
    cout << "The entry currently contains:" << endl;
    getline(inFile, line);
    cout << line << endl;
    inFile.close();
  }

  string replacementText = getEntryText();
  ofstream outputFile;
  string filename = "./entries/" + entryToEdit;
  outputFile.open(filename);
  outputFile << replacementText << endl;
  outputFile.close();
  return;
}

/**
* chooseEntryToDelete - asks the user which entry they want to edit and returns
* it as a string
*/
string chooseEntryToEdit() {
  vector<string> entries = getEntries();
  if (entries.size() == EMPTY) {
    return "NO_ENTRIES";
  }

  string entry = "";
  while(TRUE_INT) {
    printVector(entries);
    cout << "Which entry would you like to edit?" << endl;
    getline(cin, entry);
    if (vectorContainsString(entries, entry) == TRUE_INT) {
      break;
    } else {
      cout << "Please enter a valid entry.\n\n";
    }
  }
  cout << "We are going to edit the following entry: " << entry << endl;
  return entry;
}

/**
* getEntries - return a vector containing string representations of the date
* each entry was written
*/
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
    cout << "ERROR: Could not read entry directory." << endl;
  }
  return entries;
}

/**
* quit - close the program after printing a given message explaining the reason
* for the program closing, ranging from the program crashing to the user wanting
* to exit
*/
void quit(int exit_status, string exit_message) {
  if (exit_status == EXIT_SUCCESS) {
    cout << exit_message;
    exit(EXIT_SUCCESS);
  } else if (exit_status == EXIT_FAILURE) {
    perror(exit_message.c_str());
    exit(EXIT_FAILURE);
  }
}

/**
* printVector - print a vector one index at a time to the standard output
*/
void printVector(vector<string> entries) {
  cout << "Previous journal entries" << endl;
  for (int i = 0; i < entries.size(); i++) {
    cout << entries[i] << endl;
  }
  return;
}

/**
* askForCommand - ask the user to enter a command
*/
void askForCommand() {
  cout << "Please enter a command: ";
  return;
}

/**
* addSeparation - prints a line of separation
*/
void addSeparation() {
  cout << "---------------------------------------------------------------------\n\n\n";
}

/**
* waitForEnter - pauses the program by waiting for the user to press enter
*/
void waitForEnter() {
  cout << "\n\n\nPress enter to continue...";
  getchar();
  return;
}

/**
* vectorContainsString - checks a given vector<string> for a specfic string find
* and returns an integer TRUE_INT if the string is found or an integer FALSE_INT
* if the string is not found
*/
int vectorContainsString(vector<string> v, string find) {
  for (int i = 0; i < v.size(); i++) {
    if (v[i].compare(find) == COMPARE_TRUE) {
      return TRUE_INT;
    }
  }
  return FALSE_INT;
}

/**
* deleteEntry - removes an entry from the entry directory
*/
void deleteEntry() {
  cout << "DELETE FUNCTIONALITY HAS NOT BEEN ADDED YET" << endl;
  string entryToDelete = chooseEntryToDelete();

  // need to figure out the system call for deleting a file
  string entryPath = "./entries/" + entryToDelete;
  cout << entryPath.c_str() << endl;
  if (remove(entryPath.c_str()) == 0) {
    cout << "Successfully deleted " << entryToDelete << endl;
  } else {
    cout << "Error while deleting " << entryToDelete << endl;
  }
  return;
}

/**
* chooseEntryToDelete - ask the user which entry they want to delete and return
* it as a string
*/
string chooseEntryToDelete() {
  vector<string> entries = getEntries();
  if (entries.size() == EMPTY) {
    return "NO_ENTRIES";
  }

  string entry = "";
  while(TRUE_INT) {
    printVector(entries);
    cout << "Which entry would you like to delete?" << endl;
    getline(cin, entry);
    if (vectorContainsString(entries, entry) == TRUE_INT) {
      break;
    } else {
      cout << "Please enter a valid entry.\n\n";
    }
  }

  cout << "We are going to delete the following entry: " << entry << endl;
  return entry;
}

/**
* sortDatesVector - sorts a given vector<string> that is assumed to contain
* strings in the specfic date formate MM-DD-YYYY_ENTRYNUMBER
*/
vector<string> sortDatesVector(vector<string> v) {
  cout << "Dates vector is still not being sorted" << endl;
  return v;
}
