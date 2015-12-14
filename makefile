CC=mpiCC

RUN=mpirun
RFLAGS=-np

EXEC=multmat_all
OUT=out.m

RM=rm -f

PROC=144
SEED=17
# 1 = Fox / 2 = Cannon / 3 = DNS
ALGO=1
# Size of the sub-matrix
DATA=12

$(EXEC): multmat_all.o fox.o dns.o cannon.o
	@$(CC) $^ -o $@

multmat_all.o: multmat_all.cc
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
