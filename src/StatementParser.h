#pragma once
#include "Tokenizer.h"
#include "AST.h"
#include "ExpressionParser.h"
#include <vector>
#include <stdexcept>

namespace Parser {

    class StatementParser {
    private:
        std::vector<Lexer::Token> tokens;
        size_t current;

        // Helper methods (similar to ExpressionParser)
        bool isAtEnd() const;
        Lexer::Token peek() const;
        Lexer::Token previous() const;
        bool check(Lexer::TokenType type) const;
        bool match(const std::string& value);
        bool matchKeyword(const std::string& keyword);
        Lexer::Token advance();
        Lexer::Token consume(Lexer::TokenType type, const std::string& message);
        void expect(const std::string& value, const std::string& message);

        // Grammar rules for statements
        AST::StatementPtr statement();
        AST::StatementPtr variableDeclaration();
        AST::StatementPtr assignmentOrExpressionStatement();
        AST::StatementPtr ifStatement();
        AST::StatementPtr block();
        AST::StatementPtr expressionStatement();

        // Helper to parse expressions (delegates to ExpressionParser)
        AST::ExpressionPtr parseExpression();

    public:
        explicit StatementParser(const std::vector<Lexer::Token>& tokens);

        // Parse a single statement
        AST::StatementPtr parse();

        // Parse multiple statements (for blocks or whole programs)
        std::vector<AST::StatementPtr> parseStatements();
    };

} // namespace Parser