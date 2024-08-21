#include "pch.h"
#include "sms4.h"

/* System parameter */ //�̶�ֵ
static ULONG FK[4] = { 0xa3b1bac6,0x56aa3350,0x677d9197,0xb27022dc };

/* fixed parameter */
static ULONG CK[32] =
{
	0x00070e15,0x1c232a31,0x383f464d,0x545b6269,
	0x70777e85,0x8c939aa1,0xa8afb6bd,0xc4cbd2d9,
	0xe0e7eef5,0xfc030a11,0x181f262d,0x343b4249,
	0x50575e65,0x6c737a81,0x888f969d,0xa4abb2b9,
	0xc0c7ced5,0xdce3eaf1,0xf8ff060d,0x141b2229,
	0x30373e45,0x4c535a61,0x686f767d,0x848b9299,
	0xa0a7aeb5,0xbcc3cad1,0xd8dfe6ed,0xf4fb0209,
	0x10171e25,0x2c333a41,0x484f565d,0x646b7279
};

/* Sbox table: 8bits input convert to 8 bits output*/
static UCHAR SBOX_TABLE[256] =
{
	0xd6,0x90,0xe9,0xfe,0xcc,0xe1,0x3d,0xb7,0x16,0xb6,0x14,0xc2,0x28,0xfb,0x2c,0x05,
	0x2b,0x67,0x9a,0x76,0x2a,0xbe,0x04,0xc3,0xaa,0x44,0x13,0x26,0x49,0x86,0x06,0x99,
	0x9c,0x42,0x50,0xf4,0x91,0xef,0x98,0x7a,0x33,0x54,0x0b,0x43,0xed,0xcf,0xac,0x62,
	0xe4,0xb3,0x1c,0xa9,0xc9,0x08,0xe8,0x95,0x80,0xdf,0x94,0xfa,0x75,0x8f,0x3f,0xa6,
	0x47,0x07,0xa7,0xfc,0xf3,0x73,0x17,0xba,0x83,0x59,0x3c,0x19,0xe6,0x85,0x4f,0xa8,
	0x68,0x6b,0x81,0xb2,0x71,0x64,0xda,0x8b,0xf8,0xeb,0x0f,0x4b,0x70,0x56,0x9d,0x35,
	0x1e,0x24,0x0e,0x5e,0x63,0x58,0xd1,0xa2,0x25,0x22,0x7c,0x3b,0x01,0x21,0x78,0x87,
	0xd4,0x00,0x46,0x57,0x9f,0xd3,0x27,0x52,0x4c,0x36,0x02,0xe7,0xa0,0xc4,0xc8,0x9e,
	0xea,0xbf,0x8a,0xd2,0x40,0xc7,0x38,0xb5,0xa3,0xf7,0xf2,0xce,0xf9,0x61,0x15,0xa1,
	0xe0,0xae,0x5d,0xa4,0x9b,0x34,0x1a,0x55,0xad,0x93,0x32,0x30,0xf5,0x8c,0xb1,0xe3,
	0x1d,0xf6,0xe2,0x2e,0x82,0x66,0xca,0x60,0xc0,0x29,0x23,0xab,0x0d,0x53,0x4e,0x6f,
	0xd5,0xdb,0x37,0x45,0xde,0xfd,0x8e,0x2f,0x03,0xff,0x6a,0x72,0x6d,0x6c,0x5b,0x51,
	0x8d,0x1b,0xaf,0x92,0xbb,0xdd,0xbc,0x7f,0x11,0xd9,0x5c,0x41,0x1f,0x10,0x5a,0xd8,
	0x0a,0xc1,0x31,0x88,0xa5,0xcd,0x7b,0xbd,0x2d,0x74,0xd0,0x12,0xb8,0xe5,0xb4,0xb0,
	0x89,0x69,0x97,0x4a,0x0c,0x96,0x77,0x7e,0x65,0xb9,0xf1,0x09,0xc5,0x6e,0xc6,0x84,
	0x18,0xf0,0x7d,0xec,0x3a,0xdc,0x4d,0x20,0x79,0xee,0x5f,0x3e,0xd7,0xcb,0x39,0x48
};

