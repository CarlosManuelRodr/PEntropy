VERSION=10.0

OS := $(shell uname)
ifeq ($(OS),Darwin)
MLINKDIR = /Applications/Mathematica.app/Contents/SystemFiles/Links/MathLink/DeveloperKit/MacOSX-x86-64
EXTRA_LIBS = -lMLi4 -lc++ -framework Foundation
else
MLINKDIR = /opt/Mathematica/SystemFiles/Links/MathLink/DeveloperKit/Linux-x86-64
EXTRA_LIBS = -lML64i4 -lm -lpthread -lrt -lstdc++ -ldl -luuid
endif

CADDSDIR = ${MLINKDIR}/CompilerAdditions
DEFINES = MATHEMATICA
EXTRA_CFLAGS = -std=c++11 -m64 -D ${DEFINES}

INCDIR = ${CADDSDIR} -I../common/
LIBDIR = ${CADDSDIR}
OBJDIR = obj
BINDIR = bin
OUTPUT = $(BINDIR)/PEntropy

MPREP = ${CADDSDIR}/mprep
CXX = /usr/bin/c++

clean: 
	@echo "##########################"
	@echo "#                        #"
	@echo "#      Cleaning up       #"
	@echo "#                        #"
	@echo "##########################"
	@echo ""
	@rm -rf $(OBJDIR) $(BINDIR)
	

before: 
	@echo ""
	@echo "###########################################"
	@echo "#                                         #"
	@echo "#   Building PEntropy (for Mathematica)   #"
	@echo "#                                         #"
	@echo "###########################################"
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

mathematica: before build after

build: $(OBJDIR)/mathlinktm.o $(OBJDIR)/mathlink.o $(OBJDIR)/pentropy.o
	${CXX} ${EXTRA_CFLAGS} -I${INCDIR} $(OBJDIR)/mathlinktm.o $(OBJDIR)/mathlink.o $(OBJDIR)/pentropy.o -L${LIBDIR} ${EXTRA_LIBS} -o $(OUTPUT)
	
$(OBJDIR)/mathlinktm.o: $(OBJDIR)/mathlinktm.cpp
	${CXX} -c ${EXTRA_CFLAGS} -I${INCDIR} $(OBJDIR)/mathlinktm.cpp -o $@
	
$(OBJDIR)/mathlink.o: mathlink.cpp
	$(CXX) -c $(EXTRA_CFLAGS) -I${INCDIR} mathlink.cpp -o $(OBJDIR)/mathlink.o

$(OBJDIR)/pentropy.o: ../common/pentropy.cpp
	$(CXX) -c $(EXTRA_CFLAGS) -I${INCDIR} ../common/pentropy.cpp -o $(OBJDIR)/pentropy.o
	
$(OBJDIR)/mathlinktm.cpp : mathlink.tm
	${MPREP} $? -o $@
	
	
.DEFAULT_GOAL := mathematica

.PHONY: before after