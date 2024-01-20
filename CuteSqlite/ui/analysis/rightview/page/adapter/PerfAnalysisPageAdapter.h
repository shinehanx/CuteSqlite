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

 * @file   PerfAnalysisPageAdapter.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2024-01-19
 *********************************************************************/
#pragma once
#include "ui/common/adapter/QAdapter.h"
#include "core/entity/Entity.h"
#include "ui/analysis/rightview/page/supplier/PerfAnalysisSupplier.h"
#include "core/service/analysis/SelectSqlAnalysisService.h"

class PerfAnalysisPageAdapter : public QAdapter<PerfAnalysisPageAdapter, CWindow>
{
public:
	PerfAnalysisPageAdapter(HWND parentHwnd, CWindow * view, PerfAnalysisSupplier * supplier = nullptr);
	~PerfAnalysisPageAdapter();

	void save();
	void enableReportSaved();
private:
	
	PerfAnalysisSupplier * supplier = nullptr;
	SelectSqlAnalysisService * selectSqlAnalysisService = SelectSqlAnalysisService::getInstance();

	void initSupplier();	
};
