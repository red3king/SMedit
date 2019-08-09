#include "lib/catch.h"

#define private public
#include "historymanager/historymanager.h"
#undef private

#include "historymanager/operations/machine_ops.h"
#include "models/machine.h"
#include "models/project.h"


TEST_CASE("HistoryManager.submit_operation, and undo", "[historymanager]")
{
    auto hm = HistoryManager(100, 20, 20);
    hm.new_project();

    auto create = OpMachineCreate("asdf");
    unsigned int result = hm.submit_operation(create);
    
    // machine should have been created
    REQUIRE(1 == hm.current_project.machines.size());
    
    // operation should return created machine id
    auto m1 = hm.current_project.machines[0];
    REQUIRE(m1->id == result);

    // submit second operation, should work
    auto name = OpMachineName(m1, "bob");
    hm.submit_operation(name);
    REQUIRE("bob" == hm.current_project.machines[0]->name);

    // undo rename -> machine name should be blank 
    hm.undo();
    REQUIRE("" == hm.current_project.machines[0]->name);

    // redo -> name should be bob again
    hm.redo();
    REQUIRE("bob" == hm.current_project.machines[0]->name);

}



TEST_CASE("HistoryManager.condense_history()", "[historymanager]")
{
    auto hm = HistoryManager(10, 3, 3);
    hm.new_project();

    for(int i=0; i<13; i++)
    {
        auto op = OpMachineCreate("");
        hm.submit_operation(op);
    }

    REQUIRE(0 == hm.initial_project.machines.size());
    REQUIRE(13 == hm.current_project.machines.size());

    auto create = OpMachineCreate("asdf");
    hm.submit_operation(create);

    REQUIRE(3 == hm.initial_project.machines.size()); 
    REQUIRE(14 == hm.current_project.machines.size());   
}


TEST_CASE("HistoryManager operation collapsing", "[historymanager]")
{
    auto hm = HistoryManager(100, 20, 20);
    hm.new_project();

    auto create = OpMachineCreate("asdf");
    hm.submit_operation(create);
    REQUIRE(1 == hm.operations.size());

    auto mach = hm.current_project.machines[0];
    auto name = OpMachineName(mach, "one");
    hm.submit_operation(name);
    REQUIRE(2 == hm.operations.size());

    auto name2 = OpMachineName(mach, "two");
    hm.submit_operation(name2);
    REQUIRE(2 == hm.operations.size());
    REQUIRE("two" == hm.current_project.machines[0]->name);
}

