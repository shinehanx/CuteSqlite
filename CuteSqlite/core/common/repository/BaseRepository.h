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

	QSqlDatabase * getConnect();
protected:
	static T * theInstance;
	bool isInitConnect = false;
	static QSqlDatabase * connect; //singleton
    std::unordered_map<UINT, std::wstring> errorCode;
    std::unordered_map<UINT, std::wstring> errorMsg;
	std::wstring localDir;

	uint64_t getLastId(std::wstring tbl);
	std::wstring initDbFile();

	//生成WHERE语句
	std::wstring wherePrepareClause(QCondition & condition, 
		QKeywordIncludes & keywordIncludes = std::vector<std::wstring>());


	//生成ORDER BY 语句
	std::wstring orderClause(const QSortFieldList & sortFieldList, const std::wstring &defaultOrderClause=std::wstring(L" ORDER BY created_at DESC "));

	std::wstring limitClause(const QPagePair & pagePair);

	//生成IN语句
	std::wstring BaseRepository<T>::whereInClause(std::wstring field, std::vector<uint64_t> & vec);

	void queryBind(QSqlStatement & query, QCondition & condition) const;	
};

template <typename T>
uint64_t BaseRepository<T>::getLastId(std::wstring tbl)
{
	std::wstring sql = L"SELECT max(id) FROM :tbl ";

	try {
		QSqlStatement query(getConnect(), sql.c_str());
		query.bind(L":uid", uid);
		query.bind(L":analysis_type", analysisType);
		query.bind(L":analysis_date", analysisDate);
	
		//执行语句
		if (!query.executeStep()) {
			return Analysis();			
		}
		
		Q_INFO(L"Get analysis detail success");
		Analysis item = toAnalysis(query);
		return item;
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"exec sql has error, code:{}, msg:{}, sql:{}", e.getErrorCode(), _err, sql);
		throw QRuntimeException(L"10018", L"sorry, system has error.");
	}	
}

template <typename T>
QSqlDatabase * BaseRepository<T>::connect = nullptr;

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
QSqlDatabase * BaseRepository<T>::getConnect()
{
	if (connect == nullptr) {
		//auto conn = std::make_shared<QSqlDatabase>(L"HairAnalyzer");
		//connect = conn.get();
		connect = new QSqlDatabase(L"CuteSqlite");
	}
	
    Q_INFO(L"BaseRepository::getConnect, local db connect...");
    Q_INFO(L"BaseRepository::getConnect, connect.isValid():{}, connect.isOpen():{}" , connect->isValid() , connect->isOpen());

	if (!connect->isValid() || !connect->isOpen()) {
		std::wstring dbPath = initDbFile();
		Q_INFO(L"db path:{}",dbPath.c_str());
		connect->setDatabaseName(dbPath);

		if (!connect->open()) {
			Q_INFO(L"db connect.open Error:{}", connect->lastError());
			setErrorMsg(connect->lastError());
			ATLASSERT(connect->isValid() && connect->isOpen());
		}
	}

    return connect;
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
std::wstring BaseRepository<T>::initDbFile()
{
	if (localDir.empty()) {
		localDir = ResourceUtil::getProductBinDir();
	}
    ATLASSERT(!localDir.empty());

    std::wstring dbDir = localDir + L".magic\\data\\" ;
	char * ansiDbDir = StringUtil::unicodeToUtf8(dbDir);
    if (_waccess(dbDir.c_str(), 0) != 0) { //文件目录不存在
        Q_INFO(L"mkpath:{}", dbDir);
		//创建DB目录，子目录不存在，则创建
		FileUtil::createDirectory(ansiDbDir);  
    }
	free(ansiDbDir);


    std::wstring path = dbDir + L"CuteSqlite.s3db";
	char * ansiPath = StringUtil::unicodeToUtf8(path);
    if (_access(ansiPath, 0) != 0) { //文件不存在
		std::wstring origPath = localDir + L"res\\db\\CuteSqlite.s3db" ;
		char * ansiOrigPath = StringUtil::unicodeToUtf8(origPath.c_str());
		ATLASSERT(_access(ansiOrigPath, 0) == 0);

		errno_t _err;
		char _err_buf[80] = { 0 };
		FILE * origFile, *destFile;
		_err = fopen_s(&origFile, ansiOrigPath, "rb"); //原文件
		if (_err != 0 || origFile == NULL) {
			_strerror_s(_err_buf, 80, NULL);
			std::wstring _err_msg = StringUtil::utf82Unicode(_err_buf);
			Q_ERROR(L"orgin db file open error,error:{},path:{}",_err_msg,  origPath);
			ATLASSERT(_err == 0);
		}
        _err = fopen_s(&destFile, ansiPath,  "wb"); //目标文件
		if (_err != 0 || destFile == NULL) {
			_strerror_s(_err_buf, 80, NULL);
			std::wstring _err_msg = StringUtil::utf82Unicode(_err_buf);
			Q_ERROR(L"dest db file open error,error:{},path:{}",_err_msg,  path);
			ATLASSERT(_err == 0);
		}
		char ch = fgetc(origFile);		
		while (!feof(origFile)) {
			_err = fputc(ch, destFile);
			ch = fgetc(origFile);			
		}		

		fclose(destFile);
		fclose(origFile);
		free(ansiOrigPath);
    }
    
	free(ansiPath);
    return path;
}


/// <summary>
/// Wheres语句(给prepare用的)
/// </summary>
/// <param name="condition">The condition.</param>
/// <param name="keywordIncludes">如何包含keyword项，需要like %keyword%的字段</param>
/// <returns></returns>
template <typename T>
std::wstring BaseRepository<T>::wherePrepareClause(QCondition & condition, QKeywordIncludes & keywordIncludes)
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

	//keyword语句,根据字段拼接
	if (condition.find(L"keyword") != condition.end()
		&& !condition[L"keyword"].empty() && !keywordIncludes.empty()) {
		whereClause.append(L" AND (0 ");
		for (auto key : keywordIncludes) {
			whereClause.append(L" OR ")
				.append(key)
				.append(L" like :keyword");
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