ULONG get_big_endian_32(UCHAR *d) {
	unsigned int nReturn;
	nReturn = (((ULONG)d[0]) << 24) | (((ULONG)d[1]) << 16)
		| (((ULONG)d[2]) << 8) | ((ULONG)d[3]);
	return nReturn;
}

ULONG get_big_endian(UCHAR *d, UCHAR I) {
	ULONG nReturn;
	nReturn = (((ULONG)d[I+0]) << 24) | (((ULONG)d[I + 1]) << 16)
		| (((ULONG)d[I + 2]) << 8) | ((ULONG)d[I + 3]);
	return nReturn;
}

void put_big_endian_32(ULONG a, UCHAR *d)
{
	d[0] = (UCHAR)((a >> 24) & 0xff);
	d[1] = (UCHAR)((a >> 16) & 0xff);
	d[2] = (UCHAR)((a >> 8) & 0xff);
	d[3] = (UCHAR)(a & 0xff);
}

void put_big_endian(ULONG a, UCHAR *d, UCHAR I) {
	d[I + 0] = (UCHAR)(a >> 24);
	d[I + 1] = (UCHAR)(a >> 16);
	d[I + 2] = (UCHAR)(a >> 8);
	d[I + 3] = (UCHAR)(a);
}




int init_sm4_context(SM4_CONTEXT *key_obj, UCHAR *key, UCHAR mode, UCHAR padding_type) {
	int i;
	if (key != NULL && key_obj != NULL)
	{
		for (i = 0; i < 4; i++)
		{
			key_obj->sm_key.MK[i] = ((ULONG)key[i * 4]);
			key_obj->sm_key.MK[i] <<= 8;
			key_obj->sm_key.MK[i] += ((ULONG)key[i * 4 + 1]);
			key_obj->sm_key.MK[i] <<= 8;
			key_obj->sm_key.MK[i] += ((ULONG)key[i * 4 + 2]);
			key_obj->sm_key.MK[i] <<= 8;
			key_obj->sm_key.MK[i] += ((ULONG)key[i * 4 + 3]);
		}
		init_sm4_rk(&key_obj->sm_key);

		key_obj->mode = mode;
		key_obj->padding_type = padding_type;
	}
	else
		return 1;
	return 0;
}

void init_sm4_rk(SM4_KEY *key) {
	ULONG k[36];
	ULONG i = 0;

	k[0] = key->MK[0] ^ FK[0];
	k[1] = key->MK[1] ^ FK[1];
	k[2] = key->MK[2] ^ FK[2];
	k[3] = key->MK[3] ^ FK[3];
	//����rk
	for (; i < 32; i++)
	{
		k[i + 4] = k[i] ^ (calc_sm4_rk(k[i + 1] ^ k[i + 2] ^ k[i + 3] ^ CK[i]));
		key->ERK[i] = k[i + 4];
	}
	//����rk
	for (i = 0; i < 32; i++)
	{
		key->DRK[i] = key->ERK[31 - i];
	}
}

ULONG calc_sm4_rk(ULONG a)
{
	ULONG b = 0;
	ULONG rk = 0;
	UCHAR a0 = (UCHAR)(a & SM4_MASK0);
	UCHAR a1 = (UCHAR)((a & SM4_MASK1) >> 8);
	UCHAR a2 = (UCHAR)((a & SM4_MASK2) >> 16);
	UCHAR a3 = (UCHAR)((a & SM4_MASK3) >> 24);
	UCHAR b0 = SBOX_TABLE[a0];
	UCHAR b1 = SBOX_TABLE[a1];
	UCHAR b2 = SBOX_TABLE[a2];
	UCHAR b3 = SBOX_TABLE[a3];

	b = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
	rk = b ^ (SM4CROL(b, 13)) ^ (SM4CROL(b, 23));

	return rk;
}

