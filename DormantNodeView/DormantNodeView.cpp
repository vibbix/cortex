// DormantNodeView.cpp

#include "DormantNodeView.h"
// DormantNodeView
#include "DormantNodeWindow.h"
#include "DormantNodeListItem.h"
// InfoWindow
#include "InfoWindow.h"

// Interface Kit
#include <Deskbar.h>
#include <Region.h>
#include <Screen.h>
#include <ScrollBar.h>
// Media Kit
#include <MediaRoster.h>
// Storage Kit
#include <Mime.h>

__USE_CORTEX_NAMESPACE

#include <Debug.h>
#define D_METHOD(x) //PRINT (x)
#define D_MESSAGE(x) //PRINT (x)

// -------------------------------------------------------- //
// ctor/dtor (public)
// -------------------------------------------------------- //

DormantNodeView::DormantNodeView(
	BRect frame,
	const char *name,
	uint32 resizeMode)
	: BListView(frame, name, B_SINGLE_SELECTION_LIST, resizeMode),
	  m_lastItemUnder(0)
{
	D_METHOD(("DormantNodeView::DormantNodeView()\n"));
}

DormantNodeView::~DormantNodeView()
{
	D_METHOD(("DormantNodeView::~DormantNodeView()\n"));
}

// -------------------------------------------------------- //
// BListView impl. (public)
// -------------------------------------------------------- //

void DormantNodeView::AttachedToWindow()
{
	D_METHOD(("DormantNodeView::AttachedToWindow()\n"));

	// init the list if it already contains items
	if (CountItems() > 0)
	{
		DetachedFromWindow();
	}
	
	// init the resizable node-info buffer
	BMediaRoster *roster = BMediaRoster::Roster();
	const int32 bufferInc = 64;
	int32 bufferSize = bufferInc;
	dormant_node_info *infoBuffer = new dormant_node_info[bufferSize];
	int32 numNodes;
	
	// fill the buffer
	while (true)
	{
		numNodes = bufferSize;
		status_t error = roster->GetDormantNodes(infoBuffer, &numNodes);
		if (error)
		{
			return;
		}
		if (numNodes < bufferSize)
		{
			break;
		}
			
		// reallocate buffer & try again
		delete [] infoBuffer;
		bufferSize += bufferInc;
		infoBuffer = new dormant_node_info[bufferSize];
	}
	
	// populate the list
	for (int32 i = 0; i < numNodes; i++)
	{
		DormantNodeListItem *item = new DormantNodeListItem(infoBuffer[i]);
		AddItem(item);
	}
	SortItems(compareName);

	// set the min/max/zoom sizes of the window
	BRect maxRect(0.0, 0.0, 0.0, 0.0);
	BRect minRect(0.0, 0.0, 120.0, 60.0);
	for (int32 i = 0; i < CountItems(); i++)
	{
		DormantNodeListItem *item;
		item = dynamic_cast<DormantNodeListItem *>(ItemAt(i));
		if (item)
		{
			BRect r = item->getRealFrame(be_plain_font);
			if (r.Width() > maxRect.right)
			{
				maxRect.right = r.Width();
			}
			maxRect.bottom += r.Height() + 1.0;
			minRect.bottom = r.Height() + 1.0;
		}
	}
	minRect.right = B_MINI_ICON + 2 * DormantNodeListItem::M_ICON_H_MARGIN;
	maxRect.right += B_V_SCROLL_BAR_WIDTH;
	minRect.right += B_V_SCROLL_BAR_WIDTH;
	Window()->SetZoomLimits(maxRect.Width(), maxRect.Height());
	Window()->SetSizeLimits(minRect.Width(), 4000.0, 
							minRect.Height(), 4000.0);

	_inherited::AttachedToWindow();
}

void DormantNodeView::DetachedFromWindow()
{
	D_METHOD(("DormantNodeView::DetachedFromWindow()\n"));

	// remove and delete all items in the list
	while (CountItems() > 0)
	{
		BListItem *item = ItemAt(0);
		if (RemoveItem(item))
		{
			delete item;
		}
	}
}

void DormantNodeView::MessageReceived(
	BMessage *message)
{
	D_METHOD(("DormantNodeView::MessageReceived()\n"));

	switch (message->what)
	{
		case InfoView::M_INFO_WINDOW_REQUESTED:
		{
			D_MESSAGE(("DormantNodeView::MessageReceived(InfoView::M_INFO_WINDOW_REQUESTED)\n"));
			DormantNodeListItem *item;
			item = dynamic_cast<DormantNodeListItem *>(ItemAt(CurrentSelection()));
			if (item)
			{
				InfoWindow *info = new InfoWindow(item->info());
				info->Show();
			}
			break;
		}
		default:
		{
			_inherited::MessageReceived(message);
		}
	}
}

void DormantNodeView::MouseDown(
	BPoint point)
{
	D_METHOD(("DormantNodeView::MouseDown()\n"));

	BMessage* message = Window()->CurrentMessage();
	int32 buttons = message->FindInt32("buttons");
	if (buttons == B_SECONDARY_MOUSE_BUTTON)
	{
		int32 index;
		if ((index = IndexOf(point)) >= 0)
		{
			DormantNodeListItem *item = dynamic_cast<DormantNodeListItem *>(ItemAt(index));
			if (item)
			{
				Select(index);
				BRect r = item->getRealFrame(be_plain_font);
				if (r.Contains(point))
				{
					item->showContextMenu(point, this);
				}
			}
		}
	}
	else
	{
		_inherited::MouseDown(point);
	}
}

void DormantNodeView::MouseMoved(
	BPoint point,
	uint32 transit,
	const BMessage *message)
{
	D_METHOD(("DormantNodeView::MouseMoved()\n"));
	int32 index;
	if (!message && ((index = IndexOf(point)) >= 0))
	{
		DormantNodeListItem *item = dynamic_cast<DormantNodeListItem *>(ItemAt(index));
		DormantNodeListItem *last = dynamic_cast<DormantNodeListItem *>(m_lastItemUnder);
		BRect r = item->getRealFrame(be_plain_font);
		if (item && r.Contains(point))
		{
			if (item != last)
			{
				if (last)
					last->mouseOver(this, point, B_EXITED_VIEW);
				item->mouseOver(this, point, B_ENTERED_VIEW);
				m_lastItemUnder = item;
			}
			else
			{
				item->mouseOver(this, point, B_INSIDE_VIEW);
			}
		}
		else if (last)
		{
			last->mouseOver(this, point, B_EXITED_VIEW);
		}
	}
}

bool DormantNodeView::InitiateDrag(
	BPoint point,
	int32 index,
	bool wasSelected)
{
	D_METHOD(("DormantNodeView::InitiateDrag()\n"));

	DormantNodeListItem *item = dynamic_cast<DormantNodeListItem *>(ItemAt(CurrentSelection()));
	if (item)
	{
		BMessage dragMsg(M_INSTANTIATE_NODE);
		dragMsg.AddData("which", B_RAW_TYPE,
						reinterpret_cast<const void *>(&item->info()),
						sizeof(item->info()));
		point -= ItemFrame(index).LeftTop();
		DragMessage(&dragMsg, item->getDragBitmap(), B_OP_ALPHA, point);
		return true;
	}
	return false;
}

// END -- DormantNodeView.cpp --
