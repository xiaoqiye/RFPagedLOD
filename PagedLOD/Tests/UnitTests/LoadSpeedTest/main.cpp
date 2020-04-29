#include <random>
#include <string>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>
#include <Windows.h>
#include <iostream>
#include <unordered_map>

struct SConfig
{
	int BEGIN_TILE_NUM = 1;
	int END_TILE_NUM = 190;
	int EACH_TIMES = 10;
	int TOTAL_TIMES = 1000;
	std::string PREFIX = "Tile_";
	std::string ROOT_PATH = "F:/Resource/CD/Bin/";
	int SEED = 1;
};

//****************************************************************************
//FUNCTION:
static void initFromConfigureFile(SConfig& voOutValue)
{
	std::ifstream File("./Config");
	if (File.is_open())
	{
		std::string CurrentLine;
		std::getline(File, CurrentLine);
		voOutValue.BEGIN_TILE_NUM = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		voOutValue.END_TILE_NUM = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		voOutValue.EACH_TIMES = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		voOutValue.TOTAL_TIMES = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
		std::getline(File, CurrentLine);
		voOutValue.PREFIX = CurrentLine.substr(CurrentLine.find_last_of('=') + 1);
		std::getline(File, CurrentLine);
		voOutValue.ROOT_PATH = CurrentLine.substr(CurrentLine.find_last_of('=') + 1);
		std::getline(File, CurrentLine);
		voOutValue.SEED = std::stoi(CurrentLine.substr(CurrentLine.find_last_of('=') + 1));
	}
	else
	{
		throw std::exception("cannot open Config file.");
	}
	File.close();
}

struct STextureInfo
{
	unsigned int Width = 0;
	unsigned int Height = 0;
	unsigned int InternalFormat = 0;
	unsigned int Size = 0;
};
struct STexture
{
	std::string TextureFileName;
	STextureInfo TexInfo;
	std::shared_ptr<unsigned char> pTextureData = nullptr;
	bool isValid() const { return TexInfo.Size > 0; }
};
struct SGeometry
{
	std::string GeometryFileName;
	unsigned int VertexCount = 0;
	unsigned int IndexCount = 0;
	std::shared_ptr<float> pVertexData = nullptr;
	std::shared_ptr<unsigned int> pIndexData = nullptr;
};
std::shared_ptr<char> __getFileStream(const std::string& vFilePath)
{
	_ASSERTE(!vFilePath.empty());
	FILE* fp;
	fopen_s(&fp, vFilePath.c_str(), "rb");
	unsigned int FileSize;
	fread(&FileSize,sizeof(unsigned int), 1, fp);
	auto pFileStream = std::shared_ptr<char>(reinterpret_cast<char*>(std::malloc(FileSize)));
	fread(pFileStream.get(), sizeof(char), FileSize, fp);	
	fclose(fp);

	//std::ifstream InputFile(vFilePath, std::ios::binary | std::ios::in);
	//_ASSERTE(InputFile.is_open());
	//unsigned int FileSize;
	//InputFile.read(reinterpret_cast<char*>(&FileSize), sizeof(unsigned int));
	//auto pFileStream = std::shared_ptr<char>(reinterpret_cast<char*>(std::malloc(FileSize)));
	//InputFile.read(pFileStream.get(), FileSize);
	//InputFile.close();

	return pFileStream;
}

//****************************************************************************
//FUNCTION:
void __processGeometryBuffer(const void* vGeoBufferStream, std::shared_ptr<SGeometry>& voGeometry)
{
	const unsigned int VertexBufferSize = reinterpret_cast<const unsigned int*>(vGeoBufferStream)[0];
	std::shared_ptr<char> pVertexBuffer(new char[VertexBufferSize]);
	const char* pVertexBufferStream = reinterpret_cast<const char*>(vGeoBufferStream) + sizeof(unsigned int);
	std::memcpy(pVertexBuffer.get(), pVertexBufferStream, VertexBufferSize);

	const unsigned int IndexBufferSize = reinterpret_cast<const unsigned int*>(pVertexBufferStream + VertexBufferSize)[0];
	std::shared_ptr<char> pIndexBuffer(new char[IndexBufferSize]);
	const char* pIndexBufferStream = reinterpret_cast<const char*>(pVertexBufferStream) + VertexBufferSize + sizeof(unsigned int);
	std::memcpy(pIndexBuffer.get(), pIndexBufferStream, IndexBufferSize);

	voGeometry->VertexCount = VertexBufferSize / sizeof(20);
	voGeometry->pVertexData = std::reinterpret_pointer_cast<float>(pVertexBuffer);
	voGeometry->IndexCount = IndexBufferSize / sizeof(unsigned int);
	voGeometry->pIndexData = std::reinterpret_pointer_cast<unsigned int>(pIndexBuffer);
}

