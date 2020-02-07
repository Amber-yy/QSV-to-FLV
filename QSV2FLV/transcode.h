#pragma once

#define TRANS_SUCCESS 0
#define NULL_ARG 1
#define SOURCE_NOT_EXIST 2
#define SOURCE_PARSE_ERROR 3
#define NOT_SUPPORT_QSVFORMAT 4
#define FILE_ACCESS_ERROR 5

int Transcode(const char *src,const char *des);