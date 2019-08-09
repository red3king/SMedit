#include "machine_ops.h"
#include "models/project.h"
#include "signals.h"


// Create
OpMachineCreate::OpMachineCreate(string name)
{
    this->name = name;
}


unsigned int OpMachineCreate::execute(Project& project)
{
    unsigned int machine_id = project.get_next_id();
    Machine* m = new Machine(machine_id);
    m->name = name;
    project.machines.push_back(m);
    signals.fire_model_changed(MACHINE, CREATE, machine_id);
    return m->id;
}


OpMachineCreate* OpMachineCreate::clone()
{
    return new OpMachineCreate(*this);
}


// Delete
OpMachineDelete::OpMachineDelete(Machine* machine)
{
    id = machine->id;
}


OpMachineDelete* OpMachineDelete::clone()
{
    return new OpMachineDelete(*this);
}


unsigned int OpMachineDelete::execute(Project& project)
{
    signals.fire_model_changed(MACHINE, PRE_DELETE, id);
    int i = project.get_mindex_by_id(id);
    delete project.machines[i];
    project.machines.erase(project.machines.begin() + i);
    return id;
}


// Change Name
OpMachineName::OpMachineName(Machine* machine, string name)
{
    id = machine->id;
    this->name = name;
}


OpMachineName* OpMachineName::clone()
{
    return new OpMachineName(*this);
}


unsigned int OpMachineName::execute(Project& project)
{
    Machine* m = project.get_machine_by_id(id);
    m->name = name;
    signals.fire_model_changed(MACHINE, MODIFY, id);
    return id;
}


bool OpMachineName::may_collapse_impl(Operation& other)
{
    OpMachineName& other_name = dynamic_cast<OpMachineName&>(other);
    return other_name.id == id;
}


void OpMachineName::collapse(Operation& other)
{
    OpMachineName& other_name = dynamic_cast<OpMachineName&>(other);
    name = other_name.name;
}


// Change run_on_start
OpMachineRunOnStart::OpMachineRunOnStart(Machine* machine, bool run_on_start)
{
    id = machine->id;
    this->run_on_start = run_on_start;
}


OpMachineRunOnStart* OpMachineRunOnStart::clone()
{
    return new OpMachineRunOnStart(*this);
}


unsigned int OpMachineRunOnStart::execute(Project& project)
{
    Machine* m = project.get_machine_by_id(id);
    m->run_on_start = run_on_start;
    signals.fire_model_changed(MACHINE, MODIFY, id);
    return id;
}


bool OpMachineRunOnStart::may_collapse_impl(Operation& other)
{
    OpMachineRunOnStart& other_ros = dynamic_cast<OpMachineRunOnStart&>(other);
    return other_ros.id == id;
}


void OpMachineRunOnStart::collapse(Operation& other)
{
    OpMachineRunOnStart& other_ros = dynamic_cast<OpMachineRunOnStart&>(other);
    run_on_start = other_ros.run_on_start;
}



