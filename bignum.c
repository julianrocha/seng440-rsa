#include <stdio.h>
#include "bignum.h"

void bignum_init(bignum *n)
{
	for (int i = 0; i < BN_ARRAY_LEN; i++)
	{
		n->arr[i] = 0;
	}
}

void bignum_from_int(bignum *n, unsigned int i)
{
	bignum_init(n);
	n->arr[0] = i;
}

unsigned int bignum_to_int(bignum *n) { return 0; }

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
	printf("Formatted HEX:");
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