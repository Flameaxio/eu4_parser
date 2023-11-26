#include "value_type.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

namespace Parsers
{
    ValueType::ValueType() {}
    ValueType::ValueType(Type type, std::string string) : type(type), strValue(string) {}
    ValueType::ValueType(Type type, double number) : type(type), numValue(number) {}
    ValueType::ValueType(Type type, std::vector<ValueType *> array) : type(type), arrayValue(array) {}
    ValueType::ValueType(Type type, std::unordered_map<std::string, ValueType *> object) : type(type), objectValue(object) {}

    void ValueType::write_value(ValueType *value, std::string key)
    {
        switch (this->type)
        {
        case Type::String:
            this->strValue = value->strValue;
            break;
        case Type::Number:
            this->numValue = value->numValue;
            break;
        case Type::Array:
            this->arrayValue.push_back(value);
            break;
        case Type::Object:
            if (this->objectValue.find(key) != this->objectValue.end() && this->objectValue.at(key)->type != ValueType::Type::Array)
            {
                auto *new_value = new ValueType();
                new_value->type = ValueType::Type::Array;
                new_value->write_value(this->objectValue.at(key), "");
                new_value->write_value(value, "");
                this->objectValue.erase(key);
                this->objectValue.insert(std::make_pair(key, new_value));
            }
            else
            {
                this->objectValue.insert(std::make_pair(key, value));
            }
            break;
        case Type::Boolean:
            this->boolValue = value->boolValue;
            break;
        }
    }

    std::string ValueType::to_json(std::string indent)
    {
        auto result = std::string();
        auto current_indent = indent + "    ";

        switch (this->type)
        {
        case Type::String:
            result = "\"" + this->strValue + "\"";
            break;
        case Type::Number:
            result = std::to_string(this->numValue);
            break;
        case Type::Boolean:
            result = this->boolValue ? "true" : "false";
            break;
        case Type::Array:
            result += "[\n";
            for (const auto &value : this->arrayValue)
            {
                result += current_indent + value->to_json(current_indent);
                if (value != this->arrayValue.back())
                {
                    result += ", \n";
                }
            }
            result += "\n" + indent + "]";
            break;
        case Type::Object:
            result += "{\n";
            for (auto it = this->objectValue.begin(); it != this->objectValue.end();)
            {
                result += current_indent + "\"" + it->first + "\"" + ": " + it->second->to_json(current_indent);
                if (++it != this->objectValue.end())
                {
                    result += ", \n";
                }
            }
            result += "\n" + indent + "}";
            break;
        }
        return result;
    }
}