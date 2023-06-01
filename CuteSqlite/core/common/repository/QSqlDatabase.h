#pragma once
#include "stdafx.h"
#include <string>
#include <memory>
#include <unordered_map>
#include "utils/ThreadUtil.h"
#include <sqlite3/sqlite3.h>
#include "QSqlStatement.h"
//#include "QSqlColumn.h"
#include "QSqlException.h"

/**
 * @brief 数据库操作类
 * @author Qinxuehan
 * @since 2022-03-21
 */

class QSqlColumn;

namespace SQLite
{

	// Those public constants enable most usages of SQLiteCpp without including <sqlite3.h> in the client application.

	/// The database is opened in read-only mode. If the database does not already exist, an error is returned.
	extern const int OPEN_READONLY;     // SQLITE_OPEN_READONLY
	/// The database is opened for reading and writing if possible, or reading only if the file is write protected
	/// by the operating system. In either case the database must already exist, otherwise an error is returned.
	extern const int OPEN_READWRITE;    // SQLITE_OPEN_READWRITE
	/// With OPEN_READWRITE: The database is opened for reading and writing, and is created if it does not already exist.
	extern const int OPEN_CREATE;       // SQLITE_OPEN_CREATE
	/// Enable URI filename interpretation, parsed according to RFC 3986 (ex. "file:data.db?mode=ro&cache=private")
	extern const int OPEN_URI;          // SQLITE_OPEN_URI
	/// Open in memory database
	extern const int OPEN_MEMORY;       // SQLITE_OPEN_MEMORY
	/// Open database in multi-thread threading mode
	extern const int OPEN_NOMUTEX;      // SQLITE_OPEN_NOMUTEX
	/// Open database with thread-safety in serialized threading mode
	extern const int OPEN_FULLMUTEX;    // SQLITE_OPEN_FULLMUTEX
	/// Open database with shared cache enabled
	extern const int OPEN_SHAREDCACHE;  // SQLITE_OPEN_SHAREDCACHE
	/// Open database with shared cache disabled
	extern const int OPEN_PRIVATECACHE; // SQLITE_OPEN_PRIVATECACHE
	/// Database filename is not allowed to be a symbolic link (Note: only since SQlite 3.31.0 from 2020-01-22)
	extern const int OPEN_NOFOLLOW;     // SQLITE_OPEN_NOFOLLOW


	extern const int OK;                ///< SQLITE_OK (used by check() bellow)

	extern const char* const VERSION;        ///< SQLITE_VERSION string from the sqlite3.h used at compile time
	extern const int         VERSION_NUMBER; ///< SQLITE_VERSION_NUMBER from the sqlite3.h used at compile time

	/// Return SQLite version string using runtime call to the compiled library
	const char* getLibVersion() noexcept;
	/// Return SQLite version number using runtime call to the compiled library
	int   getLibVersionNumber() noexcept;

	// Public structure for representing all fields contained within the SQLite header.
	// Official documentation for fields: https://www.sqlite.org/fileformat.html#the_database_header
	struct Header {
		unsigned char headerStr[16];
		unsigned int pageSizeBytes;
		unsigned char fileFormatWriteVersion;
		unsigned char fileFormatReadVersion;
		unsigned char reservedSpaceBytes;
		unsigned char maxEmbeddedPayloadFrac;
		unsigned char minEmbeddedPayloadFrac;
		unsigned char leafPayloadFrac;
		unsigned long fileChangeCounter;
		unsigned long  databaseSizePages;
		unsigned long firstFreelistTrunkPage;
		unsigned long totalFreelistPages;
		unsigned long schemaCookie;
		unsigned long schemaFormatNumber;
		unsigned long defaultPageCacheSizeBytes;
		unsigned long largestBTreePageNumber;
		unsigned long databaseTextEncoding;
		unsigned long userVersion;
		unsigned long incrementalVaccumMode;
		unsigned long applicationId;
		unsigned long versionValidFor;
		unsigned long sqliteVersion;
	};
} //namespace SQLite

class QSqlDatabase
{
	
public:
	QSqlDatabase();
	QSqlDatabase(std::wstring name);
	QSqlDatabase(QSqlDatabase & database);
	~QSqlDatabase();

	QSqlDatabase & operator=(QSqlDatabase & database);
	

	bool isValid();
	bool isOpen();
	bool isClose();

	bool open();
	bool close();

	std::wstring lastErrorCode();
	std::wstring lastError();

	void setError(const std::wstring & code, const std::wstring & msg);
	void setErrorCode(const std::wstring & code);
	void setErrorMsg(const std::wstring & msg);

	//getter and setter
	void setDatabaseName(const std::wstring & databaseName);
	void setHostName(const std::wstring & hostName);
	void setUserName(const std::wstring & userName);
	void setPassword(const std::wstring & password);

	sqlite3 * getHandle() const noexcept;
	std::wstring getActiveName() { return activeName; };
	std::wstring getDatabaseName() { return databaseName; };
	std::wstring getHostName() { return hostName; };
	std::wstring getUserName() { return userName; };
	std::wstring getPassword() { return password; };

	void setBusyTimeout(const int aBusyTimeoutMs);

	int exec(const wchar_t * apQueries);

	int tryExec(const wchar_t* apQueries) noexcept;


	QSqlColumn execAndGet(const wchar_t* apQuery);
	bool tableExists(const wchar_t* apTableName) const;

	int64_t getLastInsertRowid() const noexcept;
	int getChanges() const noexcept;
	int getTotalChanges() const noexcept;
	int getErrorCode() const noexcept;
	const wchar_t* getErrorMsg() const noexcept;
	void createFunction(const wchar_t* apFuncName, 
		int aNbArg, 
		bool abDeterministic, 
		void* apApp, 
		void(*apFunc)(sqlite3_context *, int, sqlite3_value **), 
		void(*apStep)(sqlite3_context *, int, sqlite3_value **) /* = nullptr */, 
		void(*apFinal)(sqlite3_context *) /* = nullptr */, 
		/* NOLINT(readability/casting) */ 
		void(*apDestroy)(void *) /* = nullptr */);

	
	// Deleter functor to use with smart pointers to close the SQLite database connection in an RAII fashion.
    struct Deleter
    {
        void operator()(sqlite3* apSQLite);
    };

	/**
     * @brief BackupType for the backup() method
     */
    enum BackupType { Save, Load };

    /**
     * @brief Load or save the database content.
     *
     * This function is used to load the contents of a database file on disk
     * into the "main" database of open database connection, or to save the current
     * contents of the database into a database file on disk.
     *
     * @throw SQLite::Exception in case of error
     */
    //void backup(const char* apFilename, BackupType aType);

    /**
     * @brief Check if aRet equal SQLITE_OK, else throw a SQLite::Exception with the SQLite error message
     */
    void check(const int aRet) const
    {
        if (SQLITE_OK != aRet)
        {
            throw SQLite::QSqlException(getHandle(), aRet);
        }
    }
private:

	//std::unique_ptr<sqlite3, Deleter> handle;
	sqlite3 * handle;

	std::wstring activeName; //激活的数据库
	std::wstring databaseName; //数据库名，SQLITE数据库的路径
	std::wstring hostName; //主机名
	std::wstring userName; //用户名
	std::wstring password;//密码

	//报错
	std::unordered_map<unsigned long , std::wstring> errorCodeMap;
	std::unordered_map<unsigned long , std::wstring> errorMsgMap;

	bool isOpenFlag = false;
	
};
