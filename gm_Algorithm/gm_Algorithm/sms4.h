#pragma once
#include <stdio.h>
#include <string.h>
/*
* write by jiangyingfeng 
* 2024.8.16
*/

#ifndef unlong
typedef unsigned int unlong;
#endif /* unlong */

#ifndef unchar
typedef unsigned char unchar;
#endif /* unchar */

#ifndef ULONG
#define ULONG unsigned long
#endif
#ifndef UCHAR
#define UCHAR unsigned char
#endif
#ifndef UINT
#define UINT unsigned int
#endif

//����ת����������12 34 56 78->34 56 78 12
#define SM4CROL(uval, bits) (((uval) << (bits)) | ((uval) >> (0x20 - (bits))))
/* define MASK code for selecting expected bits from a 32 bits value */
#define SM4_MASK3 0xFF000000
#define SM4_MASK2 0x00FF0000
#define SM4_MASK1 0x0000FF00
#define SM4_MASK0 0x000000FF

#define SM4_ENCRYPT     0
#define SM4_DECRYPT     1

#define SM4_ECB			0
#define SM4_CBC			1

typedef struct sm4_key
{
	ULONG ERK[32];  /*rk for encrypt*/
	ULONG DRK[32];  /*rk for decrypt, DRK[31]=ERK[0], ... ,DRK[0]=ERK[31]*/
	ULONG MK[4];    /*sm4 key ������Կ*/
}SM4_KEY;

typedef struct sm4_context {
	SM4_KEY sm_key;

	UCHAR mode; // 0:ECB,1:CBC
	UCHAR iv[16]; //CBC use
	UCHAR left[16]; //last left data
	UCHAR flag; //1: can decrypt
	UCHAR left_size; //size of left  ��Ҫ����ֽ�
	UCHAR padding_type; //0: padding 0 or not padding, 1: PKCS5 padding

}SM4_CONTEXT;

void init_sm4_rk(SM4_KEY *keyObj);
ULONG calc_sm4_rk(ULONG a);
int init_sm4_context(SM4_CONTEXT *key_obj, UCHAR *key,UCHAR mode, UCHAR padding_type);

//�ϳ��û����ɷ����Ա任t�����Ա任L���϶��ɣ�T(.)=L(t(.)),.��ʾ32λ����
static ULONG SM4Lt(ULONG a);
//�ֺ��������÷����Ե����ṹ������Ϊ��λ���м������㣬��һ�ε�������Ϊһ�ֱ任�����е����˺ϳ��û�
static ULONG SM4F(ULONG x0, ULONG x1, ULONG x2, ULONG x3, ULONG rk);

/*
* brief ִ��һ��F�ֺ���,input��output ����16�ֽ�
*  param key_obj to be initialized
*  param flag  SM4_ENCRYPT or SM4_DECRYPT
*  param input  in  16bytes
*  param output out 16bytes
*/

void run_sm4F_once(SM4_CONTEXT *key_obj, UCHAR flag, UCHAR *input, UCHAR * output);

/*
* brief ִ��sm4 ֧��padingģʽ��1 pcks5
*  param key_obj to be initialized
*  param flag  SM4_ENCRYPT or SM4_DECRYPT
*  param input  in ����buffer
*  param in_len in �����ֽ�
*  param output in ���buffer
*  param out_len in ���buffer�ֽ�����out ���bufferʵ�ʴ洢�ֽ���
*/
int sm4_ecb(SM4_CONTEXT *key_obj, UCHAR flag, UCHAR *input, int in_len, UCHAR * output, int &out_len);

int sm4_ecb_enc(SM4_CONTEXT *key_obj, UCHAR *input, int in_len, UCHAR * output, int &out_len);

int sm4_ecb_dec(SM4_CONTEXT *key_obj, UCHAR *input, int in_len, UCHAR * output, int &out_len);

/*
* brief ��֤�Ƿ�����pkcs#5/7
* param output in 
* out_len in ����Ĭ�� 16�ֽ�
*/
bool check_pkcs5_padding(UCHAR * output,int out_len=16);