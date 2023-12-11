CC=gcc

MLFQ-Scheduler: main.o scheduler.o linkedlist.o queue.o
		$(CC) -o MLFQ-Scheduler scheduler.o main.o linkedlist.o queue.o

main.o: main.c
		$(CC) -c -o main.o main.c

scheduler.o: scheduler.c
		$(CC) -c -o scheduler.o scheduler.c

queue.o: queue.c
		$(CC) -c -o queue.o queue.c

linkedlist.o: linkedlist.c
		$(CC) -c -o linkedlist.o linkedlist.c

clean:
		rm *.o & rm MLFQ-Scheduler
