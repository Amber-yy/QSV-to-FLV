#include "transcode.h"

#include <fstream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <chrono>
#include <filesystem>

using std::chrono::system_clock;
using namespace std;

struct MetaData
{
	vector<long long> filepositions;
	vector<double> times;
	string creationdate;
	string metadatacreator;
	double duration;
	double lasttimestamp;
	double lastkeyframetimestamp;
	double framerate;
	int videocodecid = -1;
	int audiocodecid = -1;
	int audiosamplerate;
	int audiosamplesize;
	int width;
	int height;
	int videodatarate;
	int filesize;
	int audiodelay;
	bool canSeekToEnd;
	bool stereo;

	char Meta_1[410] =
	{
		0x46, 0x4C, 0x56, 0x01, 0x05, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x0A, 0x6F, 0x6E, 0x4D, 0x65, 0x74,
		0x61, 0x44, 0x61, 0x74, 0x61, 0x08, 0x00, 0x00, 0x00, 0x12, 0x00, 0x08, 0x64, 0x75, 0x72, 0x61,
		0x74, 0x69, 0x6F, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x77,
		0x69, 0x64, 0x74, 0x68, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x68,
		0x65, 0x69, 0x67, 0x68, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D,
		0x76, 0x69, 0x64, 0x65, 0x6F, 0x64, 0x61, 0x74, 0x61, 0x72, 0x61, 0x74, 0x65, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x66, 0x72, 0x61, 0x6D, 0x65, 0x72, 0x61, 0x74,
		0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x76, 0x69, 0x64, 0x65,
		0x6F, 0x63, 0x6F, 0x64, 0x65, 0x63, 0x69, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x0F, 0x61, 0x75, 0x64, 0x69, 0x6F, 0x73, 0x61, 0x6D, 0x70, 0x6C, 0x65, 0x72, 0x61,
		0x74, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x61, 0x75, 0x64,
		0x69, 0x6F, 0x73, 0x61, 0x6D, 0x70, 0x6C, 0x65, 0x73, 0x69, 0x7A, 0x65, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x73, 0x74, 0x65, 0x72, 0x65, 0x6F, 0x01, 0x00, 0x00,
		0x0C, 0x61, 0x75, 0x64, 0x69, 0x6F, 0x63, 0x6F, 0x64, 0x65, 0x63, 0x69, 0x64, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x66, 0x69, 0x6C, 0x65, 0x73, 0x69, 0x7A, 0x65,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x6C, 0x61, 0x73, 0x74, 0x74,
		0x69, 0x6D, 0x65, 0x73, 0x74, 0x61, 0x6D, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x15, 0x6C, 0x61, 0x73, 0x74, 0x6B, 0x65, 0x79, 0x66, 0x72, 0x61, 0x6D, 0x65, 0x74,
		0x69, 0x6D, 0x65, 0x73, 0x74, 0x61, 0x6D, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x0A, 0x61, 0x75, 0x64, 0x69, 0x6F, 0x64, 0x65, 0x6C, 0x61, 0x79, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x63, 0x61, 0x6E, 0x53, 0x65, 0x65, 0x6B, 0x54,
		0x6F, 0x45, 0x6E, 0x64, 0x01, 0x00, 0x00, 0x0C, 0x63, 0x72, 0x65, 0x61, 0x74, 0x69, 0x6F, 0x6E,
		0x64, 0x61, 0x74, 0x65, 0x02, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x6D, 0x65, 0x74, 0x61, 0x64, 0x61,
		0x74, 0x61, 0x63, 0x72, 0x65, 0x61, 0x74, 0x6F, 0x72, 0x02
	};

	char Meta_2[28] =
	{
		0x00, 0x09, 0x6B, 0x65, 0x79, 0x66, 0x72, 0x61, 0x6D, 0x65, 0x73, 0x03, 0x00, 0x0D, 0x66, 0x69,
		0x6C, 0x65, 0x70, 0x6F, 0x73, 0x69, 0x74, 0x69, 0x6F, 0x6E, 0x73, 0x0A
	};

