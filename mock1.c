#include <stdio.h>

/*
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
ARM is a 32-bit processor
big_num is 96 bits = 12 bytes = 3 ints
big_num range: [0, 2^96-1] = [0, 7.9e28]
big_num max value:  79,228,162,514,264,337,593,543,950,335 (29 digits)
LSB in the first index
| 2 | 1 | 0 |
| 0000 0000 0000 0000 0000 0000 0000 0000 | 0000 0000 0000 0000 0000 0000 0000 0000 | 0000 0000 0000 0000 0000 0000 0000 0000 |
*/

/*
Helper method to compute: result = base**exp % modulus
"Square and multiply algorithm"
https://www.geeksforgeeks.org/exponential-squaring-fast-modulo-multiplication/ 
*/
unsigned long long int modular_exponentiation(unsigned long long int base, unsigned long long int exp, unsigned long long int modulus)
{
    if(modulus == 1) return 0; // no computation to perform if modulus is 1

    unsigned long long int result;
    result = 1;
    while (exp > 0)
    {
        if (exp & 1) // exp % 2 != 0
            result = (result * base) % modulus; // multiply step
        exp >>= 1; // exp /= 2
        base = (base * base) % modulus; // square step
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

    p = 61;
    q = 53;
    e = 17;
    d = 2753; 

 /*
    p = 7;
    q = 19;
    e = 5;
    d = 65;
*/

    n = p * q;
    printf("Sizeof (bits): char:%lu, short:%lu, int:%lu, long:%lu, long long:%lu, size_t:%lu\n", sizeof(unsigned char)*8, sizeof(unsigned short)*8, sizeof(unsigned int)*8, sizeof(unsigned long)*8, sizeof(unsigned long long)*8, sizeof(size_t)*8);
    printf("Configuration: p=%llu, q=%llu, n=%llu, e=%llu, d=%llu\n", p, q, n, d, e);

    t = 123;
    printf("Plaintext (t) is:           %llu\n", t);

    c = encrypt(t, e, n);
    printf("Ciphertext (c) is:          %llu\n", c);

    t = decrypt(c, d, n);
    printf("Decrypted Ciphertext is:    %llu\n", t);

    return 0;
}