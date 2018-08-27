#pragma once
#include "Tools.h"
template<class _type>
class Module
{
protected:
	static _type*& instance() {
		static _type* inst = NULL;
		return inst;
	}
	static ErrorCalls& LastError() {
		static ErrorCalls lastError = SUCCESS;
		return lastError;
	}
public:
	static ErrorCalls& GetLastError()
	{
		return LastError();
	}
	static ErrorCalls StartModule(void* initValue)
	{
		if (instance() != NULL)
		{
			LastError() = FAILURE;
			return LastError();
		}

		instance() = new _type();
		LastError() = instance()->init(initValue);
		return LastError();
	}
	static _type& GetInstance()
	{
		if (instance() == NULL)
			LastError() = FAILURE;
		return *instance();
	}
	static _type* GetPointerInstance()
	{
		if (instance() == NULL)
			LastError() = FAILURE;
		return instance();
	}
	static void CloseModule()
	{
		instance()->release();
		LastError() = FAILURE;
		delete instance();
		instance() = NULL;
	}
public:
	Module() {}
	virtual ~Module() {}
};