CC=g++
CFLAGS=-pthread -O3 -std=c++11 -w -march=core-avx2 -c -o $@
LFLAGS=-pthread -O3 -std=c++11 -w -march=core-avx2 -fopenmp

all: Solver

Solver: src/solver.cpp obj/configfile.o obj/endgame_negamax.o obj/game.o obj/hashtable.o src/macros_hell.h obj/move.o obj/perft_basic.o obj/datamanipulation.o src/selectivity.h obj/search.o obj/screen.o src/worker.h
	$(CC) $(LFLAGS) obj/hashtable.o obj/utility.o obj/search.o obj/configfile.o obj/game.o obj/count_last_flip.o obj/flip_fast.o obj/move.o obj/datamanipulation.o obj/features.o obj/position.o src/possiblemoves.cpp src/possiblemoves_sse2.cpp src/possiblemoves_avx2.cpp src/possiblemoves_avx512.cpp obj/screen.o src/solver.cpp -o bin/solver_haswell

obj/configfile.o: src/configfile.cpp src/configfile.h
	$(CC) $(CFLAGS) src/configfile.cpp

obj/count_last_flip.o: src/count_last_flip.cpp src/count_last_flip.h src/macros_hell.h
	$(CC) $(CFLAGS) src/count_last_flip.cpp

obj/datamanipulation.o: src/datamanipulation.cpp src/datamanipulation.h src/macros_hell.h obj/utility.o obj/position.o
	$(CC) $(CFLAGS) src/datamanipulation.cpp

obj/endgame_negamax.o: src/endgame_negamax.cpp src/endgame_negamax.h src/macros_hell.h obj/flip_fast.o obj/count_last_flip.o obj/utility.o obj/possiblemoves.o obj/position.o src/game.h
	$(CC) $(CFLAGS) src/endgame_negamax.cpp

obj/features.o: src/features.cpp src/features.h obj/datamanipulation.o obj/position.o obj/utility.o obj/configfile.o
	$(CC) $(CFLAGS) src/features.cpp

obj/featureweightfitter.o: src/featureweightfitter.cpp src/featureweightfitter.h obj/datamanipulation.o obj/features.o obj/position.o src/statistics.h
	$(CC) $(CFLAGS) src/featureweightfitter.cpp

obj/flip_fast.o: src/flip_fast.cpp src/flip_fast.h src/macros_hell.h
	$(CC) $(CFLAGS) src/flip_fast.cpp

obj/flip_loop.o: src/flip_loop.cpp src/flip_loop.h
	$(CC) $(CFLAGS) src/flip_loop.cpp

obj/flip_parallel.o: src/flip_parallel.cpp src/flip_parallel.h
	$(CC) $(CFLAGS) src/flip_parallel.cpp

obj/game.o: src/game.cpp src/game.h src/macros_hell.h obj/flip_fast.o obj/count_last_flip.o obj/utility.o obj/possiblemoves.o obj/position.o obj/hashtable.o src/move.h src/line.h
	$(CC) $(CFLAGS) src/game.cpp

obj/generatepositions.o: src/generatepositions.cpp src/generatepositions.h obj/datamanipulation.o src/macros_hell.h obj/position.o obj/flip_fast.o obj/possiblemoves.o obj/utility.o
	$(CC) $(CFLAGS) src/generatepositions.cpp

obj/hashtable.o: src/hashtable.cpp src/hashtable.h obj/datamanipulation.o
	$(CC) $(CFLAGS) src/hashtable.cpp

obj/move.o: src/move.cpp src/move.h obj/position.o obj/hashtable.o obj/utility.o obj/features.o
	$(CC) $(CFLAGS) src/move.cpp

obj/perft_basic.o: src/perft_basic.cpp src/perft_basic.h obj/flip_fast.o src/macros_hell.h obj/possiblemoves.o obj/position.o obj/utility.o
	$(CC) $(CFLAGS) src/perft_basic.cpp

obj/position.o: src/position.cpp src/position.h obj/flip_fast.o obj/possiblemoves.o obj/utility.o src/macros_hell.h
	$(CC) $(CFLAGS) src/position.cpp

obj/possiblemoves.o: src/possiblemoves.h src/possiblemoves.cpp src/possiblemoves_sse2.cpp src/possiblemoves_avx2.cpp src/possiblemoves_avx512.cpp src/macros_hell.h
	$(CC) $(CFLAGS) src/position.cpp

obj/screen.o: src/screen.cpp src/screen.h
	$(CC) $(CFLAGS) src/screen.cpp

obj/search.o: src/search.cpp src/search.h src/macros_hell.h src/line.h obj/utility.o obj/configfile.o obj/game.o src/selectivity.h obj/screen.o
	$(CC) $(CFLAGS) src/search.cpp

obj/utility.o: src/utility.cpp src/utility.h src/macros_hell.h
	$(CC) $(CFLAGS) src/utility.cpp

clean:
	rm -f obj/*.o bin/solver_haswell
