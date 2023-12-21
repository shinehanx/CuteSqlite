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

 * @file   SQLParser.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-19
 *********************************************************************/
#include "stdafx.h"
#include "SQLParser.h"
#include "utils/StringUtil.h"
#include "core/common/exception/QRuntimeException.h"
#include "sql/SelectStatement.h"


bool SQLParser::parse(const std::wstring & sql, SQLParserResult * result)
{
	// 1. tokenize
	std::vector<std::wstring> tokens;
	bool ret = tokenize(sql, & tokens);
	if (!ret) {
		throw QRuntimeException(L"200001");
	}

	parseSelect(result, tokens);


	return true;
}

bool SQLParser::tokenize(const std::wstring & sql, std::vector<std::wstring>* tokens)
{
	size_t offset = 0;
	size_t size = sql.size();
	while (offset < size) {
		std::wstring token = tokenFromSql(sql, offset);
		if (token.empty()) {
			continue;
		}
		tokens->push_back(token);
	}
	return !tokens->empty();
}

std::wstring SQLParser::tokenFromSql(const std::wstring &sql, size_t & offset)
{
	std::wstring token;
	size_t n = sql.size();
	for (; offset < n; ++offset) {
		wchar_t c = sql.at(offset);
		if (c == L' ' || c == L'\t' || c == L'\r' || c == L'\n' || c == L'\f' || c == L'\v') {
			++offset;
			break;
		}
		
		if (c == L',' || c ==  L'(' || c == L')') {
			token.push_back(c);
			++offset;
			break;
		}

		token.push_back(c);
		if (offset + 1 < n) {
			wchar_t next = sql.at(offset + 1);
			if (next == L',' || next == L'(' || next == L')') {
				++offset;
				break;
			}
		}
	}
	token = StringUtil::cutQuotes(token);
	return token;
}

void SQLParser::parseSelect(SQLParserResult * result, const std::vector<std::wstring> & tokens)
{
	SelectStatement * curStmt = nullptr;
	Expr * lastExpr = nullptr;

	for (auto tokenIter = tokens.begin(); tokenIter != tokens.end(); tokenIter++) {
		auto & token = *tokenIter;
		std::wstring upToken = StringUtil::toupper(token);

		// token
		wchar_t * string = new wchar_t[token.size() + 1]{0};
		wmemcpy_s(string, token.size() + 1, token.c_str(), token.size());

		bool isUseString = false;
		if (upToken == L"SELECT") {			
			if (curStmt == nullptr) {
				curStmt = new SelectStatement();
				result->addStatement(curStmt);
			} else {
				curStmt = new SelectStatement();
			}
			
			Expr * expr = Expr::makeSelect(curStmt);
			expr->name = string;

			curStmt->selectList->push_back(expr);
			lastExpr = expr;

			isUseString = true;
		} else if (upToken == L"DISTINCT") {
			Expr * expr = Expr::makeLiteral(string);
			expr->select = curStmt;

			curStmt->selectDistinct = true;
			curStmt->selectList->push_back(expr);
			lastExpr->exprList->push_back(expr);

			lastExpr = expr;
			isUseString = true;
		} else if (upToken == L"UNION") {			
			Expr * expr = Expr::makeLiteral(string);
			expr->select = curStmt;

			curStmt->selectDistinct = true;
			curStmt->selectList->push_back(expr);
			isUseString = true;
		} else if (upToken == L",") {			
			Expr * expr = Expr::makeLiteral(string);
			expr->select = curStmt;

			curStmt->selectDistinct = true;
			curStmt->selectList->push_back(expr);
			isUseString = true;
		} else {			
			Expr * expr = Expr::makeLiteral(string);
			curStmt->selectList->push_back(expr);

			lastExpr->exprList->push_back(expr);
			isUseString = true;
		}

		if (!isUseString) {
			delete[] string;
		}
	}
}