	char Meta_3[8] =
	{
		0x00, 0x05, 0x74, 0x69, 0x6D, 0x65, 0x73, 0x0A
	};

	char Meta_4[6] =
	{
		 0x00, 0x00, 0x09, 0x00, 0x00, 0x09
	};

	int metadatasize_position = 0xE;
	int duration_position = 0x35;
	int width_position = 0x45;
	int height_position = 0x56;
	int videodatarate_position = 0x6E;
	int framerate_position = 0x82;
	int videocodecid_position = 0x99;
	int audiosamplerate_position = 0xB3;
	int audiosamplesize_position = 0xCD;
	int stereo_position = 0xDE;
	int audiocodecid_position = 0xEE;
	int filesize_position = 0x101;
	int lasttimestamp_position = 0x119;
	int lastkeyframetimestamp_position = 0x139;
	int audiodelay_position = 0x14E;
	int canSeekToEnd_position = 0x165;
	int creationdate_position = 0x177;
	int metadatacreator_position = 0x19A;

	int samples[6];
	int i = 0;
	int breaking = 2000;
	int lasttime = -2000;
};

struct ScopePointer
{
	char *buffer = nullptr;
	~ScopePointer()
	{
		free(buffer);
	}
};

struct FileWrap
{
	ifstream *file = nullptr;
	int it = 0;
	long long restSize = 0;
	char *data = nullptr;
	int bufferSize = 10485760;
	int currentBufferSize = 0;

	FileWrap()
	{
		data = (char *)malloc(bufferSize);
	}

	~FileWrap()
	{
		free(data);
	}

	void readFromFile()
	{
		if (restSize >= 10485760)
		{
			currentBufferSize = 10485760;
		}
		else
		{
			currentBufferSize = restSize;
		}

		it = 0;

		if (currentBufferSize > 0)
		{
			file->read(data, currentBufferSize);
			restSize -= currentBufferSize;
		}
	}

	void setFile(ifstream *source, int tell)
	{
		file = source;
		file->seekg(0, ios::end);
		restSize = file->tellg();
		file->seekg(tell, ios::beg);
		readFromFile();
	}

	int read(char *buffer, int size)
	{
		int count = 0;
		//能提供的字节
		int a = currentBufferSize - it;

		if (a >= size)
		{
			count = size;
			if (buffer != nullptr)
				memcpy(buffer, data + it, size);
			it += size;
		}
		else
		{
			//还差的字节
			int b = size - a;
			if (buffer != nullptr)
				memcpy(buffer, data + it, a);

			count = a;
			readFromFile();

			if (currentBufferSize > 0)
				count += read(buffer + a, b);
		}

		return count;
	}
};


static void ReadFile(ifstream &source, char *buffer, int size)
{
	source.read(buffer, size);
	if (source.fail())
		throw SOURCE_PARSE_ERROR;
}

static void CheckFormat(ifstream &source)
{
	char buffer[11] = { 0 };
	const char *tag = "QIYI VIDEO";
	int ver = 0;

	source.seekg(0, ios::beg);
	ReadFile(source, buffer, 10);

	if (strcmp(buffer, tag) != 0)
		throw NOT_SUPPORT_QSVFORMAT;

	source.seekg(10, ios::beg);
	ReadFile(source, (char *)&ver, 4);
	if (ver != 2)
		throw NOT_SUPPORT_QSVFORMAT;

	source.seekg(0, ios::beg);
}

static void SeekFlvBegin(ifstream &source)
{
	source.seekg(0x4a, ios::beg);
	char buffer[12];
	ReadFile(source, buffer, 12);

	long long offset = *(unsigned int *)buffer + *(unsigned int *)(buffer + 4);
	int size = *(int *)(buffer + 8);

	source.seekg(offset + size, ios::beg);
}

