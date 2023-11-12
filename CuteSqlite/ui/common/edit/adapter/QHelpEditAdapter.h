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

 * @file   QHelpEditAdapter.h
 * @brief  Base class of help edit adapters, you can inherited this class to implement abstract functions
 * 
 * @author Xuehan Qin
 * @date   2023-11-11
 *********************************************************************/
#pragma once
#include <vector>
#include <string>

class QHelpEditAdapter {
public:
	virtual std::vector<std::wstring> getTags(const std::wstring & line, const std::wstring & preline, const std::wstring & word) = 0; 
};
