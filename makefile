CXXFLAGS = -std=c++14 -iquote ./ -ggdb $(shell pkg-config --cflags --libs gtkmm-3.0 epoxy)
LDFLAGS = $(shell pkg-config --libs gtkmm-3.0 epoxy)



presrc = $(filter-out main.cpp, $(filter-out test_main.cpp, $(wildcard *.cpp))) \
	$(wildcard controllers/*.cpp) \
	$(wildcard controllers/selected_item/*.cpp) \
	$(wildcard gui/*.cpp) \
	$(wildcard gui/models/*.cpp) \
	$(wildcard gui/operations/*.cpp) \
	$(wildcard historymanager/*.cpp) \
	$(wildcard historymanager/operations/*.cpp) \
	$(wildcard models/*.cpp) \
	$(wildcard models/datatypes/*.cpp)

src := $(presrc) main.cpp
testsrc := $(presrc) test_main.cpp

csrc = $(wildcard lib/nanovg/*.c)


obj = $(src:.cpp=.o) $(csrc:.c=.o)
testobj = $(testsrc:.cpp=.o) $(csrc:.c=.o)
dep = $(obj:.o=.d)  # one dependency file for each source
testdep = $(testobj:.o=.d)

smedit: $(obj)
	$(CXX) -o $@ $^ $(LDFLAGS)

test: $(testobj)
	$(CXX) -o $@ $^ $(LDFLAGS)


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
	rm -f $(testobj) test
	rm -f $(testdep)

