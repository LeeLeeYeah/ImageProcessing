// This is a part of the Microsoft Foundation Classes C++ library. 
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// ChildView.h : interface of the CChildView class
//


#pragma once
#include <afxstr.h>
#include <atlimage.h>
#include <comdef.h>


enum SizesEnum
{
	SIZE_HALF,
	SIZE_ORIGINAL,
	SIZE_DOUBLE,
	SIZE_FILL,
	SIZE_NONE
};

// CChildView window

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions

private:
	CImage imgOriginal;
	int	m_nFilterLoad;
	SizesEnum m_nImageSize;

protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	void OnFileOpenimage(void);
	void OnFileSaveImage(void);
	void OnToolsMakeBW(void);
	void OnImageInfo(void);
	void OnUpdateSizeHalf(CCmdUI* pCmdUI);
	void OnUpdateSizeOriginal(CCmdUI* pCmdUI);
	void OnUpdateSizeDouble(CCmdUI* pCmdUI);
	void OnUpdateSizeFill(CCmdUI* pCmdUI);
	void OnChangeSize(UINT nID);
	void OnUpdateImage(CCmdUI* pCmdUI);
public:
	afx_msg void OnToolsBinarization();
	afx_msg void OnToolsBinarizationLocal();
	afx_msg void OnToolsDilation();
	afx_msg void OnToolsErosion();
	afx_msg void OnToolsOpenning();
	afx_msg void OnToolsClosing();
	afx_msg void OnToolsHMT();	
	afx_msg void OnToolsLog();
	afx_msg void OnToolsEqualization();
	afx_msg void OnToolsTranslation();
	afx_msg void OnToolsRotation();
	afx_msg void OnToolsScale();
	afx_msg void OnToolsShear();
	afx_msg void OnToolsMirror();
	afx_msg void OnToolsMeanFilter();
	afx_msg void OnToolsLaplacian();
	afx_msg void OnToolsBilateral();
};

