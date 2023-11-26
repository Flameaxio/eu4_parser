#ifndef EU4_PARSER_H
#define EU4_PARSER_H

#include "value_type.hpp"
#include <string>
#include <unordered_map>

namespace Parsers
{
    class EU4Parser
    {
    private:
        std::string path;
        std::ifstream *current_file;
        std::string current_line;
        ValueType *current_value = new ValueType();

    void extract_key_value(std::string line, std::string &key, std::string &value);
    void remove_quotes(std::string &str);
    void process_file(std::unordered_map<std::string, ValueType*> *result); 
    void process_line(std::string line, std::unordered_map<std::string, ValueType *> *result);
    ValueType process_value(std::string value);
    ValueType* parse_str_value(std::string value);
    std::string get_next_line(std::ifstream &file);

    public:
        EU4Parser(std::string path);
        ~EU4Parser();
        std::unordered_map<std::string, ValueType*> *process();
    };
}

#endif // EU4_PARSER_H