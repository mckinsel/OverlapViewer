CA_DIR=../../wgs-8.1

# A bunch of stuff from the Celera Assembler Makefile
LDFLAGS= -D_GLIBCXX_PARALLEL -fopenmp -pthread -lm -lz -L $(CA_DIR)/Linux-amd64/lib -lCA
CXXFLAGS:= -D_GLIBCXX_PARALLEL -fopenmp
CXXFLAGS+= -pthread -Wall -Wextra -Wno-write-strings -Wno-unused -Wno-char-subscripts -Wno-sign-compare -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -DX86_GCC_LINUX
CA_SUBDIRS = AS_RUN \
             AS_UTL \
             AS_UID \
             AS_MSG \
             AS_PER \
             AS_GKP \
             AS_OBT \
             AS_MER \
             AS_OVL \
             AS_OVM \
             AS_OVS \
             AS_ALN \
             AS_CGB \
             AS_BOG \
             AS_BAT \
             AS_PBR \
             AS_REZ \
             AS_CNS \
             AS_LIN \
             AS_CGW \
             AS_TER \
             AS_ENV \
             AS_REF

INC_IMPORT_DIRS += $(CA_DIR)/src $(patsubst %, $(CA_DIR)/src/%, $(strip $(CA_SUBDIRS)))
INC_DIRS = $(patsubst %, -I%, \ $(strip $(INC_IMPORT_DIRS)))

CXXFLAGS+=$(INC_DIRS)

LIB_SOURCES = CAReader.cpp Overlap.cpp OverlapPrinter.cpp
EXE_SOURCES = overlap_view.cpp

LIB_OBJECTS = $(patsubst %.cpp,%.o,$(strip $(LIB_SOURCES))) 
EXE_OBJECTS = $(patsubst %.cpp,%.o,$(strip $(EXE_SOURCES))) 

TARGETS = $(patsubst %.o,%,$(EXE_OBJECTS))

all: $(LIB_OBJECTS) $(EXE_OBJECTS) $(TARGETS)

overlap_view: $(LIB_OBJECTS) overlap_view.o
	$(CXX) $^ -o $@ $(LDFLAGS)
