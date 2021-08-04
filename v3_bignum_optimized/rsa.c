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

/*
Computes ab mod N using the constant R as defined here: https://en.wikipedia.org/wiki/Montgomery_modular_multiplication
*/
void montgomery_mult(bignum *a, bignum *b, bignum *N, int num_bits, bignum *result) 
{
	bignum t;
	bignum_init(&t);
	int i = num_bits;

    // Loop condition and counter optimized 
	for(; i != 0 ; i--) 
    {
		int t_bit = bignum_get_bit(&t, 0);
		int a_bit = bignum_get_bit(a, num_bits - i);
		int b_bit = bignum_get_bit(b, 0);
		int computed = t_bit + (a_bit * b_bit);

		if (a_bit == 1) 
        {
			bignum_add(&t, b, &t);
		}

		if (computed == 1) 
        {
			bignum_add(&t, N, &t);
		}

		bignum_rshift(&t, &t, 1);
	}

	if (bignum_cmp(&t, N) >= 0) 
    {
		bignum_sub(&t, N, &t);
	}

    // Set the result using the relevant bignum function
	bignum_assign(result, &t);
}


/*
Helper method to compute: result = base**exp % modulus
"Square and multiply algorithm"
https://www.geeksforgeeks.org/exponential-squaring-fast-modulo-multiplication/ 
*/
void modular_exponentiation(bignum base, bignum exp, bignum modulus, int num_bits, bignum r2m, bignum *result)
{
    // Initialize the current computation variables that change each iteration of the loop
    bignum cur_base;
    bignum cur_exp;
    bignum cur_res;

    // Use Montgomery Multiplication to reduce the result and base variables
    bignum_from_int(result, 1);
    montgomery_mult(result, &r2m, &modulus, num_bits, &cur_res);
    montgomery_mult(&base, &r2m, &modulus, num_bits, &cur_base);
    bignum_assign(result, &cur_res);
    bignum_assign(&base, &cur_base);

    // Computation loop for the actual exponentiation operation
    while (!bignum_is_zero(&exp))
    {
        if (exp.arr[0] & 1)
        {
            montgomery_mult(result, &base, &modulus, num_bits, &cur_res);
            bignum_assign(result, &cur_res);
        }

        bignum_rshift(&exp, &cur_exp, 1);
        montgomery_mult(&base, &base, &modulus, num_bits, &cur_base);
        bignum_assign(&exp, &cur_exp);
        bignum_assign(&base, &cur_base);
    }

    bignum one;
    bignum_from_int(&one, 1);
	montgomery_mult(result, &one, &modulus, num_bits, &cur_res);
    bignum_assign(result, &cur_res);
}

// void modular_exponentiation(bignum base, bignum exp, bignum modulus, bignum *result)
// {
//     bignum cur_res;
//     bignum cur_base;
//     bignum cur_exp;

//     bignum_from_int(result, 1);

//     while (!bignum_is_zero(&exp))
//     {
//         if (exp.arr[0] & 1)
//         {
//             // result = (result * base) % modulus; // multiply step
//             bignum_mul(result, &base, &cur_res);
//             bignum_mod(&cur_res, &modulus, result);
//         }
//         bignum_rshift(&exp, &cur_exp, 1);
//         bignum_assign(&exp, &cur_exp);

//         // base = (base * base) % modulus;			// square step
//         bignum_mul(&base, &base, &cur_base);
//         bignum_mod(&cur_base, &modulus, &base);
//     }
//     bignum_mod(result, &modulus, &cur_res);
//     bignum_assign(result, &cur_res);
// }

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
void encrypt(bignum t, bignum e, bignum n, int num_bits, bignum r2m, bignum *c)
{
    // modular_exponentiation(t, e, n, c);
    modular_exponentiation(t, e, n, num_bits, r2m, c);
}

