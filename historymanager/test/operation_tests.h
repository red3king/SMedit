#include "lib/catch.h"

#include "historymanager/operation.h"
#include "historymanager/operations/machine_ops.h"
#include "historymanager/operations/resource_ops.h"
#include "historymanager/operations/resourcelock_ops.h"
#include "models/machine.h"
#include "models/project.h"


TEST_CASE("OpMachineCreate", "[operations][machine_ops]")
{
    Project p;
    REQUIRE(0 == p.machines.size());
    unsigned int i = OpMachineCreate("").execute(p);
    REQUIRE(1 == p.machines.size());
    REQUIRE(i == p.machines[0]->id);
}


TEST_CASE("Operation.may_collapse", "[operations]")
{
    //also a test of OpMachineName.may_collapse_impl
    // too lazy to write test operation class
    auto p = Project();
    auto m1 = new Machine(1);
    auto m2 = new Machine(2);
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


TEST_CASE("Resource operations", "[operations]")
{
    auto p = Project();
    
    REQUIRE(0 == p.resources.size());

    auto rc = OpResourceCreate("res1", "/res1.py");
    rc.execute(p);

    REQUIRE(1 == p.resources.size());
    Resource* res1 = p.resources[0];
    REQUIRE("res1" == res1->name);
    REQUIRE("/res1.py" == res1->path);

    auto m = new Machine(1);
    p.machines.push_back(m);

    auto rlc = OpResourceLockCreate(m, res1, 100, 100);
    REQUIRE(0 == m->resourcelocks.size());
    rlc.execute(p);
    REQUIRE(1 == m->resourcelocks.size());
    REQUIRE(res1 == m->resourcelocks[0]->resource);

    auto rld = OpResourceLockDelete(m, m->resourcelocks[0]);
    rld.execute(p);
    REQUIRE(0 == m->resourcelocks.size());

    auto rd = OpResourceDelete(res1);
    rd.execute(p);
    REQUIRE(0 == p.resources.size());
    
}
