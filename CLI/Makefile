INCDIR = ../common/
OBJDIR = obj
BINDIR = bin

CXXFLAGS = -std=c++11 -pthread -O2
CXX = /usr/bin/c++ $(CXXFLAGS)
LD = /usr/bin/c++
OUTPUT = $(BINDIR)/PEntropy

OBJ = \
$(OBJDIR)/main.o \
$(OBJDIR)/pentropy.o
	
# CLI
all: cli

before: 
	@echo ""
	@echo "################################"
	@echo "#                              #"
	@echo "#      Building PEntropy       #"
	@echo "#                              #"
	@echo "################################"
	@echo ""
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)

after: 
	@echo ""
	@echo "#############################"
	@echo "#                           #"
	@echo "#            Done.          #"
	@echo "#                           #"
	@echo "#############################"
	@echo ""

clean: 
	@echo "######################"
	@echo "#                    #"
	@echo "#    Cleaning up     #"
	@echo "#                    #"
	@echo "######################"
	@echo ""
	@rm -f $(OBJ) $(OUTPUT)
	@rm -fd $(BINDIR) $(OBJDIR)
	
cli: before build after

build: $(OBJ)
	$(LD) $(OBJ) -o $(OUTPUT)


$(OBJDIR)/pentropy.o: ../common/pentropy.cpp
	$(CXX) -I $(INCDIR) -c ../common/pentropy.cpp -o $(OBJDIR)/pentropy.o

$(OBJDIR)/main.o: main.cpp
	$(CXX) -I $(INCDIR) -c main.cpp -o $(OBJDIR)/main.o

	
.DEFAULT_GOAL := all

.PHONY: before after clean