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

 * @file   QSqlExecuteException.h
 * @brief  Execute sql has error, throw this exception object with error rows and cols
 * 
 * @author Xuehan Qin
 * @date   2023-10-21
 *********************************************************************/
#pragma once
#include "QRuntimeException.h"

class QSqlExecuteException : public QRuntimeException
{
public:
	QSqlExecuteException(const std::wstring errCode, const std::wstring errMsg, const std::wstring &sql = std::wstring());

	uint32_t getErrRow() const { return errRow; }
	void setErrRow(uint32_t val) { errRow = val; }


	uint32_t getErrCol() const { return errCol; }
	void setErrCol(uint32_t val) { errCol = val; }

	const std::wstring & getSql() const ;

	bool getRollBack() const { return rollBack; }
	void setRollBack(bool val) { rollBack = val; }
private:
	// Execute sql has error, will be with error rows and cols
	uint32_t errRow = 0;
	uint32_t errCol = 0;
	bool rollBack = false;
	std::wstring sql;
};

