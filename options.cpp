#include "options.h"


Options options;

Options::Options()
{
    // default values
    auto_pan = true;
    open_spawned = true;
}


IOResult Options::load()
{
    // TODO
    return IOResult(true);
}


IOResult Options::save()
{
    // TODO
    return IOResult(true);
}


bool Options::get_auto_pan()
{
    return auto_pan;
}


void Options::set_auto_pan(bool auto_pan)
{
    this->auto_pan = auto_pan;
    save();
}


bool Options::get_open_spawned()
{
    return open_spawned;
}


void Options::set_open_spawned(bool open_spawned)
{
    this->open_spawned = open_spawned;
    save();
}
