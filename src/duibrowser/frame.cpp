//
// main_frame.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2011 achellies (achellies at 163 dot com)
//
// This code may be used in compiled form in any way you desire. This
// source file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//

#include "stdafx.h"
#include <windows.h>
#include <EAWebKit/EAWebKit.h>
#include <EAWebkit/EAWebkitAllocator.h>
#include <EAWebKit/EAWebKitTextInterface.h>
#include <PPMalloc/EAStackAllocator.h>
#include <stdlib.h>  // malloc / new

#include "win_impl_base.hpp"
#include "UIWebkit.hpp"
#include "frame.hpp"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

namespace EA {
	namespace TextWrapper {
		class FontServerProxy;
	}
}

using namespace EA::TextWrapper;

class MyAllocator : public Allocator
{
public:
	void* Malloc(size_t size, int flags, const char* pName)
	{
		return allocator_.Malloc(size);
	}

	void* MallocAligned(size_t size, size_t alignment, size_t offset, int flags, const char* pName)
	{
		return allocator_.MallocAligned(size, alignment, offset);
	}

	void  Free(void* p, size_t size)
	{
		return allocator_.Free(p);
	}

	void* Realloc(void* p, size_t size, int flags)
	{
		return allocator_.Realloc(p, size);
	}

private:
	StackAllocator allocator_;
};

const int kViewTickTimerId = 1001;
const int kViewTickTimerElapse = 500;
const int kDefaultFontSize = 18;
const int kMiniFontSize = 12;
const int kPageTimeoutSeconds = 30;
const int kLoadingIconChangingTimeElapse = 500;
const int kLoadingIconsCount = 4;

const TCHAR* const kWebkitControlName = _T("webkit");
const TCHAR* const kTitleControlName = _T("apptitle");
const TCHAR* const kAddressControlName = _T("addresedt");
const TCHAR* const kCloseButtonControlName = _T("closebtn");
const TCHAR* const kMinButtonControlName = _T("minbtn");
const TCHAR* const kMaxButtonControlName = _T("maxbtn");
const TCHAR* const kRestoreButtonControlName = _T("restorebtn");
const TCHAR* const kBackButtonControlName = _T("backbtn");
const TCHAR* const kForwardButtonControlName = _T("forwardbtn");
const TCHAR* const kRefreshButtonControlName = _T("refreshbtn");
const TCHAR* const kStopButtonControlName = _T("stopbtn");
const TCHAR* const kHomeButtonControlName = _T("homebtn");
const TCHAR* const kToolButtonControlName = _T("toolbtn");
const TCHAR* const kLogoControlName = _T("logo");

const char* const kHomeUrl = "http://www.baidu.com/";

MainFrame::MainFrame()
: webkit_dll_(NULL)
, webkit_(NULL)
, view_(NULL)
, font_server_(NULL)
, glyph_cache_(NULL)
, surface_(NULL)
, raster_(NULL)
, allocator_(NULL)
, font_style_(NULL)
, logo_image_index(0)
, is_loading_(false)
{
	allocator_ = new MyAllocator();
#if defined(UI_BUILD_FOR_WINCE)
	sprintf(application_name_, "DuiBrowser");
	sprintf(user_agent_, GetUserAgent().c_str());
#else
	sprintf_s(application_name_, MAX_PATH, "DuiBrowser");
	sprintf_s(user_agent_, MAX_PATH, GetUserAgent().c_str());
#endif
}

MainFrame::~MainFrame()
{
	delete allocator_;
	allocator_ = NULL;
	PostQuitMessage(0);
}

