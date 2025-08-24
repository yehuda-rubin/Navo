#include "Tokenizer.h"
#include <iostream>
#include <string>

using namespace Lexer;

void printWelcome() {
    std::cout << "=====================================" << std::endl;
    std::cout << "    Navo Tokenizer v1.0" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "Interactive tokenizer for programming languages" << std::endl;
    std::cout << "Enter code to tokenize, or 'help' for examples" << std::endl;
    std::cout << "Type 'quit' or 'exit' to quit" << std::endl;
    std::cout << "=====================================" << std::endl;
}

void printHelp() {
    std::cout << "\nExample inputs to try:" << std::endl;
    std::cout << "  number x = 42;" << std::endl;
    std::cout << "  if (x > 10) { return x; }" << std::endl;
    std::cout << "  function hello() { }" << std::endl;
    std::cout << "  \"Hello World\"" << std::endl;
    std::cout << "  // This is a comment" << std::endl;
    std::cout << "  /* Block comment */" << std::endl;
    std::cout << "  x == y && z != 5" << std::endl;
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

int main() {
    printWelcome();

    std::string input;
    while (true) {
        std::cout << "\n> ";

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
            printHelp();
            continue;
        }

        if (input == "clear" || input == "cls") {
            system("cls");  // Windows
            system("clear"); // Unix/Linux (one will work, other will be ignored)
            printWelcome();
            continue;
        }

        if (input.empty()) {
            continue;
        }

        // Tokenize the input
        try {
            auto tokens = tokenize(input);
            printTokens(tokens);
        }
        catch (const std::exception& e) {
            std::cout << "❌ Error: " << e.what() << std::endl;
        }
    }

    return 0;
}