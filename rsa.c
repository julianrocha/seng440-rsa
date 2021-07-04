#include <stdio.h>
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
        // printf("\nbase:   ");
        // print_bignum(&base);
        // printf("exp:    ");
        // print_bignum(&exp);
        // printf("result: ");
        // print_bignum(result);

        if (exp.arr[0] & 1)
        {
            bignum_mul(result, &base, &tmp_result);
            bignum_mod(&tmp_result, &modulus, result);
        }
        bignum_rshift(&exp, &tmp_exp, 1);
        bignum_assign(&exp, &tmp_exp);
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
    printf("Type sizes on this machine (bits): char:%lu, short:%lu, int:%lu, long:%lu, long long:%lu, size_t:%lu\n", sizeof(unsigned char) * 8, sizeof(unsigned short) * 8, sizeof(unsigned int) * 8, sizeof(unsigned long) * 8, sizeof(unsigned long long) * 8, sizeof(size_t) * 8);

    // For this project, keys can be generated offline
    /*
    WORKING:
    p_int = 61;
    q_int = 53;
    e_int = 17;
    d_int = 2753;
    t_int = 123;
    */
    
/*
    // WORKING:
    p_int = 11;
    q_int = 13;
    e_int = 7;
    d_int = 103;
    t_int = 9;
    */
    char n_str[] = "00000000000a3de01493a3db";
    char e_str[] = "000000000000000000010001";
    char d_str[] = "00000000000786734bf8c671";
    char t_str[] = "000000000000000073000001";

    bignum n, e, d, t, c, t_decrypted;
    bignum_from_string(&n, n_str);
    bignum_from_string(&e, e_str);
    bignum_from_string(&d, d_str);
    bignum_from_string(&t, t_str);

    printf("RSA Configuration: \n");
    printf("n: ");
    print_bignum(&n);
    printf("e: ");
    print_bignum(&e);
    printf("d: ");
    print_bignum(&d);
    printf("t: ");
    print_bignum(&t);

    printf("ENCRYPT...\n");
    encrypt(t, e, n, &c);
    printf("c: ");
    print_bignum(&c);
    printf("DECRYPT...\n");
    decrypt(c, d, n, &t_decrypted);
    printf("t: ");
    print_bignum(&t_decrypted);

    return 0;
}