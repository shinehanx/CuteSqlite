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

 * @file   QSupplier.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-01
 *********************************************************************/
#include "stdafx.h"
#include "QSupplier.h"


std::wstring QSupplier::csvFieldSeperators[5] = { L",", L"TAB", L"|", L";",L":" };
std::wstring QSupplier::csvLineSeperators[3] = { L"CR", L"CRLF", L"LF" };
std::wstring QSupplier::csvNullAsKeywords[2] = { L"YES", L"NO"};
std::wstring QSupplier::csvEncodings[2] = { L"UTF-8", L"UTF-16"};
