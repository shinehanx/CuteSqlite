#ifndef GDI_PLUS_UTILS_H
#define GDI_PLUS_UTILS_H

#include <Windows.h>
#include <Shlwapi.h>
#include <GdiPlus.h>
#include <atltypes.h>
#include <atlimage.h>
#include <atlfile.h>

// 计算每一行的帧数
#define BYTESPERLINE(w,bitsperpixel) (((w*bitsperpixel)+31)/32*4)

class GdiPlusUtil
{
public:
	static CRect GetWindowRelativeRect(HWND hwnd)
	{
		CWindow w(hwnd);
		CRect cpr;
		w.GetWindowRect(cpr);
		w.GetParent().ScreenToClient(&cpr);
		return cpr;
	}

	static std::wstring GetProductBinDir()
	{
		wchar_t szbuf[MAX_PATH];  
		::GetModuleFileNameW(NULL,szbuf,MAX_PATH);  
		::PathRemoveFileSpecW(szbuf);
		int length = lstrlen(szbuf);
		szbuf[length] = L'\\';
		szbuf[length+1] = 0;
		return szbuf;
	}

	static void DrawText(Gdiplus::Graphics& graphics,
		Gdiplus::RectF rect,LPCWSTR text,Gdiplus::Font* font,
		const Gdiplus::StringFormat* format,
		Gdiplus::Color* color)
	{
		graphics.DrawString(text,static_cast<int>(wcslen(text)),
			font,rect,format,
			&Gdiplus::SolidBrush(*color));
	}

	static void DrawTextCenter(Gdiplus::Graphics& graphics,
		Gdiplus::RectF rect,LPCWSTR text,Gdiplus::Font* font,
		Gdiplus::Color* color)
	{
		Gdiplus::StringFormat stringFormat;
		stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
		GdiPlusUtil::DrawText(graphics,rect,text,font,&stringFormat,color);
	}

	static void DrawImage(Gdiplus::Graphics& graphics,Gdiplus::Image* image,
		int x,int y)
	{
		DrawImage(graphics,image,x,y,image->GetWidth(),image->GetHeight());
	}

	static void DrawImage(Gdiplus::Graphics& graphics,Gdiplus::Image* image,
		int x,int y,int dest_width,int dest_height)
	{
		Gdiplus::Rect dest_rect(x,y,dest_width,dest_height);
		graphics.DrawImage(image,dest_rect,0,0,image->GetWidth(),image->GetHeight(),
			Gdiplus::UnitPixel);
	}

	static Gdiplus::Font* GetFont(int pixel,bool bold = false, const wchar_t* fontName = L"Microsoft Yahei UI")
	{
		Gdiplus::FontFamily fontFamily(fontName);
		auto font = new Gdiplus::Font(&fontFamily,static_cast<Gdiplus::REAL>(pixel),
			(bold)?Gdiplus::FontStyleBold:Gdiplus::FontStyleRegular,Gdiplus::UnitPixel);
		return font;
	}

	static Gdiplus::Font* GetFont(HDC hdc,int em,int charset = DEFAULT_CHARSET,
		bool bold = false,const wchar_t* fontName = L"Microsoft Yahei UI")
	{
		LOGFONT lf; 
		memset(&lf, 0, sizeof(LOGFONT)); // zero out structure 
		lf.lfHeight = em; // request a 8-pixel-height font
		lf.lfCharSet = charset;
		lstrcpy(lf.lfFaceName,fontName); // request a face name "Microsoft Yahei UI"
		if(bold)
			lf.lfWeight = FW_BOLD;
		else
			lf.lfWeight = FW_NORMAL;
		return new Gdiplus::Font(hdc,&lf);
	}

