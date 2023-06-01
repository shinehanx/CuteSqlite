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

 * @file   QDropButton.cpp
 * @brief  Drop button
 * 
 * @author Xuehan Qin
 * @date   2023-05-30
 *********************************************************************/
#include "stdafx.h"
#include "QDropButton.h"

LRESULT QDropButton::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT result = QImageButton::OnCreate(uMsg, wParam, lParam, bHandled);

	return result;
}

LRESULT QDropButton::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT result = QImageButton::OnDestroy(uMsg, wParam, lParam, bHandled);

	return result;
}

LRESULT QDropButton::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT result = QImageButton::OnMouseLeave(uMsg, wParam, lParam, bHandled);

	return result;
}

LRESULT QDropButton::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT result = QImageButton::OnMouseHover(uMsg, wParam, lParam, bHandled);

	return result;
}
