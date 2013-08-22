#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/utils.o \
	${OBJECTDIR}/src/exception.o \
	${OBJECTDIR}/src/interrupt.o \
	${OBJECTDIR}/src/p2test.0.2.o \
	${OBJECTDIR}/src/scheduler.o \
	${OBJECTDIR}/src/boot.o \
	${OBJECTDIR}/src/prgTrap.o \
	${OBJECTDIR}/src/tcb.o \
	${OBJECTDIR}/src/ssi.o \
	${OBJECTDIR}/src/msg.o \
	${OBJECTDIR}/src/tlbTrap.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/amikaya_gcc

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/amikaya_gcc: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/amikaya_gcc ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/src/utils.o: src/utils.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/utils.o src/utils.c

${OBJECTDIR}/interface/ssi.e.gch: interface/ssi.e 
	${MKDIR} -p ${OBJECTDIR}/interface
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o $@ interface/ssi.e

${OBJECTDIR}/interface/p2test.0.2.e.gch: interface/p2test.0.2.e 
	${MKDIR} -p ${OBJECTDIR}/interface
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o $@ interface/p2test.0.2.e

${OBJECTDIR}/src/exception.o: src/exception.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/exception.o src/exception.c

${OBJECTDIR}/src/interrupt.o: src/interrupt.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/interrupt.o src/interrupt.c

${OBJECTDIR}/interface/tlbTrap.e.gch: interface/tlbTrap.e 
	${MKDIR} -p ${OBJECTDIR}/interface
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o $@ interface/tlbTrap.e

${OBJECTDIR}/src/p2test.0.2.o: src/p2test.0.2.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/p2test.0.2.o src/p2test.0.2.c

${OBJECTDIR}/interface/exception.e.gch: interface/exception.e 
	${MKDIR} -p ${OBJECTDIR}/interface
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o $@ interface/exception.e

${OBJECTDIR}/interface/boot.e.gch: interface/boot.e 
	${MKDIR} -p ${OBJECTDIR}/interface
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o $@ interface/boot.e

${OBJECTDIR}/src/scheduler.o: src/scheduler.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/scheduler.o src/scheduler.c

${OBJECTDIR}/interface/interrupt.e.gch: interface/interrupt.e 
	${MKDIR} -p ${OBJECTDIR}/interface
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o $@ interface/interrupt.e

${OBJECTDIR}/src/boot.o: src/boot.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/boot.o src/boot.c

${OBJECTDIR}/src/prgTrap.o: src/prgTrap.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/prgTrap.o src/prgTrap.c

${OBJECTDIR}/src/tcb.o: src/tcb.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/tcb.o src/tcb.c

${OBJECTDIR}/interface/scheduler.e.gch: interface/scheduler.e 
	${MKDIR} -p ${OBJECTDIR}/interface
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o $@ interface/scheduler.e

${OBJECTDIR}/src/ssi.o: src/ssi.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/ssi.o src/ssi.c

${OBJECTDIR}/src/msg.o: src/msg.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/msg.o src/msg.c

${OBJECTDIR}/interface/msg.e.gch: interface/msg.e 
	${MKDIR} -p ${OBJECTDIR}/interface
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o $@ interface/msg.e

${OBJECTDIR}/interface/tcb.e.gch: interface/tcb.e 
	${MKDIR} -p ${OBJECTDIR}/interface
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o $@ interface/tcb.e

${OBJECTDIR}/src/tlbTrap.o: src/tlbTrap.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/tlbTrap.o src/tlbTrap.c

${OBJECTDIR}/interface/prgTrap.e.gch: interface/prgTrap.e 
	${MKDIR} -p ${OBJECTDIR}/interface
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o $@ interface/prgTrap.e

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/amikaya_gcc

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
