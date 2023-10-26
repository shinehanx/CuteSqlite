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

 * @file   MenuUtil.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-25
 *********************************************************************/
#pragma once

#include <wincodec.h>           // WIC

class WICFactory
{
public:
    ~WICFactory();
    static IWICImagingFactory* getWIC() { return m_instance.m_pWICFactory; }
 
private:
    HRESULT _hrOleInit{};
    IWICImagingFactory *m_pWICFactory{};
 
    static WICFactory m_instance;      //CWCIFactory类唯一的对象
 
private:
    WICFactory();
};
 
class MenuUtil
{
public:
    MenuUtil();
    ~MenuUtil();
    static HRESULT addIconToMenuItem(HMENU hmenu, int iMenuItem, BOOL fByPosition, HICON hicon);
 
private:
    static HRESULT addBitmapToMenuItem(HMENU hmenu, int iItem, BOOL fByPosition, HBITMAP hbmp);
    static void initBitmapInfo(__out_bcount(cbInfo) BITMAPINFO *pbmi, ULONG cbInfo, LONG cx, LONG cy, WORD bpp);
    static HRESULT create32BitHBITMAP(HDC hdc, const SIZE *psize, __deref_opt_out void **ppvBits, __out HBITMAP* phBmp);
};
 