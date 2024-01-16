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

 * @file   QTreeViewCtrl.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-11
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include <atlctrlx.h>
#include <atlcrack.h>

class QTreeViewCtrl : public CWindowImpl<QTreeViewCtrl, CTreeViewCtrlEx>
{
public:
	
	BEGIN_MSG_MAP_EX(QTreeViewCtrl)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	~QTreeViewCtrl();
private:
	BOOL OnEraseBkgnd(CDCHandle dc);
};
