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

 * @file   SqlSyntaxTree.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-19
 *********************************************************************/

#include "stdafx.h"
#include "SqlSyntaxTree.h"
#include "utils\StringUtil.h"

SqlSyntaxTree::SqlSyntaxTree(const std::wstring & _sql)
	:sql(_sql)
{
	create();
}


SqlSyntaxTree::~SqlSyntaxTree()
{
	destroy();
}

bool SqlSyntaxTree::empty()
{
	return entry == nullptr;
}

void SqlSyntaxTree::create()
{
	if (sql.empty()) {
		return;
	}

	size_t offset = 0;
	size_t size = sql.size();
	for (std::wstring token = tokenFromSql(offset); offset < size;) {
		if (token.empty()) {
			continue;
		}
		std::wstring upToken = StringUtil::toupper(token);
		if (upToken == L"DISTINCT") {
			continue;
		}
		if (upToken == L"SELECT") {
			// insert "select" to root node or current node (if curNode <> entry root node)
			insertTokenToLeft(curNode, token);
			insertTokenToLeft(curNode, L"< Fields >");
			continue;
		} else if (upToken == L"AS") {
			insertTokenToNearestParentRight(curNode, token);
		}


	}
}

void SqlSyntaxTree::destroy()
{
	if (empty()) {
		return;
	}

	// 后续遍历二叉树并删除节点
}


std::wstring SqlSyntaxTree::tokenFromSql(size_t & offset)
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


void SqlSyntaxTree::insertTokenToLeft(SqlSyntaxNode * curNode, const std::wstring & token)
{
	if (curNode == nullptr && entry == nullptr) {
		curNode = new SqlSyntaxNode();
		curNode->token = token;
		entry = curNode;
		return;
	}
	SqlSyntaxNode * tokenNode =  new SqlSyntaxNode();
	tokenNode->token = token;
	tokenNode->parent = curNode;

	curNode->left = tokenNode;
	curNode = tokenNode;
}


void SqlSyntaxTree::insertTokenToNearestParentRight(SqlSyntaxNode * curNode, const std::wstring & token)
{
	ATLASSERT(curNode->parent);

	auto parentNode = curNode->parent;
	for (; parentNode != nullptr; parentNode = parentNode->parent) {
		if (!parentNode->right) {
			break;
		}
	}
	ATLASSERT(!parentNode->right);

	SqlSyntaxNode * tokenNode =  new SqlSyntaxNode();
	tokenNode->token = token;
	tokenNode->parent = parentNode;

	parentNode->right = tokenNode;
	curNode = tokenNode;
}
