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

 * @file   TableStructureSupplier.cpp
 * @brief  This class supply runtime data for TableStructurePage/TableTabView/TableColumnsPage/TableIndexesPage objects
 * 
 * @author Xuehan Qin
 * @date   2023-10-21
 *********************************************************************/
#include "stdafx.h"
#include "TableStructureSupplier.h"
#include "core/common/Lang.h"

const Columns TableStructureSupplier::colsHeadColumns = { S(L"column-name"), S(L"data-type"), L"Not Null", L"PK", L"Auto Insc", L"Unique", S(L"default"), S(L"check")};
const std::vector<int> TableStructureSupplier::colsHeadSizes = { 150, 100, 70, 70, 70, 70, 150, 100 };
const std::vector<int> TableStructureSupplier::colsHeadFormats = { LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_LEFT };
const std::vector<std::wstring> TableStructureSupplier::colsDataTypeList = { L"INTEGER", L"TEXT", L"BLOB", L"REAL", L"NUMERIC"};

const Columns TableStructureSupplier::idxHeadColumns = { S(L"index-name"), S(L"columns"), S(L"index-type"), L"SQL"};
const std::vector<int> TableStructureSupplier::idxHeadSizes = { 150, 150, 150, 200 };
const std::vector<int> TableStructureSupplier::idxHeadFormats = { LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT };
const std::vector<std::wstring> TableStructureSupplier::idxTypeList = {L"Unique", L"Primary Key", L"Foreign Key",  L"Check"};



