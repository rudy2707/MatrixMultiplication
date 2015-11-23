CC=mpiCC

RUN=mpirun
RFLAGS=-np

EXEC=multmat

RM=rm -f

PROC=4
SEED=17
ALGO=1
DATA=5

$(EXEC): multmat.o fox.o dns.o cannon.o 
	$(CC) $^ -o $@

multmat.o: multmat.cc
	$(CC) -c $<

fox.o: fox.cc fox.h
	$(CC) -c $<

dns.o: dns.cc dns.h
	$(CC) -c $<

cannon.o: cannon.cc cannon.h
	$(CC) -c $<

run: $(EXEC)
	$(RUN) $(RFLAGS) $(PROC) $< $(SEED) $(ALGO) $(DATA)

clean:
	$(RM) *.o
	$(RM) $(EXEC)
