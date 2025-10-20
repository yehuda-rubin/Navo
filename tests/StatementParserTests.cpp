#include "pch.h"
#include "CppUnitTest.h"
#include "../src/Tokenizer.h"
#include "../src/StatementParser.h"
#include "../src/StatementParser.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Lexer;
using namespace Parser;
using namespace AST;

namespace StatementParserTests
{
    TEST_CLASS(StatementParserTests)
    {
    private:
        std::unique_ptr<Statement> parseStatement(const std::string& input) {
            auto tokens = tokenize(input);
            StatementParser parser(tokens);
            return parser.parse();
        }

    public:

        TEST_METHOD(ParseVariableDeclarationWithoutInitializer)
        {
            auto stmt = parseStatement("number x;");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("number x;"), stmt->toString());
        }

        TEST_METHOD(ParseVariableDeclarationWithInitializer)
        {
            auto stmt = parseStatement("number x = 42;");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("number x = 42;"), stmt->toString());

            stmt = parseStatement("word name = \"hello\";");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("word name = \"hello\";"), stmt->toString());

            stmt = parseStatement("boolean flag = true;");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("boolean flag = true;"), stmt->toString());
        }

        TEST_METHOD(ParseVariableDeclarationWithExpression)
        {
            auto stmt = parseStatement("number result = x + y * 2;");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("number result = (x + (y * 2));"), stmt->toString());
        }

        TEST_METHOD(ParseAssignmentStatement)
        {
            auto stmt = parseStatement("x = 10;");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("x = 10;"), stmt->toString());

            stmt = parseStatement("result = a + b;");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("result = (a + b);"), stmt->toString());
        }

        TEST_METHOD(ParseExpressionStatement)
        {
            auto stmt = parseStatement("x++;");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("(x++);"), stmt->toString());

            stmt = parseStatement("++y;");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("(++y);"), stmt->toString());
        }

        TEST_METHOD(ParseSimpleIfStatement)
        {
            auto stmt = parseStatement("if (x > 5) x = 10;");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("if ((x > 5)) x = 10;"), stmt->toString());
        }

        TEST_METHOD(ParseIfElseStatement)
        {
            auto stmt = parseStatement("if (x > 5) x = 10; else x = 0;");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("if ((x > 5)) x = 10; else x = 0;"), stmt->toString());
        }

        TEST_METHOD(ParseIfWithBlock)
        {
            auto stmt = parseStatement("if (x > 5) { x = 10; y = 20; }");
            Assert::IsNotNull(stmt.get());

            // Check that it contains both assignments
            std::string result = stmt->toString();
            Assert::IsTrue(result.find("x = 10") != std::string::npos);
            Assert::IsTrue(result.find("y = 20") != std::string::npos);
        }

        TEST_METHOD(ParseEmptyBlock)
        {
            auto stmt = parseStatement("{}");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("{\n}"), stmt->toString());
        }

        TEST_METHOD(ParseBlockWithStatements)
        {
            auto stmt = parseStatement("{ number x = 5; x++; }");
            Assert::IsNotNull(stmt.get());

            std::string result = stmt->toString();
            Assert::IsTrue(result.find("number x = 5") != std::string::npos);
            Assert::IsTrue(result.find("(x++)") != std::string::npos);
        }

        TEST_METHOD(ParseNestedIfStatement)
        {
            auto stmt = parseStatement("if (x > 5) if (y > 10) z = 1;");
            Assert::IsNotNull(stmt.get());

            std::string result = stmt->toString();
            Assert::IsTrue(result.find("if ((x > 5))") != std::string::npos);
            Assert::IsTrue(result.find("if ((y > 10))") != std::string::npos);
        }

        TEST_METHOD(ParseComplexExpression)
        {
            auto stmt = parseStatement("result = x + y * z > 10 && flag;");
            Assert::IsNotNull(stmt.get());

            std::string result = stmt->toString();
            Assert::IsTrue(result.find("result = ") != std::string::npos);
            // Should have proper precedence
            Assert::IsTrue(result.find("(y * z)") != std::string::npos);
        }

        // Error cases
        TEST_METHOD(ParseError_MissingSemicolon)
        {
            Assert::ExpectException<std::runtime_error>([this]() {
                parseStatement("number x = 5");
                });

            Assert::ExpectException<std::runtime_error>([this]() {
                parseStatement("x = 10");
                });
        }

        TEST_METHOD(ParseError_InvalidVariableDeclaration)
        {
            Assert::ExpectException<std::runtime_error>([this]() {
                parseStatement("number;"); // Missing variable name
                });

            Assert::ExpectException<std::runtime_error>([this]() {
                parseStatement("invalidtype x = 5;"); // Invalid type
                });
        }

        TEST_METHOD(ParseError_UnmatchedBraces)
        {
            Assert::ExpectException<std::runtime_error>([this]() {
                parseStatement("{ x = 5;"); // Missing closing brace
                });
        }

        TEST_METHOD(ParseError_InvalidIfStatement)
        {
            Assert::ExpectException<std::runtime_error>([this]() {
                parseStatement("if x > 5 x = 10;"); // Missing parentheses
                });

            Assert::ExpectException<std::runtime_error>([this]() {
                parseStatement("if (x > 5 x = 10;"); // Missing closing parenthesis
                });
        }

        TEST_METHOD(ParseError_InvalidAssignment)
        {
            Assert::ExpectException<std::runtime_error>([this]() {
                parseStatement("5 = x;"); // Can't assign to literal
                });
        }

        TEST_METHOD(ParseMultipleVariableTypes)
        {
            auto stmt = parseStatement("word message = \"Hello World\";");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("word message = \"Hello World\";"), stmt->toString());

            stmt = parseStatement("boolean isValid = false;");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("boolean isValid = false;"), stmt->toString());
        }

        TEST_METHOD(ParseComplexIfElseWithBlocks)
        {
            auto stmt = parseStatement("if (x > y) { x = x + 1; } else { y = y - 1; }");
            Assert::IsNotNull(stmt.get());

            std::string result = stmt->toString();
            Assert::IsTrue(result.find("if ((x > y))") != std::string::npos);
            Assert::IsTrue(result.find("else") != std::string::npos);
        }
    };
}