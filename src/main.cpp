#include <iostream>
#include <fstream>
#include "parsers/eu4_parser.hpp"
#include <chrono>

int main()
{
    Parsers::EU4Parser parser("test copy.eu4");
    auto start = std::chrono::high_resolution_clock::now();
    auto result = parser.process();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Time taken by parser: " << duration.count() << " milliseconds" << std::endl;
    std::ofstream out("test.json");
    out << "{\n";
    for (auto it = result->begin(); it != result->end();)
    {
        out << "\"" << it->first << "\""
            << ": " << it->second->to_json();
        if (++it != result->end())
        {
            out << ", \n";
        }
    }
    out << "\n}";

    std::cout << "Root keys:" << std::endl;
    for (auto &pair : *result)
    {
        std::cout << pair.first << std::endl;
    }

    std::string input;
    std::cout << "Enter key to search for: ";
    std::cin >> input;
    while (input != "exit")
    {
        auto output = result->find(input);
        if (output != result->end())
        {
            std::cout << output->second->to_json() << std::endl;
        }
        else
        {
            std::cout << "Key not found" << std::endl;
        }
        std::cout << "Enter key to search for or type exit to exit: ";
        std::cin >> input;
    }
    return 0;
}