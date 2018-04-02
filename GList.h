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

	void setListView(GList *view);
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

	void DoEvent(TEventUI& event);

	void SetDataCallback(IListDataCallback *pCallback);

	void ReloadData();

	void Add(CControlUI *item);

	void RemoveAll();

	void scroll(int offsetX, int offsetY);

	void scrollTo(int cx, int cy);
protected:

	bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

	void SetPos(RECT rc, bool bNeedInvalidate = true);

	int getContentSize();

private:
	CDuiPtrArray items_;

	IListDataCallback *pDataCallback_;

	int scrollY_;

	int contentSize_;

	int firstVisibleItemIndex_;

	int lastVisibleItemIndex_;

	bool inited_;

	GListVerticalScrollBar *pVerticalScrollBar_;

	int windowWidth_;

	int windowHeight_;
};


#endif  // GLIST_H__