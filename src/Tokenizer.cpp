// Tokenizer.cpp
#include "Tokenizer.h"
#include <stdexcept>
#include <cctype>
#include <algorithm>

namespace Lexer {

    std::vector<Token> tokenize(const std::string& input) {  // const reference
        std::vector<Token> tokens;
        size_t size = 0;

        while (size < input.length()) {
			// Skip whitespace
            if (std::isspace(input[size])) {
                size++;
                continue;
			}// if (std::isspace(input[size]))

			// Handle identifiers and keywords
            if (std::isalpha(input[size]) || input[size] == '_') {
                size_t start = size;
				// Match alphanumeric characters and underscores
                while (size < input.length() && (std::isalnum(input[size]) || input[size] == '_')) {
                    size++;
				} // while (size < input.length() && (std::isalnum(input[size]) || input[size] == '_'))

                std::string word = input.substr(start, size - start);

				// Check if the word is a keyword or identifier
                std::vector<std::string> keywords = {
					"if", "else", "while", "return", "for", "function", "number", "word", "boolean", "true", "false", "null", "const", "break"
					, "continue", "main", "print", "input", "or", "and", "not"
                };

				// Check if the word is in the list of keywords
                if (std::find(keywords.begin(), keywords.end(), word) != keywords.end()) {
                    tokens.push_back({ TokenType::Keyword, word });
				}// If (std::find(keywords.begin(), keywords.end(), word) != keywords.end())

				// If the word is not a keyword, treat it as an identifier
                else {
                    tokens.push_back({ TokenType::Identifier, word });
				}// else

                continue;
            }

			// Handle numbers
            if (std::isdigit(input[size])) {
                size_t start = size;
				// Match digits
                while (size < input.length() && std::isdigit(input[size])) {
                    size++;
                }
                tokens.push_back({ TokenType::Number, input.substr(start, size - start) });
                continue;
			}// if (std::isdigit(input[size]))

			// Handle strings
            if (input[size] == '"') {
                size_t start = size;
                size++;

				// Match until the closing quote
                while (size < input.length() && input[size] != '"') {
                    if (input[size] == '\\' && size + 1 < input.length()) {
                        size += 2;
                    } 
                    else {
                        size++;
                    }
                }
				// Check for closing quote
                if (size < input.length() && input[size] == '"') {
                    size++;
                    tokens.push_back({ TokenType::String, input.substr(start, size - start) });
                }
                else {
                    throw std::runtime_error("Unterminated string");
                }
                continue;
            }

			// Handle comments
            if (input[size] == '/' && size + 1 < input.length() && input[size + 1] == '/') {
                size_t start = size;
                size += 2;
                while (size < input.length() && input[size] != '\n') {
                    size++;
                }
                tokens.push_back({ TokenType::Comment, input.substr(start, size - start) });
                continue;
            }

			// Handle block comments
            if (input[size] == '/' && size + 1 < input.length() && input[size + 1] == '*') {
                size_t start = size;
                size += 2;
                while (size < input.length() && !(input[size] == '*' && size + 1 < input.length() && input[size + 1] == '/')) {
                    size++;
                }
                if (size < input.length()) {
                    size += 2;
                    tokens.push_back({ TokenType::Comment, input.substr(start, size - start) });
                }
                else {
                    throw std::runtime_error("Unterminated block comment");
                }
                continue;
            }

			// Handle operators and punctuation
            if (std::ispunct(input[size])) {
                if (size + 1 < input.length()) {
                    std::string twoCharOp = input.substr(size, 2);
                    if (twoCharOp == "==" || twoCharOp == "!=" || twoCharOp == "<=" || twoCharOp == ">=") {
                        tokens.push_back({ TokenType::Operator, twoCharOp });
                        size += 2;
                        continue;
                    }
                }
                TokenType type = TokenType::Unknown;
                std::string value(1, input[size]);
                switch (input[size]) {
                case '+': case '-': case '*': case '/': case '%':
                case '^': case '=': case '!': case '<': case '>':
                    type = TokenType::Operator; break;
                case ';': case ',': case '.': case '(': case ')':
                case '{': case '}':
                    type = TokenType::Punctuation; break;
                }
                tokens.push_back({ type, value });
                size++;
                continue;
            }

			// If we reach here, the character is unrecognized
            throw std::runtime_error("Unrecognized character: " + std::string(1, input[size]));
        }

		// Return the list of tokens
        return tokens;
    }

	// Convert TokenType to string for debugging
    std::string tokenTypeToString(TokenType type) {
        switch (type) {
        case TokenType::Identifier: return "Identifier";
        case TokenType::Keyword: return "Keyword";
        case TokenType::Number: return "Number";
        case TokenType::String: return "String";
        case TokenType::Operator: return "Operator";
        case TokenType::Punctuation: return "Punctuation";
        case TokenType::Comment: return "Comment";
        case TokenType::Whitespace: return "Whitespace";
        case TokenType::Unknown: return "Unknown";
        default: return "Unknown Type";
        }
    }

} // namespace Lexer