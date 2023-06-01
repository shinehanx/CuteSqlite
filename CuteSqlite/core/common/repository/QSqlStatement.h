#pragma once

#include "stdafx.h"
#include <string>
#include <sqlite3/sqlite3.h>
#include <map>
#include <memory>
#include "QSqlDatabase.h"
#include "QSqlUtil.h"
#include "QSqlException.h"
// Forward declarations to avoid inclusion of <sqlite3.h> in a header
struct sqlite3;
struct sqlite3_stmt;

namespace SQLite
{

	extern const int OK; ///< SQLITE_OK
}
class QSqlDatabase;

//因为QSqlStatement.h和QColumn.h互相递归包含，互相使用对方的类，所以需要解决包含的问题：
//这里使用了友元类，参考https://blog.csdn.net/u013435183/article/details/47750745，(文档里第二种方法)
//注意:第一，QSqlStatement.h不要包含#include "QSqlColumn.h"，但是QSqlStatement.cpp必须包含#include "QSqlColumn.h"，QSqlColumn.h需要类声明class QSqlColumn;(参见QSqlStatement.h)
//	   第二，QSqlColumn需要声明友元类friend class QSqlStatement;（参见QSqlColumn.h），QSqlColumn.h需要包含#include "QSqlStatement.h"
class QSqlColumn;

class QSqlStatement
{
	
public:
	/**
		* @brief Compile and register the SQL query for the provided SQLite Database Connection
		*
		* @param[in] aDatabase the SQLite Database Connection
		* @param[in] apQuery   an UTF-8 encoded query string
		*
		* Exception is thrown in case of error, then the Statement object is NOT constructed.
		*/
	QSqlStatement(const QSqlDatabase* aDatabase, const wchar_t* apQuery);

	
	// Statement is non-copyable
	QSqlStatement(const QSqlStatement&) = delete;
	QSqlStatement& operator=(const QSqlStatement&) = delete;

	// TODO: Change Statement move constructor to default
	QSqlStatement(QSqlStatement&& aStatement) noexcept;
	QSqlStatement& operator=(QSqlStatement&& aStatement) noexcept = default;

	/// Finalize and unregister the SQL query from the SQLite Database Connection.
	/// The finalization will be done by the destructor of the last shared pointer
	~QSqlStatement() = default;

	/// Reset the statement to make it ready for a new execution by calling sqlite3_reset.
	/// Throws an exception on error.
	/// Call this function before any news calls to bind() if the statement was already executed before.
	/// Calling reset() does not clear the bindings (see clearBindings()).
	void reset();

	/// Reset the statement. Returns the sqlite result code instead of throwing an exception on error.
	int tryReset() noexcept;

	/**
		* @brief Clears away all the bindings of a prepared statement.
		*
		*  Contrary to the intuition of many, reset() does not reset the bindings on a prepared statement.
		*  Use this routine to reset all parameters to NULL.
		*/
	void clearBindings(); // throw(SQLite::Exception)

	////////////////////////////////////////////////////////////////////////////
	// Bind a value to a parameter of the SQL statement,
	// in the form "?" (unnamed), "?NNN", ":VVV", "@VVV" or "$VVV".
	//
	// Can use the parameter index, starting from "1", to the higher NNN value,
	// or the complete parameter name "?NNN", ":VVV", "@VVV" or "$VVV"
	// (prefixed with the corresponding sign "?", ":", "@" or "$")
	//
	// Note that for text and blob values, the SQLITE_TRANSIENT flag is used,
	// which tell the sqlite library to make its own copy of the data before the bind() call returns.
	// This choice is done to prevent any common misuses, like passing a pointer to a
	// dynamic allocated and temporary variable (a std::wstring for instance).
	// This is under-optimized for static data (a static text define in code)
	// as well as for dynamic allocated buffer which could be transfer to sqlite
	// instead of being copied.
	// => if you know what you are doing, use bindNoCopy() instead of bind()

	SQLITECPP_PURE_FUNC
		int getIndex(const wchar_t * apName) const;