static bool SkipMetaData(FileWrap &source, bool afterNext = false)
{
	int offset = 0xd;
	if (afterNext)
		offset = 0xd - 11;

	source.read(nullptr, offset);
	int len = 0;
	char back[4];
	int backSize = 0;
	int backIt = 0;
	int re;
	while (true)
	{
		unsigned char bit;
		if (backSize == 0)
		{
			re = source.read((char *)&bit, 1);
			if (re != 1)
				return false;
		}
		else
		{
			bit = back[backIt];
			++backIt;
			if (backIt >= 4)
				backSize = 0;
		}
		++len;

		if (bit == 9)
		{
			char buffer[4];

			if (backSize == 0)
			{
				re = source.read(buffer, 4);
				if (re != 4)
					return false;
			}
			else
			{
				int s = backSize - backIt;
				int t = 4 - s;
				memcpy(buffer, back, s);
				if (t > 0)
				{
					re = source.read(buffer + s, t);
					if (re != t)
						return false;
				}
				backSize = 0;
			}

			swap(buffer[0], buffer[3]);
			swap(buffer[1], buffer[2]);
			int size = *(int *)(buffer);
			if (len == size)
				break;
			swap(buffer[0], buffer[3]);
			swap(buffer[1], buffer[2]);
			memcpy(back, buffer, 4);
			backSize = 4;
			backIt = 0;
		}
	}

	return true;
}

static bool ReadNext(FileWrap &source, char *buffer)
{
	int re = source.read(buffer, 11);
	if (re != 11)
		return false;

	if (((buffer[8] | buffer[9] | buffer[10]) == 0) &&
		(buffer[0] == 8 || buffer[0] == 9 || buffer[0] == 18))
	{
		return true;
	}

	return false;
}

static bool SeekNextTag(FileWrap &source)
{
	int offset = 11;
	char buffer[5] = { 0 };

	int re = source.read(buffer, 4);
	if (re != 4)
		return false;

	swap(buffer[1], buffer[3]);
	int size = *(int *)(buffer + 1);

	source.read(nullptr, size + offset);

	return true;
}

static int GetTimeStamp(char *tag)
{
	char buffer[4];
	memcpy(buffer, tag + 4, 4);
	swap(buffer[0], buffer[2]);
	return *(int *)buffer;
}

static int GetTagType(char *tag)
{
	return tag[0];
}

static bool IsKeyFrame(char *tag)
{
	int t = tag[11];
	return (t & 17) == 17;
}

static int ParseVideoTag(char *tag)
{
	int t = tag[11];
	return (t & 15);
}

static void ParseAudioTag(char *tag, int &codecid, int &samplerate, int &samplesize, bool &stereo)
{
	int t = (unsigned char)tag[11];
	codecid = t >> 4;
	samplerate = t >> 2 & 3;
	samplesize = t >> 1 & 1;
	stereo = t & 1;
}

static string CurrentTime()
{
	system_clock::time_point tp = system_clock::now();
	time_t raw_time = system_clock::to_time_t(tp);
	tm  *timeinfo = localtime(&raw_time);

	char buf[24] = { 0 };
	strftime(buf, 24, "%Y-%m-%d ", timeinfo);

	return string(buf);
}

static void GetMetaInfo(MetaData &meta, char *buffer, long long tell)
{
	meta.duration = GetTimeStamp(buffer) / 1000.0;
	meta.lasttimestamp = meta.duration;

	int type = GetTagType(buffer);

	if (type == 9)
	{
		if (IsKeyFrame(buffer))
		{
			if (meta.videocodecid == -1)
				meta.videocodecid = ParseVideoTag(buffer);
			int time = GetTimeStamp(buffer);
			meta.lastkeyframetimestamp = time / 1000.0;

			if (time - meta.lasttime >= meta.breaking)
			{
				meta.filepositions.push_back(tell);
				meta.times.push_back(time / 1000.0);
				meta.lasttime = time;
			}
		}

		if (meta.i < 6)
		{
			int time = GetTimeStamp(buffer);
			if (time >= 1)
			{
				meta.samples[meta.i] = time;
				++meta.i;
			}
		}
	}
	else if (type == 8 && meta.audiocodecid == -1)
	{
		ParseAudioTag(buffer, meta.audiocodecid, meta.audiosamplerate, meta.audiosamplesize, meta.stereo);
	}

}

