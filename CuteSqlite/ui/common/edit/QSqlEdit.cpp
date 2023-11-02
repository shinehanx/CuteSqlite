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

 * @file   QSqlEdit.cpp
 * @brief  SQL Editor, Hightlit the sql tags, and show the line nums
 * 
 * @author Xuehan Qin
 * @date   2023-05-25
 *********************************************************************/
#include "stdafx.h"
#include "QSqlEdit.h"
#include <Scintilla/Scintilla.h>
#include <Scintilla/SciLexer.h>
#include "utils/Log.h"
#include "utils/StringUtil.h"

const char sqlKeyWords[] =
"absolute action add admin after aggregate "
"alias all allocate alter and any are array as asc "
"assertion at authorization "
"before begin binary bit blob body boolean both breadth by "
"call cascade cascaded case cast catalog char character "
"check class clob close collate collation column commit "
"completion connect connection constraint constraints "
"constructor continue corresponding create cross cube current "
"current_date current_path current_role current_time current_timestamp "
"current_user cursor cycle "
"data date datetime day deallocate dec decimal declare default "
"deferrable deferred delete depth deref desc describe descriptor "
"destroy destructor deterministic dictionary diagnostics disconnect "
"distinct domain double drop dynamic "
"each else end end-exec equals escape every except "
"exception exec execute exists exit external "
"false fetch first float for foreign found from free full "
"function "
"general get global go goto grant group grouping "
"having host hour "
"identity if ignore immediate in indicator initialize initially "
"inner inout input insert int integer intersect interval "
"into is isolation iterate "
"join "
"key "
"language large last lateral leading left less level like "
"limit local localtime localtimestamp locator "
"map match merge minute modifies modify module month "
"names national natural nchar nclob new next no none "
"not null numeric "
"object of off old on only open operation option "
"or order ordinality out outer output "
"package pad parameter parameters partial path postfix precision prefix "
"preorder prepare preserve primary "
"prior privileges procedure public "
"read reads real recursive ref references referencing relative "
"restrict result return returns revoke right "
"role rollback rollup routine row rows "
"savepoint schema scroll scope search second section select "
"sequence session session_user set sets size smallint some| space "
"specific specifictype sql sqlexception sqlstate sqlwarning start "
"state statement static structure system_user "
"table temporary terminate than then time timestamp "
"timezone_hour timezone_minute to trailing transaction translation "
"treat trigger true "
"under union unique unknown "
"unnest update usage user using "
"value values varchar variable varying view "
"when whenever where with without work write "
"year "
"zone";

BOOL QSqlEdit::PreTranslateMessage(MSG* pMsg)
{
	
	return FALSE;
}


void QSqlEdit::init()
{
	// clear all text styles
	SendMessage(SCI_CLEARDOCUMENTSTYLE, 0, 0);
	// set the number of styling bits to 7 - the asp/html views need a lot of styling - default is 5
	// If you leave the default you will see twiggle lines instead of ASP code
	// SendMessage(SCI_SETSTYLEBITS, 7, 0);
	// set the display for indetation guides to on - this displays virtical dotted lines from the beginning of

	SendMessage(SCI_SETCODEPAGE, SC_CP_UTF8, 0);

	// a code block to the end of the block
	SendMessage(SCI_SETINDENTATIONGUIDES, TRUE, 0);
	// set tabwidth to 4
	SendMessage(SCI_SETTABWIDTH, 4, 0);
	// set indention to 4
	SendMessage(SCI_SETINDENT, 4, 0); 
	// set the caret blinking time to 400 milliseconds
	SendMessage(SCI_SETCARETPERIOD, 400, 0);
	// display fold margins
	setFold();
	// hide SelectionMargin
	SendMessage(SCI_SETMARGINWIDTHN, 1, 0);
	// set markersymbol for marker type 0 - bookmark 
	SendMessage(SCI_MARKERDEFINE, 0, SC_MARK_CIRCLE);
	// set the forground color for some styles
	SendMessage(SCI_STYLESETFORE, 0, RGB(0, 0, 0));
	SendMessage(SCI_STYLESETFORE, 2, RGB(0, 64, 0));
	SendMessage(SCI_STYLESETFORE, 5, RGB(0, 0, 255));
	SendMessage(SCI_STYLESETFORE, 6, RGB(200, 20, 0));
	SendMessage(SCI_STYLESETFORE, 9, RGB(0, 0, 255));
	SendMessage(SCI_STYLESETFORE, 10, RGB(255, 0, 64));
	SendMessage(SCI_STYLESETFORE, 11, RGB(0, 0, 0));
	// set the backgroundcolor of brace highlights
	SendMessage(SCI_STYLESETBACK, STYLE_BRACELIGHT, RGB(0, 255, 0));
	// set end of line mode to CRLF
	SendMessage(SCI_CONVERTEOLS, 2, 0);
	SendMessage(SCI_SETEOLMODE, 2, 0);
	//   SendMessage(SCI_SETVIEWEOL, TRUE, 0);

	//显示当前行的淡紫色背景
	SendMessage(SCI_SETCARETLINEVISIBLE, TRUE, 0);
	SendMessage(SCI_SETCARETLINEBACK, RGB(232, 232, 255), 0);
	SendMessage(SCI_SETCARETLINEBACKALPHA, 100, 0);
}


