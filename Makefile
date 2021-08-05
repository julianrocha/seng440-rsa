CC     := gcc
CFLAGS := -I. -Wundef -Wall -Wextra  -O3 # note the optimization flag is being used

V2_FOLDER=v2_bignum/
V3_FOLDER=v3_bignum_mont/
V4_FOLDER=v4_bignum_mont_optimized/

RSA_FILE_NAME_V2=$(V2_FOLDER)rsa
BIGNUM_FILE_NAME_V2=$(V2_FOLDER)bignum

RSA_FILE_NAME_V3=$(V3_FOLDER)/rsa
BIGNUM_FILE_NAME_V3=$(V3_FOLDER)/bignum

RSA_FILE_NAME_V4=$(V4_FOLDER)/rsa
BIGNUM_FILE_NAME_V4=$(V4_FOLDER)/bignum

all: run_rsa 

build_v2:
	@$(CC) $(CFLAGS) $(BIGNUM_FILE_NAME_V2).c $(RSA_FILE_NAME_V2).c -o $(RSA_FILE_NAME_V2).o

run_v2: build_v2
	@./$(RSA_FILE_NAME_V2).o

build_v3:
	@$(CC) $(CFLAGS) $(BIGNUM_FILE_NAME_V3).c $(RSA_FILE_NAME_V3).c -o $(RSA_FILE_NAME_V3).o

run_v3: build_v3
	@./$(RSA_FILE_NAME_V3).o

build_v4:
	@$(CC) $(CFLAGS) $(BIGNUM_FILE_NAME_V4).c $(RSA_FILE_NAME_V4).c -o $(RSA_FILE_NAME_V4).o

run_v4: build_v4
	@./$(RSA_FILE_NAME_V4).o


clean:
	@rm $(V2_FOLDER)*.o
	@rm $(V3_FOLDER)*.o
	@rm $(V4_FOLDER)*.o
	@rm $(V2_FOLDER)*.out
	@rm $(V3_FOLDER)*.out
	@rm $(V4_FOLDER)*.out