	static HFONT GetHFONT(int em,int charset = DEFAULT_CHARSET,
		bool bold = false,const wchar_t* fontName = L"Microsoft Yahei UI")
	{
		LOGFONT lf; 
		memset(&lf, 0, sizeof(LOGFONT)); // zero out structure 
		lf.lfHeight = em; // request a 8-pixel-height font
		lf.lfCharSet = charset;
		lstrcpy(lf.lfFaceName,fontName); // request a face name "Microsoft Yahei UI"
		if(bold)
			lf.lfWeight = FW_BOLD;
		else
			lf.lfWeight = FW_NORMAL;
		HFONT font = ::CreateFontIndirect(&lf);
		return font;
	}

	// 获得字体的宽度和高度
	static Gdiplus::SizeF GetTextBounds(const Gdiplus::Font* font, const Gdiplus::StringFormat& strFormat, const std::wstring & szText)
	{
		Gdiplus::GraphicsPath graphicsPathObj;
		Gdiplus::FontFamily fontfamily;
		font->GetFamily(&fontfamily);
		graphicsPathObj.AddString(szText.c_str(), -1, &fontfamily, font->GetStyle(), font->GetSize(), Gdiplus::PointF(0,0),&strFormat);
		Gdiplus::RectF rcBound;
		/// 获取边界范围
		graphicsPathObj.GetBounds(&rcBound);
		/// 返回文本的宽高
		return Gdiplus::SizeF(rcBound.Width,rcBound.Height);
	}

	// 获得字体的宽度和高度
	static Gdiplus::SizeF GetTextBounds(const Gdiplus::Font* font, const std::wstring & szText)
	{
		Gdiplus::StringFormat stringFormat;
		stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		return GdiPlusUtil::GetTextBounds(font, stringFormat, szText);
	}

	// 裁剪图片(强制缩放到图片中，JPG,PNG，BITMAP等)
	static bool thumbImage(std::wstring &srcPath, std::wstring dstPath, int dstWidth, int dstHeight,std::wstring & encodeClsId = std::wstring(L"image/jpeg"))
	{
		Gdiplus::Bitmap * srcBitmap = Gdiplus::Bitmap::FromFile(srcPath.c_str(), false);

		Gdiplus::Bitmap * dstBitmap = new Gdiplus::Bitmap(dstWidth, dstHeight);
		Gdiplus::Graphics graphics(dstBitmap);
		graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

		Gdiplus::Rect dstRect(0, 0, dstWidth, dstHeight);
		Gdiplus::Rect srcRect(0, 0, srcBitmap->GetWidth(), srcBitmap->GetHeight());
		//参考 https://learn.microsoft.com/zh-cn/windows/win32/api/gdiplusgraphics/nf-gdiplusgraphics-graphics-drawimage(image_constrectf__constrectf__unit_constimageattributes)
		Gdiplus::Status status = graphics.DrawImage((Gdiplus::Image *)srcBitmap, dstRect, 0,0,srcBitmap->GetWidth(),srcBitmap->GetHeight(), Gdiplus::UnitPixel, nullptr);
		if (status != Gdiplus::Ok) {
			return false;
		}
		
		CLSID clsid;
		int ret = GetEncoderClsid(encodeClsId.c_str(),  &clsid);
		if (ret == -1) {
			return false;
		}
		dstBitmap->Save(dstPath.c_str(), &clsid, NULL);
		DeleteObject(srcBitmap);
		DeleteObject(dstBitmap);
		return true;
	}

