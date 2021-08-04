#include <stdio.h>
#include <time.h>
#include "bignum.h"

/*
PROJECT NOTES:
Design steps:
- Look up table ? SKIP FOR NOW
- Modular exponentiation (multiply and square algorithm)
- Montgomery Modular Multiplication
- unint96 union?

- More test cases (bigger numbers)
    - p and q should be half size of n (so ~96/2=48 bits each)

- Port to VM
- Profile
- Very long integer arithmetic routines ?
    - new from string
    - toString for printing
    - multiplication
    - > / <
    - &
    - % (remainder)

Optimization Considerations:
- arithmetic on very long integers (need to write routines for this)
- true data dependencies
- expensive modular and multiplication operations
- chinese remainder theorem for decryption (using p and q instead of n)
*/

void montMult(bignum  *x, bignum *y, bignum *m, int mBits, bignum *out){

	bignum t;
	bignum_init(&t);

	int i;
	for(i = mBits; i > 0 ; i--){					//efficient loop exit

		int t0Bit = bignum_getbit(&t,0);
		int xiBit = bignum_getbit(x, mBits - i);	//loop exit requires subtraction here
		int y0Bit = bignum_getbit(y,0);
		int op = t0Bit + (xiBit * y0Bit);

		if(xiBit == 1){
			bignum_add(&t, y, &t);
		}

		if(op == 1){
			bignum_add(&t, m, &t);
		}

		bignum_rshift(&t,&t, 1);
	}

	if(bignum_cmp(&t, m) >= 0){
		bignum_sub(&t,m,&t);
	}

	bignum_assign(out,&t);
}

/*
Helper method to compute: result = base**exp % modulus
"Square and multiply algorithm"
https://www.geeksforgeeks.org/exponential-squaring-fast-modulo-multiplication/ 
*/
void modular_exponentiation(bignum base, bignum exp, bignum modulus, bignum *result)
{
    bignum tmp_result;
    bignum tmp_base;
    bignum tmp_exp;

    bignum_from_int(result, 1);

    while (!bignum_is_zero(&exp))
    {
        if (exp.arr[0] & 1)
        {
            // result = (result * base) % modulus; // multiply step
            bignum_mul(result, &base, &tmp_result);
            bignum_mod(&tmp_result, &modulus, result);
        }
        bignum_rshift(&exp, &tmp_exp, 1);
        bignum_assign(&exp, &tmp_exp);

        // base = (base * base) % modulus;			// square step
        bignum_mul(&base, &base, &tmp_base);
        bignum_mod(&tmp_base, &modulus, &base);
    }
    bignum_mod(result, &modulus, &tmp_result);
    bignum_assign(result, &tmp_result);
}
/*
ORIGINAL ALGORITHM WITH PRIMITIVE TYPES:
{
	if (modulus == 1)
		return 0; // no computation to perform if modulus is 1
	unsigned long int result = 1;
	while (exp > 0)
	{
		if (exp & 1)							// exp % 2 != 0
			result = (result * base) % modulus; // multiply step
		exp >>= 1;								// exp /= 2
		base = (base * base) % modulus;			// square step
	}
	return result % modulus;
}*/

/*
c = t**e mod n
*/
void encrypt(bignum t, bignum e, bignum n, bignum *c)
{
    modular_exponentiation(t, e, n, c);
}

/*
t = c**d mod n
*/
void decrypt(bignum c, bignum d, bignum n, bignum *t)
{
    modular_exponentiation(c, d, n, t);
}

