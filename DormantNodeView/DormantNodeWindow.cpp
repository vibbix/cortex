// DormantNodeWindow.cpp
// e.moon 2jun99

#include "DormantNodeWindow.h"
// DormantNodeView
#include "DormantNodeView.h"

#include "RouteWindow.h"

// Application Kit
#include <Application.h>
// Interface Kit
#include <ScrollBar.h>

__USE_CORTEX_NAMESPACE

#include <Debug.h>
#define D_METHOD(x) //PRINT (x)

// -------------------------------------------------------- //
// constants
// -------------------------------------------------------- //

// this should be a bit more sophisticated :)
const BRect DormantNodeWindow::s_initFrame(500.0, 350.0, 640.0, 480.0);

// -------------------------------------------------------- //
// ctor/dtor
// -------------------------------------------------------- //

DormantNodeWindow::DormantNodeWindow(BWindow* parent) :
	BWindow(s_initFrame, "Media Add-Ons",
		B_FLOATING_WINDOW_LOOK,
		B_FLOATING_SUBSET_WINDOW_FEEL,
		B_WILL_ACCEPT_FIRST_CLICK|B_AVOID_FOCUS|B_ASYNCHRONOUS_CONTROLS),
	m_parent(parent)
{
	D_METHOD(("DormantNodeWindow::DormantNodeWindow()\n"));
	ASSERT(m_parent);
	AddToSubset(m_parent);

	// Create the ListView
	BRect r = Bounds();
	r.right -= B_V_SCROLL_BAR_WIDTH;
	m_listView = new DormantNodeView(r, "Dormant Node ListView", B_FOLLOW_ALL_SIDES);

	// Add the vertical ScrollBar
	r.left = r.right + 1.0;
	r.right = r.left + B_V_SCROLL_BAR_WIDTH;
	r.InsetBy(0.0, -1.0);
	BScrollBar *scrollBar;
	AddChild(scrollBar = new BScrollBar(r, "", m_listView, 0.0, 0.0, B_VERTICAL));

	// Add the ListView
	AddChild(m_listView);

	//Show();
}

DormantNodeWindow::~DormantNodeWindow()
{
	D_METHOD(("DormantNodeWindow::~DormantNodeWindow()\n"));
}

// -------------------------------------------------------- //
// BHandler impl
// -------------------------------------------------------- //

void DormantNodeWindow::MessageReceived(
	BMessage *message)
{
	D_METHOD(("DormantNodeWindow::MessageReceived()\n"));
	switch (message->what)
	{
		default:
		{
			_inherited::MessageReceived(message);
		}
	}
}
	
// -------------------------------------------------------- //
// BWindow impl
// -------------------------------------------------------- //

bool DormantNodeWindow::QuitRequested()
{
	
	D_METHOD(("DormantNodeWindow::QuitRequested()\n"));

	// [e.moon 29nov99] the RouteWindow is now responsible for
	// closing me

	m_parent->PostMessage(RouteWindow::M_TOGGLE_DORMANT_NODE_WINDOW);	
	return false;
	//be_app->PostMessage(M_DORMANT_NODE_WINDOW_CLOSED);
	//return true;
}

// END -- DormantNodeWindow.cpp --