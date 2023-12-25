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

 * @file   ByteCodeUtil.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-25
 *********************************************************************/
#include "stdafx.h"
#include "ByteCodeUtil.h"
#include "StringUtil.h"

const std::vector<std::wstring> ByteCodeUtil::seekOpCodes{L"SeekGT", L"SeekGE", L"SeekLT", L"SeekLE"};
const std::vector<std::wstring> ByteCodeUtil::idxOpCodes{L"IdxGT", L"IdxGE", L"IdxLT", L"IdxLE"};
const std::vector<std::wstring> ByteCodeUtil::compareOpCodes{L"Eq", L"Ne", L"Lt", L"Le", L"Gt", L"Ge"};

bool ByteCodeUtil::isInSeekOpCodes(const std::wstring & opcode)
{
	return StringUtil::existsIn(opcode, seekOpCodes);
}

bool ByteCodeUtil::isInIdxOpCodes(const std::wstring & opcode)
{
	return StringUtil::existsIn(opcode, idxOpCodes);
}

bool ByteCodeUtil::isInCompareOpCodes(const std::wstring & opcode)
{
	return StringUtil::existsIn(opcode, compareOpCodes);
}
