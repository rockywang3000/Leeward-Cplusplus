CC = cc -O3
CPP = g++ -O3

all: leeward

%.o: %.c
	$(CC) -c $(<) -o $(@)

%.o: %.cpp
	$(CPP) -c $(<) -o $(@)

leeward: sptest.o matrix.o nrutil.o set.o spallocate.o spbuild.o spfactor.o spoutput.o spsolve.o sputils.o leeward.o
	$(CPP) -o leeward sptest.o matrix.o nrutil.o set.o spallocate.o spbuild.o spfactor.o spoutput.o spsolve.o sputils.o leeward.o -lm


clean		:
	rm -f sptest.o matrix.o nrutil.o set.o spallocate.o spbuild.o spfactor.o spoutput.o spsolve.o sputils.o leeward.o leeward core
