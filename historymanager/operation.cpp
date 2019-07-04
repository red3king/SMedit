#include <typeinfo>

#include "operation.h"


Operation::Operation()
{

}


bool Operation::may_collapse(Operation& other)
{
    return typeid(*this) == typeid(other) && may_collapse_impl(other);
}


bool Operation::may_collapse_impl(Operation& other)
{
    return false;
}


void Operation::collapse(Operation& other) { }


Operation::~Operation(){ }