/*
t = c**d mod n
*/
void decrypt(bignum c, bignum d, bignum n, int num_bits, bignum r2m, bignum *t)
{
    // modular_exponentiation(c, d, n, t);
    modular_exponentiation(c, d, n, num_bits, r2m, t);
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
    bignum n, e, d, t, c, t_decrypted, two, r2m;
    int num_bits;
    clock_t before;
    clock_t after;
    clock_t encrypt_cycles;
    clock_t decrypt_cycles;
    bignum_from_int(&two, 2);

     // 24 bit keys
    printf("\n--- TESTING 24 bit keys ---\n");
    char n_str_24[] = "000000000000000000a73911";
    char e_str_24[] = "000000000000000000010001";
    char d_str_24[] = "0000000000000000004a051d";
    char t_str_24[] = "000000000000000000080501";

    bignum_from_string(&n, n_str_24);
    bignum_from_string(&e, e_str_24);
    bignum_from_string(&d, d_str_24);
    bignum_from_string(&t, t_str_24);
    num_bits = bignum_num_bits(&n);
    // bignum_from_int(&r_exp, 2 * num_bits);
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
    printf("Number of bits in n: %d\n", num_bits);

    printf("ENCRYPT...\n");
	before = clock();
    encrypt(t, e, n, num_bits, r2m, &c);
	after = clock();
    encrypt_cycles = after - before;
    printf("c: ");
    print_bignum(&c);
    printf("DECRYPT...\n");
    before = clock();
    decrypt(c, d, n, num_bits, r2m, &t_decrypted);
    after = clock();
    decrypt_cycles = after - before;
    printf("t: ");
    print_bignum(&t_decrypted); 
    printf("Encrypt Cycles: %ld\nDecrypt Cycles: %ld\n", encrypt_cycles, decrypt_cycles);

    // 48 bit keys
    printf("\n--- TESTING 48 bit keys ---\n");
    char n_str_48[] = "000000000000bc046e91ae5f";
    char e_str_48[] = "000000000000000000010001";
    char d_str_48[] = "0000000000009420e4147c29";
    char t_str_48[] = "000000000000500073000001";

    bignum_from_string(&n, n_str_48);
    bignum_from_string(&e, e_str_48);
    bignum_from_string(&d, d_str_48);
    bignum_from_string(&t, t_str_48);
    num_bits = bignum_num_bits(&n);
    // bignum_from_int(&r_exp, 2 * num_bits);
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
    printf("Number of bits in n: %d\n", num_bits);

    printf("ENCRYPT...\n");
	before = clock();
    encrypt(t, e, n, num_bits, r2m, &c);
	after = clock();
    encrypt_cycles = after - before;
    printf("c: ");
    print_bignum(&c);
    printf("DECRYPT...\n");
    before = clock();
    decrypt(c, d, n, num_bits, r2m, &t_decrypted);
    after = clock();
    decrypt_cycles = after - before;
    printf("t: ");
    print_bignum(&t_decrypted); 
    printf("Encrypt Cycles: %ld\nDecrypt Cycles: %ld\n", encrypt_cycles, decrypt_cycles);

    // 96 bit keys
    printf("\n--- TESTING 96 bit keys ---\n");
    char n_str_96[] = "000d2123f00d92d08643cd4f";
    char e_str_96[] = "000000000000000000010001";
    char d_str_96[] = "1f175280fc4501ce37cb57d1";
    char t_str_96[] = "400000050000000073000001";

    bignum_from_string(&n, n_str_96);
    bignum_from_string(&e, e_str_96);
    bignum_from_string(&d, d_str_96);
    bignum_from_string(&t, t_str_96);
    num_bits = bignum_num_bits(&n);
    // bignum_from_int(&r_exp, 2 * num_bits);
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
    printf("Number of bits in n: %d\n", num_bits);

    printf("ENCRYPT...\n");
	before = clock();
    encrypt(t, e, n, num_bits, r2m, &c);
	after = clock();
    encrypt_cycles = after - before;
    printf("c: ");
    print_bignum(&c);
    printf("DECRYPT...\n");
    before = clock();
    decrypt(c, d, n, num_bits, r2m, &t_decrypted);
    after = clock();
    decrypt_cycles = after - before;
    printf("t: ");
    print_bignum(&t_decrypted); 
    printf("Encrypt Cycles: %ld\nDecrypt Cycles: %ld\n", encrypt_cycles, decrypt_cycles);
    
    return 0;
}
