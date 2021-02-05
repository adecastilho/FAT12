T1 = diskinfo
T2 = disklist
T3 = diskget
T4 = diskput
O1 = diskinfo.o helpers.o 
O2 = disklist.o helpers.o DirList.o
O3 = diskget.o helpers.o 
O4 = diskput.o helpers.o 
CFLAGS = -Wall

.phony all:

all: $(T1) $(T2) $(T3) $(T4)

$(T1): $(O1)
	gcc $(CFLAGS) -o $(T1) $(O1) 
$(T2): $(O2)
	gcc $(CFLAGS) -o $(T2) $(O2) 
$(T3): $(O3)
	gcc $(CFLAGS) -o $(T3) $(O3) 
$(T4): $(O4)
	gcc $(CFLAGS) -o $(T4) $(O4)    


.PHONY clean:
clean:
	-rm -rf *.o *.exe
 