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

 * @file   SqlSyntaxTree.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-19
 *********************************************************************/
#pragma once

#include <string>
typedef struct _SqlSyntaxNode {
	std::wstring token;
	

	struct _SqlSyntaxNode * parent = nullptr;
	struct _SqlSyntaxNode * left = nullptr;
	struct _SqlSyntaxNode * right = nullptr;
} SqlSyntaxNode;

class SqlSyntaxTree {
public:
	SqlSyntaxTree(const std::wstring & _sql);
	~SqlSyntaxTree();
	SqlSyntaxNode * entry = nullptr;
	SqlSyntaxNode * curNode = nullptr;
	bool empty();
private:
	const std::wstring & sql;

	void create();
	std::wstring tokenFromSql(size_t & offset);
	void insertTokenToLeft(SqlSyntaxNode * curNode, const std::wstring & token);

	void destroy();
	void insertTokenToNearestParentRight(SqlSyntaxNode * curNode, const std::wstring & token);
};
