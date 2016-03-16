VERSION=10.0
MLINKDIR = /opt/Mathematica/SystemFiles/Links/MathLink/DeveloperKit/Linux-x86-64
SYS = Linux-x86-64
CADDSDIR = ${MLINKDIR}/CompilerAdditions
DEFINES = MATHEMATICA
EXTRA_CFLAGS=-m64 -D ${DEFINES}

INCDIR = ${CADDSDIR}
LIBDIR = ${CADDSDIR}
SRCDIR = PEntropy
OBJDIR = obj
BINDIR = bin

MPREP = ${CADDSDIR}/mprep
RM = rm

CC = /usr/bin/cc
CXX = /usr/bin/c++ -std=c++11 -pthread

# Shared
clean: 
	@echo "##########################"
	@echo "#                        #"
	@echo "#      Cleaning up       #"
	@echo "#                        #"
	@echo "##########################"
	@echo ""
	@rm -rf $(OBJDIR) $(BINDIR)
	

# Mathematica

before_mathematica: 
	@echo ""
	@echo "########################################"
	@echo "#                                      #"
	@echo "# Building PEntropy (for Mathematica)  #"
	@echo "#                                      #"
	@echo "########################################"
	@echo ""
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)

after_mathematica: 
	@echo ""
	@echo "#############################"
	@echo "#                           #"
	@echo "#            Done.          #"
	@echo "#                           #"
	@echo "#############################"
	@echo ""

mathematica : before_mathematica $(BINDIR)/MathematicaPEntropy after_mathematica

$(BINDIR)/MathematicaPEntropy : $(OBJDIR)/maintm.o $(OBJDIR)/main.o
	${CXX} ${EXTRA_CFLAGS} -I${INCDIR} $(OBJDIR)/maintm.o $(OBJDIR)/main.o -L${LIBDIR} -lML64i4 -lm -lpthread -lrt -lstdc++ -ldl -luuid -o $@
	
$(OBJDIR)/maintm.o : $(OBJDIR)/maintm.c
	${CXX} -c ${EXTRA_CFLAGS} -I${INCDIR} $(OBJDIR)/maintm.c -o $@
	
$(OBJDIR)/main.o: PEntropy/main.cpp
	$(CXX) -c $(EXTRA_CFLAGS) -I${INCDIR} $(SRCDIR)/main.cpp -o $(OBJDIR)/main.o
	

$(OBJDIR)/maintm.c : $(SRCDIR)/main.tm
	${MPREP} $? -o $@
	
	
# CLI
all: cli

before_cli: 
	@echo ""
	@echo "################################"
	@echo "#                              #"
	@echo "#      Building PEntropy       #"
	@echo "#                              #"
	@echo "################################"
	@echo ""
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)

after_cli: 
	@echo ""
	@echo "#############################"
	@echo "#                           #"
	@echo "#            Done.          #"
	@echo "#                           #"
	@echo "#############################"
	@echo ""
	
cli: before_cli build_cli out_cli after_cli

build_cli: PEntropy/main.cpp
	$(CXX) -c $(SRCDIR)/main.cpp -o $(OBJDIR)/main.o

out_cli: $(OBJDIR)/main.o
	$(CXX) $(OBJDIR)/main.o -o $(BINDIR)/PEntropy
	
.DEFAULT_GOAL := all

.PHONY: before_cli after_cli