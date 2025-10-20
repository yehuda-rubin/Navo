#include "Tokenizer.h"
#include "ExpressionParser.h"
#include "StatementParser.h"
#include <iostream>
#include <string>

using namespace Lexer;

void printWelcome() {
    std::cout << "=====================================" << std::endl;
    std::cout << "    Navo Compiler v2.0" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "Interactive parser for programming languages" << std::endl;
    std::cout << "Enter code to parse, or 'help' for examples" << std::endl;
    std::cout << "Type 'mode expr' or 'mode stmt' to switch modes" << std::endl;
    std::cout << "Type 'quit' or 'exit' to quit" << std::endl;
    std::cout << "=====================================" << std::endl;
}

void printHelp(bool statementMode) {
    if (statementMode) {
        std::cout << "\nStatement mode examples:" << std::endl;
        std::cout << "  number x = 42;" << std::endl;
        std::cout << "  x = 100;" << std::endl;
        std::cout << "  if (x > 10) { x = 5; }" << std::endl;
        std::cout << "  if (x > 10) x = 5; else x = 0;" << std::endl;
        std::cout << "  { number y = 20; y++; }" << std::endl;
        std::cout << "  word name = \"hello\";" << std::endl;
        std::cout << "  boolean flag = true;" << std::endl;
    }
    else {
        std::cout << "\nExpression mode examples:" << std::endl;
        std::cout << "  2 + 3 * 4" << std::endl;
        std::cout << "  x > 10 && y < 20" << std::endl;
        std::cout << "  ++x || y--" << std::endl;
        std::cout << "  not (a == b)" << std::endl;
        std::cout << "  (x + y) * (z - w)" << std::endl;
    }
}

void printTokens(const std::vector<Token>& tokens) {
    if (tokens.empty()) {
        std::cout << "No tokens found." << std::endl;
        return;
    }

    std::cout << "\nFound " << tokens.size() << " tokens:" << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "[" << (i + 1) << "] "
            << tokenTypeToString(tokens[i].type)
            << ": \"" << tokens[i].value << "\"" << std::endl;
    }
    std::cout << "-----------------------------------" << std::endl;
}

bool isLikelyStatement(const std::string& input) {
    // Simple heuristic to detect if input looks like a statement
    return input.find(';') != std::string::npos ||
        input.find('{') != std::string::npos ||
        input.find("number ") != std::string::npos ||
        input.find("word ") != std::string::npos ||
        input.find("boolean ") != std::string::npos ||
        input.find("if ") != std::string::npos;
}

int main() {
    printWelcome();

    std::string input;
    bool statementMode = false; // Default to expression mode

    while (true) {
        std::string modeIndicator = statementMode ? "[STMT]" : "[EXPR]";
        std::cout << "\n" << modeIndicator << " > ";

        if (!std::getline(std::cin, input)) {
            // Handle EOF (Ctrl+Z on Windows, Ctrl+D on Unix)
            std::cout << "\nGoodbye!" << std::endl;
            break;
        }

        // Trim whitespace from input
        size_t start = input.find_first_not_of(" \t");
        size_t end = input.find_last_not_of(" \t");

        if (start == std::string::npos) {
            input = "";
        }
        else {
            input = input.substr(start, end - start + 1);
        }

        // Handle commands
        if (input == "quit" || input == "exit" || input == "q") {
            std::cout << "Goodbye!" << std::endl;
            break;
        }

        if (input == "help" || input == "h" || input == "?") {
            printHelp(statementMode);
            continue;
        }

        if (input == "mode expr") {
            statementMode = false;
            std::cout << "Switched to Expression mode" << std::endl;
            continue;
        }

        if (input == "mode stmt") {
            statementMode = true;
            std::cout << "Switched to Statement mode" << std::endl;
            continue;
        }

        if (input == "clear" || input == "cls") {
            system("cls");  // Windows
            system("clear"); // Unix/Linux (one will work, other will be ignored)
            printWelcome();
            continue;
        }

        if (input == "tokens") {
            std::cout << "Token-only mode - enter code to see tokens:" << std::endl;
            std::cout << "> ";
            if (std::getline(std::cin, input)) {
                try {
                    auto tokens = tokenize(input);
                    printTokens(tokens);
                }
                catch (const std::exception& e) {
                    std::cout << "❌ Tokenization Error: " << e.what() << std::endl;
                }
            }
            continue;
        }

        if (input.empty()) {
            continue;
        }

        // Auto-detect mode if not explicitly set and input suggests a statement
        bool useStatementMode = statementMode;
        if (!statementMode && isLikelyStatement(input)) {
            useStatementMode = true;
            std::cout << "Auto-detected statement mode for this input." << std::endl;
        }

        // Tokenize and parse the input
        try {
            auto tokens = tokenize(input);

            if (useStatementMode) {
                // Parse as statement
                Parser::StatementParser statementParser(tokens);
                auto ast = statementParser.parse();

                std::cout << "✅ Statement parsed successfully:" << std::endl;
                std::cout << "AST: " << ast->toString() << std::endl;
            }
            else {
                // Parse as expression
                Parser::ExpressionParser expressionParser(tokens);
                auto ast = expressionParser.parse();

                std::cout << "✅ Expression parsed successfully:" << std::endl;
                std::cout << "AST: " << ast->toString() << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cout << "❌ Parse Error: " << e.what() << std::endl;
            std::cout << "💡 Try switching modes with 'mode expr' or 'mode stmt'" << std::endl;
        }
    }

    return 0;
}