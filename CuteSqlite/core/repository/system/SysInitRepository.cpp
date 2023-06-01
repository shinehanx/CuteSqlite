#include "stdafx.h"
#include "SysInitRepository.h"
#include "core/common/repository/QSqlException.h"
#include "core/common/repository/QSqlDatabase.h"
#include "core/common/repository/QSqlStatement.h"
#include "core/common/repository/QSqlColumn.h"
#include "core/common/exception/QRuntimeException.h"
#include "utils/Log.h"
#include "core/entity/Entity.h"

std::wstring SysInitRepository::get(const std::wstring & name)
{
	std::wstring sql = L"SELECT * FROM sys_init name=:name;";

	try {
		QSqlStatement query(getConnect(), sql.c_str());
		query.bind(L"name", name);
		if (query.executeStep()) {
			return query.getColumn(L"val").getText();;
		}
		throw QRuntimeException(L"000012", L"sorry, has no setting,name:"  + name );
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"query sys_init has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QRuntimeException(L"000011", L"sorry, system has error when we are loading settings.");
	}
}

bool SysInitRepository::has(const std::wstring & name)
{
	std::wstring sql = L"SELECT * FROM sys_init WHERE name=:name";

	try {
		QSqlStatement query(getConnect(), sql.c_str());
		query.bind(L":name", name);
		if (query.executeStep()) {
			return true;
		}
		return false;
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"query sys_init has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QRuntimeException(L"000013", L"sorry, system has error when we are loading settings.");
	}
}

void SysInitRepository::set(const std::wstring & name, const std::wstring & val)
{
	bool hasName = has(name);
	std::wstring sql = hasName ? L"UPDATE sys_init SET val=:val WHERE name=:name;" :
		L"INSERT INTO sys_init (name, val) VALUES(:name, :val);";
	
	try {
		QSqlStatement query(getConnect(), sql.c_str());
		query.bind(L":name", name);
		query.bind(L":val", val);
		query.executeStep();
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"query sys_init has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QRuntimeException(L"000014", L"sorry, system has error when we are loading settings.");
	}
}

SysInitList SysInitRepository::getAll()
{
	SysInitList result;
	std::wstring sql = L"SELECT * FROM sys_init";
	try {
		QSqlStatement query(getConnect(), sql.c_str());
		
		while (query.executeStep()) {
			SysInit item;
			item.name = query.getColumn(L"name").getText();
			item.val = query.getColumn(L"val").getText();
			result.push_back(item);
		}
		return result;
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"query sys_init has error:{}, msg:{}", e.getErrorCode(), _err); 
		throw QRuntimeException(L"000015", L"sorry, system has error when we are loading settings.");
	}
}
