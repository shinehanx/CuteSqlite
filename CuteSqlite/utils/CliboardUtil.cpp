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

 * @file   CliboardUtil.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-16
 *********************************************************************/
#include "stdafx.h"
#include "CliboardUtil.h"

bool CliboardUtil::copyString(const ATL::CString & str)
{
	if (!::OpenClipboard(NULL))
    {
        return FALSE;
    }
 
    ATL::CString src = str;
    ::EmptyClipboard();
    int len = src.GetLength();
    int size = (len + 1) * 2;
    HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, size);
    if (!clipbuffer)
    {
        ::CloseClipboard();
        return FALSE;
    }
    char *buffer = (char*)::GlobalLock(clipbuffer);
    memcpy(buffer, src.GetBuffer(), size);
    src.ReleaseBuffer();
    ::GlobalUnlock(clipbuffer);
    ::SetClipboardData(CF_UNICODETEXT, clipbuffer);
    ::CloseClipboard();
    return TRUE;
}
