# Makefile for mipsel-linux

# example: $(X)/elf32ltsmip.h.umpscore.x
X=/usr/local/share/umps2

# example: $(L)/libumps.o
L=/usr/local/lib/umps2

# example: $(B)/mipsel-linux-gcc, $(B)/umps2-elf2umps, ...
B=/usr/bin
B2=/usr/local/bin

# example: -I $(I)/uMPS2
I=/usr/include/

# example: $(SRC)/p1test.0.2.c, $(SRC)/tcb.c, $(SRC)/msg.c,
SRC = src/

#file oggetto
OBJ = boot.o exception.o interrupt.o msg.o utils.o p2test.0.2.o prgTrap.o scheduler.o ssi.o tcb.o tlbTrap.o

CFLAGS  = -Wall -D_MIPSEL -c -Iheader/ -Iheader/uMPS_include/ -Iinterface/ -I/usr/local/include/umps2/umps/
LDFLAGS = -T $(X)/elf32ltsmip.h.umpscore.x $(L)/libumps.o $(L)/crtso.o
CC = $(B)/mipsel-linux-gcc
LD = $(B)/mipsel-linux-ld

all: kernel post-build clean
	@echo "Spostati i file per umps2 nella cartella build!!"


#azioni da compiere dopo compilazione e linking
post-build: elf-2-umps move_build_folder
#post-build: elf-2-umps

elf-2-umps:
	@$(B2)/umps2-elf2umps -k kernel
	
move_build_folder:
	@mkdir -p build
	@cp -f kernel	        /home/mone/NetBeansProjects/Amikaya_GCC/dist/kernel
	@cp -f kernel.core.umps /home/mone/NetBeansProjects/Amikaya_GCC/dist/kernel.core.umps
	@cp -f kernel.stab.umps /home/mone/NetBeansProjects/Amikaya_GCC/dist/kernel.stab.umps
	@mv -f kernel	        dist/kernel
	@mv -f kernel.core.umps dist/kernel.core.umps
	@mv -f kernel.stab.umps dist/kernel.stab.umps
	@echo "Spostati i file per umps2 nella cartella build!!"

	

#compilazione e linkaggio della prima parte del progetto

kernel:	$(OBJ)
	@$(LD) $(LDFLAGS) ${OBJ} -o kernel
	@echo "Linkato il tutto..."
	

#%.o: ${SRC}%.c
#	${CC} ${CFLAGS} $*.c
#	@echo ${COMPILED}

boot.o: ${SRC}boot.c
	${CC} ${CFLAGS} $<
	@echo ${COMPILED}
	
utils.o: ${SRC}utils.c
	${CC} ${CFLAGS} $<
	@echo ${COMPILED}	
	
exception.o: ${SRC}exception.c
	${CC} ${CFLAGS} $<
	@echo ${COMPILED}
	
tlbTrap.o: ${SRC}tlbTrap.c
	${CC} ${CFLAGS} $<
	@echo ${COMPILED}

prgTrap.o: ${SRC}prgTrap.c
	${CC} ${CFLAGS} $<
	@echo ${COMPILED}
	
scheduler.o: ${SRC}scheduler.c
	${CC} ${CFLAGS} $<
	@echo ${COMPILED}	

interrupt.o: ${SRC}interrupt.c
	${CC} ${CFLAGS} $<
	@echo ${COMPILED}	

ssi.o: ${SRC}ssi.c
	${CC} ${CFLAGS} $<
	@echo ${COMPILED}	
	
p2test.0.2.o: ${SRC}p2test.0.2.c
	${CC} ${CFLAGS} $<
	@echo ${COMPILED}

tcb.o: ${SRC}tcb.c
	${CC} ${CFLAGS} $<
	@echo ${COMPILED}
	
msg.o: ${SRC}msg.c
	${CC} ${CFLAGS} $<
	@echo ${COMPILED}

clean:
	@rm -f *.o
	@echo "Clean directory..."

clean-all:
	@rm -f build/*.umps
	@rm -f build/kernel
	@rm -f *.o
	@echo "Clean directory..."


first-release:
	@echo "\nViene compilato con la nostra prima versione!!\n"
	$(MAKE) -f Makefile-Our-Release.mk 





