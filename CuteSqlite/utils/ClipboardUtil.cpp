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

 * @file   ClipboardUtil.cpp
 * @brief  Copy string to clipboard and read string from clipboard
 * 
 * @author Xuehan Qin
 * @date   2023-06-15
 *********************************************************************/

#include "stdafx.h"
#include "ClipboardUtil.h"


bool ClipboardUtil::copyToClipboard(std::wstring & str)
{
	if (str.empty()) {
		return false;
	}
	if (!::OpenClipboard(NULL)) {
        return false;
    }
 
    ::EmptyClipboard();
    int len = static_cast<int>(str.length());
    int size = (len + 1) * 2;
    HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, size);
    if (!clipbuffer) {
        ::CloseClipboard();
        return false;
    }
    char *buffer = (char*)::GlobalLock(clipbuffer);
    memcpy(buffer, str.data(), size);
    ::GlobalUnlock(clipbuffer);
    ::SetClipboardData(CF_UNICODETEXT, clipbuffer);
    ::CloseClipboard();
    return true;
}

std::wstring ClipboardUtil::readFromClipboard()
{
	return L"";
}
