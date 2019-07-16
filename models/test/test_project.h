#include "lib/catch.h"

#include "models/project.h"
#include "models/machine.h"


TEST_CASE("Project.get_mindex_by_id", "[models][project]")
{
    Project p;
    auto m1 = new Machine(1);
    auto m2 = new Machine(2);
    auto m3 = new Machine(3);
    auto m4 = new Machine(4);
    
    p.machines.push_back(m1);
    p.machines.push_back(m2);
    p.machines.push_back(m3);
    p.machines.push_back(m4);

    REQUIRE(1 == m1->id);
    REQUIRE(4 == m4->id);

    REQUIRE(m1 == p.get_machine_by_id(m1->id));
    REQUIRE(m2 == p.get_machine_by_id(m2->id));
}