//****************************************************************************
//FUNCTION:
void __processTextureBuffer(const void* vTexBufferStream, std::shared_ptr<STexture>& voTexture)
{
	std::memcpy(&voTexture->TexInfo, vTexBufferStream, sizeof(STextureInfo));
	const unsigned int TextureDataBufferOffset = sizeof(STextureInfo);
	const char* pTextureBufferStream = reinterpret_cast<const char*>(vTexBufferStream) + TextureDataBufferOffset;

	const unsigned int TextureBufferSize = voTexture->TexInfo.Size;
	std::shared_ptr<unsigned char> TextureBuffer(new unsigned char[TextureBufferSize]);
	std::memcpy(TextureBuffer.get(), pTextureBufferStream, TextureBufferSize);

	voTexture->pTextureData = TextureBuffer;
}

uintmax_t generatePathSet(const SConfig& vConfig, std::vector<std::string>& voPathSet, int vSeed)
{
	std::default_random_engine RandomEngine = std::default_random_engine(vSeed);
	std::uniform_int_distribution<int> Distribution = std::uniform_int_distribution<int>(vConfig.BEGIN_TILE_NUM, vConfig.END_TILE_NUM);
	int TileNum;
	uintmax_t voSize = 0;
	int RunTimes = 0;
	while (RunTimes < vConfig.TOTAL_TIMES)
	{
		TileNum = Distribution(RandomEngine);
		auto FolderPath = vConfig.ROOT_PATH + vConfig.PREFIX + std::to_string(TileNum) + "/";
		boost::filesystem::directory_iterator EndItr;
		if (!boost::filesystem::is_directory(FolderPath)) continue;
		boost::filesystem::directory_iterator Itr(FolderPath);
		int FileCount = 0;
		for (; Itr != EndItr; ++Itr)
			++FileCount;
		std::vector<int> FileSet;
		auto R = std::uniform_int_distribution<int>(1, FileCount);
		for (int i = 0; i < vConfig.EACH_TIMES; ++i)
		{
			FileSet.emplace_back(R(RandomEngine));
			++RunTimes;
		}
		Itr = boost::filesystem::directory_iterator(FolderPath);

		for (int i = 0; i < vConfig.EACH_TIMES; ++i)
		{
			for (int k = 1; k < FileSet[i]; ++k)
				++Itr;
			voPathSet.emplace_back(Itr->path().string());
			voSize += file_size(*Itr);
			Itr = boost::filesystem::directory_iterator(FolderPath);
		}
	}
	return voSize;
}

void TestFileTime(const std::vector<std::string>& vPathSet, uintmax_t vSize)
{
	LARGE_INTEGER BeginTime;
	LARGE_INTEGER EndTime;
	LARGE_INTEGER Freq;
	QueryPerformanceFrequency(&Freq);
	QueryPerformanceCounter(&BeginTime);
	for (auto& i : vPathSet)
		__getFileStream(i);
	QueryPerformanceCounter(&EndTime);
	const auto Time = static_cast<double>(EndTime.QuadPart - BeginTime.QuadPart) / Freq.QuadPart;
	const auto TotalLoadSizeMB = static_cast<double>(vSize) / 1024.0 / 1024.0;
	std::cout << "File time: " << "Size = " << TotalLoadSizeMB << "MB Cost time = " << Time << "s Speed = " << TotalLoadSizeMB / Time << "MB/s" << std::endl;
}

void TestProcessTime(const std::vector<std::string>& vPathSet, uintmax_t vSize)
{
	LARGE_INTEGER BeginTime;
	LARGE_INTEGER EndTime;
	LARGE_INTEGER Freq;
	QueryPerformanceFrequency(&Freq);
	QueryPerformanceCounter(&BeginTime);
	for (auto& i : vPathSet)
	{
		auto Stream = __getFileStream(i);
		if (boost::ends_with(i, ".bin"))
		{
			auto pGeometry = std::make_shared<SGeometry>();
			__processGeometryBuffer(Stream.get(), pGeometry);
		}
		else
		{
			auto pTexture = std::make_shared<STexture>();
			__processTextureBuffer(Stream.get(), pTexture);
		}
	}
	QueryPerformanceCounter(&EndTime);
	const auto Time = static_cast<double>(EndTime.QuadPart - BeginTime.QuadPart) / Freq.QuadPart;
	const auto TotalLoadSizeMB = static_cast<double>(vSize) / 1024.0 / 1024.0;
	std::cout << "Process time: "<<"Size = " << TotalLoadSizeMB << "MB Cost time = " << Time << "s Speed = " << TotalLoadSizeMB / Time << "MB/s" << std::endl;
}