std::string MainFrame::GetUserAgent()
{
	std::string user_agent;
	// FireFox 3.0.3 uses:
	//     Mozilla/5.0 (Windows; U; Windows NT 6.0; en-US; rv:1.9.0.3) Gecko/2008092417 Firefox/3.0.3
	// IE7 uses:
	//     Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.0; Win64; x64; .NET CLR 2.0.50727; SLCC1; Media Center PC 5.0)
	// Safari uses:
	//     Mozilla/5.0 (Windows; U; Windows NT 6.0; en-US) AppleWebKit/525.19 (KHTML, like Gecko) Version/3.1.2 Safari/525.21

#if defined(EA_PLATFORM_WINDOWS)
#define kPlatformName "Windows"

	std::string osVersion;
	OSVERSIONINFO versionInfo = {0};
	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
	GetVersionEx(&versionInfo);

	if (versionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
		if (versionInfo.dwMajorVersion == 4) {
			if (versionInfo.dwMinorVersion == 0)
				osVersion = "Windows 95";
			else if (versionInfo.dwMinorVersion == 10)
				osVersion = "Windows 98";
			else if (versionInfo.dwMinorVersion == 90)
				osVersion = "Windows 98; Win 9x 4.90";
		}
	} 
	else if (versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		char szBuf[MAX_PATH] = {0};
#if defined(UI_BUILD_FOR_WINCE)
		sprintf(szBuf, "Windows NT %d.%d", versionInfo.dwMajorVersion, versionInfo.dwMinorVersion);
#else
		sprintf_s(szBuf, MAX_PATH, "Windows NT %d.%d", versionInfo.dwMajorVersion, versionInfo.dwMinorVersion);
#endif
		osVersion = szBuf;
	}

	if (osVersion.empty())
	{
		char szBuf[MAX_PATH] = {0};
#if defined(UI_BUILD_FOR_WINCE)
		sprintf(szBuf, "Windows %d.%d", versionInfo.dwMajorVersion, versionInfo.dwMinorVersion);
#else
		sprintf_s(szBuf, MAX_PATH, "Windows %d.%d", versionInfo.dwMajorVersion, versionInfo.dwMinorVersion);
#endif
	}

#else
#define kPlatformName EA_PLATFORM_NAME  // EA_PLATFORM_NAME comes from the EABase package.
#endif

	char szBuf[MAX_PATH] = {0};
#if defined(UI_BUILD_FOR_WINCE)
	sprintf(szBuf, "Mozilla/5.0 (%s; U; %s; zh-cn) AppleWebKit/525.1 (KHTML, like Gecko) EAWebKit/1.0.0 %s",   
		kPlatformName,
		osVersion.c_str(),
		application_name_);
#else
	sprintf_s(szBuf, MAX_PATH, "Mozilla/5.0 (%s; U; %s; zh-cn) AppleWebKit/525.1 (KHTML, like Gecko) EAWebKit/1.0.0 %s",   
		kPlatformName,
		osVersion.c_str(),
		application_name_);
#endif

	return user_agent = szBuf;
}

LPCTSTR MainFrame::GetWindowClassName() const
{
	return _T("DuiBrowserMainFrame");
}

CControlUI* MainFrame::CreateControl(LPCTSTR pstrClass, CPaintManagerUI* pManager)
{
	if (_tcsicmp(pstrClass, _T("Webkit")) == 0)
		return new CWebkitUI();

	return NULL;
}

