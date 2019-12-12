#pragma once

#include "utils.h"


class Options
{
    public:
        // TODO implement save / load to /from file

        Options();

        IOResult load();
        IOResult save();

        bool get_auto_pan();
        void set_auto_pan(bool auto_pan);

        bool get_open_spawned();
        void set_open_spawned(bool open_spawned);

    private:
        bool auto_pan;
        bool open_spawned;
};


extern Options options;
