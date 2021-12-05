
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
#include <stdio.h>
#include <wchar.h>
#include <vector>
#include <stdlib.h>
#include <atlstr.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct schedule {
	CString date;
	CString schedule;
	CString start_time;
	CString end_time;
	CString location;
	CString memo;
	unsigned int count = 0;
};

struct schedule my_schedule[100][13][32][100]; //년 월 일 일정개수 (년은 + 2000)
int schedule_count[100][13][32] = { 0, }; //날짜 별 스케줄 갯수(년/월/일)

FILE* file = NULL; //일정 불러오기용 파일 스트림.


// CProjectView

IMPLEMENT_DYNCREATE(CProjectView, CFormView)

BEGIN_MESSAGE_MAP(CProjectView, CFormView)
//	ON_BN_CLICKED(IDC_DELETE, &CProjectView::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_ADD, &CProjectView::OnBnClickedAdd)
//	ON_LBN_SELCHANGE(IDC_LIST2, &CProjectView::OnLbnSelchangeList2)
//	ON_BN_CLICKED(IDC_EDIT, &CProjectView::OnBnClickedEdit)
ON_CBN_SELCHANGE(IDC_COMBOSTART, &CProjectView::OnCbnSelchangeCombostart)
ON_BN_CLICKED(IDC_DELETE, &CProjectView::OnBnClickedDelete)
ON_CBN_SELCHANGE(IDC_COMBOEND, &CProjectView::OnCbnSelchangeComboend)
//ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, &CProjectView::OnDtnDatetimechangeDatetimepicker1)
ON_NOTIFY(MCN_SELECT, IDC_MONTHCALENDAR1, &CProjectView::OnMcnSelectMonthcalendar1)
ON_BN_CLICKED(IDC_EDITSCHED, &CProjectView::OnBnClickedEditsched)
ON_BN_CLICKED(IDC_SAVE, &CProjectView::OnBnClickedSave)
END_MESSAGE_MAP()

// CProjectView 생성/소멸

CProjectView::CProjectView() noexcept
	: CFormView(IDD_PROJECT_FORM)
{
	

}

CProjectView::~CProjectView()
{
}