ULONG SM4Lt(ULONG a)
{
	ULONG b = 0;
	ULONG c = 0;
	UCHAR a0 = (UCHAR)(a & SM4_MASK0);
	UCHAR a1 = (UCHAR)((a & SM4_MASK1) >> 8);
	UCHAR a2 = (UCHAR)((a & SM4_MASK2) >> 16);
	UCHAR a3 = (UCHAR)((a & SM4_MASK3) >> 24);
	UCHAR b0 = SBOX_TABLE[a0];
	UCHAR b1 = SBOX_TABLE[a1];
	UCHAR b2 = SBOX_TABLE[a2];
	UCHAR b3 = SBOX_TABLE[a3];

	b = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
	c = b ^ (SM4CROL(b, 2)) ^ (SM4CROL(b, 10)) ^ (SM4CROL(b, 18)) ^ (SM4CROL(b, 24));

	return c;
}

ULONG SM4F(ULONG x0, ULONG x1, ULONG x2, ULONG x3, ULONG rk)
{
	return (x0^SM4Lt(x1^x2^x3^rk));
}


void run_sm4F_once(SM4_CONTEXT *key_obj, UCHAR flag, UCHAR *input, UCHAR * output) {

	ULONG *pRet = NULL;
	ULONG i = 0;

	ULONG ulbuf[36];

	/* reset number counter */
	i = 0;

	/* filled up with 0*/
	memset(ulbuf, 0, sizeof(ulbuf));
	ulbuf[0] = get_big_endian(input, 0);
	ulbuf[1] = get_big_endian(input, 4);
	ulbuf[2] = get_big_endian(input, 8);
	ulbuf[3] = get_big_endian(input, 12);

	while (i < 32)
	{
		if(flag == SM4_ENCRYPT)
			ulbuf[i + 4] = SM4F(ulbuf[i], ulbuf[i + 1],
				ulbuf[i + 2], ulbuf[i + 3], key_obj->sm_key.ERK[i]);
		else if (flag == SM4_DECRYPT)
			ulbuf[i + 4] = SM4F(ulbuf[i], ulbuf[i + 1],
				ulbuf[i + 2], ulbuf[i + 3], key_obj->sm_key.DRK[i]);
		i++;
	}

	/* save encrypted contents to original area */
	put_big_endian(ulbuf[35], output, 0);
	put_big_endian(ulbuf[34], output, 4);
	put_big_endian(ulbuf[33], output, 8);
	put_big_endian(ulbuf[32], output, 12);

}

int sm4_ecb(SM4_CONTEXT *key_obj, UCHAR flag, UCHAR *input, int in_len, UCHAR * output, int &out_len) {

	key_obj->left_size = (in_len % 16 == 0) ? 0 : (16 - in_len % 16);
	if (key_obj->left_size != 0 && out_len < in_len) {
		if(flag == SM4_ENCRYPT) //���ܹ��̣�Ҫ��������ĳ���out_len >= in_len������out_len��16��������
			return 1;
	}
	UCHAR * new_buf = NULL;
	int new_size = in_len;
	bool padding_flag = false;  //��¼�Ƿ�padding
	if (key_obj->left_size != 0) {
		//���·����ڴ�ռ�
		new_size = in_len + key_obj->left_size;
		new_buf = new UCHAR[new_size];
		memset(new_buf, 0x00, new_size);
		memcpy(new_buf, input, in_len);

		if (key_obj->padding_type == 1) { //pcks5���
			int padding_value = key_obj->left_size;
			for (auto i = in_len; i < new_size; i++) {
				new_buf[i] = padding_value;
			}
		}
		padding_flag = true;
	}
	//������ʱ���
	UCHAR tmp[16] = { 0x00 };
	auto round = new_size / 16;
	while (new_size > 0) {
		if (padding_flag && new_buf) {
			run_sm4F_once(key_obj, flag, new_buf, tmp);
			new_buf = new_buf + 16;

			if (round == 1) {  //���block
				if (flag == SM4_DECRYPT && check_pkcs5_padding(tmp)) { //������������padding,����ʱ����ȥ��padding
					memcpy(output, tmp, 16 - key_obj->left_size);
				}
				else
					return 2;
			}
			else
				memcpy(output, tmp, 16);
			memset(tmp, 0x00, 16);
		}
		else { //û��padding
			run_sm4F_once(key_obj, flag, input, output);
			input = input + 16;
			output = output + 16;
		}
		new_size = new_size - 16;

		round -= 1;
	}	
	return 0;
}

