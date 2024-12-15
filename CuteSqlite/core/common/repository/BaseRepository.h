#ifndef _BASEREPOSITORY_H
#define _BASEREPOSITORY_H

#include <string>
#include <unordered_map>
#include <atldef.h>
#include <memory>
#include <fstream>
#include <vector>
#include <utility>
#include "utils/Log.h"
#include "utils/ResourceUtil.h"
#include "utils/FileUtil.h"
#include "utils/StringUtil.h"
#include "utils/ThreadUtil.h"
#include "utils/ResourceUtil.h"
#include "common/Config.h"
#include "QSqlDatabase.h"
#include "QSqlStatement.h"
#include "QSqlColumn.h"
#include "QConnect.h"

//sql where条件使用的类型
typedef std::unordered_map<std::wstring, std::wstring>  QCondition;
//sql where 条件语句生成like keyword包含的字段
typedef std::vector<std::wstring> QKeywordIncludes;

// 排序字段
typedef struct QSortField {
	std::wstring field;
	std::wstring ascDesc;
} QSortField ;
// 排序字段列表
typedef std::vector<QSortField> QSortFieldList;

//sql limit 分页语句<curpage, perpage>
typedef std::pair<int, int> QPagePair;

/**
 * @brief 所有存储类的基类
 * @author Qinxuehan
 * @since 2022-03-21
 */
template <typename T>
class BaseRepository
{
public:
	static T * getInstance();//singleton

    // 配置本地DB存放目录
    void setLocalDir(const std::wstring & path);

    std::wstring getErrorMsg();
    std::wstring getErrorCode();

	void setError(std::wstring code, std::wstring msg);
    void setErrorCode(std::wstring code);
    void setErrorMsg(std::wstring msg);

	QSqlDatabase * getSysConnect();
	void closeSysConnect();
protected:
	static T * theInstance;
	bool isInitConnect = false;
	
    std::unordered_map<UINT, std::wstring> errorCode;
    std::unordered_map<UINT, std::wstring> errorMsg;
	std::wstring localDir;

	uint64_t getLastId(std::wstring tbl);
	std::wstring initSysDbFile();

	//生成WHERE语句
	std::wstring wherePrepareClause(QCondition & condition, 
		QKeywordIncludes & keywordIncludes = std::vector<std::wstring>(), bool caseSensitive = true);


	//生成ORDER BY 语句
	std::wstring orderClause(const QSortFieldList & sortFieldList, const std::wstring &defaultOrderClause=std::wstring(L" ORDER BY created_at DESC "));

	std::wstring limitClause(const QPagePair & pagePair);

	//生成IN语句
	std::wstring whereInClause(std::wstring field, std::vector<uint64_t> & vec);

	void queryBind(QSqlStatement & query, QCondition & condition) const;
};

template <typename T>
uint64_t BaseRepository<T>::getLastId(std::wstring tbl)
{
	std::wstring sql = L"SELECT max(id) FROM :tbl ";

	try {
		QSqlStatement query(getSysConnect(), sql.c_str());;
	
		//执行语句
		if (!query.executeStep()) {
			return 0;			
		}
		
		uint64_t maxId = query.getColumn(0).getInt64();
		return maxId;
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"exec sql has error, code:{}, msg:{}, sql:{}", e.getErrorCode(), _err, sql);
		throw QRuntimeException(L"10018", L"sorry, system has error.");
	}	
}

template <typename T>
T * BaseRepository<T>::theInstance = nullptr;

template <typename T>
T * BaseRepository<T>::getInstance()
{
	if (BaseRepository::theInstance == nullptr) {
		theInstance = new T();
	}
	return theInstance;
}

// 配置本地工程存放目录
template <typename T>
void  BaseRepository<T>::setLocalDir(const std::wstring & dir)
{
    localDir = dir;
}

template <typename T>
QSqlDatabase * BaseRepository<T>::getSysConnect()
{
	if (QConnect::sysConnect == nullptr) {
		//auto conn = std::make_shared<QSqlDatabase>(L"HairAnalyzer");
		//connect = conn.get();
		QConnect::sysConnect = new QSqlDatabase(L"CuteSqlite");
	}
	
    Q_INFO(L"BaseRepository::getConnect, local db connect...");
    Q_INFO(L"BaseRepository::getConnect, connect.isValid():{}, connect.isOpen():{}" , QConnect::sysConnect->isValid() , QConnect::sysConnect->isOpen());

	if (!QConnect::sysConnect->isValid() || !QConnect::sysConnect->isOpen()) {
		std::wstring dbPath = initSysDbFile();
		Q_INFO(L"db path:{}",dbPath.c_str());
		QConnect::sysConnect->setDatabaseName(dbPath);

		if (!QConnect::sysConnect->open()) {
			Q_INFO(L"db connect.open Error:{}", QConnect::sysConnect->lastError());
			setErrorMsg(QConnect::sysConnect->lastError());
			ATLASSERT(QConnect::sysConnect->isValid() && QConnect::sysConnect->isOpen());
		}
	}

    return QConnect::sysConnect;
}

template <typename T>
void BaseRepository<T>::closeSysConnect()
{
	if (QConnect::sysConnect == nullptr) {
		return;
	}

	// 1) close the connect
	if (QConnect::sysConnect->isValid() && QConnect::sysConnect->isOpen()) {
		QConnect::sysConnect->close();
	}
	// 2) delete the ptr
	delete QConnect::sysConnect;
	QConnect::sysConnect = nullptr;
}

template <typename T>
std::wstring BaseRepository<T>::getErrorMsg()
{
	UINT threadId = ThreadUtil::currentThreadId();
    return errorMsg[threadId];
}

