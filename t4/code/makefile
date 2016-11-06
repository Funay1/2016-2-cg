EXECUTABLE = trabalhocg
CPPFLAGS = -ggdb3 -Wall
LIBS = -lpthread -lGL -lGLU -lglut -lm
#################################################
CC = g++
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))
DEPS := $(patsubst %.cpp,%.d,$(SRCS))
#################################################
all: $(EXECUTABLE)

$(EXECUTABLE): $(DEPS) $(OBJS)
	$(CC) -o $(EXECUTABLE) $(OBJS) $(LIBS)

%.d: %.cpp
	$(CC) -M $(CPPFLAGS) $< > $@
	$(CC) -M $(CPPFLAGS) $< | sed s/\\.o/.d/ > $@

%.d: %.h
	$(CC) -M $(CPPFLAGS) $< > $@
	$(CC) -M $(CPPFLAGS) $< | sed s/\\.o/.d/ > $@
clean:
	-rm -rf $(OBJS) $(EXECUTABLE) $(DEPS) *~

explain:
	@echo "--------Some Info--------"
	@echo "Executable name: $(EXECUTABLE)"
	@echo "Source files: $(SRCS)"
	@echo "Object files: $(OBJS)"
	@echo "Dependency files: $(DEPS)"

depends: $(DEPS)
	@echo "Dependencies updated"

-include $(DEPS)

run:	all
	./$(EXECUTABLE)

debug:	all
	gdb $(EXECUTABLE)

git:
	git add .
	git commit -m "Automatic commit"
	git push origin master

clean-emacs:
	rm *~
