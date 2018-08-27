#pragma once

enum ErrorCalls
{
	FAILURE = -1,
	SUCCESS
};

#define FREE_MEMORY(ptr) if(ptr != NULL) delete ptr;