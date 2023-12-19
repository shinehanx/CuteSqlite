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

 * @file   SQLParserResult.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-19
 *********************************************************************/
#pragma once
#include <vector>
#include "sql/Expr.h"
#include "sql/SQLStatement.h"


class SQLParserResult {
public:
	// Initialize with empty statement list.
    SQLParserResult();

    // Initialize with a single statement.
    // Takes ownership of the statement.
    SQLParserResult(SQLStatement * stmt);

    // Move constructor.
    SQLParserResult(SQLParserResult&& moved);

    // Deletes all statements in the result.
    virtual ~SQLParserResult();

    // Set whether parsing was successful.
    void setIsValid(bool isValid);

    // Returns true if parsing was successful.
    bool isValid() const;

    // Returns the number of statements in the result.
    size_t size() const;

    // Set the details of the error, if available.
    // Takes ownership of errorMsg.
    void setErrorDetails(wchar_t * errorMsg, int errorLine, int errorColumn);

    // Returns the error message, if an error occurred.
    const wchar_t * errorMsg() const;

    // Returns the line number of the occurrance of the error in the query.
    int errorLine() const;

    // Returns the column number of the occurrance of the error in the query.
    int errorColumn() const;

    // Adds a statement to the result list of statements.
    // SQLParserResult takes ownership of the statement.
    void addStatement(SQLStatement* stmt);

    // Gets the SQL statement with the given index.
    const SQLStatement* getStatement(int index) const;

    // Gets the non const SQL statement with the given index.
    SQLStatement* getMutableStatement(int index);

    // Get the list of all statements.
    const std::vector<SQLStatement*>& getStatements() const;

    // Returns a copy of the list of all statements in this result.
    // Removes them from this result.
    std::vector<SQLStatement*> releaseStatements();

    // Deletes all statements and other data within the result.
    void reset();

    // Does NOT take ownership.
    void addParameter(Expr* parameter);

    const std::vector<Expr*>& parameters();

private:
	// List of statements within the result.
	std::vector<SQLStatement*> statements_;

	// Flag indicating the parsing was successful.
	bool isValid_;

	// Error message, if an error occurred.
	wchar_t * errorMsg_;

	// Line number of the occurrance of the error in the query.
	int errorLine_;

	// Column number of the occurrance of the error in the query.
	int errorColumn_;

	// Does NOT have ownership.
	std::vector<Expr*> parameters_;
};
