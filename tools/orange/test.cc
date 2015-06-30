/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level 
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file 
** may not be copied, modified, or distributed except according to those terms.
*/ 
#include <sstream>
#include <fcntl.h>
#include <orange/test.h>
#include <orange/commands/CodeExecutor.h>

#define MAX_CHARACTERS_PER_LINE 40
int addedCharacters = 0;


#ifdef _WIN32 
	const char* NULLFILE = "NUL";
#else 
	const char* NULLFILE = "/dev/null";
#endif 

std::vector<RunResult *> runTest(path p) {
	std::vector<RunResult *> results; 

	CodeExecutor* executor = new CodeExecutor(); 

	// If it's a directory _with_ a ORANGE_SETTINGS file, run the project only.
	if (is_directory(p) && exists(p / "orange.settings.json")) {
		// Run the project.
		executor->compileProject(p.string()); 
		executor->runCompiled();
		RunResult* testRun = (RunResult *)executor->result();

		// Add a . if it passed, F otherwise. After printing a multiple 
		// of MAX_CHARACTERS_PER_LINE notices, print a newline.
		std::cout << (testRun->passed() ? "." : "F") << std::flush;
		if ((++addedCharacters % MAX_CHARACTERS_PER_LINE) == 0) std::cout << std::endl;

		results.push_back(testRun);
	} else if (is_directory(p)) {
		// Go through each item in the directory recursively
		for(auto& entry : boost::make_iterator_range(directory_iterator(p), directory_iterator())) {
			if (is_directory(entry) == false) {
				if (entry.path().extension().string() != ".or") continue;
			}

    	for(auto res : runTest(entry)) {
    		results.push_back(res);
    	}	
    }
	} else {
		// Disable output
		int bak, newFd;
		fflush(stdout);
		bak = dup(STDOUT_FILENO);
		newFd = open(NULLFILE, O_WRONLY);
		dup2(newFd, STDOUT_FILENO);
		close(newFd);

		// Otherwise just run the single file  
		executor->compileFile(p.string()); 
		executor->runCompiled();
		RunResult* testRun = (RunResult *)executor->result();

		// Re-enable output		
		fflush(stdout);
		dup2(bak, STDOUT_FILENO);
		close(bak);

		// Add a . if it passed, F otherwise. After printing a multiple 
		// of MAX_CHARACTERS_PER_LINE notices, print a newline.
		std::cout << (testRun->passed() ? "." : "F") << std::flush;
		if ((++addedCharacters % MAX_CHARACTERS_PER_LINE) == 0) std::cout << std::endl;

		results.push_back(testRun);
	} 

	return results;
}

float totalTestTime(std::vector<RunResult*> results) {
	unsigned long long totTimeMS = 0;
	for (auto res : results) {
		totTimeMS += res->runtime();
	}
	return totTimeMS / 1000.0f;
}

float avgTestTime(std::vector<RunResult*> results) {
	// Prevent a divide by zero here.
	if (results.size() == 0) return 0;

	unsigned long long totTimeMS = 0;
	for (auto res : results) {
		totTimeMS += res->runtime();
	}

	return (totTimeMS / results.size()) / 1000.0f;	
}

int numPassedTests(std::vector<RunResult*> results) {
	int numPassed = 0;

	for (auto res : results) {
		if (res->passed()) numPassed++;
	}

	return numPassed; 
}

int numFailedTests(std::vector<RunResult*> results) {
	int numFailed = 0;

	for (auto res : results) {
		if (res->passed() == false) numFailed++;
	}

	return numFailed; 
}

std::string getLongestTest(std::vector<RunResult*> results) {
	if (results.size() == 0) {
		return "(No tests have been ran)\n";
	}

	// The longest test, initially, is the first one.
	RunResult* longestTest = results[0]; 

	// For each result, if its runtime is longer, that one is the new longest.
	for (auto res : results) {
		if (res->runtime() > longestTest->runtime()) {
			longestTest = res; 
		}
	}

	// build a string from the test and return.
	std::stringstream ss; 
	ss << longestTest->filename() << " (" << (longestTest->runtime() / 1000.0f) << " seconds)"; 
	return ss.str();
}

void showTestResults(std::vector<RunResult*> results) {
	// Print a newline, since calling runTest() doesn't add one at the end.
	std::cout << std::endl;

	// Print out a line break
	std::cout << std::endl;

	int percPassed = ((float)numPassedTests(results)/(float)results.size()) * 100;

	std::cout << "Test results (" << totalTestTime(results) << " seconds):\n";
	std::cout << "\t" << numPassedTests(results) << "/" << results.size() << " tests passed (" << percPassed << "%).\n";
		if (avgTestTime(results) > 0.00001f)
		std::cout << "\t" << avgTestTime(results) << " seconds to run on average.\n";
	std::cout << "\tLongest test was: " << getLongestTest(results) << "\n";

	// Print out the number of failed tests if we have any
	if (numFailedTests(results) > 0) {
		std::cout << "\nErrors:\n";
		for (auto res : results) {
			// Skip over anything that passed; only get fails.
			if (res->passed()) continue; 

			// Get only the filename itself, not the full path
			path p(res->filename()); 
			std::cout << "\t" << p.relative_path().string(); 

			// If we only have one error, print on same line and continue.
			if (res->errors().size() == 1) {
				std::cout << " (" << res->errors().at(0).what() << ")\n";
				continue; 
			}

			// Otherwise, print out everything indented.
			std::cout << ":\n"; 
			for (auto err : res->errors()) {
				std::cout << "\t\t" << err.what() << std::endl;
			}
		}
	}
}

void doTestCommand(cOptionsState test) {
	addedCharacters = 0; 

	try {
		current_path(findProjectDirectory());
	} catch (std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return;
	}

	// If the user didn't enter anything, we'll just test everything in the test/ folder.
	if (test.unparsed().size() == 0) {
		auto results = runTest(path("test"));

		showTestResults(results);
		
		// Cleanup results
  	for (auto res : results) delete res;
		
		return;
	}

	// Create a results list for our multiple files.
	std::vector<RunResult*> results;

	for (auto& str : test.unparsed()) {
		path p("test" / str);
		if (exists(p) == false) {
			std::cerr << "error: " << p << " doesn't exist.\n"; 
			continue;
		}

		// Add each result from every run to our results.
		for (auto res : runTest(p)) {
			results.push_back(res);
		}
	}

	showTestResults(results);

	// Cleanup results 
	for (auto res : results) delete res; 

	// Print a newline, since our runTest() wouldn't have.
	std::cout << "\n"; 
}
