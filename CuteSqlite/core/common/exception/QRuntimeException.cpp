#include "stdafx.h"
#include "QRuntimeException.h"
#include "utils/StringUtil.h"

QRuntimeException::QRuntimeException(const wchar_t* code, const wchar_t * msg): 
	std::runtime_error(StringUtil::unicode2Utf8(msg))
{
	this->code = code;
	this->msg = msg;
}

const std::wstring QRuntimeException::getCode() const
{
	return code;
}

const std::wstring QRuntimeException::getMsg() const
{
	return msg;
}