void CProjectView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MONTHCALENDAR1, m_timeMC);
	//DDX_Control(pDX, IDC_DATETIMEPICKER1, m_timeTP);
	//DDX_Control(pDX, IDC_LIST2, m_listBox);
	DDX_Control(pDX, IDC_EDIT1, m_editbox);
	//DDX_Control(pDX, IDC_EDIT2, m_static);
	DDX_Control(pDX, IDC_LIST1, m_listview);
	DDX_Control(pDX, IDC_COMBOSTART, m_start);
	DDX_Control(pDX, IDC_COMBOEND, m_end);
	DDX_Control(pDX, IDC_EDIT3, m_location);
	DDX_Control(pDX, IDC_EDIT4, m_memo);
	DDX_Control(pDX, IDC_DELETE, m_delete);
	DDX_Control(pDX, IDC_ADD, m_date);
	DDX_Control(pDX, IDC_DATE, m_datetext);
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
	
	//달력 설정.
	CMonthCalCtrl* m_month_test = (CMonthCalCtrl*)GetDlgItem(IDC_MONTHCALENDAR1);
	m_month_test->SetColor(MCSC_BACKGROUND, RGB(243, 221, 147));

	//리스트뷰 스타일 설정
	m_listview.SetExtendedStyle(LVS_EX_ONECLICKACTIVATE | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_listview.SetBkColor(RGB(243, 221, 147));
	m_listview.SetOutlineColor(RGB(249, 174, 130));
	m_listview.SetTextBkColor(RGB(248, 157, 128));
	//m_listview.SetTextColor(RGB(199, 254, 219));


	//리스트뷰 컬럼추가
	m_listview.InsertColumn(0, L"일정", LVCFMT_CENTER, 250, -1);
	m_listview.InsertColumn(1, L"시작 시간", LVCFMT_LEFT, 115, -1);
	m_listview.InsertColumn(2, L"종료 시간", LVCFMT_LEFT, 115, -1);
	m_listview.InsertColumn(3, L"장소", LVCFMT_LEFT, 120, -1);
	m_listview.InsertColumn(4, L"메모", LVCFMT_LEFT, 120, -1);


	//날짜 초기화
	CTime currentTime = CTime::GetCurrentTime();
	CString strMsg;
	strMsg.Format(_T("%u년 %d월 %d일\n"), currentTime.GetYear(), currentTime.GetMonth(), currentTime.GetDay());
	m_datetext.SetWindowText(strMsg);

	//파일 불러오기

	CString tmp;
	WCHAR* ptr1, * next;
	WCHAR tmp_buf[1000];
	wchar_t buf[1000];
	int i = 0;

	unsigned int year, month, day;
	CString schedule, start_time, end_time, location, memo;
	int* count = NULL;

	if (0 == fopen_s(&file, "myschedule.txt", "r+,ccs=UTF-8")) {
		while (fgetws(buf, 1000, file)) { //한 줄 읽음.
			ptr1 = wcstok_s(buf, L"|", &next); //년
			tmp = (LPCTSTR)ptr1;
			year = _ttoi(tmp); //CString to int

			ptr1 = wcstok_s(next, L"|", &next); // 월
			tmp = (LPCTSTR)ptr1;
			month = _ttoi(tmp);

			ptr1 = wcstok_s(next, L"|", &next); //일
			tmp = (LPCTSTR)ptr1;
			day = _ttoi(tmp);

			ptr1 = wcstok_s(next, L"|", &next); //일정
			tmp = (LPCTSTR)ptr1;
			schedule = tmp;
			
			ptr1 = wcstok_s(next, L"|", &next); //시작시간
			tmp = (LPCTSTR)ptr1;
			start_time = tmp;

			ptr1 = wcstok_s(next, L"|", &next); //종료시간
			tmp = (LPCTSTR)ptr1;
			end_time = tmp;

			ptr1 = wcstok_s(next, L"|", &next); //장소
			tmp = (LPCTSTR)ptr1;
			location = tmp;

			ptr1 = wcstok_s(next, L"|", &next); //메모
			tmp = (LPCTSTR)ptr1;
			memo = tmp;

			while (ptr1 != NULL) {
				ptr1 = wcstok_s(NULL, L"|", &next);
				ptr1 = wcstok_s(NULL, L"\n", &next);
			}
			if (year >= 2000) {
				year -= 2000;
				count = &schedule_count[year][month][day];
				//구조체에 일정 추가.
				my_schedule[year][month][day][*count].schedule = schedule;
				my_schedule[year][month][day][*count].start_time = start_time;
				my_schedule[year][month][day][*count].end_time = end_time;
				my_schedule[year][month][day][*count].location = location;
				my_schedule[year][month][day][*count].memo = memo;
				(*count)++;
			}
		} //while
	} //if

	//리스트 초기화
	year = currentTime.GetYear();
	month = currentTime.GetMonth();
	day = currentTime.GetDay();

	strMsg.Format(_T("%u년 %d월 %d일\n"), year, month, day);

	year -= 2000;
	int* count_now = &schedule_count[year][month][day]; //현재 날짜의 일정 개수

	//리스트 업데이트.
	m_listview.DeleteAllItems();
	for (int i = 0; i < *count_now; i++) {
		m_listview.InsertItem(i, my_schedule[year][month][day][i].schedule);
		m_listview.SetItem(i, 1, LVIF_TEXT, my_schedule[year][month][day][i].start_time, 0, 0, 0, NULL);
		m_listview.SetItem(i, 2, LVIF_TEXT, my_schedule[year][month][day][i].end_time, 0, 0, 0, NULL);
		m_listview.SetItem(i, 3, LVIF_TEXT, my_schedule[year][month][day][i].location, 0, 0, 0, NULL);
		m_listview.SetItem(i, 4, LVIF_TEXT, my_schedule[year][month][day][i].memo, 0, 0, 0, NULL);
	}

	fclose(file);

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

//일정 추가 버튼
void CProjectView::OnBnClickedAdd()
{
	CString start_time, end_time, sched, location, memo;

	//입력 데이터 받아오기
	m_start.GetWindowText(start_time);
	m_end.GetWindowText(end_time);
	m_editbox.GetWindowText(sched);
	m_location.GetWindowText(location);
	m_memo.GetWindowText(memo);
	
	//달력 선택 날자 받아옴.
	CTime currentTime;
	CString strMsg;
	CMonthCalCtrl* pCalendar = (CMonthCalCtrl*)GetDlgItem(IDC_MONTHCALENDAR1);
	pCalendar->GetCurSel(currentTime);
	unsigned int year = currentTime.GetYear();
	unsigned int month = currentTime.GetMonth();
	unsigned int day = currentTime.GetDay();
	
	strMsg.Format(_T("%u년 %d월 %d일\n"), year, month, day);

	year -= 2000;
	int* count = &schedule_count[year][month][day];

	//구조체에 일정 추가.
	my_schedule[year][month][day][*count].date = strMsg;
	my_schedule[year][month][day][*count].schedule = sched;
	my_schedule[year][month][day][*count].start_time = start_time;
	my_schedule[year][month][day][*count].end_time = end_time;
	my_schedule[year][month][day][*count].location = location;
	my_schedule[year][month][day][*count].memo = memo;
	(*count)++;

	//리스트 업데이트.
	m_listview.DeleteAllItems();
	for (int i = 0; i < *count; i++) {
		m_listview.InsertItem(i, my_schedule[year][month][day][i].schedule);
		m_listview.SetItem(i, 1, LVIF_TEXT, my_schedule[year][month][day][i].start_time, 0, 0, 0, NULL);
		m_listview.SetItem(i, 2, LVIF_TEXT, my_schedule[year][month][day][i].end_time, 0, 0, 0, NULL);
		m_listview.SetItem(i, 3, LVIF_TEXT, my_schedule[year][month][day][i].location, 0, 0, 0, NULL);
		m_listview.SetItem(i, 4, LVIF_TEXT, my_schedule[year][month][day][i].memo, 0, 0, 0, NULL);
	}

	m_editbox.SetWindowText(_T(""));
	m_location.SetWindowText(_T(""));
	m_memo.SetWindowText(_T(""));
}


void CProjectView::OnCbnSelchangeCombostart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

//일정 삭제 버튼
void CProjectView::OnBnClickedDelete()
{

	//선택한 일정 삭제
	int selected_index = m_listview.GetSelectionMark();

	//달력 선택 날자 받아옴.
	CTime currentTime;
	CString strMsg;
	CMonthCalCtrl* pCalendar = (CMonthCalCtrl*)GetDlgItem(IDC_MONTHCALENDAR1);
	pCalendar->GetCurSel(currentTime);
	unsigned int year = currentTime.GetYear();
	unsigned int month = currentTime.GetMonth();
	unsigned int day = currentTime.GetDay();

	strMsg.Format(_T("%u년 %d월 %d일\n"), year, month, day);

	year -= 2000;
	int* count = &schedule_count[year][month][day]; //현재 날짜의 일정 개수

	for (int j = selected_index; j < (*count) - 1; j++) {
		my_schedule[year][month][day][j] = my_schedule[year][month][day][j + 1];
	}
	(*count)--;

	//리스트 업데이트.
	m_listview.DeleteAllItems();
	for (int i = 0; i < *count; i++) {
		m_listview.InsertItem(i, my_schedule[year][month][day][i].schedule);
		m_listview.SetItem(i, 1, LVIF_TEXT, my_schedule[year][month][day][i].start_time, 0, 0, 0, NULL);
		m_listview.SetItem(i, 2, LVIF_TEXT, my_schedule[year][month][day][i].end_time, 0, 0, 0, NULL);
		m_listview.SetItem(i, 3, LVIF_TEXT, my_schedule[year][month][day][i].location, 0, 0, 0, NULL);
		m_listview.SetItem(i, 4, LVIF_TEXT, my_schedule[year][month][day][i].memo, 0, 0, 0, NULL);
	}

}


void CProjectView::OnCbnSelchangeComboend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CProjectView::OnDtnDatetimechangeDatetimepicker1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);

	*pResult = 0;
}

