#include <stdio.h>
#include <stdbool.h>
#include "bignum.h"

// Private Functions
static void _lshift_word(bignum *a, int nwords);
static void _rshift_word(bignum *a, int nwords);
static void _lshift_one_bit(bignum *a);
static void _rshift_one_bit(bignum *a);

void bignum_init(bignum *n)
{
	for (int i = 0; i < BN_ARRAY_LEN; i++)
	{
		n->arr[i] = 0;
	}
}

void bignum_from_int(bignum *n, BN_DTYPE_TMP i)
{
	bignum_init(n);
	n->arr[0] = i;
	BN_DTYPE_TMP num_32 = 32;
	BN_DTYPE_TMP tmp = i >> num_32; /* bit-shift with U64 operands to force 64-bit results */
	n->arr[1] = tmp;
}

unsigned int bignum_to_int(bignum *n)
{
	return n->arr[0];
}

void bignum_from_string(bignum *n, char *str)
{
	bignum_init(n);
	BN_DTYPE tmp;
	int i_arr = 0;									   // lsb to msb in bignum array
	int i_str = BN_HEX_STR_LEN - 1 - (2 * WORD_BYTES); // right to left in hex string

	while (i_str >= 0)
	{
		sscanf(&str[i_str], SSCANF_FORMAT_STR, &tmp);
		n->arr[i_arr] = tmp;
		i_str -= (2 * WORD_BYTES); // 2 hex digits in one byte
		i_arr += 1;
	}
}

void bignum_to_string(bignum *n, char *str)
{
	int i_arr = BN_ARRAY_LEN - 1; // msb to lsb in bignum array
	int i_str = 0;				  // left to right in hex string

	while (i_arr >= 0)
	{
		sprintf(&str[i_str], SPRINTF_FORMAT_STR, n->arr[i_arr]);
		i_str += 2 * WORD_BYTES; // 2 hex digits in one byte
		i_arr--;
	}
	str[i_str] = '\0';
}

void print_bignum(bignum *n)
{
	char buffer[BN_HEX_STR_LEN];
	bignum_to_string(n, buffer);
	for (int i = 0; i < BN_HEX_STR_LEN; i++)
	{
		if (i % (WORD_BYTES * 2) == 0)
			printf(" |");
		if (i % 4 == 0)
			printf(" ");
		printf("%c", buffer[i]);
	}
	printf("\n");
}

void bignum_assign(bignum *dst, bignum *src) /* Copy src into dst -- dst := src */
{
	for (int i = 0; i < BN_ARRAY_LEN; i++)
	{
		dst->arr[i] = src->arr[i];
	}
}

void bignum_add(bignum *a, bignum *b, bignum *c) /* c = a + b */
{
	BN_DTYPE_TMP tmp;
	unsigned int carry = 0;
	for (int i = 0; i < BN_ARRAY_LEN; i++)
	{
		tmp = (BN_DTYPE_TMP)a->arr[i] + b->arr[i] + carry;
		carry = (tmp > MAX_VAL);
		c->arr[i] = (tmp & MAX_VAL);
	}
}

void bignum_sub(bignum *a, bignum *b, bignum *c) /* c = a - b */
{
	BN_DTYPE_TMP res;
	BN_DTYPE_TMP tmp1;
	BN_DTYPE_TMP tmp2;
	int borrow = 0;
	int i;
	for (i = 0; i < BN_ARRAY_LEN; ++i)
	{
		tmp1 = (BN_DTYPE_TMP)a->arr[i] + (MAX_VAL + 1); /* + number_base */
		tmp2 = (BN_DTYPE_TMP)b->arr[i] + borrow;
		res = (tmp1 - tmp2);
		c->arr[i] = (BN_DTYPE)(res & MAX_VAL); /* "modulo number_base" == "% (number_base - 1)" if number_base is 2^N */
		borrow = (res <= MAX_VAL);
	}
}

void bignum_mul(bignum *a, bignum *b, bignum *c) /* c = a * b */
{
	bignum row;
	bignum tmp;
	int i, j;
	// i : index for a
	// j : index for b

	bignum_init(c);

	for (i = 0; i < BN_ARRAY_LEN; ++i)
	{
		bignum_init(&row);

		for (j = 0; j < BN_ARRAY_LEN; ++j)
		{
			if (i + j < BN_ARRAY_LEN)
			{
				bignum_init(&tmp);
				BN_DTYPE_TMP intermediate = ((BN_DTYPE_TMP)a->arr[i] * (BN_DTYPE_TMP)b->arr[j]);
				bignum_from_int(&tmp, intermediate);
				_lshift_word(&tmp, i + j);
				bignum_add(&tmp, &row, &row);
			}
		}
		bignum_add(c, &row, c);
	}
}