template <typename T>
std::wstring BaseRepository<T>::getErrorCode()
{
	UINT threadId = ThreadUtil::currentThreadId();
    return errorCode[threadId];
}


template <typename T>
void BaseRepository<T>::setErrorMsg(std::wstring msg)
{
	UINT threadId = ThreadUtil::currentThreadId();
    errorMsg[threadId] = msg;
}

template <typename T>
void BaseRepository<T>::setErrorCode(std::wstring msg)
{
	UINT threadId = ThreadUtil::currentThreadId();
    errorCode[threadId] = code;
}

template <typename T>
void BaseRepository<T>::setError(std::wstring code, std::wstring msg)
{
	UINT threadId = ThreadUtil::currentThreadId();
    errorCode[threadId] = code;
    errorMsg[threadId] = msg;
}


template <typename T>
std::wstring BaseRepository<T>::initSysDbFile()
{
	if (localDir.empty()) {
		localDir = ResourceUtil::getProductBinDir();
	}
    ATLASSERT(!localDir.empty());

    std::wstring dbDir = localDir + L".magic\\data\\" ;
    std::wstring destPath = dbDir + L"CuteSqlite.s3db";
    if (_waccess(destPath.c_str(), 0) != 0) { //文件不存在
		std::wstring origPath = localDir + L"res\\db\\CuteSqlite.s3db" ;
		ATLASSERT(_waccess(origPath.c_str(), 0) == 0);

		FileUtil::copy(origPath, destPath);
    }
    
    return destPath;
}

/**
 * Wheres clause for QSqlStatement::prepare() function
 * 
 * @param condition - The condition params map of where clause.
 * @param keywordIncludes - The <key> vector for generating <like clause> in <where clause>, such as "where <key> like :keyword"
 * @return 
 */
template <typename T>
std::wstring BaseRepository<T>::wherePrepareClause(QCondition & condition, QKeywordIncludes & keywordIncludes, bool caseSensitive)
{
	//where-语句
	std::wstring whereClause;
	if (condition.empty()) {
		return whereClause;
	}
	whereClause.append(L" WHERE 1 ");
	for (auto item : condition) {
		if (item.first == L"keyword") {
			continue;;
		}
		if (item.second.empty()) {//不支持传空的字符串
			continue;;
		}
		whereClause.append(L" AND ")
			.append(item.first)
			.append(L"=")
			.append(L":")
			.append(item.first)
			.append(L" ");
	}

	// keyword语句,根据字段拼接
	if (condition.find(L"keyword") != condition.end()
		&& !condition[L"keyword"].empty() && !keywordIncludes.empty()) {
		whereClause.append(L" AND (0 ");
		for (auto key : keywordIncludes) {
			if (caseSensitive) {
				whereClause.append(L" OR ")
					.append(key)
					.append(L" like :keyword");
			} else {
				whereClause.append(L" OR ")
					.append(key)
					.append(L" like :low_keyword")
					.append(L" OR ")
					.append(key)
					.append(L" like :up_keyword");
			}
			
		}
		whereClause.append(L") ");
	}

	return whereClause;
}
template <typename T>
std::wstring BaseRepository<T>::whereInClause(std::wstring field, std::vector<uint64_t> & vec)
{
	std::wstring whereClause;
	if (vec.empty()) {
		return whereClause;
	}

	whereClause.append(L" WHERE ").append(field).append(L" in (");
	std::wstring insql = L"";
	for (auto item : vec) {
		insql.append(std::to_wstring(item)).append(L",");
	}
	//去掉最后一个逗号
	insql = !insql.empty() ? insql.substr(0,  insql.size()-1) : L"";
	whereClause.append(insql).append(L")");

	return whereClause;
}

//生成ORDER语句，如果不需要则指定默认的defaultOrderClause
template <typename T>
std::wstring BaseRepository<T>::orderClause(const QSortFieldList & sortFieldList, const std::wstring &defaultOrderClause)
{
	std::wstring orderSql = L" ORDER BY ";
	if (sortFieldList.empty()) {
		return defaultOrderClause;
	}

	for (auto item : sortFieldList) {
		orderSql.append(item.field).append(L" ");
		orderSql.append(item.ascDesc).append(L",");
	}
	if (!sortFieldList.empty()) {
		//去掉最后一个逗号
		orderSql = orderSql.substr(0, orderSql.size() - 1);

	}
	return orderSql;
}

template <typename T>
std::wstring BaseRepository<T>::limitClause(const QPagePair & pagePair)
{
	if (pagePair.first <= 0 && pagePair.second <= 0) {
		return L"";
	}
	int pos = (pagePair.first-1) * pagePair.second;
	std::wstring limitSql = L" LIMIT ";
	std::wstring perpage = to_wstring(pagePair.second);
	std::wstring strpos = to_wstring(pos);
	limitSql.append(perpage).append(L" OFFSET ").append(strpos);

	return limitSql;
}

template <typename T>
void BaseRepository<T>::queryBind(QSqlStatement & query, QCondition & condition) const
{
	if (condition.empty()) {
		return ;
	}
	for (auto item : condition) {
		if (item.first == L"keyword" && condition[L"keyword"].empty()) {
			continue;
		}
		if (item.second.empty()) {//不支持传空的字符串
			continue;
		}
		std::wstring apname = L":" + item.first;
		std::wstring val;
		if (item.first == L"keyword") {
			val = L"%";
			val.append(item.second).append(L"%");
		}else {
			val = item.second;
		}
		
		query.bind(apname, val);
	}
}


#endif // BaseLocalRepository_H


