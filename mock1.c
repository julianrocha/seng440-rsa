#include <stdio.h>

/*
Design steps:
- Look up table ?
- Modular exponentiation (multiply and square algorithm)
- Montgomery Modular Multiplication
- Very long integer arithmetic routines ?
- Port to VM
- Profile

Optimization Considerations:
- arithmetic on very long integers (need to write routines for this)
- true data dependencies
- expensive modular and multiplication operations
*/

/*
Helper method to compute: result = x*y % modulus
*/
unsigned long long int montgomery_modular_multiplication(unsigned long long x, unsigned long long y, unsigned long long modulus)
{
    return 0;
}

/*
Helper method to compute: result = base**exp % modulus
https://www.geeksforgeeks.org/exponential-squaring-fast-modulo-multiplication/ 
*/
unsigned long long int modular_exponentiation(unsigned long long int base, unsigned long long int exp, unsigned long long int modulus)
{
    unsigned long long int result;
    result = 1;
    while (exp > 0)
    {
        if (exp & 1) // exp % 2 != 0
            result = (result * base) % modulus;
        base = (base * base) % modulus;
        exp >>= 1; // exp /= 2
    }
    return result % modulus;
}

/*
c = t**e mod n
*/
unsigned long long int encrypt(unsigned long long int t, unsigned long long int e, unsigned long long int n)
{
    return modular_exponentiation(t, e, n);
}

/*
t = c**d mod n
*/
unsigned long long int decrypt(unsigned long long int c, unsigned long long int d, unsigned long long int n)
{
    return modular_exponentiation(c, d, n);
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
    unsigned long long int p, q, n, e, d, t, c;
    // For this project, keys can be generated offline

    /*
    p = 61;
    q = 53;
    e = 17;
    d = 2753;
    */

    p = 7;
    q = 19;
    e = 5;
    d = 65;

    n = p * q;
    printf("Configuration: p=%llu, q=%llu, n=%llu, e=%llu, d=%llu\n", p, q, n, d, e);

    t = 123;
    printf("Plaintext (t) is:           %llu\n", t);

    c = encrypt(t, e, n);
    printf("Ciphertext (c) is:          %llu\n", c);

    t = decrypt(c, d, n);
    printf("Decrypted Ciphertext is:    %llu\n", t);

    return 0;
}