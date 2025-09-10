#include "pch.h"
#include "CppUnitTest.h"
#include "../src/Tokenizer.h"
#include "../src/ExpressionParser.h"
#include "../src/ExpressionParser.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Lexer;
using namespace Parser;
using namespace AST;

namespace ExpressionParserTests
{
    TEST_CLASS(ExpressionParserTests)
    {
    private:
        std::unique_ptr<Expression> parseExpression(const std::string& input) {
            auto tokens = tokenize(input);
            ExpressionParser parser(tokens);
            return parser.parse();
        }

    public:

        TEST_METHOD(ParseNumber)
        {
            // Integer
            auto ast = parseExpression("42");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("42"), ast->toString());

            // Float
            ast = parseExpression("3.14");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("3.14"), ast->toString());
        }

        TEST_METHOD(ParseIdentifier)
        {
            auto ast = parseExpression("myVariable");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("myVariable"), ast->toString());
        }

        TEST_METHOD(ParseBoolean)
        {
            auto ast = parseExpression("true");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("true"), ast->toString());

            ast = parseExpression("false");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("false"), ast->toString());
        }

        TEST_METHOD(ParseSimpleBinaryOperation)
        {
            auto ast = parseExpression("2 + 3");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(2 + 3)"), ast->toString());
        }

        TEST_METHOD(ParsePrecedence_MultiplicationBeforeAddition)
        {
            // 2 + 3 * 4 should parse as 2 + (3 * 4)
            auto ast = parseExpression("2 + 3 * 4");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(2 + (3 * 4))"), ast->toString());
        }

        TEST_METHOD(ParsePrecedence_DivisionBeforeSubtraction)
        {
            // 10 - 6 / 2 should parse as 10 - (6 / 2)
            auto ast = parseExpression("10 - 6 / 2");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(10 - (6 / 2))"), ast->toString());
        }

        TEST_METHOD(ParsePrecedence_LeftAssociativity)
        {
            // 8 - 3 - 2 should parse as (8 - 3) - 2
            auto ast = parseExpression("8 - 3 - 2");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("((8 - 3) - 2)"), ast->toString());

            // 12 / 4 / 3 should parse as (12 / 4) / 3
            ast = parseExpression("12 / 4 / 3");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("((12 / 4) / 3)"), ast->toString());
        }

        TEST_METHOD(ParseParentheses)
        {
            // (2 + 3) * 4 should parse as (2 + 3) * 4
            auto ast = parseExpression("(2 + 3) * 4");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("((2 + 3) * 4)"), ast->toString());
        }

        TEST_METHOD(ParseNestedParentheses)
        {
            auto ast = parseExpression("((2 + 3) * (4 - 1))");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("((2 + 3) * (4 - 1))"), ast->toString());
        }

        TEST_METHOD(ParseUnaryOperators)
        {
            // Unary minus
            auto ast = parseExpression("-5");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(- 5)"), ast->toString());

            // Unary plus
            ast = parseExpression("+42");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(+ 42)"), ast->toString());

            // Logical not
            ast = parseExpression("not true");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(not true)"), ast->toString());

            ast = parseExpression("!false");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(! false)"), ast->toString());
        }

        TEST_METHOD(ParseChainedUnaryOperators)
        {
            // --x should parse as -(-x) (traditional unary, not decrement)
            auto ast = parseExpression("- - x");  // With spaces to be clear it's two unary operators
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(- (- x))"), ast->toString());

            // not not true
            ast = parseExpression("not not true");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(not (not true))"), ast->toString());
        }

        // NEW: Increment/Decrement Tests
        TEST_METHOD(ParsePreIncrement)
        {
            auto ast = parseExpression("++x");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(++x)"), ast->toString());

            ast = parseExpression("--y");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(--y)"), ast->toString());
        }

        TEST_METHOD(ParsePostIncrement)
        {
            auto ast = parseExpression("x++");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(x++)"), ast->toString());

            ast = parseExpression("y--");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(y--)"), ast->toString());
        }

        TEST_METHOD(ParseIncrementInExpressions)
        {
            // Pre-increment in expression
            auto ast = parseExpression("++x + 5");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("((++x) + 5)"), ast->toString());

            // Post-increment in expression
            ast = parseExpression("y++ * 2");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("((y++) * 2)"), ast->toString());
        }

        TEST_METHOD(ParseComparisonOperators)
        {
            auto ast = parseExpression("x > 5");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(x > 5)"), ast->toString());

            ast = parseExpression("a <= b");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(a <= b)"), ast->toString());
        }

        TEST_METHOD(ParseEqualityOperators)
        {
            auto ast = parseExpression("x == y");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(x == y)"), ast->toString());

            ast = parseExpression("a != b");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(a != b)"), ast->toString());
        }

        TEST_METHOD(ParseLogicalOperators)
        {
            auto ast = parseExpression("x and y");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(x and y)"), ast->toString());

            ast = parseExpression("a or b");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(a or b)"), ast->toString());

            ast = parseExpression("x && y");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(x && y)"), ast->toString());

            ast = parseExpression("a || b");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("(a || b)"), ast->toString());
        }

        TEST_METHOD(ParseComplexPrecedence)
        {
            // x > 5 and y < 10 should parse as (x > 5) and (y < 10)
            auto ast = parseExpression("x > 5 and y < 10");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("((x > 5) and (y < 10))"), ast->toString());

            // not x or y and z should parse as (not x) or (y and z)
            ast = parseExpression("not x or y and z");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("((not x) or (y and z))"), ast->toString());
        }

        TEST_METHOD(ParseVeryComplexExpression)
        {
            // Test a complex expression with all precedence levels
            auto ast = parseExpression("not a + b * c > d and e or f");
            Assert::IsNotNull(ast.get());
            // Should parse as: ((((not a) + (b * c)) > d) and e) or f
            // Due to precedence: not > * > + > > > and > or
            std::string expected = "(((((not a) + (b * c)) > d) and e) or f)";
            Assert::AreEqual(expected, ast->toString());
        }

        TEST_METHOD(ParseWithVariables)
        {
            auto ast = parseExpression("x + y * z - w / v");
            Assert::IsNotNull(ast.get());
            Assert::AreEqual(std::string("((x + (y * z)) - (w / v))"), ast->toString());
        }

        // Error cases
        TEST_METHOD(ParseError_EmptyInput)
        {
            Assert::ExpectException<std::runtime_error>([this]() {
                parseExpression("");
                });
        }

        TEST_METHOD(ParseError_UnmatchedParentheses)
        {
            Assert::ExpectException<std::runtime_error>([this]() {
                parseExpression("(2 + 3");
                });

            Assert::ExpectException<std::runtime_error>([this]() {
                parseExpression("2 + 3)");
                });
        }

        TEST_METHOD(ParseError_InvalidTokens)
        {
            Assert::ExpectException<std::runtime_error>([this]() {
                parseExpression("2 +");  // Missing operand
                });

            Assert::ExpectException<std::runtime_error>([this]() {
                parseExpression("* 3");  // * cannot be unary
                });

            Assert::ExpectException<std::runtime_error>([this]() {
                parseExpression("2 3");  // Two numbers without operator
                });
        }

        TEST_METHOD(ParseError_InvalidUnaryOperator)
        {
            Assert::ExpectException<std::runtime_error>([this]() {
                parseExpression("*5");
                });
        }

        TEST_METHOD(ParseError_InvalidIncrement)
        {
            Assert::ExpectException<std::runtime_error>([this]() {
                parseExpression("++5");  // Can't increment a number
                });

            Assert::ExpectException<std::runtime_error>([this]() {
                parseExpression("5++");  // Can't post-increment a number
                });

            Assert::ExpectException<std::runtime_error>([this]() {
                parseExpression("++");  // Incomplete increment
                });
        }
    };
}