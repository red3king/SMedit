#include "run_script.h"


ScriptOutput::ScriptOutput()
{
    error = false;
    exception_class = "";
    exception_message = "";
    result = nullptr;
}


ScriptOutput ScriptOutput::create_success(ScriptResult result)
{
    ScriptOutput output;
    output.result = result;
    return output;
}


ScriptOutput ScriptOutput::create_failure(string exception_class, string exception_message)
{
    ScriptOutput output;
    output.error = true;
    output.exception_class = exception_class;
    output.exception_message = exception_message;
    return output;
}



