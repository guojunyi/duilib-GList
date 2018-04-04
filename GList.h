#ifndef GLIST_H__
#define GLIST_H__

#include "../UIlib.h"

using namespace DuiLib;
class GList;
class IListDataCallback
{
public:
	virtual int GetItemHeight(GList* pList, int iItem) = 0;

	virtual int GetItemSeparatorHeight(GList* pList, int iItem) = 0;

	virtual int GetCount(GList* pList) = 0;

	virtual CControlUI* GetItemView(GList* pList, int iItem) = 0;
};


class GListVerticalScrollBar : public CControlUI {
public:
	GListVerticalScrollBar();

	~GListVerticalScrollBar();

	void SetScrollRange(int range);

	void SetScrollPos(int pos);

	void DoEvent(TEventUI& event);

	void SetListView(GList *view);
protected:
	int GetThumbHeight();

	bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

	int range_;

	int pos_;

	bool scrolling_;

	int downPointY_;

	int downMarginTop_;
	
	GList *listView_;
};

class GList : public CControlUI {
public:
	GList();

	~GList();

	void SetDataCallback(IListDataCallback *pCallback);

	void ReloadData();

	void Scroll(int offsetX, int offsetY);

	void ScrollTo(int cx, int cy);
protected:

	bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

	void SetPos(RECT rc, bool bNeedInvalidate = true);

	int getContentSize();

	void DoEvent(TEventUI& event);
private:
	void dispatchEvent(TEventUI &event);

	int CalculateFirstAndLastVisibleItemIndex();

	void Add(CControlUI *item);

	void RemoveAll();

	CDuiPtrArray items_;

	IListDataCallback *pDataCallback_;

	int scrollY_;

	int contentSize_;

	int firstVisibleItemIndex_;

	int lastVisibleItemIndex_;

	bool inited_;

	GListVerticalScrollBar *pVerticalScrollBar_;

	CVerticalLayoutUI *pVerticalLayout_;

	int windowWidth_;

	int windowHeight_;
};


#endif  // GLIST_H__