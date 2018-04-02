#include "GList.h"
#include "../Common.h"

GListVerticalScrollBar::GListVerticalScrollBar() {
	range_ = 0;
	pos_ = 0;
	scrolling_ = false;
}

GListVerticalScrollBar::~GListVerticalScrollBar() {
	 
}


bool GListVerticalScrollBar::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	//GLog("%d %d %d %d", m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);

	//CRenderEngine::DrawImage(hDC, m_pManager, m_rcItem, m_rcPaint, m_diNormal);
	CControlUI::PaintBkColor(hDC);
	RECT rc = this->GetPos();
	if (range_ > 0) {

		int thumbHeight = this->GetThumbHeight();
		float p = (float)pos_ / (float)range_;
		int scrollTop = p * (this->GetHeight()- thumbHeight);
		
		DWORD dwPenColor = scrolling_ ? 0x7faaaaaa : 0x7fcccccc;

		CRenderEngine::DrawRoundRect(hDC, { rc.left,rc.top+ scrollTop,rc.right,rc.top + thumbHeight + scrollTop }, this->GetWidth(), this->GetWidth(), this->GetWidth(), dwPenColor);
	}

	
	return false;
	
}

void GListVerticalScrollBar::setListView(GList *view) {
	listView_ = view;
}

int GListVerticalScrollBar::GetThumbHeight() {

	float p = (float)this->GetHeight() / ((float)range_ + (float)this->GetHeight());
	int thumbHeight = p * this->GetHeight();
	if (thumbHeight < 20) {
		thumbHeight = 20;
	}
	return thumbHeight;
}

void GListVerticalScrollBar::SetScrollRange(int range) {
	range_ = range;
}

void GListVerticalScrollBar::SetScrollPos(int pos) {
	
	if (!scrolling_) {
		pos_ = pos;
		listView_->Invalidate();
	}
	
}

void GListVerticalScrollBar::DoEvent(TEventUI& event) {
	
	if (event.Type == UIEVENT_MOUSEMOVE) {
		
		if (scrolling_) {
			POINT point = { GET_X_LPARAM(event.lParam), GET_Y_LPARAM(event.lParam) };
			RECT rc = this->GetPos();
			int scrollBarY = point.y - rc.bottom + this->GetHeight();
			int offset = scrollBarY - downPointY_;

			int thumbHeight = this->GetThumbHeight();
			int marginTop = downMarginTop_ + offset;
			if (marginTop < 0) {
				marginTop = 0;
			}
			else if (marginTop >(this->GetHeight() - thumbHeight)) {
				marginTop = this->GetHeight() - thumbHeight;
			}

			float p = (float)marginTop / (float)(this->GetHeight() - thumbHeight);

			if (p*range_ - pos_ > 2 | p*range_ - pos_<-2) {
				pos_ = p*range_;
				//GLog("UIEVENT_BUTTONDOWN:%d,%d\n", downMarginTop_, offset);
				listView_->scrollTo(0, pos_);
				listView_->Invalidate();
			}
			
		}
	}
	else if (event.Type == UIEVENT_BUTTONDOWN) {
		POINT point = { GET_X_LPARAM(event.lParam), GET_Y_LPARAM(event.lParam) };
		RECT rc = this->GetPos();
		int scrollBarX = point.x - rc.right + this->GetWidth();
		int scrollBarY = point.y - rc.bottom + this->GetHeight();

		int thumbHeight = this->GetThumbHeight();
		float p = (float)pos_ / (float)range_;
		int scrollTop = p * (this->GetHeight() - thumbHeight);

		if (scrollBarX >= 0 && scrollBarX <= this->GetWidth() && scrollBarY >= scrollTop&&scrollBarY <= (scrollTop + thumbHeight)) {
			scrolling_ = true;
			downPointY_ = scrollBarY;
			downMarginTop_ = scrollTop;
			//GLog("UIEVENT_BUTTONDOWN:%d %d\n", scrollBarX, scrollBarY);
			listView_->Invalidate();
		}
		
	}
	else if (event.Type == UIEVENT_BUTTONUP) {
		downPointY_ = 0;
		scrolling_ = false;
		listView_->Invalidate();
	}
}


