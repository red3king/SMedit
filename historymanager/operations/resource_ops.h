#pragma once

#include "historymanager/operation.h"
#include "models/resource.h"


class OpResourceCreate : public Operation
{
    public:
        OpResourceCreate(string name, string path);
        OpResourceCreate* clone();
        unsigned int execute(Project& project);
        
    private:
        string name, path;
};


class OpResourceDelete : public Operation
{
    public: 
        OpResourceDelete(Resource* to_delete);
        OpResourceDelete* clone();
        unsigned int execute(Project& project);

    private:
        unsigned int to_delete_id;
};



