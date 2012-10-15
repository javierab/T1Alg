CC=gcc
DEPS = tarea1.h RTree.h rect.h

_OBJ = RTree.o rect.o selectInsertMethod.o node.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

ODIR=obj



all: selectInsertMethod


selectInsertMethod: $(OBJ)
	gcc -o $@ $^ -lm
	mkdir -p files
	rm files/*

objdir:
	mkdir -p obj

$(ODIR)/%.o: %.c objdir $(DEPS)
	$(CC) -c -o $@ $< 
	

clean:
	rm -f $(ODIR)/*.o 

cleanall:
	rm -f $(ODIR)/*.o tarea1