void QSqlEdit::setDefaultColorFont(int nSize, const TCHAR* face)
{
	// SendMessage(SCI_SETSELFORE, TRUE, RGB(255, 255, 255));
	//选中行的颜色
	SendMessage(SCI_SETSELBACK, TRUE, RGB(192, 192, 192));

	//默认文本颜色
	SendMessage(SCI_STYLESETFORE, STYLE_DEFAULT, RGB(0x00, 0x00, 0x00));
	SendMessage(SCI_STYLESETBACK, STYLE_DEFAULT, RGB(0xff, 0xff, 0xff));
	SendMessage(SCI_STYLESETSIZE, STYLE_DEFAULT, nSize);
	SendMessage(SCI_STYLESETFONT, STYLE_DEFAULT, reinterpret_cast<LPARAM>(face));
}


void QSqlEdit::setFold(BOOL bFold /*= TRUE*/)
{
	if (bFold)
	{
		// source folding section
		// tell the lexer that we want folding information - the lexer supplies "folding levels"
		SendMessage(SCI_SETPROPERTY, (WPARAM) "fold", (LPARAM) "1");
		SendMessage(SCI_SETPROPERTY, (WPARAM) "fold.html", (LPARAM) "1");
		SendMessage(SCI_SETPROPERTY, (WPARAM) "fold.html.preprocessor", (LPARAM) "1");
		SendMessage(SCI_SETPROPERTY, (WPARAM) "fold.comment", (LPARAM) "1");
		SendMessage(SCI_SETPROPERTY, (WPARAM) "fold.at.else", (LPARAM) "1");
		SendMessage(SCI_SETPROPERTY, (WPARAM) "fold.flags", (LPARAM) "1");
		SendMessage(SCI_SETPROPERTY, (WPARAM) "fold.preprocessor", (LPARAM) "1");
		SendMessage(SCI_SETPROPERTY, (WPARAM) "styling.within.preprocessor", (LPARAM) "1");

		setMarginWidthN(2, 16); //SendMessage( SCI_SETMARGINWIDTHN, 2, 16 ); // FoldMargin

		SendMessage(SCI_SETMARGINTYPEN, 2, SC_MARGIN_SYMBOL);//页边类型
		SendMessage(SCI_SETMARGINMASKN, 2, SC_MASK_FOLDERS); //页边掩码
		SendMessage(SCI_SETMARGINWIDTHN, 2, 16); //页边宽度
		SendMessage(SCI_SETMARGINSENSITIVEN, 2, TRUE); //响应鼠标消息

		// 折叠标签样式
		SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_CIRCLEPLUS);
		SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_CIRCLEMINUS);
		SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND, SC_MARK_CIRCLEPLUSCONNECTED);
		SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_CIRCLEMINUSCONNECTED);
		SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNERCURVE);

		SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
		SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNERCURVE);

		// 折叠标签线颜色
		SendMessage(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERSUB, 0xff0000); //蓝色
		SendMessage(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERMIDTAIL, 0xff0000);
		SendMessage(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERTAIL, 0xff0000);

		SendMessage(SCI_SETFOLDFLAGS, 16 | 4, 0); //如果折叠就在折叠行的上下各画一条横线
	}
	else
	{
		setMarginWidthN(2, 0); //SendMessage( SCI_SETMARGINWIDTHN, 2, 0 ); // FoldMargin
	}
}


void QSqlEdit::toggleFold(long line)
{
	SendMessage(SCI_TOGGLEFOLD, static_cast<WPARAM>(line), 0);
}


int QSqlEdit::getMarginWidthN(int margin)
{
	LRESULT ret = SendMessage(SCI_GETMARGINWIDTHN, static_cast<WPARAM>(margin), 0);
	return static_cast<int>(ret);
}


void QSqlEdit::setMarginWidthN(int margin, int mask)
{
	SendMessage(SCI_SETMARGINWIDTHN, static_cast<WPARAM>(margin), static_cast<LPARAM>(mask));
}


void QSqlEdit::updateLineNumberWidth(void)
{
	//start 显示行号
	long  iLineMarginWidthNow;
	long  iLineMarginWidthFit;
	long iLineNum = static_cast<long>(SendMessage(SCI_GETLINECOUNT, 0, 0));
	long iLineNumCount = 1;
	while (iLineNum != 0)
	{
		++iLineNumCount;
		iLineNum /= 10;
	}
	iLineMarginWidthNow = static_cast<long>(SendMessage(SCI_GETMARGINWIDTHN, 0, 0));
	long charWidth = static_cast<long>(SendMessage(SCI_TEXTWIDTH, STYLE_LINENUMBER, (LPARAM)("9")));
	iLineMarginWidthFit = charWidth * iLineNumCount;
	if (iLineMarginWidthNow != iLineMarginWidthFit)
	{
		SendMessage(SCI_SETMARGINWIDTHN, 0, iLineMarginWidthFit);
	}
	//end of 显示行号
}


