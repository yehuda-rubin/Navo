#pragma once
#include <memory>
#include <string>
#include <iostream>
#include <vector>

namespace AST {

    // Forward declarations
    class Expression;
    class Statement;
    using ExpressionPtr = std::unique_ptr<Expression>;
    using StatementPtr = std::unique_ptr<Statement>;

    // Base class for all AST nodes
    class ASTNode {
    public:
        virtual ~ASTNode() = default;
        virtual std::string toString() const = 0;
    };

    // Base class for expressions
    class Expression : public ASTNode {
    public:
        virtual ~Expression() = default;
    };

    // Base class for statements  
    class Statement : public ASTNode {
    public:
        virtual ~Statement() = default;
    };

    // Literal number (integers and floats)
    class NumberLiteral : public Expression {
    public:
        std::string value;

        explicit NumberLiteral(const std::string& val) : value(val) {}

        std::string toString() const override {
            return value;
        }
    };

    // Variable identifier  
    class Identifier : public Expression {
    public:
        std::string name;

        explicit Identifier(const std::string& n) : name(n) {}

        std::string toString() const override {
            return name;
        }
    };

    // Boolean literal
    class BooleanLiteral : public Expression {
    public:
        bool value;

        explicit BooleanLiteral(bool val) : value(val) {}

        std::string toString() const override {
            return value ? "true" : "false";
        }
    };

    // String literal
    class StringLiteral : public Expression {
    public:
        std::string value;

        explicit StringLiteral(const std::string& val) : value(val) {}

        std::string toString() const override {
            return value; // includes the quotes
        }
    };

    // Binary operation (left operator right)
    class BinaryOperation : public Expression {
    public:
        ExpressionPtr left;
        std::string operator_;
        ExpressionPtr right;

        BinaryOperation(ExpressionPtr l, const std::string& op, ExpressionPtr r)
            : left(std::move(l)), operator_(op), right(std::move(r)) {
        }

        std::string toString() const override {
            return "(" + left->toString() + " " + operator_ + " " + right->toString() + ")";
        }
    };

    // Unary operation (operator operand)
    class UnaryOperation : public Expression {
    public:
        std::string operator_;
        ExpressionPtr operand;

        UnaryOperation(const std::string& op, ExpressionPtr expr)
            : operator_(op), operand(std::move(expr)) {
        }

        std::string toString() const override {
            return "(" + operator_ + " " + operand->toString() + ")";
        }
    };

    // Pre-increment/decrement (++x, --x)
    class PreIncrementOperation : public Expression {
    public:
        std::string operator_;  // "++" ае "--"
        std::string variable;

        PreIncrementOperation(const std::string& op, const std::string& var)
            : operator_(op), variable(var) {
        }

        std::string toString() const override {
            return "(" + operator_ + variable + ")";
        }
    };

    // Post-increment/decrement (x++, x--)
    class PostIncrementOperation : public Expression {
    public:
        std::string variable;
        std::string operator_;  // "++" ае "--"

        PostIncrementOperation(const std::string& var, const std::string& op)
            : variable(var), operator_(op) {
        }

        std::string toString() const override {
            return "(" + variable + operator_ + ")";
        }
    };

    // === STATEMENT CLASSES ===

    // Variable declaration: number x = 5;
    class VariableDeclaration : public Statement {
    public:
        std::string type;      // "number", "word", "boolean"
        std::string name;      // variable name
        ExpressionPtr initializer; // optional initial value

        VariableDeclaration(const std::string& t, const std::string& n, ExpressionPtr init = nullptr)
            : type(t), name(n), initializer(std::move(init)) {
        }

        std::string toString() const override {
            std::string result = type + " " + name;
            if (initializer) {
                result += " = " + initializer->toString();
            }
            return result + ";";
        }
    };

    // Assignment: x = 5;
    class AssignmentStatement : public Statement {
    public:
        std::string variable;
        ExpressionPtr value;

