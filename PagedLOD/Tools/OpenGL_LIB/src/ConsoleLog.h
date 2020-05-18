#pragma once
#include <string>
#include <iostream>
#include <windows.h>

#ifdef _DEBUG
#define _DEBUG_OUTPUT(prompt0) std::cout<<prompt0<<std::endl;
#define _DEBUG_OUTPUT_2(prompt0,prompt1) std::cout<<prompt0<<"###"<<prompt1<<std::endl;
#define _DEBUG_OUTPUT_3(prompt0,prompt1,prompt2) std::cout<<prompt0<<"###"<<prompt1<<"###"<<prompt2<<std::endl;
#else
#define _DEBUG_OUTPUT(prompt0) 
#define _DEBUG_OUTPUT_2(prompt0,prompt1) 
#define _DEBUG_OUTPUT_3(prompt0,prompt1,prompt2)
#endif

#define _OUTPUT_LINE(s) std::cout << s << std::endl;
#define _OUTPUT_LINE2(prompt0, prompt1) std::cout << prompt0 << prompt1 << std::endl;
#ifdef _DEBUG
#define _MEMORY_LEAK_DETECTOR _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#else 
#define _MEMORY_LEAK_DETECTOR
#endif

#define SAFE_DELETE_PTR(ptr) { delete (ptr); (ptr) = nullptr; }
#define SAFE_DELETE_ARRAY(ptr) { delete[] (ptr); (ptr) = nullptr; }

#define START_TIMER(freq, beginTime) \
	QueryPerformanceFrequency(&freq); \
	QueryPerformanceCounter(&beginTime);

#define END_TIMER(freq, beginTime, endTime) \
	QueryPerformanceCounter(&endTime); \
	double TimeNs = (endTime.QuadPart - beginTime.QuadPart) * 1000000.0 / freq.QuadPart; \
	std::cout << "cost time(ns): \t" + std::to_string(TimeNs) << std::endl;
