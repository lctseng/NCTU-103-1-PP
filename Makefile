all:
	gcc pi.c  -o pi -lpthread
	./pi 5000000
run:	
	./pi 5000000
