#include "lib/catch.h"

#include "models/project.h"
#include "models/machine.h"


TEST_CASE("Project.get_mindex_by_id", "[models][project]")
{
    Project p;
    Machine m1(1), m2(2), m3(3), m4(4);
    p.machines.push_back(m1);
    p.machines.push_back(m2);
    p.machines.push_back(m3);
    p.machines.push_back(m4);

    REQUIRE(1 == m1.id);
    REQUIRE(4 == m4.id);
    REQUIRE(0 == p.get_mindex_by_id(m1.id));
    REQUIRE(1 == p.get_mindex_by_id(m2.id));
}

