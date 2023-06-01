#pragma once

#include <unordered_map>
#include <string>
#include "utils/ThreadUtil.h"

/**
 * 所有service的模板基类
 * 定义模板：
 * typename T - BaseService的子类
 * typename S - BaseRepository的子类
 */
template <typename T, typename S>
class BaseService
{
public:
	static T * getInstance();
	static S * getRepository();
private:
	static T * theInstance;
	static S * theRepository;	
protected:
	std::unordered_map<unsigned long, std::wstring> errorCode;
	std::unordered_map<unsigned long, std::wstring> errorMsg;
public:
	std::wstring & getErrorCode();
	std::wstring & getErrorMsg();

	void setErrorCode(std::wstring code);
	void setErrorMsg(std::wstring msg);
	void setError(std::wstring code, std::wstring  msg);
};


template <typename T, typename S> 
T * BaseService<T, S>::theInstance = nullptr;

template <typename T, typename S> 
S * BaseService<T, S>::theRepository = nullptr;


template <typename T, typename S>
T * BaseService<T, S>::getInstance()
{
	if (BaseService::theInstance == nullptr) {
		BaseService::theInstance = new T();
	}

	return BaseService::theInstance;
}

template <typename T, typename S>
S * BaseService<T, S>::getRepository()
{
	if (BaseService::theRepository == nullptr) {
		BaseService::theRepository = S::getInstance();
	}
	return BaseService::theRepository;
}

template <typename T, typename S>
std::wstring & BaseService<T, S>::getErrorCode()
{
	unsigned long theadId = ThreadUtil::currentThreadId();
	return errorCode[theadId];
}

template <typename T, typename S>
std::wstring & BaseService<T, S>::getErrorMsg()
{
	unsigned long theadId = ThreadUtil::currentThreadId();
	return errorMsg[theadId];
}

template <typename T, typename S>
void BaseService<T, S>::setErrorCode(std::wstring code)
{
	unsigned long theadId = ThreadUtil::currentThreadId();
	errorCode[theadId] = code;
}

template <typename T, typename S>
void BaseService<T, S>::setErrorMsg(std::wstring msg)
{
	unsigned long theadId = ThreadUtil::currentThreadId();
	errorMsg[theadId] = msg;
}

template <typename T, typename S>
void BaseService<T, S>::setError(std::wstring code, std::wstring  msg)
{
	unsigned long theadId = ThreadUtil::currentThreadId();
	errorCode[theadId] = code;
	errorMsg[theadId] = msg;
}



