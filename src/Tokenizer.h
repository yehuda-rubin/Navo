#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include <cctype>
#include <iostream>

namespace Lexer {

    // Enum for token types
    enum class TokenType {
        Identifier,
        Keyword,
        Number, 
        String,
        Operator,
        Punctuation,
        Comment,
        Whitespace,
        Unknown
    }; // enum TokenType

    // Struct to represent a token
    struct Token {
        TokenType type;
        std::string value;

        // Constructor for easier token creation
        Token(TokenType t, const std::string& v) : type(t), value(v) {}
    }; // struct Token

    // Function to tokenize a string input
    std::vector<Token> tokenize(const std::string& input);
    std::string tokenTypeToString(TokenType type);

} // namespace Lexer