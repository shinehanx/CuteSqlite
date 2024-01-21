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

 * @file   DbPragmaParamPageAdapter.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2024-01-19
 *********************************************************************/
#include "stdafx.h"
#include "DbPragmaParamPageAdapter.h"
#include "common/AppContext.h"
#include "utils/ResourceUtil.h"
#include "utils/EntityUtil.h"
#include "core/common/Lang.h"
#include "core/common/exception/QSqlExecuteException.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/message/QMessageBox.h"

DbPragmaParamPageAdapter::DbPragmaParamPageAdapter(HWND parentHwnd, CWindow * view, DbPragmaParamSupplier * supplier)
{
	this->parentHwnd = parentHwnd;
	this->dataView = view;
	this->supplier = supplier;	
	
}


DbPragmaParamPageAdapter::~DbPragmaParamPageAdapter()
{
	
}


void DbPragmaParamPageAdapter::initSupplier()
{
	try {
		
	} catch (QSqlExecuteException & ex) {
		QPopAnimate::report(ex);
	} catch (QRuntimeException & ex) {
		QPopAnimate::report(ex);
	}
	
}

bool DbPragmaParamPageAdapter::save()
{
	std::wstring text = S(L"pragma-save-confirm");
	auto & changedPragmas = supplier->getChangedPragams();
	std::vector<std::wstring> sqls;
	for (auto & pragma : changedPragmas) {
		std::wstring sql;
		sql.append(pragma.labelText);
		if (pragma.valBackType == ASSIGN_VAL) {
			sql.append(L"=").append(pragma.val).append(L";");
		} else if (pragma.valBackType == ASSIGN_STR){
			std::wstring val = pragma.val;
			sql.append(L"='").append(StringUtil::escapeSql(val)).append(L"';");
		} else if (pragma.valBackType == PARAM_VAL) {
			sql.append(L"(").append(pragma.val).append(L");");
		} else if (pragma.valBackType == PARAM_STR) {
			std::wstring val = pragma.val;
			sql.append(L"(").append(StringUtil::escapeSql(val)).append(L");");
		} else {
			sql.append(L";");
		}
		sqls.push_back(sql);
	}
	if (sqls.empty()) {
		return false;
	}
	text.append(L"\r\n").append(StringUtil::implode(sqls, L"\r\n"));
	if (QMessageBox::confirm(parentHwnd, text, S(L"confirm-exec-text")) != Config::CUSTOMER_FORM_YES_BUTTON_ID) {
		return false; 
	}
	try {
		for (auto & sql : sqls) {
			pragmaService->execPragma(supplier->getRuntimeUserDbId(), sql);
		}
		QPopAnimate::success(S(L"save-success-text"));
		supplier->setIsDirty(false);
		return true;
	} catch (QSqlExecuteException & ex) {
		QPopAnimate::report(ex);
	} catch (QRuntimeException & ex) {
		QPopAnimate::report(ex);
	}
	return false;
}

void DbPragmaParamPageAdapter::enableSaved()
{
	// RightAnalysisView, LeftNavigationView will be accept this msg
	AppContext::getInstance()->dispatch(Config::MSG_ANALYSIS_DIRTY_DB_PRAGMAS_ID, WPARAM(supplier->getRuntimeUserDbId()), LPARAM(false));
}

ParamElemDataList DbPragmaParamPageAdapter::getDbPragmaParams(uint64_t userDbId)
{
	ParamElemDataList result;
	const ParamElemDataList & pragmas = supplier->getPragmas();
	try {
		for (auto & pragma : pragmas) {
			auto item = EntityUtil::copy(pragma);
			// read pragmas data from database
			if (item.valRwType != WRITE_ONLY) {
				auto & pragmaSql = item.labelText;
				item.val = pragmaService->execOnePragma(userDbId, pragmaSql);
			}

			result.push_back(item);
		}
	} catch (QSqlExecuteException & ex) {
		QPopAnimate::report(ex);
	} catch (QRuntimeException & ex) {
		QPopAnimate::report(ex);
	}
	
	return result;
}