static void CreateTempFile(const char *src, MetaData &meta)
{
	ifstream source;
	source.open(src, ios_base::in | ios_base::binary);
	if (!source.is_open())
		throw SOURCE_NOT_EXIST;

	CheckFormat(source);
	SeekFlvBegin(source);

	FileWrap file;
	file.setFile(&source, source.tellg());

	if (!SkipMetaData(file))
		throw SOURCE_PARSE_ERROR;

	ofstream out(string(src) + ".temp", ios_base::out | ios_base::binary);
	if (!out.is_open())
		throw FILE_ACCESS_ERROR;

	const int bufferSize = 10485760;
	char *dataBuffer = (char *)malloc(bufferSize);
	ScopePointer pt;
	pt.buffer = dataBuffer;
	int bufferIt = 0;
	ScopePointer ptd;
	int dataSize = 0;
	long long tell = 0;
	int maxSize = 0;

	while (true)
	{
		char header[11];
		if (ReadNext(file, header))
		{
			char buffer[5] = { 0 };
			memcpy(buffer, header, 4);
			swap(buffer[1], buffer[3]);
			int size = *(int *)(buffer + 1);

			if (size + 15 > dataSize)
			{
				free(ptd.buffer);
				ptd.buffer = (char *)malloc(size + 15);
				dataSize = size + 15;
			}

			char *data = ptd.buffer;
			memcpy(data, header, 11);
			int re = file.read(data + 11, size + 4);
			GetMetaInfo(meta, data, tell);
			tell += size + 15;
			maxSize = maxSize > (size + 15) ? maxSize : (size + 15);

			if (re == size + 4)
			{
				if (bufferIt + size + 15 >= bufferSize)
				{
					out.write(dataBuffer, bufferIt);
					bufferIt = 0;
				}

				memcpy(dataBuffer + bufferIt, data, size + 15);
				bufferIt += size + 15;
			}

		}
		else
		{
			bool ok = SkipMetaData(file, true);
			ok = ok && SeekNextTag(file);
			ok = ok && SeekNextTag(file);
			if (!ok)
				break;
		}
	}

	if (bufferIt != 0)
		out.write(dataBuffer, bufferIt);

	meta.framerate = 9000.0 / (meta.samples[5] + meta.samples[4] + meta.samples[3] - meta.samples[2] - meta.samples[1] - meta.samples[0]);

	meta.width = 0;
	meta.height = 0;
	meta.videodatarate = 0;
	meta.filesize = 0;
	meta.audiodelay = 0;
	meta.canSeekToEnd = true;
	meta.creationdate = CurrentTime();
	meta.metadatacreator = "QSV2FLV by Amber";
}

static char *DoubleToBytes(char *buffer, double d)
{
	memcpy(buffer, &d, 8);
	for (int i = 0; i < 4; ++i)
		swap(buffer[i], buffer[7 - i]);
	return buffer;
}

static char *IntToBytes(char *buffer, int d)
{
	memcpy(buffer, &d, 4);
	for (int i = 0; i < 2; ++i)
		swap(buffer[i], buffer[3 - i]);
	return buffer;
}

