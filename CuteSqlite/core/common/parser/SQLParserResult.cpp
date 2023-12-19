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

 * @file   SQLParserResult.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-19
 *********************************************************************/
#include "stdafx.h"
#include "SQLParserResult.h"
#include <algorithm>

/**
 * Initialize with empty statement list..
 * 
 * @return 
 */
SQLParserResult::SQLParserResult():
	isValid_(false),
    errorMsg_(nullptr)
{

}

/**
 * Initialize with a single statement.
 * Takes ownership of the statement..
 * 
 * @param stmt
 * @return 
 */
SQLParserResult::SQLParserResult(SQLStatement* stmt) :
	isValid_(false),
    errorMsg_(nullptr) 
{
    addStatement(stmt);
}


SQLParserResult::SQLParserResult(SQLParserResult&& moved)
{
	isValid_ = moved.isValid_;
    errorMsg_ = moved.errorMsg_;
    statements_ = std::move(moved.statements_);

    moved.errorMsg_ = nullptr;
    moved.reset();
}

/**
 * Deletes all statements in the result..
 * 
 * @return 
 */
SQLParserResult::~SQLParserResult()
{
	reset();
}

/**
 * Set whether parsing was successful..
 * 
 * @param isValid
 */
void SQLParserResult::setIsValid(bool isValid)
{
	this->isValid_ = isValid;
}

/**
 * Returns true if parsing was successful..
 * 
 * @return 
 */
bool SQLParserResult::isValid() const
{
	return isValid_;
}

/**
 * Returns the number of statements in the result..
 * 
 * @return 
 */
size_t SQLParserResult::size() const
{
	return statements_.size();
}

/**
 * Set the details of the error, if available.
 * Takes ownership of errorMsg..
 * 
 * @param errorMsg
 * @param errorLine
 * @param errorColumn
 */
void SQLParserResult::setErrorDetails(wchar_t * errorMsg, int errorLine, int errorColumn)
{
	errorMsg_ = errorMsg;
    errorLine_ = errorLine;
    errorColumn_ = errorColumn;
}

const wchar_t * SQLParserResult::errorMsg() const
{
	return errorMsg_;
}

int SQLParserResult::errorLine() const
{
	return errorLine_;
}

int SQLParserResult::errorColumn() const
{
	return errorColumn_;
}

void SQLParserResult::addStatement(SQLStatement* stmt)
{
	statements_.push_back(stmt);
}

const SQLStatement* SQLParserResult::getStatement(int index) const
{
	return statements_.at(index);
}

SQLStatement* SQLParserResult::getMutableStatement(int index)
{
	return statements_[index];
}

const std::vector<SQLStatement*>& SQLParserResult::getStatements() const
{
	return statements_;
}

std::vector<SQLStatement*> SQLParserResult::releaseStatements()
{
	std::vector<SQLStatement*> copy = statements_;

    statements_.clear();

    return copy;
}

void SQLParserResult::reset()
{
	for (SQLStatement* statement : statements_) {
		delete statement;
    }
    statements_.clear();

    isValid_ = false;

    free(errorMsg_);
    errorMsg_ = nullptr;
    errorLine_ = -1;
    errorColumn_ = -1;
}

void SQLParserResult::addParameter(Expr* parameter)
{
	parameters_.push_back(parameter);
    std::sort(parameters_.begin(), parameters_.end(),
    [](const Expr * a, const Expr * b) {
      return a->ival < b->ival;
    });
}

const std::vector<Expr*>& SQLParserResult::parameters()
{
	return parameters_;
}
