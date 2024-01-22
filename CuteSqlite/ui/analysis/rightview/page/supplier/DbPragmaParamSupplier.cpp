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

 * @file   PerAnalysisSupplier.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-07
 *********************************************************************/
#include "stdafx.h"
#include "DbPragmaParamSupplier.h"
#include <vector>
#include <algorithm>
#include "core/common/Lang.h"


void DbPragmaParamSupplier::addChangedPragam(const ParamElemData & data)
{
	setIsDirty(true);
	auto iter = std::find_if(changedPragams.begin(), changedPragams.end(), [&data](const auto & item) {
		return data.labelText == item.labelText;
	});
	if (iter == changedPragams.end()) {
		changedPragams.push_back(data);
		return;
	}
	(*iter).val = data.val;
}


void DbPragmaParamSupplier::clearChangedPragams()
{
	changedPragams.clear();
	setIsDirty(false);
}

const ParamElemDataList DbPragmaParamSupplier::pragmas = {
	{ EDIT_ELEM, L"PRAGMA analysis_limit", L"", {}, S(L"pragma-analysis-limit-tips"), READ_WRITE, ASSIGN_VAL },
	{ EDIT_ELEM, L"PRAGMA application_id", L"", {}, S(L"pragma-application-id-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA auto_vacuum", L"", { L"NONE|0", L"FULL|1", L"INCREMENTAL|2" }, S(L"pragma-auto-vacuum-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA automatic_index", L"", { L"false|0", L"true|1"}, S(L"pragma-automatic-index-tips"), READ_WRITE, ASSIGN_VAL },
	{ EDIT_ELEM, L"PRAGMA busy_timeout", L"", {}, S(L"pragma-busy-timeout-tips"), READ_WRITE, ASSIGN_VAL }, 
	{ EDIT_ELEM, L"PRAGMA cache_size", L"", {}, S(L"pragma-cache-size-tips"), READ_WRITE, ASSIGN_VAL },
	{ EDIT_ELEM, L"PRAGMA cache_spill", L"", {},  S(L"pragma-cache-spill-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA cell_size_check", L"", { L"false|0", L"true|1" }, S(L"pragma-cell-size-check-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA checkpoint_fullfsync", L"", { L"false|0", L"true|1" }, S(L"pragma-checkpoint-fullfsync-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA count_changes", L"", { L"false|0", L"true|1" }, S(L"pragma-count-changes-tips"), READ_WRITE, ASSIGN_VAL },
	{ EDIT_ELEM, L"PRAGMA data_store_directory", L"", {}, S(L"pragma-data-store-directory-tips"), READ_WRITE, ASSIGN_STR },
	{ READ_ELEM, L"PRAGMA data_version", L"", {}, S(L"pragma-data-version-tips"), READ_ONLY, IGNORE_VAL },
	{ EDIT_ELEM, L"PRAGMA default_cache_size", L"", {}, S(L"pragma-default-cache-size-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA defer_foreign_keys", L"", { L"false|0", L"true|1" }, S(L"pragma-defer-foreign-keys-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA empty_result_callbacks", L"", { L"false|0", L"true|1" }, S(L"pragma-empty-result-callbacks-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_EDIT_ELEM, L"PRAGMA encoding", L"", {L"UTF-8", L"UTF-16", L"UTF-16le", L"UTF-16be"}, S(L"pragma-encoding-tips"), READ_WRITE, ASSIGN_STR },
	{ COMBO_READ_ELEM, L"PRAGMA foreign_keys", L"", { L"false|0", L"true|1" }, S(L"pragma-foreign-keys-tips"), READ_WRITE, ASSIGN_VAL },
	{ READ_ELEM, L"PRAGMA freelist_count", L"", {}, S(L"pragma-freelist-count-tips"), READ_ONLY, IGNORE_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA full_column_names", L"",  { L"false|0", L"true|1" }, S(L"pragma-full-column-names-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA fullfsync", L"", { L"false|0", L"true|1" }, S(L"pragma-fullfsync-tips"), READ_WRITE, ASSIGN_VAL },
	{ EDIT_ELEM, L"PRAGMA hard_heap_limit", L"", {}, S(L"pragma-hard-heap-limit-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA ignore_check_constraints", L"", { L"false|0", L"true|1" }, S(L"pragma-ignore-check-constraints-tips"), READ_WRITE, ASSIGN_VAL },
	{ EDIT_ELEM, L"PRAGMA incremental_vacuum", L"", {}, S(L"pragma-incremental-vacuum-tips"), WRITE_ONLY, PARAM_VAL },
	{ READ_ELEM, L"PRAGMA integrity_check", L"", {}, S(L"pragma-integrity-check-tips"), READ_ONLY, IGNORE_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA journal_mode", L"", { L"delete", L"truncate", L"persist", L"memory", L"wal", L"off"}, S(L"pragma-journal-mode-tips"), READ_WRITE, ASSIGN_VAL },
	{ EDIT_ELEM, L"PRAGMA journal_size_limit", L"", {}, S(L"pragma-journal-size-limit-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA legacy_alter_table", L"", { L"false|0", L"true|1" }, S(L"pragma-legacy-alter-table-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA locking_mode", L"", { L"normal", L"exclusive" }, S(L"pragma-locking-mode-tips"), READ_WRITE, ASSIGN_VAL },
	{ EDIT_ELEM, L"PRAGMA max_page_count", L"", {}, S(L"pragma-max-page-count-tips"), READ_WRITE, ASSIGN_VAL },
	{ EDIT_ELEM, L"PRAGMA mmap_size", L"", {}, S(L"pragma-mmap-size-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA optimize", L"", {L"0xfffe", L"-1", L"0x02"}, S(L"pragma-optimize-tips"), WRITE_ONLY, PARAM_VAL },
	{ READ_ELEM, L"PRAGMA page_count", L"", {}, S(L"pragma-page-count-tips"), READ_ONLY, IGNORE_VAL },
	{ EDIT_ELEM, L"PRAGMA page_size", L"", {}, S(L"pragma-page-size-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA parser_trace", L"", { L"false|0", L"true|1" }, S(L"pragma-parser-trace-tips"), WRITE_ONLY, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA query_only", L"", { L"false|0", L"true|1" }, S(L"pragma-query-only-tips"), READ_WRITE, ASSIGN_VAL },
	{ EDIT_ELEM, L"PRAGMA quick_check", L"", {}, S(L"pragma-quick-check-tips"), READ_WRITE, PARAM_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA read_uncommitted", L"", { L"false|0", L"true|1" }, S(L"pragma-read-uncommitted-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA recursive_triggers", L"", { L"false|0", L"true|1" }, S(L"pragma-recursive-triggers-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA reverse_unordered_selects", L"", { L"false|0", L"true|1" }, S(L"pragma-reverse-unordered-selects-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA schema_version", L"", { L"false|0", L"true|1" }, S(L"pragma-schema-version-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA secure_delete", L"", { L"false|0", L"true|1" , L"fast|fast"}, S(L"pragma-secure-delete-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA short_column_names", L"", { L"false|0", L"true|1" }, S(L"pragma-short-column-names-tips"), READ_WRITE, ASSIGN_VAL },
	{ EDIT_ELEM, L"PRAGMA soft_heap_limit", L"", {}, S(L"pragma-soft-heap-limit-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA synchronous", L"", { L"off|0", L"normal|1", L"full|2", L"extra|3" }, S(L"pragma-synchronous-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA temp_store", L"", { L"default|0", L"file|1", L"memory|2" }, S(L"pragma-temp-store-tips"), READ_WRITE, ASSIGN_VAL },
	{ EDIT_ELEM, L"PRAGMA threads", L"", {}, S(L"pragma-threads-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA trusted_schema", L"", { L"false|0", L"true|1"}, S(L"pragma-trusted-schema-tips"), READ_WRITE, ASSIGN_VAL },
	{ EDIT_ELEM, L"PRAGMA user_version", L"", {}, S(L"pragma-user-version-tips"), READ_WRITE, ASSIGN_VAL },
	{ EDIT_ELEM, L"PRAGMA wal_autocheckpoint", L"", {}, S(L"pragma-wal-autocheckpoint-tips"), READ_WRITE, ASSIGN_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA wal_checkpoint", L"", { L"passive", L"full", L"restart", L"truncate" }, S(L"pragma-wal-checkpoint-tips"), WRITE_ONLY, PARAM_VAL },
	{ COMBO_READ_ELEM, L"PRAGMA writable_schema", L"", {L"false|0", L"true|1", L"reset|reset" }, S(L"pragma-writable-schema-tips"), READ_WRITE, ASSIGN_VAL }
};
