#pragma once

#include "historymanager/operation.h"
#include "models/boxentity.h"
#include "const.h"


class OpBoxEntityResize : public Operation
{
    public:
        OpBoxEntityResize(Machine* machine, BoxEntity* entity, BorderType resize_border, float new_position);
        OpBoxEntityResize* clone();

        unsigned int execute(Project& project);
        bool may_collapse_impl(Operation& other);
        void collapse(Operation& other);

    private:
        unsigned int entity_id, machine_id;
        BorderType resize_border;
        float new_position;
};
