/**
 * @file    QSqlException.h
 * @ingroup SQLiteCpp
 * @brief   Encapsulation of the error message from SQLite3 on a std::runtime_error.
 *
 * Copyright (c) 2012-2022 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once


#include <stdexcept>

// Forward declaration to avoid inclusion of <sqlite3.h> in a header
struct sqlite3;



namespace SQLite
{


/**
 * @brief Encapsulation of the error message from SQLite3, based on std::runtime_error.
 */
class QSqlException : public std::runtime_error
{
public:
    /**
     * @brief Encapsulation of the error message from SQLite3, based on std::runtime_error.
     *
     * @param[in] aErrorMessage The string message describing the SQLite error
     * @param[in] ret           Return value from function call that failed.
     */
    QSqlException(const wchar_t* aErrorMessage, int ret);

    QSqlException(const std::wstring& aErrorMessage, int ret) :
        QSqlException(aErrorMessage.c_str(), ret)
    {
    }

    /**
     * @brief Encapsulation of the error message from SQLite3, based on std::runtime_error.
     *
     * @param[in] aErrorMessage The string message describing the SQLite error
     */
    explicit QSqlException(const wchar_t* aErrorMessage) :
        QSqlException(aErrorMessage, -1) // 0 would be SQLITE_OK, which doesn't make sense
    {
    }
    explicit QSqlException(const std::wstring& aErrorMessage) :
        QSqlException(aErrorMessage.c_str(), -1) // 0 would be SQLITE_OK, which doesn't make sense
    {
    }

   /**
     * @brief Encapsulation of the error message from SQLite3, based on std::runtime_error.
     *
     * @param[in] apSQLite The SQLite object, to obtain detailed error messages from.
     */
    explicit QSqlException(sqlite3* apSQLite);

    /**
     * @brief Encapsulation of the error message from SQLite3, based on std::runtime_error.
     *
     * @param[in] apSQLite  The SQLite object, to obtain detailed error messages from.
     * @param[in] ret       Return value from function call that failed.
     */
    QSqlException(sqlite3* apSQLite, int ret);

    /// Return the result code (if any, otherwise -1).
    int getErrorCode() const noexcept
    {
        return mErrcode;
    }

    /// Return the extended numeric result code (if any, otherwise -1).
    int getExtendedErrorCode() const noexcept
    {
        return mExtendedErrcode;
    }

    // Return a string, solely based on the error code
    const std::wstring getErrorStr() const noexcept;

private:
    int mErrcode;         ///< Error code value
    int mExtendedErrcode; ///< Detailed error code if any
};


}  // namespace SQLite
