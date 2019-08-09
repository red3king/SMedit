#pragma once

#include <string>

#include "historymanager/operation.h"
#include "models/machine.h"
#include "models/project.h"


class OpMachineCreate : public Operation
{
    public:
        OpMachineCreate(string name);
        unsigned int execute(Project& project);
        OpMachineCreate* clone();

    private:
        string name;
};


class OpMachineDelete : public Operation
{
    public:
        OpMachineDelete(Machine* machine);
        OpMachineDelete* clone();
        
        unsigned int execute(Project& project);

    private:
        unsigned int id;        
};


class OpMachineName : public Operation
{
    public:
        OpMachineName(Machine* machine, string name);
        OpMachineName* clone();

        unsigned int execute(Project& project);
        bool may_collapse_impl(Operation& other);
        void collapse(Operation& other);

        unsigned int id;
        string name;

    private:
};


class OpMachineRunOnStart : public Operation
{
    public:
        OpMachineRunOnStart(Machine* machine, bool run_on_start);
        OpMachineRunOnStart* clone();

        unsigned int execute(Project& project);
        bool may_collapse_impl(Operation& other);
        void collapse(Operation& other);

        unsigned int id;
        bool run_on_start;
};


