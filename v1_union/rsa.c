#include <stdio.h>

/*
Design steps:
- Look up table ? SKIP FOR NOW
- Modular exponentiation (multiply and square algorithm)
- Montgomery Modular Multiplication
- unint96 union?

- More test cases (bigger numbers)

- Port to VM
- Profile
- Very long integer arithmetic routines ?

Optimization Considerations:
- arithmetic on very long integers (need to write routines for this)
- true data dependencies
- expensive modular and multiplication operations
*/

// This approach uses a union defined as a uint96_t.
typedef union {
    __uint128_t val : 96;
} __attribute__((packed)) uint96_t;

/*
Helper method to compute: result = x*y % modulus
*/
uint96_t montgomery_modular_multiplication(uint96_t x, uint96_t y, uint96_t modulus)
{
    uint96_t result;
    result.val = 0;
    return result;
}

/*
Helper method to compute: result = base**exp % modulus
https://www.geeksforgeeks.org/exponential-squaring-fast-modulo-multiplication/ 
*/
uint96_t modular_exponentiation(uint96_t base, uint96_t exp, uint96_t modulus)
{
    uint96_t result;
    result.val = 1;
    while (exp.val > 0)
    {
        if (exp.val & 1) // exp.val % 2 != 0
            result.val = (result.val * base.val) % modulus.val;
        base.val = (base.val * base.val) % modulus.val;
        exp.val >>= 1; // exp /= 2
    }
    uint96_t final_res;
    final_res.val = result.val % modulus.val;
    return final_res;
}

/*
c = t**e mod n
*/
uint96_t encrypt(uint96_t t, uint96_t e, uint96_t n)
{
    return modular_exponentiation(t, e, n);
}

/*
t = c**d mod n
*/
uint96_t decrypt(uint96_t c, uint96_t d, uint96_t n)
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
    uint96_t p, q, n, e, d, t, c;
    // For this project, keys can be generated offline

    /*
    p = 61;
    q = 53;
    e = 17;
    d = 2753;
    */

    p.val = 7;
    q.val = 19;
    e.val = 5;
    d.val = 65;

    // The variables do not print properly because C is lacking a dedicated format specifier for __uint128_t
    n.val = p.val * q.val;
    printf("Configuration: p=%llu, q=%llu, n=%llu, e=%llu, d=%llu\n", p.val, q.val, n.val, d.val, e.val);

    t.val = 123;
    printf("Plaintext (t) is:           %llu\n", t.val);

    c = encrypt(t, e, n);
    printf("Ciphertext (c) is:          %llu\n", c.val);

    t = decrypt(c, d, n);
    printf("Decrypted Ciphertext is:    %llu\n", t.val);

    return 0;
}