void TestMapTime()
{
	std::unordered_map<unsigned int, unsigned int> m1;
	std::unordered_map<std::string, unsigned int> m2;
	std::vector<std::unordered_map<unsigned int, unsigned int>> m3(100);
	std::string t = "I:\\Resource\\CD\\Bin\\Tile_11";
	for (unsigned int i = 0; i < 100000; ++i)
	{
		m1[i] = i;
		m2[std::to_string(i) + t] = i;
	}
	for (unsigned int i = 0; i < 100; ++i)
	{
		for (unsigned int k = 0; k < 1000; ++k)
		{
			m3[i][k] = i;
		}
	}
	LARGE_INTEGER BeginTime;
	LARGE_INTEGER EndTime;
	LARGE_INTEGER Freq;
	QueryPerformanceFrequency(&Freq);
	QueryPerformanceCounter(&BeginTime);
	uintmax_t sum = 0;
	for (unsigned int i = 0; i < 100000; i += 4)
	{
		sum += m1[i];
	}
	QueryPerformanceCounter(&EndTime);
	auto Time = static_cast<double>(EndTime.QuadPart - BeginTime.QuadPart) / Freq.QuadPart;
	std::cout << "m1 time: " << Time << std::endl;

	QueryPerformanceFrequency(&Freq);
	QueryPerformanceCounter(&BeginTime);
	sum = 0;
	for (unsigned int i = 0; i < 100000; i += 4)
	{
		sum += m2[std::to_string(i) + t];
	}
	QueryPerformanceCounter(&EndTime);
	Time = static_cast<double>(EndTime.QuadPart - BeginTime.QuadPart) / Freq.QuadPart;
	std::cout << "m2 time: " << Time << std::endl;

	QueryPerformanceFrequency(&Freq);
	QueryPerformanceCounter(&BeginTime);
	sum = 0;
	for (unsigned int i = 0; i < 50; ++i)
	{
		for (unsigned int k = 0; k < 1000; k+=2)
		{
			sum += m3[i][k];
		}
	}
	QueryPerformanceCounter(&EndTime);
	Time = static_cast<double>(EndTime.QuadPart - BeginTime.QuadPart) / Freq.QuadPart;
	std::cout << "m3 time: " << Time << std::endl;
}

int main()
{
	const std::string FilePath = "./testData/Tile_21_L24_000054100.bint";
	std::vector<double> TimeSet;
	auto Stream = __getFileStream(FilePath);
	LARGE_INTEGER BeginTime;
	LARGE_INTEGER EndTime;
	LARGE_INTEGER Freq;

	for (unsigned int i = 0; i < 10000; i++)
	{
		if (boost::ends_with(FilePath, ".bin"))
		{
			auto pGeometry = std::make_shared<SGeometry>();
			
			QueryPerformanceFrequency(&Freq);
			QueryPerformanceCounter(&BeginTime);
			//process
			__processGeometryBuffer(Stream.get(), pGeometry);

			QueryPerformanceCounter(&EndTime);
			const auto Time = static_cast<double>(EndTime.QuadPart - BeginTime.QuadPart) / Freq.QuadPart;

			TimeSet.emplace_back(Time);
		}
		else
		{
			auto pTexture = std::make_shared<STexture>();

			QueryPerformanceFrequency(&Freq);
			QueryPerformanceCounter(&BeginTime);
			//process
			__processTextureBuffer(Stream.get(), pTexture);

			QueryPerformanceCounter(&EndTime);
			const auto Time = static_cast<double>(EndTime.QuadPart - BeginTime.QuadPart) / Freq.QuadPart;

			TimeSet.emplace_back(Time);
		}
	}
	//total time
	double TotalTime=0;
	for (auto &i : TimeSet)
		TotalTime += i;

	double AverageTime = TotalTime / TimeSet.size();

	std::cout << "Total time:" << TotalTime << "," << "Average Time:" << AverageTime << std::endl;
	
	/*SConfig Config;
	initFromConfigureFile(Config);
	std::vector<std::string> PathSet;
	const auto Size = generatePathSet(Config, PathSet, Config.SEED + 1);
	TestProcessTime(PathSet, Size);
	
	std::vector<std::string> PathSet1;
	const auto Size1 = generatePathSet(Config, PathSet1, Config.SEED + 2);
	TestFileTime(PathSet1, Size1);*/

	//TestMapTime();

	////// Open the file that we want to map.
	//HANDLE hFile = ::CreateFile(L"F:/Resource/CD/Bin/Tile_1/Tile_1.bin",
	//	GENERIC_READ | GENERIC_WRITE,
	//	0,
	//	NULL,
	//	OPEN_ALWAYS,
	//	FILE_ATTRIBUTE_NORMAL,
	//	NULL);
	//// Create a file-mapping object for the file.
	//HANDLE hFileMapping = ::CreateFileMapping(hFile,
	//	NULL,
	//	PAGE_WRITECOPY,
	//	0, 0,
	//	NULL);
	//// Now map the file
	//PCHAR pbFile = (PCHAR)::MapViewOfFile(hFileMapping, FILE_MAP_COPY, 0, 0, 0);
	//
	//// Close all file handle
	//::UnmapViewOfFile(pbFile);
	//::CloseHandle(hFileMapping);
	//::CloseHandle(hFile);
	
	return 0;
}