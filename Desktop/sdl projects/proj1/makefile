CFLAGS = -I/opt/homebrew/include -D_THREAD_SAFE
LDFLAGS = -L/opt/homebrew/lib -lSDL2

main: main.c
	clang $(CFLAGS) main.c -o main $(LDFLAGS)
clean:
	rm -f main