void MainFrame::OnFinalMessage(HWND hWnd)
{
	if ((webkit_ != NULL) && (view_ != NULL))
	{
		view_->CancelLoad();
		view_->ShutdownView();
		webkit_->DestroyView(view_);
		webkit_->DestroyFontServerWrapperInterface(font_server_);
		webkit_->DestroyGlyphCacheWrapperInterface(glyph_cache_);	
		webkit_->Shutdown();
		webkit_->Destroy();

		if (font_style_)
			font_style_->Destroy();

		view_ = NULL;
		webkit_ = NULL;

		FreeLibrary(webkit_dll_);
		webkit_dll_ = NULL;
	}

	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

tString MainFrame::GetSkinFile()
{
	return _T("main.xml");
}

LRESULT MainFrame::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
#if defined(UI_BUILD_FOR_WIN32) && !defined(UI_BUILD_FOR_WINCE)
	BOOL bZoomed = ::IsZoomed(m_hWnd);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if( ::IsZoomed(m_hWnd) != bZoomed ) 
	{
		if( !bZoomed ) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(paint_manager_.FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(paint_manager_.FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(true);
		}
		else 
		{
			CControlUI* pControl = static_cast<CControlUI*>(paint_manager_.FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(paint_manager_.FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(false);
		}
	}
#else
	return __super::OnSysCommand(uMsg, wParam, lParam, bHandled);
#endif

	return 0;
}

LRESULT MainFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT MainFrame::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if (wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	return FALSE;
}

void MainFrame::OnTimer(TNotifyUI& msg)
{
	switch (msg.wParam)
	{
	case kViewTickTimerId:
		{
			if (view_ != NULL)
				view_->Tick();

			static int timer_count = 0;

			timer_count += kViewTickTimerElapse;
			if (timer_count < kLoadingIconChangingTimeElapse)
				return;

			timer_count = 0;

			LoadInfo& load_info = view_->GetLoadInfo();

			if (!is_loading_)
				logo_image_index = 0;
			else
				++logo_image_index;

			if (logo_image_index > kLoadingIconsCount - 1)
				logo_image_index = 0;
			
			CControlUI* logo = paint_manager_.FindControl(kLogoControlName);
			if (logo !=  NULL)
			{
				TCHAR szBuf[MAX_PATH] = {0};

#if defined(UI_BUILD_FOR_WINCE)
				_stprintf(szBuf, _T(""));
#else
				_stprintf_s(szBuf, MAX_PATH - 1, _T("file='$logo.png' source='%d,0,%d,64'"), logo_image_index * 64, (logo_image_index + 1) * 64);
#endif

				tString logo_image = logo->GetBkImage();
				if (_tcsicmp(logo_image.c_str(), szBuf) != 0)
				{
					logo->SetBkImage(szBuf);
					logo->Invalidate();

					UpdateNavigatingButtonStatus();
				}
			}

			//paint_manager_.KillTimer(msg.pSender, kViewTickTimerId);
		}
		break;
	default:
		break;
	}
}

LRESULT MainFrame::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return __super::MessageHandler(uMsg, wParam, lParam, bHandled);
}

void MainFrame::OnExit(TNotifyUI& msg)
{
	Close();
}

void MainFrame::Init()
{
	typedef EA::WebKit::IEAWebkit* (* PF_CreateEAWebkitInstance)(void);
	PF_CreateEAWebkitInstance create_webkit_instance = NULL;

	webkit_dll_ = LoadLibrary(_T("EAWebkit.dll"));
	if (webkit_dll_ != NULL)
#if defined(UI_BUILD_FOR_WINCE)
		create_webkit_instance = reinterpret_cast<PF_CreateEAWebkitInstance>(GetProcAddress(webkit_dll_, _T("CreateEAWebkitInstance")));
#else
		create_webkit_instance = reinterpret_cast<PF_CreateEAWebkitInstance>(GetProcAddress(webkit_dll_, "CreateEAWebkitInstance"));
#endif

	CWebkitUI* webkit_control = static_cast<CWebkitUI*>(paint_manager_.FindControl(kWebkitControlName));
	if ((create_webkit_instance != NULL) && (webkit_control != NULL))
	{
		webkit_ = create_webkit_instance();
		webkit_->Init(NULL);

		glyph_cache_ = webkit_->CreateGlyphCacheWrapperInterface(NULL);
		font_server_ = webkit_->CreateFontServerWrapperInterface(NULL);

		wchar_t szWindowsDir[MAX_PATH] = {0};
		GetWindowsDirectoryW(szWindowsDir, MAX_PATH);
		wchar_t szFontDir[MAX_PATH] = {0};
#if defined(UI_BUILD_FOR_WINCE)
		swprintf(szFontDir, L"%s\\Fonts", szWindowsDir);
#else
		swprintf_s(szFontDir, MAX_PATH - 1, L"%s\\Fonts", szWindowsDir);
#endif
		uint32_t nCount = font_server_->AddDirectory(szFontDir, L"*.tt?");
		//uint32_t nCount = font_server_->AddDirectory(L"./", L"*.tt?");

		font_style_ = font_server_->CreateTextStyle();
		font_style_->SetSize(kDefaultFontSize);
		font_style_->SetSmooth(kSmoothEnabled);
		
		nCount = font_server_->EnumerateFonts(NULL, 0);
		if (nCount > 0)
		{
			EA::Internal::FontDescription* font_description_array = new EA::Internal::FontDescription[nCount];
			font_server_->EnumerateFonts(font_description_array, nCount);
			for (uint32_t index = 0; index < nCount; ++index)
			{
				if (_tcsicmp(_T("Simsun"), (font_description_array + index)->mFamily) == 0)
				{}
			}
			delete[] font_description_array;
		}

		Parameters& param = webkit_->GetParameters();
		param.mpUserAgent = user_agent_;
		param.mpApplicationName = application_name_;

		//param.mbEnableGammaCorrection = false;
		//param.mColors[kColorActiveSelectionBack] = Color::MAGENTA;
		//param.mColors[kColorActiveSelectionFore] = Color::BLUE;

//		// default "en-us"
//		param.mpLocale = "zh-cn";
//
		param.mDefaultFontSize = kDefaultFontSize;
		param.mDefaultMonospaceFontSize = kDefaultFontSize;
		param.mMinimumFontSize = kMiniFontSize;
		param.mMinimumLogicalFontSize = kMiniFontSize;

		param.mEnableSmoothText = true;
		param.mFontSmoothingEnabled = true;

		param.mSystemFontDescription.mSize = kDefaultFontSize;
		sprintf_s(param.mSystemFontDescription.mFamilies, sizeof(param.mSystemFontDescription.mFamilies) / sizeof(param.mSystemFontDescription.mFamilies[0]),\
			"Arial Unicode MS");
#if 1
		sprintf_s(param.mFontFamilyStandard, sizeof(param.mFontFamilyStandard) / sizeof(param.mFontFamilyStandard[0]), "Arial Unicode MS");
		sprintf_s(param.mFontFamilySerif, sizeof(param.mFontFamilySerif) / sizeof(param.mFontFamilySerif[0]), "Times New Roman");
		sprintf_s(param.mFontFamilySansSerif, sizeof(param.mFontFamilySansSerif) / sizeof(param.mFontFamilySansSerif[0]), "Arial Unicode MS");
		sprintf_s(param.mFontFamilyMonospace, sizeof(param.mFontFamilyMonospace) / sizeof(param.mFontFamilyMonospace[0]), "MS Courier New");
		sprintf_s(param.mFontFamilyCursive, sizeof(param.mFontFamilyCursive) / sizeof(param.mFontFamilyCursive[0]), "Times New Roman");
		sprintf_s(param.mFontFamilyFantasy, sizeof(param.mFontFamilyFantasy) / sizeof(param.mFontFamilyFantasy[0]), "System");
#else
		//iFonts.default_font = _strdup("Bitstream Vera Sans");
		//iFonts.cursive_font = _strdup("Times New Roman");
		//iFonts.fantasy_font = _strdup("System");
		//iFonts.monospace_font = _strdup("Bitstream Vera Sans Mono");
		//iFonts.sans_serif_font = _strdup("Tahoma");
		//iFonts.serif_font = _strdup("Bitstream Vera Serif");
#endif
		webkit_->SetParameters(param);

		view_ = webkit_->CreateView();
		webkit_->SetViewNotification(this);
		raster_ = webkit_->GetEARasterInstance();
	}
}

void MainFrame::OnPrepare(TNotifyUI& msg)
{
	CControlUI* app_title = paint_manager_.FindControl(kTitleControlName);
	CWebkitUI* webkit_control = static_cast<CWebkitUI*>(paint_manager_.FindControl(kWebkitControlName));
	if ((webkit_control != NULL) && (app_title != NULL))
	{
		paint_manager_.SetTimer(app_title, kViewTickTimerId, kViewTickTimerElapse);
		webkit_control->SetEARasterAndView(raster_, view_);

		//view_->SetURI("file:///E:/Webkit/chapter01/chapter01.html");
		view_->SetURI(kHomeUrl);
	}
}

void MainFrame::UpdateNavigatingButtonStatus()
{
	if (view_ != NULL)
	{
		LoadInfo& load_info = view_->GetLoadInfo();

		CButtonUI* refresh_button = static_cast<CButtonUI*>(paint_manager_.FindControl(kRefreshButtonControlName));
		CButtonUI* stop_button = static_cast<CButtonUI*>(paint_manager_.FindControl(kStopButtonControlName));
		if (stop_button != NULL)
		{
			if (!is_loading_)
			{
				stop_button->SetEnabled(false);
				stop_button->SetVisible(false);
				refresh_button->SetVisible();
			}
			else
			{
				stop_button->SetVisible(true);
				stop_button->SetEnabled();
				refresh_button->SetVisible(false);
			}
		}

		CButtonUI* back_button = static_cast<CButtonUI*>(paint_manager_.FindControl(kBackButtonControlName));
		CButtonUI* forward_button = static_cast<CButtonUI*>(paint_manager_.FindControl(kForwardButtonControlName));
		if ((back_button != NULL) && (forward_button != NULL))
		{}
	}
}

void MainFrame::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, DuiLib::kWindowInit) == 0)
	{
		OnPrepare(msg);
	}
	else if (_tcsicmp(msg.sType, DuiLib::kReturn) == 0)
	{
		CEditUI* address_edit = static_cast<CEditUI*>(paint_manager_.FindControl(kAddressControlName));
		if ((view_ != NULL) && (address_edit != NULL) && _tcslen(address_edit->GetText()) > 0)
		{
			tString input_url = address_edit->GetText();			
			view_->ResetForNewLoad();
			view_->CancelLoad();
			if (input_url.find(_T("file://")) != tString::npos)
			{}
			else if ((input_url.find(_T("http://")) == tString::npos) && (input_url.find(_T("https://")) == tString::npos))
				input_url = _T("http://") + input_url;
			view_->SetURI(StringConvertor::WideToUtf8(input_url.c_str()));
		}
	}
	else if (_tcsicmp(msg.sType, DuiLib::kClick) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(), kCloseButtonControlName) == 0)
		{
			OnExit(msg);
		}
		else if (_tcsicmp(msg.pSender->GetName(), kMinButtonControlName) == 0)
		{
#if defined(UI_BUILD_FOR_WINCE)
			::ShowWindow(m_hWnd, SW_MINIMIZE);
#else
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
#endif
		}
		else if (_tcsicmp(msg.pSender->GetName(), kMaxButtonControlName) == 0)
		{
#if defined(UI_BUILD_FOR_WINCE)
			::ShowWindow(m_hWnd, SW_MAXIMIZE);
			CControlUI* pControl = static_cast<CControlUI*>(paint_manager_.FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(paint_manager_.FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(true);
#else
			SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
#endif
		}
		else if (_tcsicmp(msg.pSender->GetName(), kRestoreButtonControlName) == 0)
		{
#if defined(UI_BUILD_FOR_WINCE)
			::ShowWindow(m_hWnd, SW_RESTORE);
			CControlUI* pControl = static_cast<CControlUI*>(paint_manager_.FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(paint_manager_.FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(false);
#else
			SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
#endif
		}
		else if (_tcsicmp(msg.pSender->GetName(), kToolButtonControlName) == 0)
		{}
		else if (_tcsicmp(msg.pSender->GetName(), kHomeButtonControlName) == 0)
		{
			if (view_ != NULL)
			{
				view_->ResetForNewLoad();
				view_->CancelLoad();
				view_->SetURI(kHomeUrl);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kBackButtonControlName) == 0)
		{
			if (view_ != NULL)
				view_->GoBack();
		}
		else if (_tcsicmp(msg.pSender->GetName(), kForwardButtonControlName) == 0)
		{
			if (view_ != NULL)
				view_->GoForward();
		}
		else if (_tcsicmp(msg.pSender->GetName(), kRefreshButtonControlName) == 0)
		{
			if (view_ != NULL)
				view_->Refresh();
		}
		else if (_tcsicmp(msg.pSender->GetName(), kStopButtonControlName) == 0)
		{
			if (view_ != NULL)
				view_->CancelLoad();
		}
	}
	else if (_tcsicmp(msg.sType, DuiLib::kTimer) == 0)
	{
		return OnTimer(msg);
	}
}

bool MainFrame::ViewUpdate(ViewUpdateInfo& view_update_info)
{
	CWebkitUI* webkit_control = static_cast<CWebkitUI*>(paint_manager_.FindControl(kWebkitControlName));
	if (webkit_control != NULL)
		webkit_control->LayoutChanged();
	return true;
}

bool MainFrame::LoadUpdate(LoadInfo& load_info)
{
	CControlUI* app_title = paint_manager_.FindControl(kTitleControlName);
	if ((app_title != NULL) && (load_info.mLET == kLETTitleReceived) && _tcslen(webkit_->GetCharacters(load_info.mPageTitle)) > 0)
	{
		app_title->SetText(webkit_->GetCharacters(load_info.mPageTitle));
	}

	CEditUI* address_edit = static_cast<CEditUI*>(paint_manager_.FindControl(kAddressControlName));
	if ((address_edit != NULL) && (kLETResponseReceived == load_info.mLET) && _tcslen(webkit_->GetCharacters(load_info.mURI)) > 0 &&
		_tcsicmp(webkit_->GetCharacters(load_info.mURI), navigating_url_.c_str()) != 0)
	{
		navigating_url_ = webkit_->GetCharacters(load_info.mURI);
		address_edit->SetText(navigating_url_.c_str());
	}

	if (load_info.mLET == kLETLoadStarted)
		is_loading_ = true;
	else if ((load_info.mLET == kLETLoadFailed) || (kLETLoadCompleted == load_info.mLET))
		is_loading_ = false;

	return true;
}