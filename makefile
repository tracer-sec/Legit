CC=g++
CFLAGS=-c -Wall -std=c++11
LDFLAGS=-Wl,-rpath,/usr/local/lib
AR=ar
ARFLAGS=-rcs

SRCDIR=Legit
TSTDIR=LegitTests
OBJDIR=build
BINDIR=bin

SOURCES= $(shell find $(SRCDIR) -type f -name *.cpp)
OBJECTS= $(patsubst $(SRCDIR)/%,$(OBJDIR)/$(SRCDIR)/%,$(SOURCES:.cpp=.o))
TESTSOURCES = $(shell find $(TSTDIR) -type f -name *.cpp)
TESTOBJECTS = $(patsubst $(TSTDIR)/%,$(OBJDIR)/$(TSTDIR)/%,$(TESTSOURCES:.cpp=.o))

LIB=$(BINDIR)/liblegit.a
TESTS=$(BINDIR)/LegitTests

all: setup $(LIB) $(TESTS)

$(LIB): $(OBJECTS) ; $(AR) $(ARFLAGS) $(LIB) $(OBJECTS)
$(TESTS): $(LIB) $(TESTOBJECTS) ; $(CC) $(LDFLAGS) $(TESTOBJECTS) -o $@ -L$(BINDIR) -L../../lib/Botan-2.0.1 -llegit -lbotan-2

$(OBJDIR)/$(SRCDIR)/%.o: $(SRCDIR)/%.cpp ; $(CC) $(CFLAGS) -o $@ $< -I../../lib/Botan-2.0.1/build/include
$(OBJDIR)/$(TSTDIR)/%.o: $(TSTDIR)/%.cpp ; $(CC) $(CFLAGS) -o $@ $< -ILegit

clean: ; rm -f $(BINDIR)/* $(OBJDIR)/$(SRCDIR)/* $(OBJDIR)/$(TSTDIR)/*

setup: ; mkdir -p $(BINDIR) $(OBJDIR)/$(SRCDIR) $(OBJDIR)/$(TSTDIR)

print-%  : ; @echo $* = $($*)

