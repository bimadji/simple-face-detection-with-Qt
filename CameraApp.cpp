#include "pch.h"
#include "framework.h"
#include "CameraApp.h"
#include "CameraDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CCameraApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CCameraApp theApp;

BOOL CCameraApp::InitInstance()
{
    CWinApp::InitInstance();

    CCameraDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();

    return FALSE;
} 