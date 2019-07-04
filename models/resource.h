#pragma once

#include <string>

#include "entity.h"

using std::string;


class Resource : public Entity
{
    public:
        Resource(unsigned int id=0);

        string path;
        string name;
};
