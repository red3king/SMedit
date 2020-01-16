#pragma once

#include <Python.h>
#include <string>
#include <stdexcept>
#include <map>
#include <vector>

#include <iostream>

#include "scriptoutput.h"


using std::string;
using std::vector;
using std::map;


template<typename T>
inline PyObject* _create_pyobject(T t)
{
    throw std::invalid_argument("this function should not be called, implement an overload (see below).");
    return nullptr;
}


template<>
inline PyObject* _create_pyobject<int>(int t)
{
    return PyLong_FromLong(t);
}


template<>
inline PyObject* _create_pyobject<float>(float t)
{
    return PyFloat_FromDouble(t);
}


template<>
inline PyObject* _create_pyobject<double>(double t)
{
    return PyFloat_FromDouble(t);
}


template<>
inline PyObject* _create_pyobject<string>(string t)
{
    return PyUnicode_FromString(t.c_str());
}


template<>
inline PyObject* _create_pyobject<nullptr_t>(nullptr_t t)
{
    return Py_None;
}


template<typename T>
inline vector<PyObject*> _create_pyobjects(T t)
{
    auto py_objects = vector<PyObject*>();
    py_objects.push_back(_create_pyobject(t));
    return py_objects;
}


template<typename T, typename... Args>
inline vector<PyObject*> _create_pyobjects(T t, Args... args)
{
    auto py_objects = _create_pyobjects(args...);
    PyObject* obj = _create_pyobject(t);
    py_objects.insert(0, obj);
    return py_objects;
}



class PyScript
{
    public:

        string name;
        PyObject *module, *function;

        PyScript()
        {
            module = nullptr;
            function = nullptr;
        }

        ~PyScript()
        {
            if(function != nullptr)
                Py_DECREF(function);

            if(module != nullptr)
                Py_DECREF(module);
        }

        static bool create(PyScript*& out, string& err_out, string name)
        {
            name = "py_embedded.scripts." + name;
            
            out = new PyScript();
            out->name = name;
            err_out = "";
            
            // Import module
            auto name_obj = PyUnicode_FromString(name.c_str());  // ours 
            auto module = PyImport_Import(name_obj);            // ours, won't decref here becuase owned by PyScript out now.
            Py_DECREF(name_obj);

            if(module == nullptr)
            {
                err_out = "could not import module";
                return false;
            }

            // Get function
            auto function = PyObject_GetAttrString(module, "execute");
            if(function == nullptr)
            {
                Py_DECREF(module);
                err_out = "could not find execute() in module";
                return false;        
            }
            
            out->module = module;
            out->function = function;
            return true;
        }

        bool get_pyerr(string& exception_class, string& exception_message)
        {
            if(PyErr_Occurred() == nullptr)
            {
                exception_class = "";
                exception_message = "";
                return false;
            }
            
            PyObject *ptype, *pvalue, *ptraceback;
            PyObject *pystr, *module_name, *pyth_module, *pyth_func;
            char *str;

            PyErr_Fetch(&ptype, &pvalue, &ptraceback);  // We own these references
            PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);
                       
           
            auto name_py_str = PyObject_GetAttrString(ptype, "__name__"); // ours
            exception_class = py_str_to_str(name_py_str);
            Py_DECREF(name_py_str);
            
            module_name = PyUnicode_FromString("traceback");  // ours
            pyth_module = PyImport_Import(module_name); // ours
            Py_DECREF(module_name);

            if (pyth_module == NULL) 
            {
                exception_class = "unknown";
                exception_message = "could not import traceback";
                goto end;
            }
                      
            pyth_func = PyObject_GetAttrString(pyth_module, "format_exception");  // ours
            
            if (pyth_func && PyCallable_Check(pyth_func)) 
            {
                PyObject *pyth_val;
                pyth_val = PyObject_CallFunctionObjArgs(pyth_func, ptype, pvalue, ptraceback, NULL); // ours
                exception_message = concat_list(pyth_val);

                Py_DECREF(pyth_val);
                Py_DECREF(pyth_func);
            }
            
