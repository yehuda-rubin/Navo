#pragma once
#include "Tokenizer.h"
#include "AST.h"
#include <vector>
#include <stdexcept>

namespace Parser {

    class ExpressionParser {
    private:
        std::vector<Lexer::Token> tokens;
        size_t current;

        // Helper methods
        bool isAtEnd() const;
        Lexer::Token peek() const;
        Lexer::Token previous() const;
        bool check(Lexer::TokenType type) const;
        bool match(const std::string& value);
        Lexer::Token advance();
        Lexer::Token consume(Lexer::TokenType type, const std::string& message);

        // Grammar rules (with increment/decrement support)
        AST::ExpressionPtr expression();
        AST::ExpressionPtr logicalOr();
        AST::ExpressionPtr logicalAnd();
        AST::ExpressionPtr equality();
        AST::ExpressionPtr comparison();
        AST::ExpressionPtr term();
        AST::ExpressionPtr factor();
        AST::ExpressionPtr unary();
        AST::ExpressionPtr postfix();
        AST::ExpressionPtr primary();

    public:
        explicit ExpressionParser(const std::vector<Lexer::Token>& tokens);
        AST::ExpressionPtr parse();
    };

} // namespace Parser