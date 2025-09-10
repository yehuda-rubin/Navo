// Tokenizer.cpp
#include "Tokenizer.h"
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <unordered_set>

namespace Lexer {

    std::vector<Token> tokenize(const std::string& input) {
        std::vector<Token> tokens;
        size_t pos = 0;

        // Keywords set for O(1) lookup
        static const std::unordered_set<std::string> keywords = {
            "if", "else", "while", "return", "for", "function", "number", "word",
            "boolean", "true", "false", "null", "const", "break", "continue",
            "main", "print", "input", "or", "and", "not", "do", "switch",
            "case", "default", "struct", "class", "public", "private", "protected"
        };

        while (pos < input.length()) {
            // Skip whitespace
            if (std::isspace(input[pos])) {
                pos++;
                continue;
            }

            // Handle identifiers and keywords
            if (std::isalpha(input[pos]) || input[pos] == '_') {
                size_t start = pos;
                while (pos < input.length() && (std::isalnum(input[pos]) || input[pos] == '_')) {
                    pos++;
                }

                std::string word = input.substr(start, pos - start);

                if (keywords.count(word) > 0) {
                    tokens.emplace_back(TokenType::Keyword, word);
                }
                else {
                    tokens.emplace_back(TokenType::Identifier, word);
                }
                continue;
            }

            // Handle numbers (integers and floats)
            if (std::isdigit(input[pos])) {
                size_t start = pos;

                // Read integer part
                while (pos < input.length() && std::isdigit(input[pos])) {
                    pos++;
                }

                // Check for decimal point
                if (pos < input.length() && input[pos] == '.') {
                    pos++; // consume '.'

                    // Must have at least one digit after decimal point
                    if (pos >= input.length() || !std::isdigit(input[pos])) {
                        throw std::runtime_error("Invalid float: missing digits after decimal point");
                    }

                    // Read fractional part
                    while (pos < input.length() && std::isdigit(input[pos])) {
                        pos++;
                    }
                }

                tokens.emplace_back(TokenType::Number, input.substr(start, pos - start));
                continue;
            }

            // Handle strings
            if (input[pos] == '"') {
                size_t start = pos;
                pos++; // skip opening quote

                while (pos < input.length() && input[pos] != '"') {
                    if (input[pos] == '\\' && pos + 1 < input.length()) {
                        pos += 2; // skip escape sequence
                    }
                    else {
                        pos++;
                    }
                }

                if (pos >= input.length()) {
                    throw std::runtime_error("Unterminated string literal");
                }

                pos++; // skip closing quote
                tokens.emplace_back(TokenType::String, input.substr(start, pos - start));
                continue;
            }

            // Handle single-line comments
            if (pos + 1 < input.length() && input[pos] == '/' && input[pos + 1] == '/') {
                size_t start = pos;
                pos += 2;
                while (pos < input.length() && input[pos] != '\n') {
                    pos++;
                }
                tokens.emplace_back(TokenType::Comment, input.substr(start, pos - start));
                continue;
            }

            // Handle block comments
            if (pos + 1 < input.length() && input[pos] == '/' && input[pos + 1] == '*') {
                size_t start = pos;
                pos += 2;

                // Look for closing */
                while (pos < input.length() &&
                    !(input[pos] == '*' && pos + 1 < input.length() && input[pos + 1] == '/')) {
                    pos++;
                }

                if (pos < input.length()) {
                    // Found closing */, consume it
                    pos += 2;
                    tokens.emplace_back(TokenType::Comment, input.substr(start, pos - start));
                }
                else {
                    // Reached end without finding closing */
                    throw std::runtime_error("Unterminated block comment");
                }
                continue;
            }

            // Handle multi-character operators
            if (pos + 1 < input.length()) {
                std::string twoChar = input.substr(pos, 2);

                // Comparison and logical operators
                if (twoChar == "==" || twoChar == "!=" || twoChar == "<=" ||
                    twoChar == ">=" || twoChar == "&&" || twoChar == "||" ||
                    twoChar == "++" || twoChar == "--" || twoChar == "+=" ||
                    twoChar == "-=" || twoChar == "*=" || twoChar == "/=" ||
                    twoChar == "%=" || twoChar == "<<" || twoChar == ">>") {
                    tokens.emplace_back(TokenType::Operator, twoChar);
                    pos += 2;
                    continue;
                }
            }

            // Handle single-character tokens
            char ch = input[pos];
            switch (ch) {
                // Arithmetic operators
            case '+': case '-': case '*': case '/': case '%':
            case '^': case '=': case '!': case '<': case '>':
            case '&': case '|': case '~':
                tokens.emplace_back(TokenType::Operator, std::string(1, ch));
                break;

                // Punctuation and delimiters
            case ';': case ',': case '.': case ':':
            case '(': case ')': case '{': case '}': case '[': case ']':
                tokens.emplace_back(TokenType::Punctuation, std::string(1, ch));
                break;

            default:
                throw std::runtime_error("Unrecognized character: '" + std::string(1, ch) + "'");
            }

            pos++;
        }

        return tokens;
    }

    std::string tokenTypeToString(TokenType type) {
        switch (type) {
        case TokenType::Identifier:   return "Identifier";
        case TokenType::Keyword:      return "Keyword";
        case TokenType::Number:       return "Number";
        case TokenType::String:       return "String";
        case TokenType::Operator:     return "Operator";
        case TokenType::Punctuation:  return "Punctuation";
        case TokenType::Comment:      return "Comment";
        case TokenType::Whitespace:   return "Whitespace";
        case TokenType::Unknown:      return "Unknown";
        default:                      return "Invalid";
        }
    }

} // namespace Lexer