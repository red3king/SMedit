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
        
        string load_failure_error_message;
        bool _loaded_successfully;
        
        PyObject *module, *function;

        PyScript()
        {
            module = nullptr;
            function = nullptr;
            _loaded_successfully = false;
        }

        ~PyScript()
        {
            if(function != nullptr)
                Py_DECREF(function);

            if(module != nullptr)
                Py_DECREF(module);
        }
        
        bool loaded_successfully(string &err_out)
        {
            err_out = load_failure_error_message;
            return _loaded_successfully;
        }
        
        void set_failed_load(string error_message)
        {
            _loaded_successfully = false;
            load_failure_error_message = error_message;
        }
        
        void set_successful_load(PyObject *module, PyObject *function)
        {
            _loaded_successfully = true;
            this->module = module;
            this->function = function;
        }

        static void create(PyScript*& out, string name)
        {
            name = "py_embedded.scripts." + name;
            
            out = new PyScript();
            out->name = name;
            
            // Import module
            auto name_obj = PyUnicode_FromString(name.c_str());  // ours 
            auto module = PyImport_Import(name_obj);            // ours, won't decref here becuase owned by PyScript out now.
            Py_DECREF(name_obj);

            if(module == nullptr)
            {
                string exc_class, exc_msg;
                get_pyerr(exc_class, exc_msg);
                
                string err_out =  "could not import module: " + name;
                err_out += "\nexception class: " + exc_class;
                err_out += "\nexception message: " + exc_msg;
                out->set_failed_load(err_out);
                return;
            }

            // Get function
            auto function = PyObject_GetAttrString(module, "execute");
            if(function == nullptr)
            {
                Py_DECREF(module);
                string err_out = "could not find execute() in module";
                out->set_failed_load(err_out);  
                return;
            }
            
            out->set_successful_load(module, function);
        }

        static bool get_pyerr(string& exception_class, string& exception_message)
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
                exception_message = pyth_val == nullptr ? "null" : concat_list(pyth_val);

                if(pyth_val != nullptr)
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

            if(ptraceback != nullptr)
                Py_DECREF(ptraceback);

            Py_DECREF(pyth_module);
            PyErr_Restore(NULL, NULL, NULL);
            
            return true;
        }
        
        static string concat_list(PyObject* fmt_exc_output)
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
        
        static string py_str_to_str(PyObject *str)
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
                // Steals reference to args[i]
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
            // Don't decref args tuple elements, their refs were stolen
            Py_DECREF(args_tuple);
    
            if(result != nullptr)
                Py_DECREF(result);
    
            
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
            // TODO  existence check!
            add_to_path("./resources/pyscripts"); // for scripts
            add_to_path("./server/"); // for definitions
        }
        
        void add_to_path(string location)
        {
            PyObject *path = PySys_GetObject((char*)"path");    // borrowed
            auto str = PyUnicode_FromString(location.c_str());   // ours
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
                std::cout << "###### can't find " + script_name + ", creating it." << std::endl;
                
                PyScript* new_script;
                PyScript::create(new_script, script_name);               
                scripts[script_name] = new_script;
            }
            
            auto script = scripts[script_name];
            string err;
            
            std::cout << "###### try run " + script_name << std::endl;
            
            if(!(script->loaded_successfully(err)))
            {
                std::cout << "###### script fail load " + script_name << std::endl;
                return ScriptOutput::create_failure("Couldn't initialize " + script_name, err);
                
            }
            
            else
                std::cout << "##### script ok load, running!" <<std::endl;
                
            return script->execute(args);
        }
};


extern PYScripts scripts;


