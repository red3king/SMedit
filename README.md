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


### Building SMedit Client (Debian-based systems)

Install Prequesites:  

(Tested on Ubuntu 20.04)

Install python3 pip, build tools and development libraries:

```
sudo apt-get install make python3-pip libgtkmm-3.0-dev libboost-all-dev libgtksourceviewmm-3.0-dev
```
The client includes an embedded python interpreter, so you also need libpython3.8-dev and probably python3.8-dev. However, when testing on a fresh ubuntu 20.04 vm, it appears they already were either preinstalled or pulled in by one of the above packages.


Install the cog templating program with the python 3 pip:
```
sudo pip3 install cogapp
```

Once the prerequisites are installed, use make to build the client:

```
make -j8  # replace with number of cores
          # if your system locks up and the compiler gets killed,
          # you need more ram or a lower j value
```

There is no install target for make for now. 
There are a couple bugs in my makefile; if you are developing, see the bugs file.


### Preparing the Server
The cleanest way to set up the server is to install the server's dependencies in a virtualenv. First, install venv:
```
sudo apt-get install python3-venv
```

Then, set up your venv:

```
cd SMedit/server
python3.8 -m venv venv
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








