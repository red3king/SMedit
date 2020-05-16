# SMedit

SMEdit is two programs: a graphical state machine builder, and a headless runner program. You create a project consisting of state machine prototypes in the builder, which can be uploaded to a remote runner with one click. The builder has a view for watching the execution of your machines in real time, with support for pausing and restarting.

The goal of this software package is to allow you to build complicated flowchart logic which can be visually inspected for correct operation. It's kind of like Scratch except for advanced users with python experience.

For more detailed information, see documentation.txt in the "example projects and documentation" directory.


**Creating a demo FSM:**
![Builder, building](doc/img/build.gif)

The client is a linux native program built with GTK and openGL which lets you visually lay out your state machine and type python code into each state. 

**Running the demo:**
![Builder, running](doc/img/run.gif)

The server is built on the event loop provided by the Twisted web framework. The client program is also used to control the server and view the state of execution.

**SECURITY WARNING:** as it stands, the server is designed to execute python code received over the network. **there is no authentication yet**. It will be added when I'm finished implementing all the major planned features.

Note: SMedit is just a placeholder name and I will probably rename it once I can come up with a better name.



## Getting Started

How to build and run SMedit.


### Building SMedit Client

Install Prequesites:  

(Tested on Kubuntu 19.10)

Note: building the client requires c++ 17 features, so a recent compiler is needed.

Install python3 pip, libraries:

```
sudo apt-get install python3-pip python3-venv libgtkmm-3.0-dev libgtksourceviewmm-3.0-dev libboost-all-dev
```

Install cog with the python 3 pip:
```
sudo pip3 install cogapp
```

Once the prerequisites are installed, use make to build the server:

```
make
```

There is no install target for make for now. 
There is currently a bug in the makefile, it doesn't always realize it needs to recompile when headers or certain files are changed. To fix this, run make clean and then make again.


### Preparing the Runner

The easiest way to set up the server is to install the server's dependencies in a virtualenv. 
To do this, 

```
cd SMedit/server
pyvenv-3.7 venv
source venv/bin/activate
pip install -r requirements.txt
```

## Running SMedit

To run the client:
```
./smedit
```

To launch the server, run the following:

```
cd SMedit/server/
source venv/bin/activate
./main.py
```

## Building your first state machines
See documentation.txt in the "example projects and documentation" directory. After that try modifying some of the examples.


## Built With / Acknowledgments
* Nanovg - Mikko Mononen [https://github.com/memononen/nanovg](https://github.com/memononen/nanovg)

* Icons: Iconnic, Ralf Schmitzer, Dmitry Baranovskiy, Alice Design. All from [http:://thenounproject.com](http:://thenounproject.com)

* Hack Font  -  https://sourcefoundry.org/hack/

* nlohmann::json  -  Niels Lohmann [https://github.com/nlohmann/json](https://github.com/nlohmann/json)

* Base64 - Rene Nyffenegger  [https://github.com/ReneNyffenegger/cpp-base64](https://github.com/ReneNyffenegger/cpp-base64)

* Boost::asio, crc, thread, lockfree - Boost project [https://www.boost.org](https://www.boost.org)

* This readme is based off PurpleBooth's Readme.md [template](https://gist.github.com/PurpleBooth/109311bb0361f32d87a2#file-readme-template-md)



## Author

**Rory Timar**








