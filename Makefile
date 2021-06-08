FILE_NAME= mock1

all: build run

build:
	@gcc $(FILE_NAME).c -o $(FILE_NAME).o

run: build
	@./$(FILE_NAME).o

clean:
	@rm *.o