	// 裁剪图片(强制缩放到图片中，仅支持BITMAP,使用GDI效率高些)
	static bool thumbBitmap(std::wstring &srcPath, std::wstring dstPath, int dstWidth, int dstHeight)
	{
		HBITMAP hSourcehBitmap = (HBITMAP)::LoadImage(ModuleHelper::GetModuleInstance(), srcPath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		HBITMAP hCopyBitmap = GdiPlusUtil::stretchBitmap(hSourcehBitmap, dstWidth, dstHeight);
		DeleteObject(hSourcehBitmap);
				
		CImage saveBtimap;
		saveBtimap.Attach(hCopyBitmap);
		saveBtimap.Save(dstPath.c_str());
		saveBtimap.Destroy();
		return true;
	}
	
	/**
	 * 缩放BITMAP.
	 * 
	 * @param hSourcehBitmap
	 * @param dstWidth
	 * @param dstHeight
	 * @return 
	 */
	static HBITMAP stretchBitmap(HBITMAP hSourcehBitmap, int dstWidth, int dstHeight)
	{
		CDC sourcedc;
		CDC destdc;
		sourcedc.CreateCompatibleDC(NULL);
		destdc.CreateCompatibleDC(NULL);
		//the   bitmap   information.
		BITMAP   bm = {0};
		//get   the   bitmap   information.
		::GetObject(hSourcehBitmap,   sizeof(bm),   &bm);
		//   create   a   bitmap   to   hold   the   result
		HBITMAP   hbmresult   =   ::CreateCompatibleBitmap(CClientDC(NULL),   dstWidth ,   dstHeight);
		HBITMAP   hbmoldsource   =   (HBITMAP)::SelectObject(   sourcedc.m_hDC ,   hSourcehBitmap);
		HBITMAP   hbmolddest   =   (HBITMAP)::SelectObject(   destdc.m_hDC ,   hbmresult   );

		destdc.SetStretchBltMode(COLORONCOLOR);
		destdc.StretchBlt(0, 0, dstWidth ,  dstHeight , sourcedc,   0,   0, bm.bmWidth, bm.bmHeight, SRCCOPY ); 

		//   restore   dcs
		::SelectObject(  sourcedc.m_hDC ,   hbmoldsource   );
		::SelectObject(  destdc.m_hDC ,   hbmolddest   );
		::DeleteObject(  sourcedc.m_hDC );
		::DeleteObject(  destdc.m_hDC );

		return   hbmresult;
	}

	/**
	 * 获得格式的编码器
	 * https://learn.microsoft.com/zh-cn/windows/win32/gdiplus/-gdiplus-retrieving-the-class-identifier-for-an-encoder-use 
	 * 
	 * @param format
	 * @param pClsid
	 * @return 
	 */
	static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
	{
	   UINT  num = 0;          // number of image encoders
	   UINT  size = 0;         // size of the image encoder array in bytes

	   Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	   Gdiplus::GetImageEncodersSize(&num, &size);
	   if(size == 0)
		  return -1;  // Failure

	   pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	   if(pImageCodecInfo == NULL)
		  return -1;  // Failure

	   Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	   for(UINT j = 0; j < num; ++j)
	   {
		  if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		  {
			 *pClsid = pImageCodecInfo[j].Clsid;
			 free(pImageCodecInfo);
			 return j;  // Success
		  }    
	   }

	   free(pImageCodecInfo);
	   return -1;  // Failure
	}

	/**
	 * 加载GDI+ Image/Bitmap 需要的stream.，调用方需要释放<IStream *,HGLOBAL>占用的内存
	 * 
	 * @param path 图片路径
	 * @return <IStream * - 文件流指针,HGLOBAL-内存指针> ，掉用方需释放内存和文件流
	 */
	static std::pair<IStream *,HGLOBAL>  loadImageToStream(const wchar_t * path)
	{
		CAtlFile file;
		if (S_OK != file.Create(path, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING)) {
			return {nullptr, nullptr};
		}
		ULONGLONG ulBmpSize;
		if (S_OK != file.GetSize(ulBmpSize)) {
			return {nullptr, nullptr};
		}
		HGLOBAL hMemBmp = GlobalAlloc(GMEM_FIXED, ulBmpSize);
		IStream * stream = NULL;
		if (S_OK != CreateStreamOnHGlobal(hMemBmp, FALSE, &stream)) {
			return {nullptr, nullptr};
		}

		BYTE* bytes = (BYTE *)GlobalLock(hMemBmp);
		file.Seek(0, FILE_BEGIN);
		file.Read(bytes, static_cast<DWORD>(ulBmpSize));

		return { stream, hMemBmp };
	}
};


#endif