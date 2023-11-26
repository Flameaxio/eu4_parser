#ifndef VALUE_TYPE_H
#define VALUE_TYPE_H

#include <string>
#include <vector>
#include <unordered_map>

namespace Parsers
{
    class ValueType
    {
    public:
        enum class Type
        {
            String,
            Number,
            Object,
            Array,
            Boolean
        };

        Type type;
        std::string strValue;
        double numValue;
        std::vector<ValueType*> arrayValue;
        std::unordered_map<std::string, ValueType*> objectValue;
        bool boolValue;
        ValueType* parent = nullptr;

        ValueType();
        ValueType(Type, std::string);
        ValueType(Type, double);
        ValueType(Type, std::vector<ValueType*>);
        ValueType(Type, std::unordered_map<std::string, ValueType*>);

        void write_value(ValueType* value, std::string key);

        std::string to_json(std::string indent = "");
    };
}

#endif