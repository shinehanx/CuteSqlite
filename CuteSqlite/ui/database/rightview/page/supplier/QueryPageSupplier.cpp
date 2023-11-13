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


const std::list<std::tuple<int, std::wstring, std::wstring>> QueryPageSupplier::pragmas = {
	{1713, L"analysis_limit", L"PRAGMA analysis_limit;"},
	{1714, L"application_id", L"PRAGMA application_id;"},
	{1715, L"auto_vacuum", L"PRAGMA auto_vacuum;"},
	{1716, L"automatic_index", L"PRAGMA automatic_index;"},
	{1717, L"busy_timeout", L"PRAGMA busy_timeout;"},
	{1718, L"cache_size", L"PRAGMA cache_size;"},
	{1719, L"cache_spill", L"PRAGMA cache_spill;"},
	{1720, L"case_sensitive_like", L"PRAGMA case_sensitive_like=<true/false>;"},
	{1721, L"cell_size_check", L"PRAGMA cell_size_check;"},
	{1722, L"checkpoint_fullfsync", L"PRAGMA checkpoint_fullfsync;"},
	{1723, L"collation_list", L"PRAGMA collation_list;"},
	{1724, L"compile_options", L"PRAGMA compile_options;"},
	{1725, L"count_changes", L"PRAGMA count_changes;"},
	{1726, L"data_store_directory", L"PRAGMA data_store_directory;"},
	{1727, L"data_version", L"PRAGMA data_version;"},
	{1728, L"database_list", L"PRAGMA database_list;"},
	{1729, L"default_cache_size", L"PRAGMA default_cache_size;"},
	{1730, L"defer_foreign_keys", L"PRAGMA defer_foreign_keys;"},
	{1731, L"empty_result_callbacks", L"PRAGMA empty_result_callbacks;"},
	{1732, L"encoding", L"PRAGMA encoding;"},
	{1733, L"foreign_key_check", L"PRAGMA foreign_key_check;"},
	{1734, L"foreign_key_check(table-name)", L"PRAGMA foreign_key_check(<table>);"},
	{1735, L"foreign_key_list(table-name)", L"PRAGMA foreign_key_list(<table>);"},
	{1736, L"foreign_keys", L"PRAGMA foreign_keys;"},
	{1737, L"freelist_count", L"PRAGMA freelist_count;"},
	{1738, L"fullfsync", L"PRAGMA fullfsync;"},
	{1739, L"function_list", L"PRAGMA function_list;"},
	{1740, L"ignore_check_constraints", L"PRAGMA ignore_check_constraints=<true/false>;"},
	{1741, L"incremental_vacuum", L"PRAGMA incremental_vacuum;"},
	{1742, L"index_info(index-name)", L"PRAGMA index_info(<index_name>);"},
	{1743, L"index_list(table-name)", L"PRAGMA index_list(<table>);"},
	{1744, L"index_xinfo(index-name)", L"PRAGMA index_xinfo(<index_name>);"},
	{1745, L"integrity_check", L"PRAGMA integrity_check;"},
	{1746, L"integrity_check(N)", L"PRAGMA integrity_check(<N>);"},
	{1747, L"integrity_check(table-name)", L"PRAGMA integrity_check(<table-name>);"},
	{1748, L"journal_mode", L"PRAGMA journal_mode;"},
	{1749, L"journal_size_limit", L"PRAGMA journal_size_limit;"},
	{1750, L"legacy_alter_table", L"PRAGMA legacy_alter_table;"},
	{1751, L"legacy_file_format", L"PRAGMA legacy_file_format;"},
	{1752, L"locking_mode", L"PRAGMA locking_mode;"},
	{1753, L"max_page_count", L"PRAGMA max_page_count;"},
	{1754, L"mmap_size", L"PRAGMA mmap_size;"},
	{1755, L"module_list", L"PRAGMA module_list;"},
	{1756, L"optimize", L"PRAGMA optimize;"},
	{1757, L"optimize(MASK)", L"PRAGMA optimize(<mask>);"},
	{1758, L"page_count", L"PRAGMA page_count;"},
	{1759, L"page_size", L"PRAGMA page_size;"},
	{1760, L"parser_trace", L"PRAGMA parser_trace=<true/false>;"},
	{1761, L"pragma_list", L"PRAGMA pragma_list;"},
	{1762, L"query_only", L"PRAGMA query_only;"},
	{1763, L"quick_check", L"PRAGMA quick_check;"},
	{1764, L"quick_check(N)", L"PRAGMA quick_check(<N>);"},
	{1765, L"quick_check(table-name)", L"PRAGMA quick_check(<table>);"},
	{1766, L"read_uncommitted", L"PRAGMA read_uncommitted;"},
	{1767, L"recursive_triggers", L"PRAGMA recursive_triggers;"},
	{1768, L"reverse_unordered_selects", L"PRAGMA reverse_unordered_selects;"},
	{1769, L"schema_version", L"PRAGMA schema_version;"},
	{1770, L"secure_delete", L"PRAGMA secure_delete;"},
	{1771, L"short_column_names", L"PRAGMA short_column_names;"},
	{1772, L"shrink_memory", L"PRAGMA shrink_memory;"},
	{1773, L"soft_heap_limit", L"PRAGMA soft_heap_limit;"},
	{1774, L"stats", L"PRAGMA stats;"},
	{1775, L"synchronous", L"PRAGMA synchronous;"},
	{1776, L"table_info(table-name)", L"PRAGMA table_info(<table>);"},
	{1777, L"table_list", L"PRAGMA table_list;"},
	{1778, L"table_list(table-name)", L"PRAGMA table_list(<table>);"},
	{1779, L"table_xinfo(table-name)", L"PRAGMA table_xinfo(<table>);"},
	{1780, L"temp_store", L"PRAGMA temp_store;"},
	{1781, L"temp_store_directory", L"PRAGMA temp_store_directory;"},
	{1782, L"threads", L"PRAGMA threads;"},
	{1783, L"trusted_schema", L"PRAGMA trusted_schema;"},
	{1784, L"user_version", L"PRAGMA user_version;"},
	{1785, L"vdbe_addoptrace", L"PRAGMA vdbe_addoptrace;"},
	{1786, L"vdbe_debug", L"PRAGMA vdbe_debug;"},
	{1787, L"vdbe_listing", L"PRAGMA vdbe_listing;"},
	{1788, L"vdbe_trace", L"PRAGMA vdbe_trace;"},
	{1789, L"wal_autocheckpoint", L"PRAGMA wal_autocheckpoint;"},
	{1790, L"wal_checkpoint", L"PRAGMA wal_checkpoint;"},
	{1791, L"wal_checkpoint(PASSIVE)", L"PRAGMA wal_checkpoint(PASSIVE);"},
	{1792, L"wal_checkpoint(FULL)", L"PRAGMA wal_checkpoint(FULL);"},
	{1793, L"wal_checkpoint(RESTART)", L"PRAGMA wal_checkpoint(RESTART);"},
	{1794, L"wal_checkpoint(TRUNCATE)", L"PRAGMA wal_checkpoint(TRUNCATE);"},
	{1795, L"writable_schema", L"PRAGMA writable_schema;"},
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
