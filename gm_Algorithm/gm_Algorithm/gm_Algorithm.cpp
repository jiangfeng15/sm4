// gm_Algorithm.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <iomanip>
#include "sms4.h"

int main()
{
	UCHAR plain_text[30] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10,0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54 };
	auto in_len = 30;

	UCHAR encropty_key[16] = { 0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10 };

	UCHAR *cipher_text = new UCHAR[in_len + 16];
	auto out_len = in_len + 16;
	
	memset(cipher_text, 0x00, out_len);

	SM4_CONTEXT *sm4_context = new SM4_CONTEXT();
	init_sm4_context(sm4_context, encropty_key, SM4_ECB, 1);

	sm4_ecb_enc(sm4_context, plain_text, in_len, cipher_text, out_len);

	std::cout << "原始数据" << std::endl;
	for (int i = 0; i < in_len; i++) {
		std::cout <<"0x"<<std::hex << std::setw(2) << std::setfill('0') << (static_cast<int>(plain_text[i])&0xff)<<" ";
	}
	std::cout << std::endl;


	std::cout << "加密数据" << std::endl;
	for (int i = 0; i < out_len; i++) {
		//printf("%02x ", plain_text[i]);
		std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << (static_cast<int>(cipher_text[i]) & 0xff) << " ";
	}
	std::cout << std::endl;

	memset(plain_text, 0x00, sizeof(plain_text));

	UCHAR *dec_buf = new UCHAR[out_len + 16];
	auto dec_len = out_len + 16;
	memset(dec_buf, 0x00, dec_len);

	sm4_ecb_dec(sm4_context, cipher_text, out_len, dec_buf, dec_len);

	std::cout << "解密数据" << std::endl;
	for (int i = 0; i < dec_len; i++) {
		//printf("%02x ", plain_text[i]);
		std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << (static_cast<int>(dec_buf[i]) & 0xff) << " ";
	}
	std::cout << std::endl;
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
