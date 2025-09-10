#include "ExpressionParser.h"
#include <iostream>

namespace Parser {

    ExpressionParser::ExpressionParser(const std::vector<Lexer::Token>& tokens)
        : tokens(tokens), current(0) {
    }

    bool ExpressionParser::isAtEnd() const {
        return current >= tokens.size();
    }

    Lexer::Token ExpressionParser::peek() const {
        if (isAtEnd()) {
            return Lexer::Token(Lexer::TokenType::Unknown, "");
        }
        return tokens[current];
    }

    Lexer::Token ExpressionParser::previous() const {
        if (current == 0) {
            return Lexer::Token(Lexer::TokenType::Unknown, "");
        }
        return tokens[current - 1];
    }

    bool ExpressionParser::check(Lexer::TokenType type) const {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    bool ExpressionParser::match(const std::string& value) {
        if (!isAtEnd() && peek().value == value) {
            advance();
            return true;
        }
        return false;
    }

    Lexer::Token ExpressionParser::advance() {
        if (!isAtEnd()) current++;
        return previous();
    }

    Lexer::Token ExpressionParser::consume(Lexer::TokenType type, const std::string& message) {
        if (check(type)) {
            return advance();
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

    // Expression ::= LogicalOr
    AST::ExpressionPtr ExpressionParser::expression() {
        return logicalOr();
    }

    // LogicalOr ::= LogicalAnd ( ('or' | '||') LogicalAnd )*
    AST::ExpressionPtr ExpressionParser::logicalOr() {
        auto expr = logicalAnd();

        while (match("or") || match("||")) {
            std::string operator_ = previous().value;
            auto right = logicalAnd();
            expr = AST::makeBinary(std::move(expr), operator_, std::move(right));
        }

        return expr;
    }

    // LogicalAnd ::= Equality ( ('and' | '&&') Equality )*
    AST::ExpressionPtr ExpressionParser::logicalAnd() {
        auto expr = equality();

        while (match("and") || match("&&")) {
            std::string operator_ = previous().value;
            auto right = equality();
            expr = AST::makeBinary(std::move(expr), operator_, std::move(right));
        }

        return expr;
    }

    // Equality ::= Comparison ( ('==' | '!=') Comparison )*
    AST::ExpressionPtr ExpressionParser::equality() {
        auto expr = comparison();

        while (match("==") || match("!=")) {
            std::string operator_ = previous().value;
            auto right = comparison();
            expr = AST::makeBinary(std::move(expr), operator_, std::move(right));
        }

        return expr;
    }

    // Comparison ::= Term ( ('>' | '>=' | '<' | '<=') Term )*
    AST::ExpressionPtr ExpressionParser::comparison() {
        auto expr = term();

        while (match(">") || match(">=") || match("<") || match("<=")) {
            std::string operator_ = previous().value;
            auto right = term();
            expr = AST::makeBinary(std::move(expr), operator_, std::move(right));
        }

        return expr;
    }

    // Term ::= Factor ( ('+' | '-') Factor )*
    AST::ExpressionPtr ExpressionParser::term() {
        auto expr = factor();

        while (match("+") || match("-")) {
            std::string operator_ = previous().value;
            auto right = factor();
            expr = AST::makeBinary(std::move(expr), operator_, std::move(right));
        }

        return expr;
    }

    // Factor ::= Unary ( ('*' | '/' | '%') Unary )*
    AST::ExpressionPtr ExpressionParser::factor() {
        auto expr = unary();

        while (match("*") || match("/") || match("%")) {
            std::string operator_ = previous().value;
            auto right = unary();
            expr = AST::makeBinary(std::move(expr), operator_, std::move(right));
        }

        return expr;
    }

    // Unary ::= ('not' | '!' | '-' | '+' | '++' | '--') Unary | Postfix
    AST::ExpressionPtr ExpressionParser::unary() {
        // Handle traditional unary operators
        if (match("not") || match("!") || match("-") || match("+")) {
            std::string operator_ = previous().value;
            auto right = unary();
            return AST::makeUnary(operator_, std::move(right));
        }

        // Handle pre-increment/decrement (++x, --x)
        if (match("++") || match("--")) {
            std::string operator_ = previous().value;
            if (check(Lexer::TokenType::Identifier)) {
                advance();
                std::string variable = previous().value;
                return AST::makePreIncrement(operator_, variable);
            }
            else {
                throw std::runtime_error("Expected identifier after " + operator_);
            }
        }

        return postfix();
    }

    // Postfix ::= Primary ( '++' | '--' )?
    AST::ExpressionPtr ExpressionParser::postfix() {
        auto expr = primary();

        // Check for post-increment/decrement (x++, x--)
        if (match("++") || match("--")) {
            std::string operator_ = previous().value;

            // Verify that the expression is an identifier
            if (auto identifier = dynamic_cast<AST::Identifier*>(expr.get())) {
                std::string variable = identifier->name;
                // Release the unique_ptr ownership before creating new node
                expr.release();
                return AST::makePostIncrement(variable, operator_);
            }
            else {
                throw std::runtime_error("Post-increment/decrement can only be applied to variables");
            }
        }

        return expr;
    }

    // Primary ::= Number | Identifier | Boolean | '(' Expression ')'
    AST::ExpressionPtr ExpressionParser::primary() {
        if (match("true")) {
            return AST::makeBoolean(true);
        }
        if (match("false")) {
            return AST::makeBoolean(false);
        }

        if (check(Lexer::TokenType::Number)) {
            advance();
            return AST::makeNumber(previous().value);
        }

        if (check(Lexer::TokenType::Identifier)) {
            advance();
            return AST::makeIdentifier(previous().value);
        }

        if (match("(")) {
            auto expr = expression();
            if (!match(")")) {
                throw std::runtime_error("Expected ')' after expression");
            }
            return expr;
        }

        // Better error messages
        if (isAtEnd()) {
            throw std::runtime_error("Unexpected end of input");
        }
        else {
            throw std::runtime_error("Unexpected token: '" + peek().value + "'");
        }
    }

    AST::ExpressionPtr ExpressionParser::parse() {
        auto result = expression();

        // Check if we consumed all tokens - this catches "2 3" type errors
        if (!isAtEnd()) {
            throw std::runtime_error("Unexpected token after expression: '" + peek().value + "'");
        }

        return result;
    }

} // namespace Parser