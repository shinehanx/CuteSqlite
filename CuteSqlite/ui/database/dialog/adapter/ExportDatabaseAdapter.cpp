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

 * @file   ExportDatabaseAdapter.cpp
 * @brief  Export the table(s),view(s),trigger(s) of the selected database in SQL.
 * 
 * @author Xuehan Qin
 * @date   2023-10-04
 *********************************************************************/
#include "stdafx.h"
#include "ExportDatabaseAdapter.h"
#include "core/common/Lang.h"
#include "core/common/exception/QRuntimeException.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/message/QMessageBox.h"

ExportDatabaseAdapter::ExportDatabaseAdapter(HWND parentHwnd, ATL::CWindow * view)
{
	this->parentHwnd = parentHwnd;
	this->dataView = view;
}

ExportDatabaseAdapter::~ExportDatabaseAdapter()
{

}

UserDbList ExportDatabaseAdapter::getDbs()
{
	loadDbs();
	return dbs;
}

void ExportDatabaseAdapter::loadDbs()
{
	try {
		dbs = databaseService->getAllUserDbs();
	}
	catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}
}

uint64_t ExportDatabaseAdapter::getSeletedUserDbId()
{
	return databaseSupplier->getSeletedUserDbId();
}
