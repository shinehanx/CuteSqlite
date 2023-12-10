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

 * @file   PerfAnalysisPageAdapter.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-01
 *********************************************************************/
#include "stdafx.h"
#include "PerfAnalysisPageAdapter.h"
#include "utils/ResourceUtil.h"
#include "core/common/Lang.h"
#include "core/common/exception/QSqlExecuteException.h"
#include "ui/common/message/QPopAnimate.h"

PerfAnalysisPageAdapter::PerfAnalysisPageAdapter(HWND parentHwnd, CWindow * view, PerfAnalysisSupplier * supplier)
{
	this->parentHwnd = parentHwnd;
	this->dataView = view;
	this->supplier = supplier;
	initSupplier();
}


PerfAnalysisPageAdapter::~PerfAnalysisPageAdapter()
{
	
}

void PerfAnalysisPageAdapter::initSupplier()
{
	try {
		// initialize the supplier runtime data
		supplier->setRuntimeUserDbId(supplier->getSqlLog().userDbId);
		// 1. Get explain data list from database
		DataList explainDatas = selectSqlAnalysisService->explainSql(supplier->getSqlLog().userDbId, supplier->getSqlLog().sql);	
		supplier->setExplainDataList(explainDatas);

		// 2.Get explain query plan from database
		ExplainQueryPlans expQueryPlans = selectSqlAnalysisService->explainQueryPlanSql(supplier->getSqlLog().userDbId, supplier->getSqlLog().sql);
		supplier->setExplainQueryPlans(expQueryPlans);

		// 3. Convert explain data list to ByteCodeResults
		ByteCodeResults results = selectSqlAnalysisService->explainReadByteCodeToResults(supplier->getRuntimeUserDbId(), explainDatas);
		supplier->setByteCodeResults(results);
	} catch (QSqlExecuteException & ex) {
		QPopAnimate::report(ex);
	}
	
}



