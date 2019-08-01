#pragma once

#include "historymanager/operation.h"
#include "models/resource.h"


class ResourceChgOperation : public Operation
{
    public:
        ResourceChgOperation(Resource* resource);
        unsigned int execute(Project& project);
        virtual void execute_impl(Resource* resource)=0;

    private:
        unsigned int resource_id;
};


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


class OpResourcePath : public ResourceChgOperation
{
    public:
        OpResourcePath(Resource* resource, string path);
        OpResourcePath* clone();
        void execute_impl(Resource* resource);

    private:
        string path;
};


class OpResourceName : public ResourceChgOperation
{
    public:
        OpResourceName(Resource* resource, string name);
        OpResourceName* clone();
        void execute_impl(Resource* resource);

    private:
        string name;
};
