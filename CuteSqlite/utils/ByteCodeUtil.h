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

 * @file   ByteCodeUtil.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-25
 *********************************************************************/
#pragma once
#include <vector>
#include <string>

class ByteCodeUtil {
public:
	static bool isInSeekOpCodes(const std::wstring & opcode);
	static bool isInIdxOpCodes(const std::wstring & opcode);
	static bool isInCompareOpCodes(const std::wstring & opcode);
private:
	static const std::vector<std::wstring> seekOpCodes;
	static const std::vector<std::wstring> idxOpCodes;
	static const std::vector<std::wstring> compareOpCodes;
};
