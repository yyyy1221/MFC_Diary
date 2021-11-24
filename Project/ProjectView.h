
// ProjectView.h: CProjectView 클래스의 인터페이스
//

#pragma once
#include "afxwin.h"
#include "afxdtctl.h"


class CProjectView : public CFormView
{
protected: // serialization에서만 만들어집니다.
	CProjectView() noexcept;
	DECLARE_DYNCREATE(CProjectView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_PROJECT_FORM };
#endif

// 특성입니다.
public:
	CProjectDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.

// 구현입니다.
public:
	virtual ~CProjectView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	CMonthCalCtrl m_timeMC;
	CDateTimeCtrl m_timeTP;
	CListBox m_listBox;
	CEdit m_editbox;
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnEnChangeEdit2();
	CEdit m_static;
	afx_msg void OnLbnSelchangeList2();
	afx_msg void OnBnClickedEdit();
};

#ifndef _DEBUG  // ProjectView.cpp의 디버그 버전
inline CProjectDoc* CProjectView::GetDocument() const
   { return reinterpret_cast<CProjectDoc*>(m_pDocument); }
#endif

