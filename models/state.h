#pragma once

#include <string>
#include <vector>

#include "boxentity.h"
#include "models/datatypes/lvov.h"
#include "models/datatypes/arg.h"
#include "models/datatypes/argdef.h"

#define STS_INITIAL "Initial"       // list of argument names
#define STS_CODE "Code"
#define STS_RETURN "Return"         // return vov
#define STS_SPAWN "Spawn"           // spawn string name or variable, vovlist arguments, string pid var
#define STS_JOIN "Join"             // string variable



#define DEFAULT_STATE_CODE "def code_func(vars_dict, trigger_event=None):\r\n    pass\r\n"



using std::string;
using std::vector;

class Transition;


enum StateType { INITIAL, CODE, RETURN, SPAWN, JOIN };

string state_type_to_string(StateType type);
StateType string_to_state_type(string input);


class State : public BoxEntity
{
    /*
     *
     *  Creating a state:
     *      - user clicks button or s key
     *      - mouse now has a mini state icon thingy on it
     *      - user clicks on worksheet
     *      - state appears there and is selected in the selected item window
     *
     *
     *  Moving a state:
     *      - user moves over state
     *      - mouse now has a "move it" cursor
     *      - user clicks and drags to move state. state is moving with mouse, x and y and all attached transitions     *           updating
     *      - when the user lets go, nothing happens because the state was following the cursor the whole time
     *
     *
     *  Modifying a state:
     *      - user clicks on state
     *      - it's selected, appears in selected item window
     *
     *  Deleting a state:
     *      - user selects state,
     *      - and either clicks the delete button or the delete key
     *      - since transitions cannot be dangling, a modal pops up to confirm that associated transitions
     *          will be deleted. (if they can be dangling, no need for this modal)
     *
    * */


    public:
        State(unsigned int id=0);
        State(json jdata);
        State(const State &other);

        json to_json();


        string name;
        StateType type;
        
        // Initial
        vector<ArgDef> initial_args;

        // Code
        string code;    

        //Return  done
        bool has_return_value;
        LVOV return_value;  // ignore if has_return_value = false

        // Spawn / Run
        bool launch_synchronous;
        LVOV launch_task_name;
        vector<Arg> launch_args;
        string launch_result_variable;  // stores pid for spawn, result for run

        // Join  done
        string join_pid_variable;
        string join_result_variable; 

        vector<Transition*> incoming_transitions, outgoing_transitions;

        vector<Transition*> get_all_transitions();
        void add_transition(Transition* transition, bool incoming);
        void remove_transition(Transition* transition, bool incoming);
        void update_transition_positions();

    private:
};


