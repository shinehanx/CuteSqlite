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

 * @file   PerformUtil.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-07-06
 *********************************************************************/
#include "stdafx.h"
#include "PerformUtil.h"
#include <atlstr.h>

std::chrono::steady_clock::time_point PerformUtil::begin()
{
	return std::chrono::high_resolution_clock::now();
}

std::wstring PerformUtil::end(std::chrono::steady_clock::time_point _begin)
{
	auto _end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(_end - _begin);
	CString execTime;
	execTime.Format(L"%.3f ms", static_cast<double>(elapsed.count() * 1.0 / 1000.0));
	return execTime.GetString();
}

