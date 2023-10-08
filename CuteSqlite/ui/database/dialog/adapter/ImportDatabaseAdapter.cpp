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

 * @file   ImportDatabaseAdapter.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-08
 *********************************************************************/
#include "stdafx.h"
#include "ImportDatabaseAdapter.h"
#include "core/common/Lang.h"
#include "core/common/exception/QRuntimeException.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/message/QMessageBox.h"

ImportDatabaseAdapter::ImportDatabaseAdapter(HWND parentHwnd, ATL::CWindow * view)
{
	this->parentHwnd = parentHwnd;
	this->dataView = view;
}

ImportDatabaseAdapter::~ImportDatabaseAdapter()
{

}

UserDbList ImportDatabaseAdapter::getDbs()
{
	loadDbs();
	return dbs;
}

void ImportDatabaseAdapter::loadDbs()
{
	try {
		dbs = databaseService->getAllUserDbs();
	}
	catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}
}