	/**
		* @brief Bind an int value to a parameter "?", "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*/
	void bind(const int aIndex, const int32_t       aValue);
	/**
		* @brief Bind a 32bits unsigned int value to a parameter "?", "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*/
	void bind(const int aIndex, const uint32_t      aValue);
	/**
		* @brief Bind a 64bits int value to a parameter "?", "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*/
	void bind(const int aIndex, const int64_t       aValue);

	/**
		* @brief Bind a 64bits int value to a parameter "?", "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*/
	void bind(const int aIndex, const uint64_t       aValue);

	/**
		* @brief Bind a double (64bits float) value to a parameter "?", "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*/
	void bind(const int aIndex, const double        aValue);
	/**
		* @brief Bind a string value to a parameter "?", "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* @note Uses the SQLITE_TRANSIENT flag, making a copy of the data, for SQLite internal use
		*/
	void bind(const int aIndex, const std::wstring&  aValue);
	/**
		* @brief Bind a text value to a parameter "?", "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* @note Uses the SQLITE_TRANSIENT flag, making a copy of the data, for SQLite internal use
		*/
	void bind(const int aIndex, const wchar_t*         apValue);
	/**
		* @brief Bind a binary blob value to a parameter "?", "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* @note Uses the SQLITE_TRANSIENT flag, making a copy of the data, for SQLite internal use
		*/
	void bind(const int aIndex, const void*         apValue, const int aSize);
	/**
		* @brief Bind a string value to a parameter "?", "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1).
		*
		* The string can contain null characters as it is binded using its size.
		*
		* @warning Uses the SQLITE_STATIC flag, avoiding a copy of the data. The string must remains unchanged while executing the statement.
		*/
	void bindNoCopy(const int aIndex, const std::wstring&    aValue);
	/**
		* @brief Bind a text value to a parameter "?", "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* Main usage is with null-terminated literal text (aka in code static strings)
		*
		* @warning Uses the SQLITE_STATIC flag, avoiding a copy of the data. The string must remains unchanged while executing the statement.
		*/
	void bindNoCopy(const int aIndex, const wchar_t*           apValue);
	/**
		* @brief Bind a binary blob value to a parameter "?", "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* @warning Uses the SQLITE_STATIC flag, avoiding a copy of the data. The string must remains unchanged while executing the statement.
		*/
	void bindNoCopy(const int aIndex, const void*           apValue, const int aSize);
	/**
		* @brief Bind a NULL value to a parameter "?", "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* @see clearBindings() to set all bound parameters to NULL.
		*/
	void bind(const int aIndex);

	/**
		* @brief Bind an int value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*/
	void bind(const wchar_t* apName, const int32_t         aValue)
	{
		bind(getIndex(apName), aValue);
	}
	/**
		* @brief Bind a 32bits unsigned int value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*/
	void bind(const wchar_t* apName, const uint32_t        aValue)
	{
		bind(getIndex(apName), aValue);
	}
	/**
		* @brief Bind a 64bits int value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*/
	void bind(const wchar_t* apName, const int64_t         aValue)
	{
		bind(getIndex(apName), aValue);
	}

	/**
		* @brief Bind a 64bits int value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*/
	void bind(const wchar_t* apName, const uint64_t         aValue)
	{
		bind(getIndex(apName), aValue);
	}

