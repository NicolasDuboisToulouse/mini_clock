CFLAGS=`pkg-config gtk+-2.0 --cflags`
LDFLAGS=`pkg-config gtk+-2.0 --libs`


clock: clock.o
	gcc -Wall $< -o $@ $(LDFLAGS)

clock.o: clock.c
	gcc -Wall -g -c $< -o $@ $(CFLAGS) -Werror

