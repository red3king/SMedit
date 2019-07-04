#include "lib/catch.h"

#include "historymanager/operation.h"
#include "historymanager/operations/machine_ops.h"
#include "models/machine.h"
#include "models/project.h"


TEST_CASE("OpMachineCreate", "[operations][machine_ops]")
{
    Project p;
    REQUIRE(0 == p.machines.size());
    unsigned int i = OpMachineCreate().execute(p);
    REQUIRE(1 == p.machines.size());
    REQUIRE(i == p.machines[0].id);
}


TEST_CASE("Operation.may_collapse", "[operations]")
{
    //also a test of OpMachineName.may_collapse_impl
    // too lazy to write test operation class
    auto p = Project();
    auto m1 = Machine(1);
    auto m2 = Machine(2);
    p.machines.push_back(m1);
    p.machines.push_back(m2);

    auto a = OpMachineName(m1, "first");
    auto b = OpMachineName(m1, "second");
    auto other = OpMachineDelete(m1);
    auto other_machine = OpMachineName(m2, "asdf");

    // operations of different type cant collapse
    REQUIRE(false == a.may_collapse(other));

    // OpMachineName cant collapse between two diff. machines
    REQUIRE(false == a.may_collapse(other_machine));

    // Can collapse when target same
    REQUIRE(true == a.may_collapse(b));
}
