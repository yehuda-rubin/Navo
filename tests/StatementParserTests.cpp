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
        // Helper for single-statement tests
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
            Assert::IsTrue(result.find("(y * z)") != std::string::npos);
        }

        TEST_METHOD(ParseWhileStatement)
        {
            auto stmt = parseStatement("while (x < 10) x++;");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("while ((x < 10)) (x++);"), stmt->toString());
        }

        TEST_METHOD(ParseWhileWithBlock)
        {
            auto stmt = parseStatement("while (true) { x = 1; y = 2; }");
            Assert::IsNotNull(stmt.get());

            std::string result = stmt->toString();
            Assert::IsTrue(result.find("while (true)") != std::string::npos);
            Assert::IsTrue(result.find("x = 1") != std::string::npos);
            Assert::IsTrue(result.find("y = 2") != std::string::npos);
        }

        TEST_METHOD(ParseForStatementFull)
        {
            auto stmt = parseStatement("for (number i = 0; i < 10; i++) x = x + i;");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("for (number i = 0; (i < 10); (i++)) x = (x + i);"), stmt->toString());
        }

        TEST_METHOD(ParseForStatementEmpty)
        {
            auto stmt = parseStatement("for (;;) { x++; }");
            Assert::IsNotNull(stmt.get());

            std::string result = stmt->toString();
            Assert::IsTrue(result.find("for (;") != std::string::npos);
            Assert::IsTrue(result.find("(x++)") != std::string::npos);
            Assert::AreEqual(std::string("for (; ; ) {\n  (x++);\n}"), result);
        }

        TEST_METHOD(ParseForStatementNoInit)
        {
            auto stmt = parseStatement("for (; i < 10; i++) x = 1;");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("for (; (i < 10); (i++)) x = 1;"), stmt->toString());
        }

        TEST_METHOD(ParseForStatementNoCond)
        {
            auto stmt = parseStatement("for (number i = 0; ; i++) x = 1;");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("for (number i = 0; ; (i++)) x = 1;"), stmt->toString());
        }

        TEST_METHOD(ParseForStatementNoIncrement)
        {
            auto stmt = parseStatement("for (number i = 0; i < 10; ) x = 1;");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("for (number i = 0; (i < 10); ) x = 1;"), stmt->toString());
        }

        TEST_METHOD(ParseForStatementExprInitializer)
        {
            auto stmt = parseStatement("for (i = 0; i < 10; i++) x = 1;");
            Assert::IsNotNull(stmt.get());
            Assert::AreEqual(std::string("for (i = 0; (i < 10); (i++)) x = 1;"), stmt->toString());
        }

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