import rsa

KEY_LEN = 96
HEXLEN = 2 * int(96 / 4)  # 96 bit hex numbers
# TODO: multiply by 2 while we have 192 bit keys in bignum


def padhex(val):
	return hex(val)[2:].zfill(HEXLEN)


# set accurate=True to ensure keys are KEY_LEN bits and no smaller
publickey, privatekey = rsa.newkeys(KEY_LEN, accurate=True)
print(f"{padhex(publickey.n)=} {padhex(publickey.e)=} {padhex(privatekey.d)=}")
