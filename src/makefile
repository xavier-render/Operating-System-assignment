CC = gcc
OBJ = scheduler.o simulator.o
EXEC = program
CFLAGS = -Wall

#--------PROGRAM-EXECUTEABLES-----------------------------------------
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) -lpthread

#--------OBJECTS------------------------------------------------------
scheduler.o: scheduler.c scheduler.h
	$(CC) -c scheduler.c $(CFLAGS)

simulator.o: simulator.c scheduler.h	 
	$(CC) -c simulator.c $(CFLAGS)

#--------UTILS--------------------------------------------------------
clean:
	rm -f $(OBJ) $(EXEC)