GList::GList() {
	pDataCallback_ = NULL;
	scrollY_ = 0;
	contentSize_ = 0;
	firstVisibleItemIndex_ = 0;
	lastVisibleItemIndex_ = 0;
	inited_ = false;
}

GList::~GList() {

}


void GList::SetDataCallback(IListDataCallback *pCallback) {
	pDataCallback_ = pCallback;
}


void GList::SetPos(RECT rc, bool bNeedInvalidate) {
	CControlUI::SetPos(rc, bNeedInvalidate);
	if (!inited_) {
		inited_ = true;
		pVerticalScrollBar_ = new GListVerticalScrollBar();
		pVerticalScrollBar_->setListView(this);
		//pVerticalScrollBar_->SetBkColor(0xfff23232);

		RECT rc = this->GetPos();
		pVerticalScrollBar_->SetPos({
			rc.left + this->GetWidth() - 10,
			rc.top,
			rc.left + this->GetWidth(),
			rc.bottom
		});
		
		//this->GetHorizontalScrollBar()->SetVisible(true);
		
		this->ReloadData();
	}
	else {
		if (windowWidth_ != this->GetWidth() || windowHeight_ != this->GetHeight()) {
			this->ReloadData();
			RECT rc = this->GetPos();
			pVerticalScrollBar_->SetPos({
				rc.left + this->GetWidth() - 10,
				rc.top,
				rc.left + this->GetWidth(),
				rc.bottom
			});
		}
	}

	pVerticalScrollBar_->SetScrollRange(contentSize_-this->GetHeight());
	

	//
}

void GList::ReloadData() {

	if (NULL == pDataCallback_) {
		return;
	}


	int count = pDataCallback_->GetCount(this);
	contentSize_ = 0;
	for (int i = 0; i < count; i++) {
		contentSize_ += pDataCallback_->GetItemHeight(this, i);
		if (i != count - 1) {
			contentSize_ += pDataCallback_->GetItemSeparatorHeight(this,i);
		}
	}

	
	if (contentSize_ <= this->GetHeight()) {
		firstVisibleItemIndex_ = 0;
		lastVisibleItemIndex_ = count - 1;
	}else {
		int topSize = 0;
		for (int i = 0; i < count; i++) {
			int itemHeight = pDataCallback_->GetItemHeight(this, i);
			int itemSeparatorHeight = pDataCallback_->GetItemSeparatorHeight(this, i);
			topSize += itemHeight;
			topSize += itemSeparatorHeight;

			if (topSize >= scrollY_) {
				firstVisibleItemIndex_ = i;
				break;
			}
		}


		topSize = 0;
		for (int i = 0; i < firstVisibleItemIndex_; i++) {
			topSize += pDataCallback_->GetItemHeight(this, i);
			topSize += pDataCallback_->GetItemSeparatorHeight(this, i);
		}
		topSize = topSize-scrollY_;

		for (int i = firstVisibleItemIndex_; i < pDataCallback_->GetCount(this); i++) {
			int itemHeight = pDataCallback_->GetItemHeight(this, i);
			topSize += itemHeight;
			

			if (topSize >= this->GetHeight()) {
				lastVisibleItemIndex_ = i;
				break;
			}

			topSize += pDataCallback_->GetItemSeparatorHeight(this, i);
		}
		
	}


	this->RemoveAll();
	RECT rc = this->GetPos();
	int marginTop = rc.top;
	for (int i = 0; i < firstVisibleItemIndex_; i++) {
		marginTop += pDataCallback_->GetItemHeight(this, i);
		marginTop += pDataCallback_->GetItemSeparatorHeight(this, i);
	}
	marginTop = marginTop - scrollY_;

	for (int i = firstVisibleItemIndex_; i <= lastVisibleItemIndex_; i++) {
		CControlUI *item = pDataCallback_->GetItemView(this, i);
		this->Add(item);

		int itemHeight = pDataCallback_ == NULL ? 0 : pDataCallback_->GetItemHeight(this, i);
		int itemSeparatorHeight = pDataCallback_ == NULL ? 0 : pDataCallback_->GetItemSeparatorHeight(this, i);

		item->SetPos({
			rc.left,
			marginTop,
			rc.left + this->GetWidth(),
			marginTop + itemHeight
		});
		marginTop += itemHeight;
		marginTop += itemSeparatorHeight;
	}
	
	

	windowWidth_ = this->GetWidth();
	windowHeight_ = this->GetHeight();

	if (contentSize_ > this->GetHeight()) {

		if (lastVisibleItemIndex_ == pDataCallback_->GetCount(this) - 1) {
			CControlUI *item = pDataCallback_->GetItemView(this, lastVisibleItemIndex_);
			RECT lastVisibleItemRc = item->GetRelativePos();
			if (lastVisibleItemRc.bottom < this->GetHeight()) {
				scroll(0, lastVisibleItemRc.bottom - this->GetHeight());
			}
		}
		
	}

	pVerticalScrollBar_->SetScrollPos(scrollY_);
}


