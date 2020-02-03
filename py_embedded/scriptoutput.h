#pragma once

#include <variant>
#include <string>

using std::variant;
using std::string;
using std::nullptr_t;


typedef variant<nullptr_t, long, double, string> ScriptResult;


class ScriptOutput
{
    public:
        ScriptOutput();

        static ScriptOutput create_success(ScriptResult result);
        static ScriptOutput create_failure(string exception_class, string exception_message);

        bool error;
        string exception_class;
        string exception_message;

        ScriptResult result;

    private:
};