void bignum_div(bignum *a, bignum *b, bignum *c) /* c = a / b */
{
	bignum current;
	bignum denom;
	bignum tmp;

	bignum_from_int(&current, 1); // int current = 1;
	bignum_assign(&denom, b);	  // denom = b
	bignum_assign(&tmp, a);		  // tmp   = a

	const BN_DTYPE_TMP half_max = 1 + (BN_DTYPE_TMP)(MAX_VAL / 2);
	bool overflow = false;
	while (bignum_cmp(&denom, a) != LARGER) // while (denom <= a) {
	{
		if (denom.arr[BN_ARRAY_LEN - 1] >= half_max)
		{
			overflow = true;
			break;
		}
		_lshift_one_bit(&current); //   current <<= 1;
		_lshift_one_bit(&denom);   //   denom <<= 1;
	}
	if (!overflow)
	{
		_rshift_one_bit(&denom);   // denom >>= 1;
		_rshift_one_bit(&current); // current >>= 1;
	}
	bignum_init(c); // int answer = 0;

	while (!bignum_is_zero(&current)) // while (current != 0)
	{
		if (bignum_cmp(&tmp, &denom) != SMALLER) //   if (dividend >= denom)
		{
			bignum_sub(&tmp, &denom, &tmp); //     dividend -= denom;
			bignum_or(c, &current, c);		//     answer |= current;
		}
		_rshift_one_bit(&current); //   current >>= 1;
		_rshift_one_bit(&denom);   //   denom >>= 1;
	}
}

void bignum_mod(bignum *a, bignum *b, bignum *c)
{
	bignum tmp;
	bignum_divmod(a, b, &tmp, c);
}

void bignum_divmod(bignum *a, bignum *b, bignum *c, bignum *d)
{
	bignum tmp;

	/* c = (a / b) */
	bignum_div(a, b, c);

	/* tmp = (c * b) */
	bignum_mul(c, b, &tmp);

	/* d = a - tmp */
	bignum_sub(a, &tmp, d);
}

void bignum_rshift(bignum *a, bignum *b, unsigned int nbits) /* b = a >> nbits */
{
	bignum_assign(b, a);
	/* Handle shift in multiples of word-size */
	const int nbits_pr_word = (WORD_BYTES * 8);
	int nwords = nbits / nbits_pr_word;
	if (nwords != 0)
	{
		_rshift_word(b, nwords);
		nbits -= (nwords * nbits_pr_word);
	}

	if (nbits != 0)
	{
		int i;
		for (i = 0; i < (BN_ARRAY_LEN - 1); ++i)
		{
			b->arr[i] = (b->arr[i] >> nbits) | (b->arr[i + 1] << ((8 * WORD_BYTES) - nbits));
		}
		b->arr[i] >>= nbits;
	}
}

void bignum_or(bignum *a, bignum *b, bignum *c) /* c = a | b */
{
	for (int i = 0; i < BN_ARRAY_LEN; i++)
	{
		c->arr[i] = (a->arr[i] | b->arr[i]);
	}
}

int bignum_cmp(bignum *a, bignum *b) /* Compare: returns LARGER, EQUAL or SMALLER */
{
	int i = BN_ARRAY_LEN - 1;
	while (i >= 0)
	{
		if (a->arr[i] > b->arr[i])
		{
			return LARGER;
		}
		else if (a->arr[i] < b->arr[i])
		{
			return SMALLER;
		}
		i--;
	}

	return EQUAL;
}

int bignum_is_zero(bignum *n) /* For comparison with zero */
{
	int i;
	for (i = 0; i < BN_ARRAY_LEN; ++i)
	{
		if (n->arr[i])
		{
			return 0;
		}
	}

	return 1;
}

static void _lshift_word(bignum *a, int nwords)
{
	int i;
	/* Shift whole words */
	for (i = (BN_ARRAY_LEN - 1); i >= nwords; --i)
	{
		a->arr[i] = a->arr[i - nwords];
	}
	/* Zero pad shifted words. */
	for (; i >= 0; --i)
	{
		a->arr[i] = 0;
	}
}

static void _rshift_word(bignum *a, int nwords)
{
	int i;
	if (nwords >= BN_ARRAY_LEN)
	{
		for (i = 0; i < BN_ARRAY_LEN; ++i)
		{
			a->arr[i] = 0;
		}
		return;
	}

	for (i = 0; i < BN_ARRAY_LEN - nwords; ++i)
	{
		a->arr[i] = a->arr[i + nwords];
	}
	for (; i < BN_ARRAY_LEN; ++i)
	{
		a->arr[i] = 0;
	}
}

static void _lshift_one_bit(bignum *a)
{
	for (int i = BN_ARRAY_LEN - 1.; i > 0; i--)
	{
		a->arr[i] = (a->arr[i] << 1) | (a->arr[i - 1] >> ((8 * WORD_BYTES) - 1));
	}
	a->arr[0] <<= 1;
}

static void _rshift_one_bit(bignum *a)
{
	for (int i = 0; i < (BN_ARRAY_LEN - 1); ++i)
	{
		a->arr[i] = (a->arr[i] >> 1) | (a->arr[i + 1] << ((8 * WORD_BYTES) - 1));
	}
	a->arr[BN_ARRAY_LEN - 1] >>= 1;
}