//달력에서 날짜 선택.
void CProjectView::OnMcnSelectMonthcalendar1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMSELCHANGE pSelChange = reinterpret_cast<LPNMSELCHANGE>(pNMHDR);

	//달력 선택 날자 받아옴.
	CTime currentTime;
	CString strMsg;
	CMonthCalCtrl* pCalendar = (CMonthCalCtrl*)GetDlgItem(IDC_MONTHCALENDAR1);
	pCalendar->GetCurSel(currentTime);
	unsigned int year = currentTime.GetYear();
	unsigned int month = currentTime.GetMonth();
	unsigned int day = currentTime.GetDay();

	strMsg.Format(_T("%u년 %d월 %d일\n"), year, month, day);
	m_datetext.SetWindowText(strMsg);

	year -= 2000;
	int* count = &schedule_count[year][month][day];

	//리스트 업데이트.
	m_listview.DeleteAllItems();
	for (int i = 0; i < *count; i++) {
		m_listview.InsertItem(i, my_schedule[year][month][day][i].schedule);
		m_listview.SetItem(i, 1, LVIF_TEXT, my_schedule[year][month][day][i].start_time, 0, 0, 0, NULL);
		m_listview.SetItem(i, 2, LVIF_TEXT, my_schedule[year][month][day][i].end_time, 0, 0, 0, NULL);
		m_listview.SetItem(i, 3, LVIF_TEXT, my_schedule[year][month][day][i].location, 0, 0, 0, NULL);
		m_listview.SetItem(i, 4, LVIF_TEXT, my_schedule[year][month][day][i].memo, 0, 0, 0, NULL);
	}

	*pResult = 0;
}

