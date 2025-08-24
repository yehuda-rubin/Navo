#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Tokenizer.h"
#include <stdexcept>

using namespace Lexer;

// Test cases for the tokenizer
TEST_CASE("Basic assignment tokenization", "[tokenizer]") {
    auto tokens = tokenize("number x = 10;");
    REQUIRE(tokens.size() == 5);
    CHECK(tokens[0].type == TokenType::Keyword);
    CHECK(tokens[0].value == "number");
    CHECK(tokens[1].type == TokenType::Identifier);
    CHECK(tokens[1].value == "x");
    CHECK(tokens[2].type == TokenType::Operator);
    CHECK(tokens[2].value == "=");
    CHECK(tokens[3].type == TokenType::Number);
    CHECK(tokens[3].value == "10");
    CHECK(tokens[4].type == TokenType::Punctuation);
    CHECK(tokens[4].value == ";");
}

// Test cases for various token types
TEST_CASE("Unterminated string throws exception", "[tokenizer]") {
    REQUIRE_THROWS_AS(tokenize("\"hello"), std::runtime_error);
    REQUIRE_THROWS_WITH(tokenize("\"hello"), Catch::Contains("Unterminated string"));
}

// Test cases for block comments
TEST_CASE("Unterminated block comment throws exception", "[tokenizer]") {
    REQUIRE_THROWS_AS(tokenize("/* comment"), std::runtime_error);
    REQUIRE_THROWS_WITH(tokenize("/* comment"), Catch::Contains("Unterminated block comment"));
}

// Test cases for operators and punctuation
TEST_CASE("Tokenization with whitespace", "[tokenizer]") {
    auto tokens = tokenize("  if (x > 10) { return x; }  ");
    REQUIRE(tokens.size() == 11);
    CHECK(tokens[0].type == TokenType::Keyword);
    CHECK(tokens[0].value == "if");
    CHECK(tokens[1].type == TokenType::Punctuation);
    CHECK(tokens[1].value == "(");
    CHECK(tokens[2].type == TokenType::Identifier);
    CHECK(tokens[2].value == "x");
    CHECK(tokens[3].type == TokenType::Operator);
    CHECK(tokens[3].value == ">");
    CHECK(tokens[4].type == TokenType::Number);
    CHECK(tokens[4].value == "10");
    CHECK(tokens[5].type == TokenType::Punctuation);
    CHECK(tokens[5].value == ")");
    CHECK(tokens[6].type == TokenType::Punctuation);
    CHECK(tokens[6].value == "{");
}

// Test cases for keywords and identifiers
TEST_CASE("Multiple keywords and identifiers", "[tokenizer]") {
    auto tokens = tokenize("function myFunc() { return number; }");
    REQUIRE(tokens.size() == 9);
    CHECK(tokens[0].type == TokenType::Keyword);
    CHECK(tokens[0].value == "function");
    CHECK(tokens[1].type == TokenType::Identifier);
    CHECK(tokens[1].value == "myFunc");
    CHECK(tokens[2].type == TokenType::Punctuation);
    CHECK(tokens[2].value == "(");
    CHECK(tokens[3].type == TokenType::Punctuation);
    CHECK(tokens[3].value == ")");
    CHECK(tokens[4].type == TokenType::Punctuation);
    CHECK(tokens[4].value == "{");
    CHECK(tokens[5].type == TokenType::Keyword);
    CHECK(tokens[5].value == "return");
    CHECK(tokens[6].type == TokenType::Keyword);
    CHECK(tokens[6].value == "number");
}

// Test cases for operators and punctuation
TEST_CASE("Single keyword", "[tokenizer]") {
    auto tokens = tokenize("while");
    REQUIRE(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::Keyword);
    CHECK(tokens[0].value == "while");
}

// Test cases for identifiers
TEST_CASE("Empty input returns empty vector", "[tokenizer]") {
    auto tokens = tokenize("");
    REQUIRE(tokens.size() == 0);
}

// Test cases for whitespace handling
TEST_CASE("Whitespace only input returns empty vector", "[tokenizer]") {
    auto tokens = tokenize("   \t\n  ");
    REQUIRE(tokens.size() == 0);
}