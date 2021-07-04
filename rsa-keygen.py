import rsa

KEY_LEN = 52
HEXLEN = int(96 / 4)  # 96 bit hex numbers


def padhex(val):
	return hex(val)[2:].zfill(HEXLEN)


# set accurate=True to ensure keys are KEY_LEN bits and no smaller
publickey, privatekey = rsa.newkeys(KEY_LEN, accurate=True)
print(f"{padhex(publickey.n)=} {padhex(publickey.e)=} {padhex(privatekey.d)=}")
