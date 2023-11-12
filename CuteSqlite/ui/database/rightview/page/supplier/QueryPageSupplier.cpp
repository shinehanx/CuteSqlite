/*****************************************************************//**
 * Copyright 2023 Xuehan Qin 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and

 * limitations under the License.

 * @file   QueryPageSupplier.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-30
 *********************************************************************/
#include "stdafx.h"
#include "QueryPageSupplier.h"
#include "utils/StringUtil.h"

const std::vector<std::wstring> QueryPageSupplier::sqlTags = {
	L"SELECT", L"SELECT *", L"SELECT * FROM", L"CREATE", L"CREATE TABLE", L"CREATE VIRTUAL TABLE", L"CREATE INDEX", L"CREATE INDEX", L"CREATE TRIGGER", L"CREATE VIEW", L"CREATE VIRTUAL TABLE",L"CREATE INDEX",L"CREATE UNIQUE"
	L"ALTER", L"ALTER TABLE", L"DELETE", L"DELETE FROM", L"BEGIN", L"BEGIN TRANSATION", L"COMMIT", L"COMMIT TRANSACTION", L"END TRANSATION",
	L"DROP", L"DROP INDEX", L"DROP TABLE", L"DROP TRIGGER", L"DROP VIEW", L"DROP COLUMN",L"INSERT", L"INSERT INTO", L"REPLACE", L"REPLACE INTO", L"UPDATE", L"PRAGMA",
	L"SAVEPOINT", L"RELEASE", L"RELEASE SAVEPOINT", L"ROLLBACK", L"ROLLBACK TO", L"ROLLBACK TRANSACTION", L"ANALYZE", L"ATTACH", L"ATTACH DATABASE", L"DETACH", L"DETACH DATABASE",
	L"EXPLAIN", L"REINDEX", L"IF ", L"NOT", L"NOT NULL", L"EXISTS", L"IF NOT EXISTS", L"IF EXISTS", L"RETURNING", L"VACUUM", L"WITH",L"WITH RECURSIVE"
	L"AUTOINCREMENT", L"PRIMARY", L"PRIMARY KEY", L"DEFAULT", L"CONSTRAINT", L"FOREIGN", L"FOREIGN KEY", L"UNIQUE", L"CHECK", L"FROM", L"WHERE", 
	L"OR", L"AND", L"ADD COLUMN", L"VALUES", L"RECURSIVE", L"DISTINCT", L"AS", L"GROUP", L"GROUP BY", L"ORDER", L"ORDER BY", L"BY", L"HAVING", L"LIMIT", L"OFFSET",
	L"JOIN", L"LEFT", L"LEFT JOIN", L"RIGHT", L"RIGHT JOIN", L"INNER", L"INNER JOIN", L"FULL", L"OUTER", L"CROSS",L"WITHOUT",L"ROWID",L"STRICT",
	L"IS", L"IS NULL", L"IS NOT NULL", L"ON", L"INTEGER", L"TEXT", L"NUMERIC", L"REAL", L"BLOB", L"VARCHAR", L"DATETIME", L"VIRTUAL",L"USING",L"INDEXED", L"USING INDEXED",L"NOT INDEXED",
	L"LIKE", L"BETWEEN", L"CASE", L"WHEN", L"ELSE", L"IN", L"COLLATE", L"MATCH",L"ESCAPE",L"REGEXP", L"ISNULL", L"NOTNULL",L"WINDOW", L"QUERY", L"PLAN",
	L"RENAME",L"RENAME COLUMN",L"RENAME TO",L"COLUMN",L"DEFERRED", L"IMMEDIATE", L"EXCLUSIVE",L"TEMP",L"TEMPORARY",L"BEFORE",L"AFTER", L"INSTEAD", L"INSTEAD OF",L"FOR FETCH ROW",
	L"ABORT", L"FAIL",L"IGNORE",L"MATERIALIZED",L"UNION",L"UNION ALL",L"INTERSECT",L"EXCEPT", L"CONFLICT", L"REFERENCES"
};


UserTableStrings & QueryPageSupplier::getCacheUserTableStrings(uint64_t userDbId)
{
	ATLASSERT(userDbId);
	return cacheUserTableMap[userDbId];
}


void QueryPageSupplier::setCacheUserTableStrings(uint64_t userDbId, UserTableStrings & tblStrs)
{
	ATLASSERT(userDbId);
	cacheUserTableMap[userDbId] = tblStrs;
}


Columns & QueryPageSupplier::getCacheTableColumns(uint64_t userDbId, const std::wstring & tblName)
{
	ATLASSERT(userDbId && !tblName.empty());
	std::pair<uint64_t, std::wstring> pair({ userDbId, tblName });
	return cacheTableColumnsMap[pair];
}


void QueryPageSupplier::setCacheTableColumns(uint64_t userDbId, const std::wstring & tblName, const Columns & columns)
{
	ATLASSERT(userDbId && !tblName.empty());
	std::pair<uint64_t, std::wstring> pair({ userDbId, tblName });
	cacheTableColumnsMap[pair] = columns;
}

void QueryPageSupplier::splitToSqlVector(std::wstring sql)
{
	sqlVector.clear();
	if (sql.empty()) {
		return;
	}
	
	if (sql.find(L';') == std::wstring::npos) {
		sqlVector.push_back(sql);
		return ;
	}

	sqlVector = StringUtil::splitNotIn(sql, L";", L"BEGIN", L"END", L"TRANSACTION", true);
}
