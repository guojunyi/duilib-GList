# duilib-GList

``` c++
listView_ = new GList();
listView_->SetDataCallback(this);
layout->Add(listView_);


//IListDataCallback
//返回每一行的高度
int GetItemHeight(GList* pList, int iItem) {
	if (iItem == 0) {
		return 120;
	}
	else {
		return 120;
	}
}
//返回每一行分割线的高度
int GetItemSeparatorHeight(GList* pList, int iItem) {
	return 1;
}
//返回有多少个列表
int GetCount(GList* pList) {
	return 1000000;
}
//返回每个列表的view
CControlUI* GetItemView(GList* pList, int iItem) {
	CHorizontalLayoutUI *item = new CHorizontalLayoutUI();
	item->SetBkColor(0xfff0f0f0);

	GButton *button = new GButton();
	button->SetNormalImage(_T("temp.jpg"));
	button->SetPadding({ 10,20,10,20 });
	button->SetFixedWidth(148);
	item->Add(button);

	CLabelUI *label = new CLabelUI();
	char buffer[20];
	memset(buffer, 0, 20);
	sprintf_s(buffer, "%d", iItem);
	label->SetText(CA2W(buffer));
	item->Add(label);
	//item->SetTextColor(0xff000000);
	return item;
}
```
