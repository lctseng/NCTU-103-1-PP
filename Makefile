all:
	gcc pi.c -O2 -o hw1_pthread -lpthread
run:	
	./pi 5000000