//일정 수정 버튼
void CProjectView::OnBnClickedEditsched()
{
	//현재 선택한 일정
	int selected_index = m_listview.GetSelectionMark();

	CString start_time, end_time, sched, location, memo;

	//입력 데이터 받아오기
	m_start.GetWindowText(start_time);
	m_end.GetWindowText(end_time);
	m_editbox.GetWindowText(sched);
	m_location.GetWindowText(location);
	m_memo.GetWindowText(memo);

	//달력 선택 날자 받아옴.
	CTime currentTime;
	CString strMsg;
	CMonthCalCtrl* pCalendar = (CMonthCalCtrl*)GetDlgItem(IDC_MONTHCALENDAR1);
	pCalendar->GetCurSel(currentTime);
	unsigned int year = currentTime.GetYear();
	unsigned int month = currentTime.GetMonth();
	unsigned int day = currentTime.GetDay();

	strMsg.Format(_T("%u년 %d월 %d일\n"), year, month, day);

	year -= 2000;

	//구조체 일정 수정.
	my_schedule[year][month][day][selected_index].date = strMsg;
	my_schedule[year][month][day][selected_index].schedule = sched;
	my_schedule[year][month][day][selected_index].start_time = start_time;
	my_schedule[year][month][day][selected_index].end_time = end_time;
	my_schedule[year][month][day][selected_index].location = location;
	my_schedule[year][month][day][selected_index].memo = memo;


	int* count = &schedule_count[year][month][day];

	//리스트 업데이트.
	m_listview.DeleteAllItems();
	for (int i = 0; i < *count; i++) {
		m_listview.InsertItem(i, my_schedule[year][month][day][i].schedule);
		m_listview.SetItem(i, 1, LVIF_TEXT, my_schedule[year][month][day][i].start_time, 0, 0, 0, NULL);
		m_listview.SetItem(i, 2, LVIF_TEXT, my_schedule[year][month][day][i].end_time, 0, 0, 0, NULL);
		m_listview.SetItem(i, 3, LVIF_TEXT, my_schedule[year][month][day][i].location, 0, 0, 0, NULL);
		m_listview.SetItem(i, 4, LVIF_TEXT, my_schedule[year][month][day][i].memo, 0, 0, 0, NULL);
	}
}


void CProjectView::OnBnClickedSave()
{
	FILE* file2 = NULL; //파일 저장용 파일 스트림.
	if (0 == fopen_s(&file2, "myschedule.txt", "w+,ccs=UTF-8")) {
		CString str, tmp;
		CString str_year, str_month, str_day;
		wchar_t* wcs;
		str = "";
		for (int i = 0; i < 100; i++) { //2000년~2099년
			for (int j = 1; j < 13; j++) { //1월~12월
				for (int k = 1; k < 32; k++) { //1일~31일
					for (int l = 0; l < schedule_count[i][j][k]; l++) {//count가 0이면 pass.

						str_year.Format(_T("%d"), i + 2000);//년
						str += str_year;
						str += "|";
						str_month.Format(_T("%d"), j); //월
						str += str_month;
						str += "|";
						str_day.Format(_T("%d"), k); //dlf
						str += str_day;
						str += "|";

						str += my_schedule[i][j][k][l].schedule;
						str += "|";
						str += my_schedule[i][j][k][l].start_time;
						str += "|";
						str += my_schedule[i][j][k][l].end_time;
						str += "|";
						str += my_schedule[i][j][k][l].location;
						str += "|";
						str += my_schedule[i][j][k][l].memo;
						str += "\n";

						wcs = str.GetBuffer();
						fputws(wcs, file);
						str = "";
						//str을 파일 버퍼에 넣었다가 파일에 추가.???
					}
				}
			}
		}
	}

	//CString str, tmp;
	//CString str_year, str_month, str_day;
	//wchar_t* wcs;
	//str = "";
	//for (int i = 0; i < 100; i++) { //2000년~2099년
	//	for (int j = 1; j < 13; j++) { //1월~12월
	//		for (int k = 1; k < 32; k++) { //1일~31일
	//			for (int l = 0; l < schedule_count[i][j][k]; l++) {//count가 0이면 pass.

	//				str_year.Format(_T("%d"), i+2000);//년
	//				str += str_year;
	//				str += "|";
	//				str_month.Format(_T("%d"), j); //월
	//				str += str_month;
	//				str += "|";
	//				str_day.Format(_T("%d"), k); //dlf
	//				str += str_day;
	//				str += "|";

	//				str += my_schedule[i][j][k][l].schedule;
	//				str += "|";
	//				str += my_schedule[i][j][k][l].start_time;
	//				str += "|";
	//				str += my_schedule[i][j][k][l].end_time;
	//				str += "|";
	//				str += my_schedule[i][j][k][l].location;
	//				str += "|";
	//				str += my_schedule[i][j][k][l].memo;
	//				str += "\n";

	//				wcs = str.GetBuffer();
	//				fputws(wcs, file);
	//				str = "";
	//				//str을 파일 버퍼에 넣었다가 파일에 추가.???
	//			}
	//		}
	//	}
	//}
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
