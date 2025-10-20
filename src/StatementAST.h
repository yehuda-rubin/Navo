#pragma once
#include <vector>
#include "AST.h"

namespace AST
{
    // Forward declarations for statements
    class Statement;
    using StatementPtr = std::unique_ptr<Statement>;

    // Base class for statements
    class Statement : public ASTNode {
    public:
        virtual ~Statement() = default;
    };

    // Variable declaration: number x = 5;
    class VariableDeclaration : public Statement {
	public:
        std::string type;        // "number", "word", "boolean"
        std::string name;        // variable name
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

    // Assignment statement: x = expression;
    class AssignmentStatement : public Statement {
    public:
        std::string variable;    // variable name
        ExpressionPtr value;     // expression to assign

        AssignmentStatement(const std::string& var, ExpressionPtr val)
            : variable(var), value(std::move(val)) {
        }

        std::string toString() const override {
            return variable + " = " + value->toString() + ";";
        }
    };

    // Expression statement: expression;
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

    // Block statement: { statement1; statement2; }
    class BlockStatement : public Statement {
    public:
        std::vector<StatementPtr> statements;

        BlockStatement() = default;

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

    // Program: collection of statements
    class Program : public ASTNode {
    public:
        std::vector<StatementPtr> statements;

        Program() = default;

        void addStatement(StatementPtr stmt) {
            statements.push_back(std::move(stmt));
        }

        std::string toString() const override {
            std::string result = "Program:\n";
            for (const auto& stmt : statements) {
                result += stmt->toString() + "\n";
            }
            return result;
        }
    };

	// Helper functions to create AST nodes
    inline StatementPtr makeVarDecl(const std::string& type, const std::string& name, ExpressionPtr init = nullptr) {
        return std::make_unique<VariableDeclaration>(type, name, std::move(init));
    }

    inline StatementPtr makeAssignment(const std::string& var, ExpressionPtr value) {
        return std::make_unique<AssignmentStatement>(var, std::move(value));
    }

    inline StatementPtr makeExpressionStmt(ExpressionPtr expr) {
        return std::make_unique<ExpressionStatement>(std::move(expr));
    }

    inline StatementPtr makeBlock() {
        return std::make_unique<BlockStatement>();
    }

    inline std::unique_ptr<Program> makeProgram() {
        return std::make_unique<Program>();
    }
}