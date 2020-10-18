#include <cstdio>
#include <cstdlib>
#include <sys/wait.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

const int FAIL = 1;

const int INIT = 0;
const int INCREMENT = 1;
const int CORRECT_ARGS = 3;

using namespace std;

const string SPACE = " ";
const string SUPPR_OUTPUT = " > /dev/null";
const string ARG = ".args";
const string IN = ".in";
const string OUT = ".out";
const string EXISTS = "test -f";
const string CAT = "cat";

// Extracts arguments from testName.args, then appends all of them to output in order.
void extractArguments(string& testName, stringstream& output) {
	ifstream args {testName + ARG}; // Access .args file
	args >> noskipws;
	string temp;
	char spaceholder;
	while (args >> temp) { // "Write out" all the arguments to the buffer
		output << temp;
		output << SPACE;
		args >> spaceholder; // extract 1 space char from args to progress the loop
	}
}

// Compares the expected and actual out streams.
bool compareResult(FILE *outfile, istream& expected) {
	char a = INIT, b = INIT;
	// Read char by char and compare
	while (!feof(outfile)) { // Loops while we're not done reading from outfile
		if (!expected) return false; // expected file ended earlier than actual output, a mismatch
		fscanf(outfile, "%c", &a); // Read 1 character from outfile. Store to a
		expected.read(&b, INCREMENT); // Read 1 character from expected. Store to b
		if (a != b) return false; // Mismatch
	}
	// If one of the files didn't end, then they are not the same.
	if (expected) return false;  // outfile ended; we expect expected to have ended as well.

	// If end of file was reached simultaneously and there were no mismatches, then the test passes.
	return true;
}

// Print summaries for failed tests
void printSummary(string& argName, stringstream& command, const bool in, const bool args) {
	cout << "Test failed: " << argName << endl;
	cout << "Args:" << endl;
	if (args) system((CAT + SPACE + argName + ARG).c_str());
	cout << "Input:" << endl;
	if (in) system((CAT + SPACE + argName + IN).c_str());
	cout << "Expected:" << endl;
	system((CAT + SPACE + argName + OUT).c_str());
	cout << "Actual:" << endl;
	string actual = command.str(); // This is the complete command generated from main and extractArgs.
	system(command.str().c_str()); // We use system() to generate actual output
}

// First checks for argument count, then runs a loop to test myprogram, stem by stem.
int main (int argc, char *argv[]) {
	int exit_code = INIT;
	if (argc != CORRECT_ARGS) { // Incorrect number of command line arguments (<=2 or >=4)
		cerr << "Error: include file suite and program name as arguments." << endl;
		return FAIL;
	}

	const string MYPROGRAM = argv[2]; // name of myprogram
	ifstream stems {argv[1]}; // contents of suite-file
	stems >> noskipws; // For delimitation
	string testName; // Temp variable to hold stem names for generating streams for each file
	
	while (stems >> testName) { // We go through suite-file, stem by stem (word by word)
		bool haveArgs = false; // Suppress error message prints when these files don't exist
		bool haveIn = false;
		stringstream command;
		command << MYPROGRAM + SPACE; // Fill the buffer with program name to begin with
		
		exit_code = system((EXISTS + SPACE + testName + OUT + SUPPR_OUTPUT).c_str()); // Check if .out file exists
		exit_code = WEXITSTATUS(exit_code);
		if (exit_code) { // If .out file doesn't exist, then this is an error and we end the program.
			cerr << "Error: " << testName << ".out is unreadable or doesn't exist." << endl;
			return exit_code;
		}
		// If BOTH .arg and .in do NOT exist, then it's an error
		int status1 = system((EXISTS + SPACE + testName + IN + SUPPR_OUTPUT).c_str());
		int status2 = system((EXISTS + SPACE + testName + ARG + SUPPR_OUTPUT).c_str());
		if (WEXITSTATUS(status1) != INIT && WEXITSTATUS(status2) != INIT) {
			cerr << "Error: You must have at least one of " <<
					testName + ARG << " or " << testName + IN << ". " << endl;
			return FAIL;
		}

		// If the .arg file exists, we must extract all arguments from it.
		exit_code = system((EXISTS + SPACE + testName + ARG + SUPPR_OUTPUT).c_str());
		if (WEXITSTATUS(exit_code) == INIT) {
			haveArgs = true;
			extractArguments(testName, command); // Saves all argument names to command.
		}
		// If the .in file exists, we add input redirection to our command stream.
		exit_code = system((EXISTS + SPACE + testName + IN + SUPPR_OUTPUT).c_str());
		if (WEXITSTATUS(exit_code) == INIT) {
			haveIn = true;
			command << "< " << testName + IN; 
		}
		
		// Now we are done creating our command buffer. We put it into a string and use popen.
		FILE *outfile = popen(command.str().c_str(), "r"); // Actual output
		
		ifstream expected {testName + OUT}; // Access the .out file
		expected >> noskipws;

		// Now we are ready to test. We first launch testResult() to compare expected & actual outputs.
		bool testResult = compareResult(outfile, expected);
		if (!testResult) printSummary(testName, command, haveIn, haveArgs); // print summary if test fails

		pclose(outfile);
		char spaceholder; // To remove whitespace for further progress in the loop
		// Remove as many as needed
		while (stems.peek() == ' ' || stems.peek() == '\n') stems >> spaceholder;
	}
	return exit_code;
}