//�¼�����
void GList::DoEvent(TEventUI& event) {

	if (event.Type == UIEVENT_SCROLLWHEEL) {
		//GLog("UIEVENT_SCROLLWHEEL %d\n", event.wParam);
		if (event.wParam == 1) {

			//GLog("%d %d %d\n", this->getContentSize(), this->GetHeight(), scrollY_);
			if (this->getContentSize() > this->GetHeight()
				&& scrollY_ < (this->getContentSize() - this->GetHeight())) {
				scroll(0, -30);
				Invalidate();
			}
		}
		else {
			if (scrollY_ <= 0) {
				this->scrollTo(0, 0);
			}
			else {
				this->scroll(0, 30);
				Invalidate();
			}

		}
	}

	

	pVerticalScrollBar_->DoEvent(event);
	//GLog("event:%d\n", event.Type);
	//CContainerUI::DoEvent(event);
}

void GList::RemoveAll() {
	for (int it = 0;it < items_.GetSize(); it++) {
		static_cast<CControlUI*>(items_[it])->Delete();
	}

	items_.Empty();
}

void GList::Add(CControlUI *item) {
	if (m_pManager != NULL) m_pManager->InitControls(item, this);
	items_.Add(item);
}

void GList::scrollTo(int cx, int cy) {
	scrollY_ = cy;


	int topSize = 0;
	for (int i = 0; i < pDataCallback_->GetCount(this); i++) {
		int itemHeight = pDataCallback_->GetItemHeight(this, i);
		int itemSeparatorHeight = pDataCallback_->GetItemSeparatorHeight(this, i);
		topSize += itemHeight;
		topSize += itemSeparatorHeight;

		if (topSize >= scrollY_) {
			firstVisibleItemIndex_ = i;
			topSize -= itemHeight;
			topSize -= itemSeparatorHeight;
			break;
		}
	}



	int topSize2 = topSize - scrollY_;

	for (int i = firstVisibleItemIndex_; i < pDataCallback_->GetCount(this); i++) {
		int itemHeight = pDataCallback_->GetItemHeight(this, i);
		topSize2 += itemHeight;

		if (topSize2 >= this->GetHeight()) {
			lastVisibleItemIndex_ = i;
			break;
		}

		topSize2 += pDataCallback_->GetItemSeparatorHeight(this, i);
	}


	this->RemoveAll();

	RECT rc = this->GetPos();
	int marginTop = rc.top + topSize - scrollY_;
	for (int i = firstVisibleItemIndex_; i <= lastVisibleItemIndex_; i++) {
		CControlUI *item = pDataCallback_->GetItemView(this, i);
		this->Add(item);

		int itemHeight = pDataCallback_ == NULL ? 0 : pDataCallback_->GetItemHeight(this, i);
		int itemSeparatorHeight = pDataCallback_ == NULL ? 0 : pDataCallback_->GetItemSeparatorHeight(this, i);

		item->SetPos({
			rc.left,
			marginTop,
			rc.left + this->GetWidth(),
			marginTop + itemHeight
		});
		marginTop += itemHeight;
		marginTop += itemSeparatorHeight;
	}

	pVerticalScrollBar_->SetScrollPos(scrollY_);
}

