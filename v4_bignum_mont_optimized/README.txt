V4 implements several optimizations to the following functions:

- bignum_init
- bignum_assign
- _rshift_word
- bignum_add

The optimizations include the following:

- loop unrolling
- loop counter optimization
- loop initialization and exit condition optimization
- addition of the register keyword to relevant variables
- operator strength reduction