static void WriteFlv(const char *src, const char *des, MetaData meta)
{
	ifstream source(string(src) + ".temp", ios_base::in | ios_base::binary);
	if (!source.is_open())
		throw FILE_ACCESS_ERROR;

	ofstream flv(des, ios_base::out | ios_base::binary);
	if (!flv.is_open())
		throw FILE_ACCESS_ERROR;

	char buffer[8];
	char bufferInt[4];

	flv.write(meta.Meta_1, 410);
	flv.seekp(meta.duration_position, ios::beg);
	flv.write(DoubleToBytes(buffer, meta.duration), 8);
	flv.seekp(meta.videodatarate_position, ios::beg);
	flv.write(DoubleToBytes(buffer, meta.videodatarate), 8);
	flv.seekp(meta.framerate_position, ios::beg);
	flv.write(DoubleToBytes(buffer, meta.framerate), 8);
	flv.seekp(meta.videocodecid_position, ios::beg);
	flv.write(DoubleToBytes(buffer, meta.videocodecid), 8);
	flv.seekp(meta.audiosamplerate_position, ios::beg);
	flv.write(DoubleToBytes(buffer, meta.audiosamplerate), 8);
	flv.seekp(meta.audiosamplesize_position, ios::beg);
	flv.write(DoubleToBytes(buffer, meta.audiosamplesize), 8);
	flv.seekp(meta.stereo_position, ios::beg);
	flv.write((char *)&meta.stereo, 1);
	flv.seekp(meta.audiocodecid_position, ios::beg);
	flv.write(DoubleToBytes(buffer, meta.audiocodecid), 8);
	flv.seekp(meta.lasttimestamp_position, ios::beg);
	flv.write(DoubleToBytes(buffer, meta.lasttimestamp), 8);
	flv.seekp(meta.lastkeyframetimestamp_position, ios::beg);
	flv.write(DoubleToBytes(buffer, meta.lastkeyframetimestamp), 8);
	flv.seekp(meta.canSeekToEnd_position, ios::beg);
	flv.write((char *)&meta.canSeekToEnd, 1);
	flv.seekp(meta.metadatacreator_position, ios::beg);
	short len = (short)meta.metadatacreator.length();
	buffer[0] = ((char *)&len)[1];
	buffer[1] = ((char *)&len)[0];
	flv.write(buffer, 2);
	flv.write(meta.metadatacreator.c_str(), meta.metadatacreator.length());

	flv.seekp(0, ios::end);
	flv.write(meta.Meta_2, 28);
	int count = meta.filepositions.size();
	int headerSize = meta.filepositions.size() * 18 + meta.metadatacreator.length() + 466;
	flv.write(IntToBytes(bufferInt, count), 4);
	for (int i = 0; i < count; ++i)
	{
		flv.put(0);
		flv.write(DoubleToBytes(buffer, headerSize + meta.filepositions[i]), 8);
	}

	flv.write(meta.Meta_3, 8);
	flv.write(bufferInt, 4);
	for (int i = 0; i < count; ++i)
	{
		flv.put(0);
		flv.write(DoubleToBytes(buffer, meta.times[i]), 8);
	}

	int tell = flv.tellp();
	flv.seekp(meta.metadatasize_position, ios::beg);
	tell -= 18;
	IntToBytes(bufferInt, tell);
	bufferInt[0] = 0;
	flv.write(bufferInt + 1, 3);
	flv.seekp(0, ios::end);
	flv.write(meta.Meta_4, 6);
	tell = flv.tellp();
	tell -= 13;
	flv.write(IntToBytes(bufferInt, tell), 4);
	const int bufferSize = 10485760;
	char *dataBuffer = (char *)malloc(bufferSize);
	source.seekg(0, ios::end);
	int sourceSize = source.tellg();
	source.seekg(0, ios::beg);

	while (sourceSize >= bufferSize)
	{
		source.read(dataBuffer, bufferSize);
		flv.write(dataBuffer, bufferSize);
		sourceSize -= bufferSize;
	}

	if (sourceSize > 0)
	{
		source.read(dataBuffer, sourceSize);
		flv.write(dataBuffer, sourceSize);
	}

	double t = flv.tellp();
	flv.seekp(meta.filesize_position, ios::beg);
	flv.write(DoubleToBytes(buffer, t), 4);
	flv.seekp(meta.creationdate_position, ios::beg);
	flv.write(meta.creationdate.c_str(), meta.creationdate.length());
}

static void CreateFlvFile(const char *src, const char * des, MetaData &meta)
{
	WriteFlv(src, des, meta);
}

int Transcode(const char * src, const char * des)
{
	if (src == nullptr || des == nullptr)
		return NULL_ARG;

	int result = TRANS_SUCCESS;

	try
	{
		MetaData meta;
		//从qsv文件中提取不含元信息的flv数据
		CreateTempFile(src, meta);

		//将提取到的flv数据连同元信息写入目标文件
		CreateFlvFile(src, des, meta);

	}
	catch (int e)
	{
		result = e;
	}
	
	filesystem::remove(string(src) + ".temp");

	return result;
}

