CC     := gcc
CFLAGS := -I. -Wundef -Wall -Wextra -O

RSA_FILE_NAME=rsa
BIGNUM_FILE_NAME=bignum

all: run_rsa run_bignum

build_rsa:
	@$(CC) $(CFLAGS) $(BIGNUM_FILE_NAME).c $(RSA_FILE_NAME).c -o $(RSA_FILE_NAME).o

run_rsa: build_rsa
	@./$(RSA_FILE_NAME).o

build_bignum:
	@$(CC) $(CFLAGS) $(BIGNUM_FILE_NAME).c $(BIGNUM_FILE_NAME)_tester.c -o $(BIGNUM_FILE_NAME).o	

run_bignum: build_bignum
	@./$(BIGNUM_FILE_NAME).o

clean:
	@rm *.o