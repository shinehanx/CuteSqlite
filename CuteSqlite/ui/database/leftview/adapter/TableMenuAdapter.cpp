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

 * @file   TableMenuAdapter.cpp
 * @brief  Select database or folder then pop up the menu,
 *		   This class will impletement all the features of table popmenu   
 * 
 * @author Xuehan Qin
 * @date   2023-08-17
 *********************************************************************/

#include "stdafx.h"
#include "TableMenuAdapter.h"

TableMenuAdapter::TableMenuAdapter(HWND parentHwnd, CTreeViewCtrlEx * view)
{
	this->parentHwnd = parentHwnd;
	this->dataView = view;
}

TableMenuAdapter::~TableMenuAdapter()
{

}