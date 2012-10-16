CC=gcc

ODIR=obj


all: insertSequence searchSequence deleteSequence idiSequence


idiSequence: RTree.c rect.c node.c idiSequence.c
	gcc -o idiSequence RTree.c rect.c node.c idiSequence.c -lm
	mkdir -p files


deleteSequence: RTree.c rect.c node.c deleteSequence.c
	gcc -o deleteSequence RTree.c rect.c node.c deleteSequence.c -lm
	mkdir -p files

insertSequence: RTree.c rect.c node.c insertSequence.c
	gcc -o insertSequence RTree.c rect.c node.c insertSequence.c -lm
	mkdir -p files
	
searchSequence: RTree.c rect.c node.c searchSequence.c
	gcc -o searchSequence RTree.c rect.c node.c searchSequence.c -lm
	mkdir -p files

cleantree:
	rm Tree
	rm files/*

clean:
	rm -f $(ODIR)/*.o 

cleanall:
	rm -f $(ODIR)/*.o tarea1
