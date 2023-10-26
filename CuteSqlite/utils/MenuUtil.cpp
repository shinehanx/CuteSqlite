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

 * @file   MenuUtil.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-25
 *********************************************************************/
#include "stdafx.h"
#include "MenuUtil.h"
 
#pragma comment(lib, "WindowsCodecs")    // WIC

WICFactory WICFactory::m_instance;
 
WICFactory::WICFactory()
{
    _hrOleInit = ::OleInitialize(NULL);
    CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));
}
 
WICFactory::~WICFactory()
{
    if (m_pWICFactory)
    {
        m_pWICFactory->Release();
        m_pWICFactory = NULL;
    }
 
    if (SUCCEEDED(_hrOleInit))
    {
        OleUninitialize();
    }
}
 
typedef DWORD ARGB;
 
MenuUtil::MenuUtil()
{
}
 
MenuUtil::~MenuUtil()
{
}
 
HRESULT MenuUtil::addIconToMenuItem(HMENU hmenu, int iMenuItem, BOOL fByPosition, HICON hicon)
{
    if (WICFactory::getWIC() == nullptr)
        return 0;
    HBITMAP hbmp = NULL;
 
    IWICBitmap *pBitmap;
    HRESULT hr = WICFactory::getWIC()->CreateBitmapFromHICON(hicon, &pBitmap);
    if (SUCCEEDED(hr))
    {
        IWICFormatConverter *pConverter;
        hr = WICFactory::getWIC()->CreateFormatConverter(&pConverter);
        if (SUCCEEDED(hr))
        {
            hr = pConverter->Initialize(pBitmap, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
            if (SUCCEEDED(hr))
            {
                UINT cx, cy;
                hr = pConverter->GetSize(&cx, &cy);
                if (SUCCEEDED(hr))
                {
                    const SIZE sizIcon = { (int)cx, -(int)cy };
                    BYTE *pbBuffer;
                    hr = create32BitHBITMAP(NULL, &sizIcon, reinterpret_cast<void **>(&pbBuffer), &hbmp);
                    if (SUCCEEDED(hr))
                    {
                        const UINT cbStride = cx * sizeof(ARGB);
                        const UINT cbBuffer = cy * cbStride;
                        hr = pConverter->CopyPixels(NULL, cbStride, cbBuffer, pbBuffer);
                    }
                }
            }
 
            pConverter->Release();
        }
 
        pBitmap->Release();
    }
 
    if (SUCCEEDED(hr))
    {
        hr = addBitmapToMenuItem(hmenu, iMenuItem, fByPosition, hbmp);
    }
 
    if (FAILED(hr))
    {
        DeleteObject(hbmp);
        hbmp = NULL;
    }
 
    return hr;
}
 
HRESULT MenuUtil::addBitmapToMenuItem(HMENU hmenu, int iItem, BOOL fByPosition, HBITMAP hbmp)
{
    HRESULT hr = E_FAIL;
 
    MENUITEMINFO mii = { sizeof(mii) };
    mii.fMask = MIIM_BITMAP;
    mii.hbmpItem = hbmp;
    if (::SetMenuItemInfo(hmenu, iItem, fByPosition, &mii))
    {
        hr = S_OK;
    }
 
    return hr;
}
 
void MenuUtil::initBitmapInfo(BITMAPINFO * pbmi, ULONG cbInfo, LONG cx, LONG cy, WORD bpp)
{
    ZeroMemory(pbmi, cbInfo);
    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biPlanes = 1;
    pbmi->bmiHeader.biCompression = BI_RGB;
 
    pbmi->bmiHeader.biWidth = cx;
    pbmi->bmiHeader.biHeight = cy;
    pbmi->bmiHeader.biBitCount = bpp;
}
 
HRESULT MenuUtil::create32BitHBITMAP(HDC hdc, const SIZE * psize, void ** ppvBits, HBITMAP * phBmp)
{
    *phBmp = NULL;
 
    BITMAPINFO bmi;
    initBitmapInfo(&bmi, sizeof(bmi), psize->cx, psize->cy, 32);
 
    HDC hdcUsed = hdc ? hdc : GetDC(NULL);
    if (hdcUsed)
    {
        *phBmp = CreateDIBSection(hdcUsed, &bmi, DIB_RGB_COLORS, ppvBits, NULL, 0);
        if (hdc != hdcUsed)
        {
            ReleaseDC(NULL, hdcUsed);
        }
    }
    return (NULL == *phBmp) ? E_OUTOFMEMORY : S_OK;
}
