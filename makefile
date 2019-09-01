CXXFLAGS = -std=c++17 -iquote ./ -ggdb $(shell pkg-config --cflags  gtkmm-3.0 gtksourceviewmm-3.0 epoxy)
LDFLAGS = -lstdc++fs $(shell pkg-config --libs gtkmm-3.0 gtksourceviewmm-3.0 epoxy)

# generated source files (generated in place by cog)
autogens = $(wildcard historymanager/operations/*.cpp) \
		   $(wildcard historymanager/operations/*.h)

# source files used by test and smedit
presrc = $(filter-out main.cpp, $(filter-out test_main.cpp, $(wildcard *.cpp))) \
	$(wildcard controllers/*.cpp) \
	$(wildcard controllers/selected_item/*.cpp) \
	$(wildcard controllers/selected_item/selected_state/*.cpp) \
	$(wildcard gui/*.cpp) \
	$(wildcard gui/models/*.cpp) \
	$(wildcard gui/operations/*.cpp) \
	$(wildcard historymanager/*.cpp) \
	$(wildcard historymanager/operations/*.cpp) \
	$(wildcard models/*.cpp) \
	$(wildcard models/datatypes/*.cpp)

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