void GList::scroll(int offsetX, int offsetY) {
	if (NULL == pDataCallback_) {
		return;
	}

	if ((scrollY_ - offsetY) > (this->getContentSize() - this->GetHeight())) {
		offsetY = scrollY_ + this->GetHeight() - this->getContentSize();
	}

	if (scrollY_ - offsetY < 0) {
		offsetY = scrollY_;
	}
	


	scrollY_ -= offsetY;


	int topSize = 0;
	for (int i = 0; i < pDataCallback_->GetCount(this); i++) {
		int itemHeight = pDataCallback_->GetItemHeight(this, i);
		int itemSeparatorHeight = pDataCallback_->GetItemSeparatorHeight(this, i);
		topSize += itemHeight;
		topSize += itemSeparatorHeight;

		if (topSize >= scrollY_) {
			firstVisibleItemIndex_ = i;
			topSize -= itemHeight;
			topSize -= itemSeparatorHeight;
			break;
		}
	}



	int topSize2 = topSize - scrollY_;

	for (int i = firstVisibleItemIndex_; i < pDataCallback_->GetCount(this); i++) {
		int itemHeight = pDataCallback_->GetItemHeight(this, i);
		topSize2 += itemHeight;

		if (topSize2 >= this->GetHeight()) {
			lastVisibleItemIndex_ = i;
			break;
		}

		topSize2 += pDataCallback_->GetItemSeparatorHeight(this, i);
	}


	this->RemoveAll();

	RECT rc = this->GetPos();
	int marginTop = rc.top + topSize - scrollY_;
	for (int i = firstVisibleItemIndex_; i <= lastVisibleItemIndex_; i++) {
		CControlUI *item = pDataCallback_->GetItemView(this, i);
		this->Add(item);

		int itemHeight = pDataCallback_ == NULL ? 0 : pDataCallback_->GetItemHeight(this, i);
		int itemSeparatorHeight = pDataCallback_ == NULL ? 0 : pDataCallback_->GetItemSeparatorHeight(this, i);

		item->SetPos({
			rc.left,
			marginTop,
			rc.left + this->GetWidth(),
			marginTop + itemHeight
		});
		marginTop += itemHeight;
		marginTop += itemSeparatorHeight;
	}

	pVerticalScrollBar_->SetScrollPos(scrollY_);
}

int GList::getContentSize() {
	return contentSize_;
}

bool GList::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	//GLog("DoPaint:%d %d %d %d\n", rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);
	RECT rcTemp = { 0 };
	if (!::IntersectRect(&rcTemp, &rcPaint, &m_rcItem)) return true;

	CRenderClip clip;
	CRenderClip::GenerateClip(hDC, rcTemp, clip);
	bool ret = CControlUI::DoPaint(hDC, rcPaint, pStopControl);

	for (int it = 0; it < items_.GetSize(); it++) {
		CControlUI* pControl = static_cast<CControlUI*>(items_[it]);
		pControl->Paint(hDC, rcPaint, pStopControl);
	}

	pVerticalScrollBar_->Paint(hDC, rcPaint, pStopControl);
	return ret;

}