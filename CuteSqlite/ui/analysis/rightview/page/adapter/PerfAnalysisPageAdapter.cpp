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


int PerfAnalysisPageAdapter::getWhereClauseCount()
{
	const auto & explainDatas = supplier->getRuntimeExplainDataList();
	int result = 0;
	for (const auto & item : explainDatas) {
		if (item.at(EXP_OPCODE) != L"Next" 
			&& item.at(EXP_OPCODE) != L"Prev" 
			&& item.at(EXP_OPCODE) != L"IdxGE"
			&& item.at(EXP_OPCODE) != L"IdxGT"
			&& item.at(EXP_OPCODE) != L"IdxLE"
			&& item.at(EXP_OPCODE) != L"IdxLT"
			) {
			continue;
		}
		if (item.at(EXP_OPCODE) == L"Next" || item.at(EXP_OPCODE) == L"Prev") {
			auto & fromAddr = item.at(EXP_P2); // loop start addr
			auto & toAddr = item.at(EXP_ADDR); // Next opcode addr
			if (fromAddr.empty() || toAddr.empty()) {
				return 0; // failed jump addr
			}
			int whereColumnCount = getWhereColumnCount(fromAddr, toAddr);
			result += whereColumnCount;
		} else {
			result += 1; // index 
		}
		
	}
	return result;
}

/**
 * Total the count of where clauses from bytecode.
 * 
 * @param fromAddr - index string for from-addr
 * @param toAddr - index string for to-addr
 * @return 
 */
int PerfAnalysisPageAdapter::getWhereColumnCount(const std::wstring& fromAddr, const std::wstring& toAddr)
{
	if (fromAddr.empty() || toAddr.empty()) {
		return 0;
	}
	const auto & explainDatas = supplier->getRuntimeExplainDataList();
	size_t n = explainDatas.size();
	size_t i = 0;
	int nFromAddr = std::stoi(fromAddr);
	int nToAddr = std::stoi(toAddr);
	if (nToAddr < nToAddr) {
		return 0;
	}
	int nFound = 0;
	for (auto iter = explainDatas.begin(); iter != explainDatas.end(); iter++) {
		auto & item = *iter;
		int nAddr = std::stoi(item.at(EXP_ADDR));
		if (nAddr < nFromAddr) {
			i++;
			continue;
		}

		if (nAddr >= nToAddr) {
			i++;
			break;
		}

		auto & opcode = item.at(EXP_OPCODE);
		if (opcode != L"Column") {
			i++;
			continue;
		}

		if (opcode == L"Rewind" || opcode == L"Next") {
			i++;
			continue;
		}

		auto nextIter = std::next(iter);
		if (nextIter == explainDatas.end()) {
			break;
		}
		auto & nextOpCode = (*nextIter).at(EXP_OPCODE);
		if (nextOpCode == L"IfPos" || nextOpCode == L"Rewind" || nextOpCode == L"Column" || nextOpCode == L"ResultRow")  {
			break;
		}
		nFound++; //found where column
		i++;
	}
	return nFound;
}


int PerfAnalysisPageAdapter::getWhereColumnCountForIdx(const std::wstring& idxOpCode)
{
	return 1;
}

void PerfAnalysisPageAdapter::initSupplier()
{
	// initialize the supplier runtime data
	DataList explainDatas = selectSqlAnalysisService->explainSql(supplier->getSqlLog().userDbId, supplier->getSqlLog().sql);
	supplier->setRuntimeUserDbId(supplier->getSqlLog().userDbId);
	supplier->setRuntimeExplainDataList(explainDatas);
}



