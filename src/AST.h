#pragma once
#include <memory>
#include <string>
#include <iostream>

namespace AST {

    // Forward declarations
    class Expression;
    using ExpressionPtr = std::unique_ptr<Expression>;

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

    // Helper functions to create AST nodes
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

} // namespace AST