int sm4_ecb_enc(SM4_CONTEXT *key_obj, UCHAR *input, int in_len, UCHAR * output, int &out_len) {
	key_obj->left_size = in_len % 16; // (in_len % 16 == 0) ? 0 : (16 - in_len % 16);
	if (key_obj->left_size != 0 && out_len < in_len) {
		//���ܹ��̣�Ҫ��������ĳ���out_len >= in_len������out_len��16��������
		return 1;
	}
	int new_size = in_len;
	bool padding_flag = false;  //��¼�Ƿ�padding

	//////////////////////////////////////
	//�������һ�鲻��16�ֽڵ����飬���pkcs5
	if (key_obj->left_size != 0) {
		memset(key_obj->left, 0x00, sizeof(key_obj->left));
		memcpy(key_obj->left, input + (in_len - key_obj->left_size), key_obj->left_size);

		if (key_obj->padding_type == 1) { //pcks5���
			int padding_value = 16 - key_obj->left_size;
			for (auto i = key_obj->left_size; i < 16; i++) {
				key_obj->left[i] = padding_value;
			}
		}
		padding_flag = true;
		new_size = in_len + 16 - key_obj->left_size;
	}
	///////////////////////////////////////////////////////////////////////
	//������ʱ���
	UCHAR tmp[16] = { 0x00 };
	auto round = new_size / 16;
	bool first_round = true;
	while (round > 0) {

		if (!first_round && padding_flag && round == 1) {  //���block
			run_sm4F_once(key_obj, SM4_ENCRYPT, key_obj->left, tmp);
			memcpy(output, tmp, 16);
		}
		else{
			run_sm4F_once(key_obj, SM4_ENCRYPT, input, output);
			input = input + 16;
			output = output + 16;
			first_round = false;
		}
		round -= 1;
	}
	out_len = new_size;
	return 0;
}

int sm4_ecb_dec(SM4_CONTEXT *key_obj, UCHAR *input, int in_len, UCHAR * output, int &out_len) {

	if ((in_len % 16 != 0) && out_len < in_len) {
		//���ܹ��̣�Ҫ��������ĳ���out_len >= in_len������out_len��16��������
		return 1;
	}
	UCHAR * new_buf = NULL;
	int new_size = in_len;
	bool padding_flag = false;  //��¼�Ƿ�padding

	//������ʱ���
	UCHAR tmp[16] = { 0x00 };
	auto round = in_len / 16;
	int real_out_len = 0;
	bool first_round = true;
	while (round > 0) {

		if (!first_round && round == 1) {  //���block
			run_sm4F_once(key_obj, SM4_DECRYPT, input, tmp);
			if (check_pkcs5_padding(tmp)) {
				memcpy(output, tmp, 16 - tmp[15]);
				real_out_len += 16 - tmp[15];
			}
			else {
				memcpy(output, tmp, 16);
				real_out_len += 16;
			}			
		}
		else {
			run_sm4F_once(key_obj, SM4_DECRYPT, input, output);
			input = input + 16;
			output = output + 16;
			real_out_len += 16;
			first_round = false;
		}
		round -= 1;
	}
	out_len = real_out_len;
	return 0;
}

bool check_pkcs5_padding(UCHAR * output, int out_len) {
	auto padding_value = output[out_len - 1];
	if (padding_value > 16)
		return false;
	bool pkcs5_flag = true;
	for (auto i = 0; i < padding_value; i++) {
		if (padding_value != output[out_len - i - 1]) {
			pkcs5_flag = false;
		}
	}
	return pkcs5_flag;
}