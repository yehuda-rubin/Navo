#include "pch.h"
#include "CppUnitTest.h"
#include "../src/Tokenizer.h"
#include "../src/Tokenizer.cpp"
#include <string>
#include <vector>
#include <stdexcept>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Lexer;

namespace TokenizerTests
{
    TEST_CLASS(TokenizerTests)
    {
    public:

        TEST_METHOD(BasicAssignmentTokenization)
        {
            // Arrange
            std::string input = "number x = 10;";

            // Act
            auto tokens = tokenize(input);

            // Assert
            Assert::AreEqual(size_t(5), tokens.size());
            Assert::AreEqual(static_cast<int>(TokenType::Keyword), static_cast<int>(tokens[0].type));
            Assert::AreEqual(std::string("number"), tokens[0].value);
            Assert::AreEqual(static_cast<int>(TokenType::Identifier), static_cast<int>(tokens[1].type));
            Assert::AreEqual(std::string("x"), tokens[1].value);
            Assert::AreEqual(static_cast<int>(TokenType::Operator), static_cast<int>(tokens[2].type));
            Assert::AreEqual(std::string("="), tokens[2].value);
            Assert::AreEqual(static_cast<int>(TokenType::Number), static_cast<int>(tokens[3].type));
            Assert::AreEqual(std::string("10"), tokens[3].value);
            Assert::AreEqual(static_cast<int>(TokenType::Punctuation), static_cast<int>(tokens[4].type));
            Assert::AreEqual(std::string(";"), tokens[4].value);
        }

        TEST_METHOD(UnterminatedStringThrowsException)
        {
            // Arrange & Act & Assert
            Assert::ExpectException<std::runtime_error>([]() {
                tokenize("\"hello");
                });
        }

        TEST_METHOD(UnterminatedBlockCommentThrowsException)
        {
            // Arrange & Act & Assert
            Assert::ExpectException<std::runtime_error>([]() {
                tokenize("/* comment");
                });
        }

        TEST_METHOD(TokenizationWithWhitespace)
        {
            // Arrange
            std::string input = "  if (x > 10) { return x; }  ";

            // Act
            auto tokens = tokenize(input);

            // Assert
            Assert::AreEqual(size_t(11), tokens.size());
            Assert::AreEqual(static_cast<int>(TokenType::Keyword), static_cast<int>(tokens[0].type));
            Assert::AreEqual(std::string("if"), tokens[0].value);
            Assert::AreEqual(static_cast<int>(TokenType::Punctuation), static_cast<int>(tokens[1].type));
            Assert::AreEqual(std::string("("), tokens[1].value);
            Assert::AreEqual(static_cast<int>(TokenType::Identifier), static_cast<int>(tokens[2].type));
            Assert::AreEqual(std::string("x"), tokens[2].value);
            Assert::AreEqual(static_cast<int>(TokenType::Operator), static_cast<int>(tokens[3].type));
            Assert::AreEqual(std::string(">"), tokens[3].value);
            Assert::AreEqual(static_cast<int>(TokenType::Number), static_cast<int>(tokens[4].type));
            Assert::AreEqual(std::string("10"), tokens[4].value);
            Assert::AreEqual(static_cast<int>(TokenType::Punctuation), static_cast<int>(tokens[5].type));
            Assert::AreEqual(std::string(")"), tokens[5].value);
            Assert::AreEqual(static_cast<int>(TokenType::Punctuation), static_cast<int>(tokens[6].type));
            Assert::AreEqual(std::string("{"), tokens[6].value);
        }

        TEST_METHOD(MultipleKeywordsAndIdentifiers)
        {
            // Arrange
            std::string input = "function myFunc() { return number; }";

            // Act
            auto tokens = tokenize(input);

            // Assert
            Assert::AreEqual(size_t(9), tokens.size());
            Assert::AreEqual(static_cast<int>(TokenType::Keyword), static_cast<int>(tokens[0].type));
            Assert::AreEqual(std::string("function"), tokens[0].value);
            Assert::AreEqual(static_cast<int>(TokenType::Identifier), static_cast<int>(tokens[1].type));
            Assert::AreEqual(std::string("myFunc"), tokens[1].value);
            Assert::AreEqual(static_cast<int>(TokenType::Punctuation), static_cast<int>(tokens[2].type));
            Assert::AreEqual(std::string("("), tokens[2].value);
            Assert::AreEqual(static_cast<int>(TokenType::Punctuation), static_cast<int>(tokens[3].type));
            Assert::AreEqual(std::string(")"), tokens[3].value);
            Assert::AreEqual(static_cast<int>(TokenType::Punctuation), static_cast<int>(tokens[4].type));
            Assert::AreEqual(std::string("{"), tokens[4].value);
            Assert::AreEqual(static_cast<int>(TokenType::Keyword), static_cast<int>(tokens[5].type));
            Assert::AreEqual(std::string("return"), tokens[5].value);
            Assert::AreEqual(static_cast<int>(TokenType::Keyword), static_cast<int>(tokens[6].type));
            Assert::AreEqual(std::string("number"), tokens[6].value);
        }

        TEST_METHOD(SingleKeyword)
        {
            // Arrange
            std::string input = "while";

            // Act
            auto tokens = tokenize(input);

            // Assert
            Assert::AreEqual(size_t(1), tokens.size());
            Assert::AreEqual(static_cast<int>(TokenType::Keyword), static_cast<int>(tokens[0].type));
            Assert::AreEqual(std::string("while"), tokens[0].value);
        }

        TEST_METHOD(EmptyInputReturnsEmptyVector)
        {
            // Arrange
            std::string input = "";

            // Act
            auto tokens = tokenize(input);

            // Assert
            Assert::AreEqual(size_t(0), tokens.size());
        }

        TEST_METHOD(WhitespaceOnlyInputReturnsEmptyVector)
        {
            // Arrange
            std::string input = "   \t\n  ";

            // Act
            auto tokens = tokenize(input);

            // Assert
            Assert::AreEqual(size_t(0), tokens.size());
        }

        TEST_METHOD(MixedTokenTypes)
        {
            // Arrange
            std::string input = "x + 5";

            // Act
            auto tokens = tokenize(input);

            // Assert
            Assert::AreEqual(size_t(3), tokens.size());
            Assert::AreEqual(static_cast<int>(TokenType::Identifier), static_cast<int>(tokens[0].type));
            Assert::AreEqual(static_cast<int>(TokenType::Operator), static_cast<int>(tokens[1].type));
            Assert::AreEqual(static_cast<int>(TokenType::Number), static_cast<int>(tokens[2].type));
        }

        TEST_METHOD(KeywordRecognition_If)
        {
            // Arrange
            std::string input = "if";

            // Act
            auto tokens = tokenize(input);

            // Assert
            Assert::AreEqual(size_t(1), tokens.size());
            Assert::AreEqual(static_cast<int>(TokenType::Keyword), static_cast<int>(tokens[0].type));
            Assert::AreEqual(std::string("if"), tokens[0].value);
        }

        TEST_METHOD(KeywordRecognition_Function)
        {
            // Arrange
            std::string input = "function";

            // Act
            auto tokens = tokenize(input);

            // Assert
            Assert::AreEqual(size_t(1), tokens.size());
            Assert::AreEqual(static_cast<int>(TokenType::Keyword), static_cast<int>(tokens[0].type));
            Assert::AreEqual(std::string("function"), tokens[0].value);
        }

        TEST_METHOD(IdentifierRecognition)
        {
            // Arrange
            std::string input = "myVariable";

            // Act
            auto tokens = tokenize(input);

            // Assert
            Assert::AreEqual(size_t(1), tokens.size());
            Assert::AreEqual(static_cast<int>(TokenType::Identifier), static_cast<int>(tokens[0].type));
            Assert::AreEqual(std::string("myVariable"), tokens[0].value);
        }

        TEST_METHOD(NumberRecognition)
        {
            // Arrange
            std::string input = "123";

            // Act
            auto tokens = tokenize(input);

            // Assert
            Assert::AreEqual(size_t(1), tokens.size());
            Assert::AreEqual(static_cast<int>(TokenType::Number), static_cast<int>(tokens[0].type));
            Assert::AreEqual(std::string("123"), tokens[0].value);
        }
    };
}