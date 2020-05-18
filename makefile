
# use sed to turn -I/usr/blah into -isystem/usr/blah to silence gdkmm warnings
# TODO combine tr and sed commands

CXXFLAGS = -O0 -std=c++17 -iquote ./ -ggdb $(shell pkg-config --cflags  gtkmm-3.0 gtksourceviewmm-3.0 epoxy python-3.8-embed | sed 's/-I/-isystem/g') -fPIE

LDFLAGS = -lstdc++fs $(shell { pkg-config --libs gtkmm-3.0 gtksourceviewmm-3.0 epoxy; python3.8-config --ldflags --embed; } | tr "\n" " " ) -L/usr/lib -lpthread -lboost_system -lboost_thread




# generated source files (generated in place by cog)
autogens = $(wildcard historymanager/operations/*.cpp) \
		   $(wildcard historymanager/operations/*.h) \
		   $(wildcard net/actions/simple_commands.cpp) \
		   $(wildcard net/actions/simple_commands.h)

# source files used by test and smedit
presrc = $(filter-out main.cpp, $(filter-out test_main.cpp, $(wildcard *.cpp))) \
	$(wildcard controllers/*.cpp) \
	$(wildcard controllers/selected_item/*.cpp) \
	$(wildcard controllers/selected_item/selected_state/*.cpp) \
	$(wildcard controllers/run/*.cpp) \
	$(wildcard gui/*.cpp) \
	$(wildcard gui/models/*.cpp) \
	$(wildcard gui/operations/*.cpp) \
	$(wildcard historymanager/*.cpp) \
	$(wildcard historymanager/operations/*.cpp) \
	$(wildcard models/*.cpp) \
	$(wildcard models/datatypes/*.cpp) \
	$(wildcard net/*.cpp) \
	$(wildcard net/actions/*.cpp) \
	$(wildcard py_embedded/*.cpp) \
	$(wildcard lib/base64/*.cpp) 

src := $(presrc) main.cpp
testsrc := $(presrc) test_main.cpp

# c language files
csrc = $(wildcard lib/nanovg/*.c)


obj = $(src:.cpp=.o) $(csrc:.c=.o)
testobj = $(testsrc:.cpp=.o) $(csrc:.c=.o)
dep = $(obj:.o=.d)  # one dependency file for each source
testdep = $(testobj:.o=.d)


all: smedit

code-gen: $(autogens)
	cog -r $^

smedit: code-gen smedit-compile

smedit-compile: $(obj)
	$(CXX) -o smedit $^ $(LDFLAGS)

test: code-gen test-compile

test-compile: $(testobj)
	$(CXX) -o smtest $^ $(LDFLAGS)

	
# install / uninstall stuff:
prefix=/usr/local
    
.PHONY: install
install: smedit
	install -m 0755 smedit $(prefix)/bin
	# left off here, gotta add add those resources	
	#install -m 0644 *.png $(prefix)/share/our_program/icons

.PHONY: uninstall
uninstall:
	rm -f $(prefix)/bin/smedit



ifeq ($(MAKECMDGOALS),smedit)
-include $(dep)   # include all dep files in the makefile
endif

ifeq ($(MAKECMDGOALS),test)
-include $(testdep)   # include all dep files in the makefile
endif


# rule to generate a dep file by using the C preprocessor
# (see man cpp for details on the -MM and -MT options)
%.d: %.cpp
	@$(CPP) $(CPPFLAGS) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@

%.d: %.c
	@$(CPP) $(CPPFLAGS) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@


.PHONY: clean
clean:
	# delete stuff
	rm -f $(obj) smedit
	rm -f $(dep)

	# delete test stuff
	rm -f $(testobj) smtest
	rm -f $(testdep)

