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
#include "utils/ResourceUtil.h"
#include "utils/EntityUtil.h"
#include "core/common/Lang.h"
#include "core/common/exception/QSqlExecuteException.h"
#include "ui/common/message/QPopAnimate.h"
#include "common/AppContext.h"

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

void DbPragmaParamPageAdapter::save()
{
	try {
		
		supplier->setIsDirty(false);

		QPopAnimate::success(S(L"save-success-text"));
	} catch (QSqlExecuteException & ex) {
		QPopAnimate::report(ex);
	} catch (QRuntimeException & ex) {
		QPopAnimate::report(ex);
	}
}

void DbPragmaParamPageAdapter::enableReportSaved()
{
	// RightAnalysisView, LeftNavigationView will be accept this msg
	// AppContext::getInstance()->dispatch(Config::MSG_ANALYSIS_SAVE_PERF_REPORT_ID, WPARAM(parentHwnd), LPARAM(supplier->getSqlLogId()));
}

ParamElemDataList DbPragmaParamPageAdapter::getDbPragmaParams(uint64_t userDbId)
{
	ParamElemDataList result;
	const ParamElemDataList & pragmas = supplier->getPragmas();
	for (auto & pragma : pragmas) {
		auto item = EntityUtil::copy(pragma);
		// read pragmas data from database
		if (item.valRwType != WRITE_ONLY) {
			auto & pragmaSql = item.labelText;
			item.val = pragmaService->execOnePragma(userDbId, pragmaSql);
		}
		
		result.push_back(item);
	}
	
	return result;
}