	/**
		* @brief Bind a double (64bits float) value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*/
	void bind(const wchar_t * apName, const double          aValue)
	{
		bind(getIndex(apName), aValue);
	}
	/**
		* @brief Bind a string value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* @note Uses the SQLITE_TRANSIENT flag, making a copy of the data, for SQLite internal use
		*/
	void bind(const wchar_t * apName, const std::wstring&  aValue)
	{
		bind(getIndex(apName), aValue);
	}
	/**
		* @brief Bind a text value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* @note Uses the SQLITE_TRANSIENT flag, making a copy of the data, for SQLite internal use
		*/
	void bind(const wchar_t * apName, const wchar_t*           apValue)
	{
		bind(getIndex(apName), apValue);
	}
	/**
		* @brief Bind a binary blob value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* @note Uses the SQLITE_TRANSIENT flag, making a copy of the data, for SQLite internal use
		*/
	void bind(const wchar_t * apName, const void*           apValue, const int aSize)
	{
		bind(getIndex(apName), apValue, aSize);
	}
	/**
		* @brief Bind a string value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* The string can contain null characters as it is binded using its size.
		*
		* @warning Uses the SQLITE_STATIC flag, avoiding a copy of the data. The string must remains unchanged while executing the statement.
		*/
	void bindNoCopy(const wchar_t * apName, const std::wstring&  aValue)
	{
		bindNoCopy(getIndex(apName), aValue);
	}
	/**
		* @brief Bind a text value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* Main usage is with null-terminated literal text (aka in code static strings)
		*
		* @warning Uses the SQLITE_STATIC flag, avoiding a copy of the data. The string must remains unchanged while executing the statement.
		*/
	void bindNoCopy(const wchar_t* apName, const wchar_t*         apValue)
	{
		bindNoCopy(getIndex(apName), apValue);
	}
	/**
		* @brief Bind a binary blob value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* @warning Uses the SQLITE_STATIC flag, avoiding a copy of the data. The string must remains unchanged while executing the statement.
		*/
	void bindNoCopy(const wchar_t* apName, const void*         apValue, const int aSize)
	{
		bindNoCopy(getIndex(apName), apValue, aSize);
	}
	/**
		* @brief Bind a NULL value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* @see clearBindings() to set all bound parameters to NULL.
		*/
	void bind(const wchar_t* apName) // bind NULL value
	{
		bind(getIndex(apName));
	}


	/**
		* @brief Bind an int value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*/
	void bind(const std::wstring& aName, const int32_t         aValue)
	{
		bind(aName.c_str(), aValue);
	}
	/**
		* @brief Bind a 32bits unsigned int value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*/
	void bind(const std::wstring& aName, const uint32_t        aValue)
	{
		bind(aName.c_str(), aValue);
	}
	/**
		* @brief Bind a 64bits int value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*/
	void bind(const std::wstring& aName, const int64_t         aValue)
	{
		bind(aName.c_str(), aValue);
	}

	/**
		* @brief Bind a 64bits int value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*/
	void bind(const std::wstring& aName, const uint64_t         aValue)
	{
		bind(aName.c_str(), aValue);
	}

	/**
		* @brief Bind a double (64bits float) value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*/
	void bind(const std::wstring& aName, const double          aValue)
	{
		bind(aName.c_str(), aValue);
	}
	/**
		* @brief Bind a string value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* @note Uses the SQLITE_TRANSIENT flag, making a copy of the data, for SQLite internal use
		*/
	void bind(const std::wstring& aName, const std::wstring&    aValue)
	{
		bind(aName.c_str(), aValue);
	}
	/**
		* @brief Bind a text value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* @note Uses the SQLITE_TRANSIENT flag, making a copy of the data, for SQLite internal use
		*/
	void bind(const std::wstring& aName, const wchar_t*           apValue)
	{
		bind(aName.c_str(), apValue);
	}
	/**
		* @brief Bind a binary blob value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* @note Uses the SQLITE_TRANSIENT flag, making a copy of the data, for SQLite internal use
		*/
	void bind(const std::wstring& aName, const void*           apValue, const int aSize)
	{
		bind(aName.c_str(), apValue, aSize);
	}
	/**
		* @brief Bind a string value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* The string can contain null characters as it is binded using its size.
		*
		* @warning Uses the SQLITE_STATIC flag, avoiding a copy of the data. The string must remains unchanged while executing the statement.
		*/
	void bindNoCopy(const std::wstring& aName, const std::wstring& aValue)
	{
		bindNoCopy(aName.c_str(), aValue);
	}
	/**
		* @brief Bind a text value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* Main usage is with null-terminated literal text (aka in code static strings)
		*
		* @warning Uses the SQLITE_STATIC flag, avoiding a copy of the data. The string must remains unchanged while executing the statement.
		*/
	void bindNoCopy(const std::wstring& aName, const wchar_t*        apValue)
	{
		bindNoCopy(aName.c_str(), apValue);
	}
	/**
		* @brief Bind a binary blob value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* @warning Uses the SQLITE_STATIC flag, avoiding a copy of the data. The string must remains unchanged while executing the statement.
		*/
	void bindNoCopy(const std::wstring& aName, const void*        apValue, const int aSize)
	{
		bindNoCopy(aName.c_str(), apValue, aSize);
	}
	/**
		* @brief Bind a NULL value to a named parameter "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement (aIndex >= 1)
		*
		* @see clearBindings() to set all bound parameters to NULL.
		*/
	void bind(const std::wstring& aName) // bind NULL value
	{
		bind(aName.c_str());
	}

