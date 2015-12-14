CC=mpiCC

RUN=mpirun
RFLAGS=-np

EXEC=multmat
OUT=out.m

RM=rm -f

PROC=64
SEED=17
# 1 = Fox / 2 = Cannon / 3 = DNS
ALGO=2
# Size of the sub-matrix
DATA=8

$(EXEC): multmat.o fox.o dns.o cannon.o
	@$(CC) $^ -o $@

multmat.o: multmat.cc
	@$(CC) -c $<

fox.o: fox.cc fox.h
	@$(CC) -c $<

dns.o: dns.cc dns.h
	@$(CC) -c $<

cannon.o: cannon.cc cannon.h
	@$(CC) -c $<

run: $(EXEC)
	@$(RUN) $(RFLAGS) $(PROC) $< $(ALGO) $(SEED) $(DATA)

test: $(EXEC)
	@$(RUN) $(RFLAGS) $(PROC) $< $(ALGO) $(SEED) $(DATA) >> $(OUT)

clean:
	@$(RM) *.o
	@$(RM) out.m
	@$(RM) $(EXEC)
