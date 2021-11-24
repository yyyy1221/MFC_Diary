
// ProjectView.cpp: CProjectView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Project.h"
#endif

#include "ProjectDoc.h"
#include "ProjectView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CProjectView

IMPLEMENT_DYNCREATE(CProjectView, CFormView)

BEGIN_MESSAGE_MAP(CProjectView, CFormView)
	ON_BN_CLICKED(IDC_DELETE, &CProjectView::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_ADD, &CProjectView::OnBnClickedAdd)
END_MESSAGE_MAP()

// CProjectView 생성/소멸

CProjectView::CProjectView() noexcept
	: CFormView(IDD_PROJECT_FORM)
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CProjectView::~CProjectView()
{
}

void CProjectView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MONTHCALENDAR1, m_timeMC);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_timeTP);
	DDX_Control(pDX, IDC_LIST2, m_listBox);
	DDX_Control(pDX, IDC_EDIT1, m_editbox);
}

BOOL CProjectView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CFormView::PreCreateWindow(cs);
}

void CProjectView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

}


// CProjectView 진단

#ifdef _DEBUG
void CProjectView::AssertValid() const
{
	CFormView::AssertValid();
}

void CProjectView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CProjectDoc* CProjectView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CProjectDoc)));
	return (CProjectDoc*)m_pDocument;
}
#endif //_DEBUG


// CProjectView 메시지 처리기




//일정 삭제 버튼
void CProjectView::OnBnClickedDelete()
{
	int nIndex = m_listBox.GetCurSel();
	if (nIndex != LB_ERR) {
		m_listBox.DeleteString(nIndex);
		/*CString str;
		m_listBox.GetText(nIndex, str);*/
	}
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

//일정 추가 버튼
void CProjectView::OnBnClickedAdd()
{
	CString str;
	m_editbox.GetWindowText(str);
	m_listBox.AddString(str);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