std::wstring QSqlEdit::getSelText()
{
	std::wstring text;
	int len = static_cast<int>(SendMessage(SCI_GETSELTEXT, 0, 0));
	if (!len) {
		return text;
	}
	char * buffer = new char[len];
	memset(buffer, 0, len);
	SendMessage(SCI_GETSELTEXT, len, LPARAM(buffer));
	wchar_t * unicodeStr = StringUtil::utf8ToUnicode(buffer);
	if (unicodeStr == nullptr) {
		return text;
	}
	text = unicodeStr;
	free(unicodeStr);
	delete[] buffer;
	return text;
}

std::wstring QSqlEdit::getText()
{
	std::wstring text;
	int len = static_cast<int>(SendMessage(SCI_GETTEXT, 0, 0));
	if (!len) {
		return text;
	}
	char * buffer = new char[len];
	memset(buffer, 0, len);
	SendMessage(SCI_GETTEXT, len, LPARAM(buffer));
	wchar_t * unicodeStr = StringUtil::utf8ToUnicode(buffer);
	if (unicodeStr == nullptr) {
		return text;
	}
	text = unicodeStr;
	free(unicodeStr);
	delete[] buffer;
	return text;
}

void QSqlEdit::replaceSelText(std::wstring & text)
{
	if (text.empty()) {
		return;
	}
	char * utf8Str = StringUtil::unicodeToUtf8(text);
	long charWidth = static_cast<long>(SendMessage(SCI_REPLACESEL, NULL, (LPARAM)utf8Str));
	::free(utf8Str);
}

void QSqlEdit::setText(const std::wstring & text)
{
	if (text.empty()) {
		return;
	}
	char * utf8Str = StringUtil::unicodeToUtf8(text);
	long charWidth = static_cast<long>(SendMessage(SCI_SETTEXT, NULL, (LPARAM)utf8Str));
	::free(utf8Str);
}

void QSqlEdit::addText(const std::wstring & text)
{
	if (text.empty()) {
		return;
	}
	char * utf8Str = StringUtil::unicodeToUtf8(text);
	long charWidth = static_cast<long>(SendMessage(SCI_ADDTEXT, strlen(utf8Str), (LPARAM)utf8Str));
	::free(utf8Str);
}

void QSqlEdit::clearText()
{
	SendMessage(SCI_CLEARALL, NULL, NULL);
}

long QSqlEdit::lineFromPosition(long pos)
{
	LRESULT ret = SendMessage(SCI_LINEFROMPOSITION, static_cast<WPARAM>(pos), 0);
	return static_cast<long>(ret);
}


void QSqlEdit::setSqlSyntax()
{
	SendMessage(SCI_SETLEXER, SCLEX_SQL);
	SendMessage(SCI_SETKEYWORDS, 0, (LPARAM)sqlKeyWords);
	// 下面设置各种语法元素前景色  
	SendMessage(SCI_STYLESETFORE, SCE_SQL_WORD, 0x00FF0000);   //关键字
	SendMessage(SCI_STYLESETFORE, SCE_SQL_STRING, 0x001515A3); //字符串
	SendMessage(SCI_STYLESETFORE, SCE_SQL_CHARACTER, 0x001515A3); //字符  
	SendMessage(SCI_STYLESETFORE, SCE_SQL_COMMENT, 0x00008000);//块注释  
	SendMessage(SCI_STYLESETFORE, SCE_SQL_COMMENTLINE, 0x00008000);//行注释  
	SendMessage(SCI_STYLESETFORE, SCE_SQL_COMMENTDOC, 0x00008000);//文档注释（/**开头）  
	SendMessage(SCI_STYLESETFORE, SCE_SQL_NUMBER, 0x000010ff);//数字  
	SendMessage(SCI_STYLESETFORE, SCE_SQL_OPERATOR, 0x0000c0f0);//操作  
}

void QSqlEdit::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);
	//crateOrShowEditor(clientRect);
}

HWND QSqlEdit::Create(HWND hWndParent, const ATL::_U_RECT& rect, DWORD dwExStyle, UINT nID /* = 0*/)
{
	bkgBrush = ::CreateSolidBrush(bkgColor);

	HMODULE hmod2 = ::LoadLibrary(L"SciLexer.dll");

	m_hWnd = CWindow::Create(_T("Scintilla"), hWndParent, rect, _T(""), WS_CHILD | WS_VISIBLE, dwExStyle, nID);
	return m_hWnd;
}


void QSqlEdit::initEdit(int nSize, const TCHAR* face)
{
	setSqlSyntax();
	init();
	setDefaultColorFont(nSize, face);
	updateLineNumberWidth();
}


int QSqlEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return 0;
}


void QSqlEdit::OnSize(UINT nType, CSize size)
{
	createOrShowUI();
}



