#pragma once

#include <string>

#include "entity.h"

using std::string;


class Resource : public Entity
{
    public:
        Resource(unsigned int id=0);

        // TODO - am I going to load the while .py into this class?
        // or just metadata about what api it provides?
        
        // it would be better to have the .pys outside the json,
        // for both Resource and State run code,  so
        // users can load them up like normal python code in vim

        // only issue is when they change the api a Resource provides,
        // smedit may need to re-scan the python file contents on load
        // if it needs to be aware of the api (only if its also aware enough
        //  of the contents of the state code to be validating all usages of  
        // the functions provided by the resources...)

        // that's kinda the whole point of resources, though, to have a validated
        // way of knowing there can be no deadlock, so I will be parsing those pys
        
        // its not hard to write an ast "visitor" which will simply examine for usages
        // of a subclass of Resource, so that sholdnt be too bad.

        string path;
        string name;
};
