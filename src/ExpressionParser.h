#pragma once
#include "Tokenizer.h"
#include "AST.h"
#include <vector>
#include <stdexcept>

namespace Parser {

    class ExpressionParser {
    protected:
        std::vector<Lexer::Token> tokens;
        size_t current;

        // Helper methods
		bool isAtEnd() const; // Check if we've consumed all tokens
		Lexer::Token peek() const; // Look at the current token without consuming it
		Lexer::Token previous() const; // Look at the last consumed token
		bool check(Lexer::TokenType type) const; // Check if the current token matches a type
		bool match(const std::string& value); // Check and consume if the current token matches a specific value
		Lexer::Token advance(); // Consume the current token and return it
		Lexer::Token consume(Lexer::TokenType type, const std::string& message); // Consume a token of a specific type or throw an error

        // Grammar rules (with increment/decrement support)
		AST::ExpressionPtr expression(); // Entry point
		AST::ExpressionPtr logicalOr(); // New method for logical OR
		AST::ExpressionPtr logicalAnd(); // New method for logical AND
		AST::ExpressionPtr equality(); // Updated to handle == and !=
		AST::ExpressionPtr comparison(); // Updated to handle >, >=, <, <=
		AST::ExpressionPtr term(); // Updated to handle + and -
		AST::ExpressionPtr factor(); // Updated to handle *, /, and %
		AST::ExpressionPtr unary(); // Updated to handle unary +, -, not, !, ++, --
		AST::ExpressionPtr postfix(); // New method for post-increment/decrement
		AST::ExpressionPtr primary(); // Updated to handle booleans

    public:
        explicit ExpressionParser(const std::vector<Lexer::Token>& tokens);
        AST::ExpressionPtr parse();
    };

} // namespace Parser