#include <string>
#include <fstream>
#include <iostream>
#include <ctime>

#include "transcode.h"

using namespace std;

int main(int argc,char **argv)
{
	string in, out;

	if (argc == 3)
	{
		in = argv[1];
		out = argv[2];
	}
	else
	{
		cout << "请输入原始文件路径：\n";
		cin >> in;
		cout << "请输入目标文件路径：\n";
		cin >> out;
	}

	clock_t start = clock();
	int result = Transcode(in.c_str(),out.c_str());
	clock_t end = clock();

	if (result == TRANS_SUCCESS)
	{
		cout <<"转码成功，耗时："<< end - start<<"ms\n";
	}
	else
	{
		cout << "转码失败，错误代码：" << result << "\n";
	}

	return 0;
}