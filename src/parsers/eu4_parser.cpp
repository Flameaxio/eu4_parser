#include "eu4_parser.hpp"
#include "../utils/numbers.hpp"
#include "../utils/strings.hpp"
#include <iostream>
#include <fstream>
#include <functional>
#include <unordered_map>
#include <algorithm>

namespace Parsers
{
    EU4Parser::EU4Parser(std::string path)
    {
        this->path = path;
    }

    EU4Parser::~EU4Parser()
    {
    }

    std::unordered_map<std::string, ValueType *> *EU4Parser::process()
    {
        std::cout << "Opening file " << this->path << std::endl;

        auto *result = new std::unordered_map<std::string, ValueType *>();

        this->current_file = new std::ifstream(this->path);
        if (!this->current_file->is_open())
        {
            std::cout << "File not opened" << std::endl;
            throw std::runtime_error("File not opened");
        }

        this->process_file(result);

        std::cout << "File opened" << std::endl;

        return result;
    }

    void EU4Parser::extract_key_value(std::string line, std::string &key, std::string &value)
    {
        size_t pos = line.find('=');
        if (pos == std::string::npos)
        {
            if (line.size() > 1 && line.back() == '{')
            {
                // Corner case when we have a key but opening bracket is without "="
                auto unfiltered_key = line.substr(0, line.length() - 1);
                key = Utils::strip(unfiltered_key);
                value = "{";
                return;
            }
            value = Utils::strip(line);
            return;
        }

        auto unfiltered_key = line.substr(0, pos);
        auto unfiltered_value = line.substr(pos + 1);

        key = Utils::strip(unfiltered_key);
        value = Utils::strip(unfiltered_value);
    }

    void EU4Parser::remove_quotes(std::string &str)
    {
        if (str[0] == '\"')
        {
            str.erase(0, 1);
        }

        if (str[str.length() - 1] == '\"')
        {
            str.erase(str.length() - 1, 1);
        }
    }

    void EU4Parser::process_file(std::unordered_map<std::string, ValueType *> *result)
    {
        while (std::getline(*this->current_file, this->current_line))
        {
            this->process_line(this->current_line, result);
        }
    }

    void EU4Parser::process_line(std::string line, std::unordered_map<std::string, ValueType *> *result)
    {
        auto str_key = std::string();
        auto str_value = std::string();
        this->extract_key_value(line, str_key, str_value);
        if (str_value.find_first_not_of(' ') == std::string::npos)
        {
            return;
        }

        auto next_line = std::string();

        if (str_value.size() > 1 && str_value.find_first_of("{") != std::string::npos)
        {
            // Edge case when opening bracket is on the same line
            auto pos = str_value.find_first_of("{") + 1;
            auto substr = str_value.substr(0, pos);
            next_line = str_value.substr(pos);
            str_value = Utils::strip(substr);
        }
        else if (str_value.size() > 1 && str_value.find_first_of("\"") == std::string::npos && str_value.find_first_of("			") != std::string::npos)
        {
            // Edge case when there are multiple spaces between key and value
            auto pos = str_value.find_first_of("			");
            auto substr = str_value.substr(0, pos);
            next_line = str_value.substr(pos);
            next_line = Utils::strip(next_line);
            str_value = Utils::strip(substr);
        }
        else if (str_value.size() > 1 && str_value.find_last_of("}") != std::string::npos)
        {
            // Edge case when closing bracket is on the same line
            auto pos = str_value.find_last_of("}");
            auto substr = str_value.substr(0, pos);
            next_line = str_value.substr(pos);
            str_value = Utils::strip(substr);
        }

        this->remove_quotes(str_key);
        this->remove_quotes(str_value);

        // Proccess key
        if (str_key == "{")
        {
            // Open an object
            if (this->current_value->type == ValueType::Type::Object)
            {
                // Nested object
                auto *new_value = new ValueType();
                new_value->type = ValueType::Type::Object;
                new_value->parent = this->current_value;
                this->current_value = new_value;
            }
            else
            {
                // Root object
                this->current_value->type = ValueType::Type::Object;
                result->insert(std::make_pair(str_key, this->current_value));
            }
        }
        else if (str_key == "}" || str_value == "}")
        {
            // End of object or array
            if (this->current_value->parent != nullptr)
            {
                // Move up the tree
                this->current_value = this->current_value->parent;
            }
            else
            {
                // End of object tree
                this->current_value = new ValueType();
            }
        }
        else if (str_value == "{")
        {
            if (this->current_value->type == ValueType::Type::Array || this->current_value->type == ValueType::Type::Object)
            {
                auto type = str_key.empty() ? ValueType::Type::Array : ValueType::Type::Object;
                // Correct type to object if needed
                this->current_value->type = type;
                // Nested value
                auto *new_value = new ValueType();
                new_value->parent = this->current_value;
                new_value->type = ValueType::Type::Array; // Assume array
                this->current_value->write_value(new_value, str_key);
                this->current_value = new_value;
            }
            else
            {
                // Root object
                this->current_value->type = ValueType::Type::Array; // Assume array
                result->insert(std::make_pair(str_key, this->current_value));
            }
        }
        else
        {
            if (this->current_value->type != ValueType::Type::Array && this->current_value->type != ValueType::Type::Object && this->current_value->parent == nullptr)
            {
                if (result->find(str_key) != result->end() && result->at(str_key)->type != ValueType::Type::Array)
                {
                    // if there is already a value with this key, we need to convert it to an array
                    auto *new_value = new ValueType();
                    new_value->type = ValueType::Type::Array;
                    new_value->write_value(result->at(str_key), "");
                    new_value->write_value(this->parse_str_value(str_value), "");
                    result->erase(str_key);
                    result->insert(std::make_pair(str_key, new_value));
                }
                else
                {
                    result->insert(std::make_pair(str_key, this->parse_str_value(str_value)));
                }
            }
            else
            {
                auto type = str_key.empty() ? ValueType::Type::Array : ValueType::Type::Object;
                this->current_value->type = type;
                this->current_value->write_value(this->parse_str_value(str_value), str_key);
            }
        }

        if (!next_line.empty())
        {
            this->process_line(next_line, result);
        }
    }

    ValueType *EU4Parser::parse_str_value(std::string value)
    {
        auto typed_value = new ValueType();

        if (value == "yes")
        {
            typed_value->type = ValueType::Type::Boolean;
            typed_value->boolValue = true;
        }
        else if (value == "no")
        {
            typed_value->type = ValueType::Type::Boolean;
            typed_value->boolValue = false;
        }
        else if (Utils::is_number(value))
        {
            typed_value->type = ValueType::Type::Number;
            typed_value->numValue = std::stod(value);
        }
        else
        {
            typed_value->type = ValueType::Type::String;
            typed_value->strValue = value;
        }

        return typed_value;
    }
}
