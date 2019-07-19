#include "boxentity_ops.h"
#include "models/machine.h"


OpBoxEntityResize::OpBoxEntityResize(Machine* machine, BoxEntity* entity, BorderType resize_border, float new_position)
{
    entity_id = entity->id;
    machine_id = machine->id;
    this->resize_border = resize_border;
    this->new_position = new_position;
}


OpBoxEntityResize* OpBoxEntityResize::clone()
{
    return new OpBoxEntityResize(*this);
}   


unsigned int OpBoxEntityResize::execute(Project& project)
{
    Machine* machine = project.get_machine_by_id(machine_id);
    BoxEntity* box_entity = machine->get_state_by_id(entity_id);

    if(box_entity == nullptr)
        box_entity = machine->get_resourcelock_by_id(entity_id);

    switch(resize_border)
    {
        case BT_TOP:
            box_entity->h = box_entity->y + box_entity->h - new_position;
            box_entity->y = new_position;
            break;

        case BT_RIGHT:
            box_entity->w = new_position - box_entity->x;
            break;

        case BT_BOTTOM:
            box_entity->h = new_position - box_entity->y;
            break;

        case BT_LEFT:
            box_entity->w = box_entity->x + box_entity->w - new_position;
            box_entity->x = new_position;
            break;
    }

    return entity_id;
}


bool OpBoxEntityResize::may_collapse_impl(Operation& other)
{
    OpBoxEntityResize& other_ber = dynamic_cast<OpBoxEntityResize&>(other);
    return other_ber.entity_id == entity_id && other_ber.resize_border == resize_border;
}


void OpBoxEntityResize::collapse(Operation& other)
{
    OpBoxEntityResize& other_ber = dynamic_cast<OpBoxEntityResize&>(other);
    new_position = other_ber.new_position;
}
