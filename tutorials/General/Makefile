#**************************************
#
#  a directory obj must be created in the running directory
#
#  make depgen
#  make clean; make;
#
#  
#
#***************************************
#
# Name of the main file in the top directory
# the file extension must be same as SEXT


TARGET = myAna

REPDIR = ../../TShine

#
# -- Choose compiler, clang for MacOS
#

CXX = clang++  
#CXX = g++ -fopenmp
#CXX = g++
#CXX = /usr/bin/g++44


##########################################
#
# normally no need to change below here
#

INCDIR = ${REPDIR}/include

SRCDIR = ${REPDIR}/src

OBJDIR = obj
SEXT = cc
OEXT = o

INC = -I./$(INCDIR) 
TOPDIR = ${PWD}

#
# ===================================================================
#


ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLDFLAGS   := $(shell root-config --ldflags)
ROOTLIBS     := -lTreePlayer $(shell root-config --libs) 
ROOTGLIBS    := $(shell root-config --glibs)

LIBS          = $(ROOTLIBS) -lMinuit2 
GLIBS         = $(ROOTGLIBS) -lMinuit2

OBJTARG = ${OBJDIR}/${TARGET}.${OEXT}
SRCTARG = $(TARGET).$(SEXT)

SRCS = $(wildcard $(SRCDIR)/*$(SEXT))

#SRCS = $(shell find $(SRCDIR) -name '*$(SEXT)' | sed 's!.*/!!')

#TMPLIST = $(SRCS:.${SEXT}=.${OEXT})
#OLIST=$(patsubst ${SRCDIR}%,${OBJDIR}%,$(TMPLIST))
OLIST=$(patsubst ${SRCDIR}%,${OBJDIR}%,$(subst .$(SEXT),.$(OEXT),$(SRCS)))

OBJS = ${OLIST}




# include option -std=c++0x to use library <tuple>
#OPT = -g -Wall
#OPT = -O3 -g -Wall -fPIC -fpermissive -Dlong32="int" -Dlong64="long long"
#OPT = -O3 -g -std=c++0x -fopenmp -Wall -fPIC -Dlong32="int" -Dlong64="long long"
#OPT = -O3 -g -fopenmp -fpermissive -Wall -fPIC -Dlong32="int" -Dlong64="long long"
OPT = -O3 -g  -std=c++0x -Wall -fPIC -Dlong32="int"  -Dlong64="long long"
#OPT = -O3 -g  -Wall -fPIC -Dlong32="int"  -Dlong64="long long"
#OPT = -pg -O3 -Wall -fPIC -Dlong32="int" -Dlong64="long long"


DEPFILE = .deps


all: ${OBJTARG} ${OBJS} 
	${CXX} ${OPT} ${ROOTLDFLAGS} ${INC} -o ${TARGET} ${OBJTARG} ${OBJS} ${ROOTLIBS} 

depend: depgen all


${OBJTARG}: $(SRCTARG)
	@echo ""
	@echo "====> " $@ " from " $<
	@echo "root options: " ${ROOTCFLAGS}
	${CXX} ${OPT} ${ROOTCFLAGS} -c ${TARGET}.${SEXT} -o ${OBJTARG} ${INC}
	@echo "-----"
	@echo " "


$(OBJDIR)/%.$(OEXT): $(SRCDIR)/%.$(SEXT) 
	@echo " "
	@echo " "
	@echo " "
	@echo "====== > build file: " $@ " from: " $(SRCDIR)/$(*F).cc 
	@echo "" 
	@echo " --->> Dependencies newer then the target: " $?
	@echo " "
#	F=$(subst .$(OEXT),.$(SEXT),$(patsubst ${OBJDIR}%,${SRCDIR}%,$@))
#	${CXX} ${OPT} ${ROOTCFLAGS} -c $(subst .${OEXT},.${SEXT},$(patsubst ${OBJDIR}%,${SRCDIR}%,$@)) -o $@ ${INC}
	${CXX} ${OPT} ${ROOTCFLAGS} -c  $(SRCDIR)/$(*F).cc -o $@ ${INC}



#${OBJS}: 	
#	@echo " " 
#	@echo "====== > build: " $@ 
#	@echo "compiling: " $(patsubst ${OBJDIR}%,${SRCDIR}%,$*).${SEXT}
#	${CXX} ${OPT} ${ROOTCFLAGS} -c $(patsubst ${OBJDIR}%,${SRCDIR}%,$*).${SEXT} -o $@ ${INC}
#	@echo " ---- "
#	@echo " "



#
# -- include dependencies
#

-include ${DEPFILE}

#
# --
#

list:
	@echo ""
	@echo " ---- list ---- "
	@echo "sources: " $(SRCS)
#	@echo "olist: " $(OLIST)
	@echo "objs:" $(OBJS)
	@echo ""
	@echo $(ROOTCFLAGS)
	@echo $(ROOTLDFLAGS)	
	@echo $(ROOTLIBS)		
	@echo $(ROOTGLIBS)
	@echo $(LIBS)
	@echo " ------ "

clean:
#	rm -rf ${OBJDIR}/*.${OEXT}
	rm -rf ${OBJDIR}/*
#	rm -rf ${OBJS}


depgen: 
	cd $(SRCDIR); makedepend -f- -p./$(OBJDIR)/ -o.$(OEXT) -I${TOPDIR}/${INCDIR} -- *.${SEXT} > ./.deptemp; mv ./.deptemp ${TOPDIR}/${DEPFILE} 
	cd ${TOPDIR}; makedepend -f-  -p./$(OBJDIR)/ -o.$(OEXT) -I${TOPDIR} -I${TOPDIR}/${INCDIR} -- ${TARGET}.${SEXT} >> ${DEPFILE}