	////////////////////////////////////////////////////////////////////////////

	/**
		* @brief Execute a step of the prepared query to fetch one row of results.
		*
		*  While true is returned, a row of results is available, and can be accessed
		* through the getColumn() method
		*
		* @see exec() execute a one-step prepared statement with no expected result
		* @see tryExecuteStep() try to execute a step of the prepared query to fetch one row of results, returning the sqlite result code.
		* @see Database::exec() is a shortcut to execute one or multiple statements without results
		*
		* @return - true  (SQLITE_ROW)  if there is another row ready : you can call getColumn(N) to get it
		*                               then you have to call executeStep() again to fetch more rows until the query is finished
		*         - false (SQLITE_DONE) if the query has finished executing : there is no (more) row of result
		*                               (case of a query with no result, or after N rows fetched successfully)
		*
		* @throw SQLite::Exception in case of error
		*/
	bool executeStep();

	/**
		* @brief Try to execute a step of the prepared query to fetch one row of results, returning the sqlite result code.
		*
		*
		*
		* @see exec() execute a one-step prepared statement with no expected result
		* @see executeStep() execute a step of the prepared query to fetch one row of results
		* @see Database::exec() is a shortcut to execute one or multiple statements without results
		*
		* @return the sqlite result code.
		*/
	int tryExecuteStep() noexcept;

	/**
		* @brief Execute a one-step query with no expected result, and return the number of changes.
		*
		*  This method is useful for any kind of statements other than the Data Query Language (DQL) "SELECT" :
		*  - Data Definition Language (DDL) statements "CREATE", "ALTER" and "DROP"
		*  - Data Manipulation Language (DML) statements "INSERT", "UPDATE" and "DELETE"
		*  - Data Control Language (DCL) statements "GRANT", "REVOKE", "COMMIT" and "ROLLBACK"
		*
		* It is similar to Database::exec(), but using a precompiled statement, it adds :
		* - the ability to bind() arguments to it (best way to insert data),
		* - reusing it allows for better performances (efficient for multiple insertion).
		*
		* @see executeStep() execute a step of the prepared query to fetch one row of results
		* @see tryExecuteStep() try to execute a step of the prepared query to fetch one row of results, returning the sqlite result code.
		* @see Database::exec() is a shortcut to execute one or multiple statements without results
		*
		* @return number of row modified by this SQL statement (INSERT, UPDATE or DELETE)
		*
		* @throw SQLite::Exception in case of error, or if row of results are returned while they are not expected!
		*/
	int exec();

	////////////////////////////////////////////////////////////////////////////

	/**
		* @brief Return a copy of the column data specified by its index
		*
		*  Can be used to access the data of the current row of result when applicable,
		* while the executeStep() method returns true.
		*
		*  Throw an exception if there is no row to return a Column from:
		* - if provided index is out of bound
		* - before any executeStep() call
		* - after the last executeStep() returned false
		* - after a reset() call
		*
		*  Throw an exception if the specified index is out of the [0, getColumnCount()) range.
		*
		* @param[in] aIndex    Index of the column, starting at 0
		*
		* @note    This method is not const, reflecting the fact that the returned Column object will
		*          share the ownership of the underlying sqlite3_stmt.
		*
		* @warning The resulting Column object must not be memorized "as-is".
		*          Is is only a wrapper around the current result row, so it is only valid
		*          while the row from the Statement remains valid, that is only until next executeStep() call.
		*          Thus, you should instead extract immediately its data (getInt(), getText()...)
		*          and use or copy this data for any later usage.
		*/
	QSqlColumn  getColumn(const int aIndex) const ;

