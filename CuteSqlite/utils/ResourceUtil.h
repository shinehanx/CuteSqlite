#pragma once

#include <string>

using namespace std;

class ResourceUtil
{

public:
	ResourceUtil();
	~ResourceUtil();

	// res Ŀ¼
	static wstring getResourceDir();
	static wstring getResourcePath(wchar_t * fileName);

	// exeĿ¼
	static wstring getProductBinDir();
	static wstring getProductResDir();
	static wstring getProductImagesDir();
	static std::wstring getProductUploadDir();
	static wstring getProductTmpDir();

	static std::wstring getDesktopDir();
	static char* convertUnicodeToANSI(const wchar_t* unicode);
};