/*
- Modulus: n = pq
- Public exponent: e
- Private exponent: d
- Public key: (n, e)
- Private key: (n, d)
- Plaintext: t
- Ciphertext: c
*/
int main()
{
    printf("Type sizes on this machine (bits): char:%u, short:%u, int:%u, long:%u, long long:%u, size_t:%u\n", sizeof(unsigned char) * 8, sizeof(unsigned short) * 8, sizeof(unsigned int) * 8, sizeof(unsigned long) * 8, sizeof(unsigned long long) * 8, sizeof(size_t) * 8);

    // For this project, keys can be generated offline
    bignum n, e, d, t, c, t_decrypted, two, r_exp, r2m;
    int nBits;
    clock_t before;
    clock_t after;
    clock_t encrypt_cycles;
    clock_t decrypt_cycles;
    bignum_from_int(&two, 2);

     // 24 bit keys
    printf("\nTESTING 24 bit keys\n");
    char n_str_24[] = "000000000000000000a73911";
    char e_str_24[] = "000000000000000000010001";
    char d_str_24[] = "0000000000000000004a051d";
    char t_str_24[] = "000000000000000000080501";

    bignum_from_string(&n, n_str_24);
    bignum_from_string(&e, e_str_24);
    bignum_from_string(&d, d_str_24);
    bignum_from_string(&t, t_str_24);
    nBits = bignum_numbits(&n);
    // bignum_from_int(&r_exp, 2 * nBits);
    // modular_exponentiation(two, r_exp, n, &r2m);
    bignum_from_string(&r2m, "00000000000000000032f55d"); // pre-computed from code above

    printf("RSA Configuration: \n");
    printf("n: ");
    print_bignum(&n);
    printf("e: ");
    print_bignum(&e);
    printf("d: ");
    print_bignum(&d);
    printf("t: ");
    print_bignum(&t);
    printf("r2m");
    print_bignum(&r2m);
    printf("Number of bits in n: %d\n", nBits);

    printf("ENCRYPT...\n");
	before = clock();
    encrypt(t, e, n, &c);
	after = clock();
    encrypt_cycles = after - before;
    printf("c: ");
    print_bignum(&c);
    printf("DECRYPT...\n");
    before = clock();
    decrypt(c, d, n, &t_decrypted);
    after = clock();
    decrypt_cycles = after - before;
    printf("t: ");
    print_bignum(&t_decrypted); 
    printf("Encrypt Cycles: %ld\nDecrypt Cycles: %ld\n", encrypt_cycles, decrypt_cycles);

    // 48 bit keys
    printf("\nTESTING 48 bit keys\n");
    char n_str_48[] = "000000000000bc046e91ae5f";
    char e_str_48[] = "000000000000000000010001";
    char d_str_48[] = "0000000000009420e4147c29";
    char t_str_48[] = "000000000000500073000001";

    bignum_from_string(&n, n_str_48);
    bignum_from_string(&e, e_str_48);
    bignum_from_string(&d, d_str_48);
    bignum_from_string(&t, t_str_48);
    nBits = bignum_numbits(&n);
    // bignum_from_int(&r_exp, 2 * nBits);
    // modular_exponentiation(two, r_exp, n, &r2m);
    bignum_from_string(&r2m, "000000000000507fb204bae6"); // pre-computed from code above

    printf("RSA Configuration: \n");
    printf("n: ");
    print_bignum(&n);
    printf("e: ");
    print_bignum(&e);
    printf("d: ");
    print_bignum(&d);
    printf("t: ");
    print_bignum(&t);
    printf("r2m");
    print_bignum(&r2m);
    printf("Number of bits in n: %d\n", nBits);

    printf("ENCRYPT...\n");
	before = clock();
    encrypt(t, e, n, &c);
	after = clock();
    encrypt_cycles = after - before;
    printf("c: ");
    print_bignum(&c);
    printf("DECRYPT...\n");
    before = clock();
    decrypt(c, d, n, &t_decrypted);
    after = clock();
    decrypt_cycles = after - before;
    printf("t: ");
    print_bignum(&t_decrypted); 
    printf("Encrypt Cycles: %ld\nDecrypt Cycles: %ld\n", encrypt_cycles, decrypt_cycles);

    // 96 bit keys
    printf("\nTESTING 96 bit keys\n");
    char n_str_96[] = "000d2123f00d92d08643cd4f";
    char e_str_96[] = "000000000000000000010001";
    char d_str_96[] = "1f175280fc4501ce37cb57d1";
    char t_str_96[] = "400000050000000073000001";

    bignum_from_string(&n, n_str_96);
    bignum_from_string(&e, e_str_96);
    bignum_from_string(&d, d_str_96);
    bignum_from_string(&t, t_str_96);
    nBits = bignum_numbits(&n);
    // bignum_from_int(&r_exp, 2 * nBits);
    // modular_exponentiation(two, r_exp, n, &r2m);
    bignum_from_string(&r2m, "2db52cacc055e1ec000dc3c9"); // Must be pre-computed with double width bignums

    printf("RSA Configuration: \n");
    printf("n: ");
    print_bignum(&n);
    printf("e: ");
    print_bignum(&e);
    printf("d: ");
    print_bignum(&d);
    printf("t: ");
    print_bignum(&t);
    printf("r2m");
    print_bignum(&r2m);
    printf("Number of bits in n: %d\n", nBits);

    printf("ENCRYPT...\n");
	before = clock();
    encrypt(t, e, n, &c);
	after = clock();
    encrypt_cycles = after - before;
    printf("c: ");
    print_bignum(&c);
    printf("DECRYPT...\n");
    before = clock();
    decrypt(c, d, n, &t_decrypted);
    after = clock();
    decrypt_cycles = after - before;
    printf("t: ");
    print_bignum(&t_decrypted); 
    printf("Encrypt Cycles: %ld\nDecrypt Cycles: %ld\n", encrypt_cycles, decrypt_cycles);
    
    return 0;
}
