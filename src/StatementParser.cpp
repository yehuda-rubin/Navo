#include "StatementParser.h"
#include <iostream>

namespace Parser {

    StatementParser::StatementParser(const std::vector<Lexer::Token>& tokens)
        : tokens(tokens), current(0) {
    }

    bool StatementParser::isAtEnd() const {
        return current >= tokens.size();
    }

    Lexer::Token StatementParser::peek() const {
        if (isAtEnd()) {
            return Lexer::Token(Lexer::TokenType::Unknown, "");
        }
        return tokens[current];
    }

    Lexer::Token StatementParser::previous() const {
        if (current == 0) {
            return Lexer::Token(Lexer::TokenType::Unknown, "");
        }
        return tokens[current - 1];
    }

    bool StatementParser::check(Lexer::TokenType type) const {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    bool StatementParser::match(const std::string& value) {
        if (!isAtEnd() && peek().value == value) {
            advance();
            return true;
        }
        return false;
    }

    bool StatementParser::matchKeyword(const std::string& keyword) {
        if (!isAtEnd() && peek().type == Lexer::TokenType::Keyword && peek().value == keyword) {
            advance();
            return true;
        }
        return false;
    }

    Lexer::Token StatementParser::advance() {
        if (!isAtEnd()) current++;
        return previous();
    }

    Lexer::Token StatementParser::consume(Lexer::TokenType type, const std::string& message) {
        if (check(type)) {
            return advance();
        }

        std::string error = message + ". Got: ";
        if (!isAtEnd()) {
            error += "'" + peek().value + "' (type: " + std::to_string(static_cast<int>(peek().type)) + ")";
        }
        else {
            error += "end of input";
        }
        throw std::runtime_error(error);
    }

    // Helper: consume a specific token value
    void StatementParser::expect(const std::string& value, const std::string& message) {
        if (match(value)) {
            return;
        }

        std::string error = message + ". Got: ";
        if (!isAtEnd()) {
            error += "'" + peek().value + "'";
        }
        else {
            error += "end of input";
        }
        throw std::runtime_error(error);
    }

    // Parse a full expression by delegating to ExpressionParser
    AST::ExpressionPtr StatementParser::parseExpression() {
        if (isAtEnd()) {
            throw std::runtime_error("Expected expression");
        }

        // Collect all tokens until we hit a terminator
        std::vector<Lexer::Token> exprTokens;
        int depth = 0; // Track parentheses depth

        while (!isAtEnd()) {
            const auto& token = peek();

            // Handle parentheses depth
            if (token.value == "(") {
                depth++;
            }
            else if (token.value == ")") {
                if (depth == 0) {
                    // This closing paren is not part of our expression
                    break;
                }
                depth--;
            }

            // Terminators (only when not inside parentheses)
            if (depth == 0) {
                if (token.value == ";" || token.value == "{" || token.value == "}") {
                    break;
                }

                // Non-expression keywords terminate
                if (token.type == Lexer::TokenType::Keyword) {
                    if (token.value != "true" && token.value != "false" &&
                        token.value != "not" && token.value != "and" && token.value != "or") {
                        break;
                    }
                }
            }

            exprTokens.push_back(token);
            advance();
        }

        if (exprTokens.empty()) {
            throw std::runtime_error("Expected expression");
        }

        // Use ExpressionParser to parse the collected tokens
        ExpressionParser parser(exprTokens);
        return parser.parse();
    }

    // Main statement dispatcher
    AST::StatementPtr StatementParser::statement() {
        // Variable declarations
        if (check(Lexer::TokenType::Keyword)) {
            const auto& kw = peek().value;
            if (kw == "number" || kw == "word" || kw == "boolean") {
                return variableDeclaration();
            }
            if (kw == "if") {
                return ifStatement();
            }
        }

        // Blocks
        if (peek().value == "{") {
            return block();
        }

        // Assignment or expression statement
        return assignmentOrExpressionStatement();
    }

    // VariableDeclaration ::= Type Identifier ['=' Expression] ';'
    AST::StatementPtr StatementParser::variableDeclaration() {
        // Type keyword
        auto typeToken = consume(Lexer::TokenType::Keyword, "Expected type keyword");
        std::string type = typeToken.value;

        // Identifier
        auto nameToken = consume(Lexer::TokenType::Identifier, "Expected variable name");
        std::string name = nameToken.value;

        // Optional initializer
        AST::ExpressionPtr initializer = nullptr;
        if (match("=")) {
            initializer = parseExpression();
        }

        // Semicolon
        expect(";", "Expected ';' after variable declaration");

        return AST::makeVariableDeclaration(type, name, std::move(initializer));
    }

    // Determine if this is assignment or expression statement
    AST::StatementPtr StatementParser::assignmentOrExpressionStatement() {
        // Look ahead: if we have identifier followed by '=', it's assignment
        if (current < tokens.size() &&
            tokens[current].type == Lexer::TokenType::Identifier &&
            current + 1 < tokens.size() &&
            tokens[current + 1].value == "=") {

            // Assignment
            std::string varName = advance().value;
            expect("=", "Expected '=' in assignment");
            auto value = parseExpression();
            expect(";", "Expected ';' after assignment");

            return AST::makeAssignment(varName, std::move(value));
        }

        // Expression statement
        return expressionStatement();
    }

    // ExpressionStatement ::= Expression ';'
    AST::StatementPtr StatementParser::expressionStatement() {
        auto expr = parseExpression();
        expect(";", "Expected ';' after expression");
        return AST::makeExpressionStatement(std::move(expr));
    }

    // IfStatement ::= 'if' '(' Expression ')' Statement ['else' Statement]
    AST::StatementPtr StatementParser::ifStatement() {
        if (!matchKeyword("if")) {
            throw std::runtime_error("Expected 'if'");
        }

        expect("(", "Expected '(' after 'if'");
        auto condition = parseExpression();
        expect(")", "Expected ')' after if condition");

        auto thenStatement = statement();

        AST::StatementPtr elseStatement = nullptr;
        if (matchKeyword("else")) {
            elseStatement = statement();
        }

        return AST::makeIf(std::move(condition), std::move(thenStatement), std::move(elseStatement));
    }

    // Block ::= '{' Statement* '}'
    AST::StatementPtr StatementParser::block() {
        expect("{", "Expected '{'");

        std::vector<AST::StatementPtr> statements;

        while (!isAtEnd() && peek().value != "}") {
            statements.push_back(statement());
        }

        expect("}", "Expected '}' after block");

        return AST::makeBlock(std::move(statements));
    }

    // Parse a single statement from the token stream
    AST::StatementPtr StatementParser::parse() {
        auto result = statement();

        if (!isAtEnd()) {
            throw std::runtime_error("Unexpected token after statement: '" + peek().value + "'");
        }

        return result;
    }

    // Parse multiple statements
    std::vector<AST::StatementPtr> StatementParser::parseStatements() {
        std::vector<AST::StatementPtr> statements;

        while (!isAtEnd()) {
            statements.push_back(statement());
        }

        return statements;
    }

} // namespace Parser