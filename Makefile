all: install

install:
	gcc -std=gnu99 -o main main.c -lm
test:
	./main.out
clean:
	rm main.out