	/**
		* @brief Return a copy of the column data specified by its column name (less efficient than using an index)
		*
		*  Can be used to access the data of the current row of result when applicable,
		* while the executeStep() method returns true.
		*
		*  Throw an exception if there is no row to return a Column from :
		* - if provided name is not one of the aliased column names
		* - before any executeStep() call
		* - after the last executeStep() returned false
		* - after a reset() call
		*
		*  Throw an exception if the specified name is not an on of the aliased name of the columns in the result.
		*
		* @param[in] apName   Aliased name of the column, that is, the named specified in the query (not the original name)
		*
		* @note    Uses a map of column names to indexes, build on first call.
		*
		* @note    This method is not const, reflecting the fact that the returned Column object will
		*          share the ownership of the underlying sqlite3_stmt.
		*
		* @warning The resulting Column object must not be memorized "as-is".
		*          Is is only a wrapper around the current result row, so it is only valid
		*          while the row from the Statement remains valid, that is only until next executeStep() call.
		*          Thus, you should instead extract immediately its data (getInt(), getText()...)
		*          and use or copy this data for any later usage.
		*
		*  Throw an exception if the specified name is not one of the aliased name of the columns in the result.
		*/
	QSqlColumn  getColumn(const wchar_t* apName) const;

#if __cplusplus >= 201402L || (defined(_MSC_VER) && _MSC_VER >= 1900) // c++14: Visual Studio 2015
     /**
     * @brief Return an instance of T constructed from copies of the first N columns
     *
     *  Can be used to access the data of the current row of result when applicable,
     * while the executeStep() method returns true.
     *
     *  Throw an exception if there is no row to return a Column from:
     * - if provided column count is out of bound
     * - before any executeStep() call
     * - after the last executeStep() returned false
     * - after a reset() call
     *
     *  Throw an exception if the specified column count is out of the [0, getColumnCount()) range.
     *
     * @tparam  T   Object type to construct
     * @tparam  N   Number of columns
     *
     * @note Requires std=C++14
     */
    template<typename T, int N>
    T       getColumns();

private:
    /**
    * @brief Helper function used by getColumns<typename T, int N> to expand an integer_sequence used to generate
    *        the required Column objects
    */
    template<typename T, const int... Is>
    T       getColumns(const std::integer_sequence<int, Is...>);

public:
#endif

	/**
		* @brief Test if the column value is NULL
		*
		* @param[in] aIndex    Index of the column, starting at 0
		*
		* @return true if the column value is NULL
		*
		*  Throw an exception if the specified index is out of the [0, getColumnCount()) range.
		*/
	bool    isColumnNull(const int aIndex) const;

	/**
		* @brief Test if the column value is NULL
		*
		* @param[in] apName    Aliased name of the column, that is, the named specified in the query (not the original name)
		*
		* @return true if the column value is NULL
		*
		*  Throw an exception if the specified name is not one of the aliased name of the columns in the result.
		*/
	bool    isColumnNull(const wchar_t* apName) const;

	/**
		* @brief Return a pointer to the named assigned to the specified result column (potentially aliased)
		*
		* @param[in] aIndex    Index of the column in the range [0, getColumnCount()).
		*
		* @see getColumnOriginName() to get original column name (not aliased)
		*
		*  Throw an exception if the specified index is out of the [0, getColumnCount()) range.
		*/
	const wchar_t* getColumnName(const int aIndex) const;

#ifdef SQLITE_ENABLE_COLUMN_METADATA
	/**
		* @brief Return a pointer to the table column name that is the origin of the specified result column
		*
		*  Require definition of the SQLITE_ENABLE_COLUMN_METADATA preprocessor macro :
		* - when building the SQLite library itself (which is the case for the Debian libsqlite3 binary for instance),
		* - and also when compiling this wrapper.
		*
		*  Throw an exception if the specified index is out of the [0, getColumnCount()) range.
		*/
	const wchar_t* getColumnOriginName(const int aIndex) const;
#endif

	/**
		* @brief Return the index of the specified (potentially aliased) column name
		*
		* @param[in] apName    Aliased name of the column, that is, the named specified in the query (not the original name)
		*
		* @note Uses a map of column names to indexes, build on first call.
		*
		*  Throw an exception if the specified name is not known.
		*/
	int getColumnIndex(const wchar_t* apName) const;