            else
            {
                if(pyth_func)
                    Py_DECREF(pyth_func);
                
                exception_class = "unk";
                exception_message = "could find traceback.format_exception";
                goto end;
            }
            
            end:
            
            Py_DECREF(ptype);
            Py_DECREF(pvalue);
            Py_DECREF(ptraceback);
            Py_DECREF(pyth_module);
            
            return true;
        }
        
        string concat_list(PyObject* fmt_exc_output)
        {
            string tb_str = "";
            int num_lines = (int) PyList_Size(fmt_exc_output);
            
            for(int i=0; i<num_lines; i++)
            {
                PyObject *line_str = PyList_GetItem(fmt_exc_output, i);
                Py_INCREF(line_str);
                tb_str += py_str_to_str(line_str);
                Py_DECREF(line_str);
            }
            
            return tb_str;
        }
        
        string py_str_to_str(PyObject *str)
        {
            char *result;
            result = (char*) PyUnicode_AsUTF8AndSize(str, nullptr);
            return string(result);
        }
        
        ScriptOutput execute(vector<PyObject*> args)
        {
            // Make args tuple
            auto args_tuple = PyTuple_New(args.size());
            
            for(int i=0; i<args.size(); i++)
                PyTuple_SetItem(args_tuple, i, args[i]);
            
            ScriptResult value;
            bool error;
            string exception_class, exception_message;
            
            auto result = PyObject_CallObject(function, args_tuple);
            
            if(result == nullptr)
                error = true;

            else
            {
                error = false;

                if(result == Py_None)
                    value = nullptr;    

                else if(PyLong_Check(result))
                    value = PyLong_AsLong(result);

                else if(PyFloat_Check(result))
                    value = PyFloat_AsDouble(result);

                else if(PyUnicode_Check(result))
                {
                    Py_ssize_t len;
                    char* ptr = (char*) PyUnicode_AsUTF8AndSize(result, &len);
                    value = string(ptr, (int)len);
                }

                else
                    throw std::invalid_argument("script returned unsupported output type");
            }

            if(error)
                get_pyerr(exception_class, exception_message);
            
            // Clean up params and result
            Py_DECREF(args_tuple);
    
            if(result != nullptr)
                Py_DECREF(result);
    
            for(int i=0; i<args.size(); i++)
            {
                if(args[i] == Py_None || args[i] == Py_True || args[i] == Py_False)
                    continue;

                Py_DECREF(args[i]);
            }
            
            if(error)
                return ScriptOutput::create_failure(exception_class, exception_message);

            else
                return ScriptOutput::create_success(value);
        }

    private:
};


class PYScripts
{
    public:

        PYScripts()
        {

        }

        void initialize()
        {
            Py_Initialize();

            // add current directory to path
            PyObject *path = PySys_GetObject((char*)"path");    // borrowed
            auto str = PyUnicode_FromString(".");   // ours
            PyList_Append(path, str);
            Py_DECREF(str);
        }

        void tear_down()
        {
            Py_Finalize();
        }

        ScriptOutput run(string script_name)
        {
            return _run(script_name, vector<PyObject*>());
        }

        template<typename T>
        ScriptOutput run(string script_name, T t)
        {
            auto args = vector<PyObject*>();
            args.push_back(_create_pyobject(t));
            
            return _run(script_name, args);
        }

        template<typename T, typename... Args>
        ScriptOutput run(string script_name, T t, Args... args)
        {
            vector<PyObject*> arg_vec = _create_pyobjects(t, args...);
            
            return _run(script_name, arg_vec);
        }
        
    private:
        map<string, PyScript*> scripts;
        
        ScriptOutput _run(string script_name, vector<PyObject*> args)
        {
            if(scripts.find(script_name) == scripts.end())
            {
                PyScript* new_script;
                string err;
                
                if(!PyScript::create(new_script, err, script_name))
                    return ScriptOutput::create_failure("Couldn't initialize " + script_name, err);
                
                scripts[script_name] = new_script;
            }
            
            return scripts[script_name]->execute(args);
        }
};


extern PYScripts scripts;