        AssignmentStatement(const std::string& var, ExpressionPtr val)
            : variable(var), value(std::move(val)) {
        }

        std::string toString() const override {
            return variable + " = " + value->toString() + ";";
        }
    };

    // Expression statement: x++; or functionCall();
    class ExpressionStatement : public Statement {
    public:
        ExpressionPtr expression;

        explicit ExpressionStatement(ExpressionPtr expr)
            : expression(std::move(expr)) {
        }

        std::string toString() const override {
            return expression->toString() + ";";
        }
    };

    // Block: { statement1; statement2; }
    class Block : public Statement {
    public:
        std::vector<StatementPtr> statements;

        explicit Block(std::vector<StatementPtr> stmts = {})
            : statements(std::move(stmts)) {
        }

        void addStatement(StatementPtr stmt) {
            statements.push_back(std::move(stmt));
        }

        std::string toString() const override {
            std::string result = "{\n";
            for (const auto& stmt : statements) {
                result += "  " + stmt->toString() + "\n";
            }
            result += "}";
            return result;
        }
    };

    // If statement: if (condition) thenBlock else elseBlock
    class IfStatement : public Statement {
    public:
        ExpressionPtr condition;
        StatementPtr thenStatement;
        StatementPtr elseStatement; // optional

        IfStatement(ExpressionPtr cond, StatementPtr thenStmt, StatementPtr elseStmt = nullptr)
            : condition(std::move(cond)), thenStatement(std::move(thenStmt)), elseStatement(std::move(elseStmt)) {
        }

        std::string toString() const override {
            std::string result = "if (" + condition->toString() + ") " + thenStatement->toString();
            if (elseStatement) {
                result += " else " + elseStatement->toString();
            }
            return result;
        }
    };

    // Helper functions to create AST nodes (existing)
    inline ExpressionPtr makeNumber(const std::string& value) {
        return std::make_unique<NumberLiteral>(value);
    }

    inline ExpressionPtr makeIdentifier(const std::string& name) {
        return std::make_unique<Identifier>(name);
    }

    inline ExpressionPtr makeBoolean(bool value) {
        return std::make_unique<BooleanLiteral>(value);
    }

    inline ExpressionPtr makeBinary(ExpressionPtr left, const std::string& op, ExpressionPtr right) {
        return std::make_unique<BinaryOperation>(std::move(left), op, std::move(right));
    }

    inline ExpressionPtr makeUnary(const std::string& op, ExpressionPtr operand) {
        return std::make_unique<UnaryOperation>(op, std::move(operand));
    }

    inline ExpressionPtr makePreIncrement(const std::string& op, const std::string& var) {
        return std::make_unique<PreIncrementOperation>(op, var);
    }

    inline ExpressionPtr makePostIncrement(const std::string& var, const std::string& op) {
        return std::make_unique<PostIncrementOperation>(var, op);
    }

    inline ExpressionPtr makeString(const std::string& value) {
        return std::make_unique<StringLiteral>(value);
    }

    // Helper functions for statements (NEW)
    inline StatementPtr makeVariableDeclaration(const std::string& type, const std::string& name, ExpressionPtr init = nullptr) {
        return std::make_unique<VariableDeclaration>(type, name, std::move(init));
    }

    inline StatementPtr makeAssignment(const std::string& var, ExpressionPtr value) {
        return std::make_unique<AssignmentStatement>(var, std::move(value));
    }

    inline StatementPtr makeExpressionStatement(ExpressionPtr expr) {
        return std::make_unique<ExpressionStatement>(std::move(expr));
    }

    inline StatementPtr makeBlock(std::vector<StatementPtr> statements = {}) {
        return std::make_unique<Block>(std::move(statements));
    }

    inline StatementPtr makeIf(ExpressionPtr condition, StatementPtr thenStmt, StatementPtr elseStmt = nullptr) {
        return std::make_unique<IfStatement>(std::move(condition), std::move(thenStmt), std::move(elseStmt));
    }

} // namespace AST