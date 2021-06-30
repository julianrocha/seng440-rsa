#include <stdio.h>
#include "bignum.h"

int main()
{
	bignum my_num;
	char *hex_string = "a0f00c0b200e00d100bc8635"; // a0f0 0c0b 200e 00d1 00bc 8635
	bignum_from_string(&my_num, hex_string);
	print_bignum(&my_num);
}