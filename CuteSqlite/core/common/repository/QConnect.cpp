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

 * @file   QConnect.cpp
 * @brief  QConnect::userConnectPool - The user connect pool for connecting user databases(Multiple)
 * @brief  QConnect::sysConnect - The CuteSqlite system connect for connecting system database of CuteSqlite itself.(Single)
 * 
 * @author Xuehan Qin
 * @date   2023-10-25
 *********************************************************************/
#include "stdafx.h"
#include "QConnect.h"

// The user connect pool for connecting user databases(Multiple)
std::unordered_map<uint64_t, QSqlDatabase *> QConnect::userConnectPool;

// The CuteSqlite system connect for connecting system database of CuteSqlite itself.(Single)
QSqlDatabase * QConnect::sysConnect = nullptr;
