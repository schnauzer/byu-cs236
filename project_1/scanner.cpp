#include <iostream>
#include <cctype>

#include "scanner.h"

static const std::string ALPHA_NUMERIC = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

uint scanner_pos = 0;

bool read_until(std::string* input, std::string delimiter, std::string* output) {
	size_t pos = input->find(delimiter, scanner_pos);
	std::string token;

	if (pos != std::string::npos) {
		*output = input->substr(scanner_pos, (pos - scanner_pos));
		scanner_pos = pos + delimiter.length();
		return true;
	} else {
		return false;
	}
}

bool read_string(std::string* input, std::string value, std::string* output) {
	std::string token = input->substr(scanner_pos, value.length());

	if (token == value) { // input must start with desired string
		// input->erase(0, value.length());
		scanner_pos += value.length();
		*output = token;
		return true;
	} else {
		return false;
	}
}

// TO WHOM IT MAY CONCERN
// the following 3 code snippets may make little sense to you
// a trained eye will note, however, that this is only because they are "low complexity"
// my personal favorite, the "function in halves" has a cyclomatic complexity of 1
// what a triumph! THIS is what maintanable code looks like!!!!!!
//
// In all seriousness though, the main loop of a lexer can be expected to be long
// and have a lot of if/elses. This does not equate with complexity, as a human
// can easily understand that verbosity and simplicity are not mutually exclusive.

bool read_identifier(std::string* input, std::string* output, Token_type* type) {
	if (isalpha(input->at(scanner_pos))) {
		size_t pos = input->find_first_not_of(ALPHA_NUMERIC, scanner_pos);
		*output = input->substr(scanner_pos, (pos - scanner_pos));
		// input->erase(0, pos);
		scanner_pos = pos;

		if (*output == "Schemes") *type = SCHEMES;
		else if (*output == "Facts") *type = FACTS;
		else if (*output == "Rules") *type = RULES;
		else if (*output == "Queries") *type = QUERIES;
		else *type = ID;

		return true;
	} else {
		return false;
	}
}

// read_punctuation is down to a complexity of 1! It must be so maintainable!
#define FIRST_HALF\
	if (read_string(input, ",", output)) {\
		*type = COMMA;\
	} else if (read_string(input, ".", output)) {\
		*type = PERIOD;\
	} else if (read_string(input, "?", output)) {\
		*type = Q_MARK;\
	} else if (read_string(input, "(", output)) {\
		*type = LEFT_PAREN;\
	} else
#define SECOND_HALF\
	if (read_string(input, ")", output)) {\
		*type = RIGHT_PAREN;\
	} else if (read_string(input, ":-", output)) {\
		*type = COLON_DASH;\
	} else if (read_string(input, ":", output)) {\
		*type = COLON;\
	} else if (read_string(input, ".", output)) {\
		*type = PERIOD;\
	} else {\
		return false;\
	}
bool read_punctuation(std::string* input, std::string* output, Token_type* type) {
	FIRST_HALF
	SECOND_HALF

	return true;
}

#define SKIP_NEWLINES if (read_string(&str, "\n", &output)) { line++; continue; }

#define SKIP_WHITESPACE if ( read_string(&str, "\t", &output) || read_string(&str, " ", &output)) continue;

// it's actually about ethics in game journalism!
#define MAIN_LOOP while ((scanner_pos + 1) < str.length())

int Scanner::lex_file(std::string str, std::queue<Token>* queue) {
	int line = 1;

	MAIN_LOOP {
		std::string output;
		Token_type type;

		SKIP_NEWLINES;

		SKIP_WHITESPACE;

		if (read_string(&str, "#", &output)) { // comments
			// TODO: make sure this handles EOF
			read_until(&str, "\n", &output);
			line++;
			continue;
		}

		if (read_punctuation(&str, &output, &type)) { // punctuation
			// do nothing, output and type have been populated
		} else if (read_string(&str, "'", &output)) { // strings
			type = STRING;

			if (read_until(&str, "'", &output)) {
				// string ended, check for lines inside string

				size_t pos = (&output)->find("\n");
				if (pos != std::string::npos) {
					return line;
				}
			} else {
				// string did not end
				return line;
			}
		} else if (read_identifier(&str, &output, &type)) {
			// do nothing
		} else {
			return line;
		}

		Token token(type, output, line);
		queue->push(token);
	}

	return -1;
}