	/**
		* @brief Return the declared type of the specified result column for a SELECT statement.
		*
		*  This is the type given at creation of the column and not the actual data type.
		*  SQLite stores data types dynamically for each value and not per column.
		*
		* @param[in] aIndex    Index of the column in the range [0, getColumnCount()).
		*
		*  Throw an exception if the type can't be determined because:
		*  - the specified index is out of the [0, getColumnCount()) range
		*  - the statement is not a SELECT query
		*  - the column at aIndex is not a table column but an expression or subquery
		*/
	const char * getColumnDeclaredType(const int aIndex) const;


	/// Get number of rows modified by last INSERT, UPDATE or DELETE statement (not DROP table).
	int getChanges() const noexcept;


	////////////////////////////////////////////////////////////////////////////

	/// Return the UTF-8 SQL Query.
	const std::wstring& getQuery() const
	{
		return mQuery;
	}

	// Return a UTF-8 string containing the SQL text of prepared statement with bound parameters expanded.
	std::wstring getExpandedSQL() const;

	/// Return the number of columns in the result set returned by the prepared statement
	int getColumnCount() const
	{
		return mColumnCount;
	}
	/// true when a row has been fetched with executeStep()
	bool hasRow() const
	{
		return mbHasRow;
	}
	/// true when the last executeStep() had no more row to fetch
	bool isDone() const
	{
		return mbDone;
	}

	/// Return the number of bind parameters in the statement
	int getBindParameterCount() const noexcept;

	/// Return the numeric result code for the most recent failed API call (if any).
	int getErrorCode() const noexcept;
	/// Return the extended numeric result code for the most recent failed API call (if any).
	int getExtendedErrorCode() const noexcept;
	/// Return UTF-8 encoded English language explanation of the most recent failed API call (if any).
	const wchar_t* getErrorMsg() const noexcept;

	/// Shared pointer to SQLite Prepared Statement Object
	using TStatementPtr = std::shared_ptr<sqlite3_stmt>;

private:
	/**
		* @brief Check if a return code equals SQLITE_OK, else throw a SQLite::Exception with the SQLite error message
		*
		* @param[in] aRet SQLite return code to test against the SQLITE_OK expected value
		*/
	void check(const int aRet) const
	{
		if (SQLITE_OK != aRet) 
		{
			throw SQLite::QSqlException(mpSQLite, aRet);
		}
	}

	/**
		* @brief Check if there is a row of result returned by executeStep(), else throw a SQLite::Exception.
		*/
	void checkRow() const
	{
		if (false == mbHasRow)
		{
			throw SQLite::QSqlException(L"No row to get a column from. executeStep() was not called, or returned false.");
		}
	}

	/**
		* @brief Check if there is a Column index is in the range of columns in the result.
		*/
	void checkIndex(const int aIndex) const
	{
		if ((aIndex < 0) || (aIndex >= mColumnCount))
		{
			throw SQLite::QSqlException(L"Column index out of range.");
		}
	}

	/**
		* @brief Prepare statement object.
		*
		* @return Shared pointer to prepared statement object
		*/
	TStatementPtr prepareStatement();

	/**
		* @brief Return a prepared statement object.
		*
		* Throw an exception if the statement object was not prepared.
		* @return raw pointer to Prepared Statement Object
		*/
	sqlite3_stmt* getPreparedStatement() const;

	std::wstring             mQuery;                 //!< UTF-8 SQL Query
	sqlite3*                mpSQLite;               //!< Pointer to SQLite Database Connection Handle
	TStatementPtr           mpPreparedStatement;    //!< Shared Pointer to the prepared SQLite Statement Object
	int                     mColumnCount = 0;       //!< Number of columns in the result of the prepared statement
	bool                    mbHasRow = false;       //!< true when a row has been fetched with executeStep()
	bool                    mbDone = false;         //!< true when the last executeStep() had no more row to fetch

	/// Map of columns index by name (mutable so getColumnIndex can be const)
	mutable std::map<std::wstring, int>  mColumnNames;
};



