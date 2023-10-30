#pragma once
#include <stdexcept>
#include <string>
class QRuntimeException : public std::runtime_error
{
public:
	/**
     * @brief Encapsulation of the error message from application on std::runtime_error.
     *
     * @param[in] code The string message describing the runtime error
     * @param[in] msg           Return value from function call that failed.
     */
    QRuntimeException(const wchar_t* code, const wchar_t * msg);

	/**
     * @brief Encapsulation of the error message from application on std::runtime_error.
     *
     * @param[in] code The string message describing the runtime error
     * @param[in] msg           Return value from function call that failed.
     */
	QRuntimeException(const std::wstring code, const std::wstring msg) :
		QRuntimeException(code.c_str(), msg.c_str()) {};

	QRuntimeException(const wchar_t* code);
	QRuntimeException(const std::wstring code)
		:QRuntimeException(code.c_str()) {};

	const std::wstring getCode() const;
	const std::wstring getMsg() const;
private:
	std::wstring code;
	std::wstring msg;
};