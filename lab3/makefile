EXECUTABLE = main
CPPFLAGS = -Wall
LIBS = -lGL -lGLU -lglut -lm
#################################################
CC = gcc
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,%.o,$(SRCS))
DEPS := $(patsubst %.c,%.d,$(SRCS))
#################################################
all: $(EXECUTABLE)
 
$(EXECUTABLE): $(DEPS) $(OBJS)
	$(CC) -o $(EXECUTABLE) $(OBJS) $(LIBS)
 
%.d: %.c
	$(CC) -M $(CPPFLAGS) $< > $@
	$(CC) -M $(CPPFLAGS) $< | sed s/\\.o/.d/ > $@
 
%.d: %.h
	$(CC) -M $(CPPFLAGS) $< > $@
	$(CC) -M $(CPPFLAGS) $< | sed s/\\.o/.d/ > $@
clean:
	-rm $(OBJS) $(EXECUTABLE) $(DEPS) *~
 
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
 
clean-emacs:
	rm *~
