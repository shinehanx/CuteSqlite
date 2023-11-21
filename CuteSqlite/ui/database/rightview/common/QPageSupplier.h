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

 * @file   QPageSupplier.h
 * @brief  The base PageSupplier class.
 *		   It's child class supply to store runtime data and handle functions.
 *         These runtime data is use for multiple pages
 * 
 * @author Xuehan Qin
 * @date   2023-10-30
 *********************************************************************/
#pragma once
#include "core/entity/Entity.h"
#include "core/common/supplier/QSupplier.h"

class QPageSupplier : public QSupplier {
public:
	// Getter or setter
	PageOperateType getOperateType() const { return operateType; }
	void setOperateType(PageOperateType val) { operateType = val; }

	bool getIsDirty() { return isDirty; }
	void setIsDirty(bool isDirty) { this->isDirty = isDirty; }
private:
	PageOperateType operateType; // new table - NEW_TABLE, alter table - MOD_TABLE
	bool isDirty = false;
	
};
