// InfoWindow.cpp

#include "InfoWindow.h"
// InfoWindow
#include "AppNodeInfoView.h"
#include "ConnectionInfoView.h"
#include "DormantNodeInfoView.h"
#include "EndPointInfoView.h"
#include "FileNodeInfoView.h"
#include "LiveNodeInfoView.h"
// NodeManager
#include "AddOnHostProtocol.h"
#include "NodeRef.h"

// Application Kit
#include <Roster.h>
// Interface Kit
#include <Screen.h>
// Media Kit
#include <MediaRoster.h>

__USE_CORTEX_NAMESPACE

#include <Debug.h>
#define D_METHOD(x) //PRINT (x)

// -------------------------------------------------------- //
// *** static member init
// -------------------------------------------------------- //

const BPoint InfoWindow::M_DEFAULT_OFFSET	= BPoint(20.0, 20.0);
const BPoint InfoWindow::M_INIT_POSITION	= BPoint(20.0, 20.0);

int32 InfoWindow::s_windowCount				= 0;
BPoint InfoWindow::s_lastWindowPosition		= M_INIT_POSITION;

// -------------------------------------------------------- //
// *** ctor/dtor (public)
// -------------------------------------------------------- //

InfoWindow::InfoWindow(
	const NodeRef *ref)
	: BWindow(InfoView::M_DEFAULT_FRAME, "", B_DOCUMENT_WINDOW, 0)
{
	D_METHOD(("InfoWindow::InfoWindow(live_node)\n"));

	BMediaRoster *roster = BMediaRoster::CurrentRoster();
	dormant_node_info dormantNodeInfo;

	if (ref->kind() & B_FILE_INTERFACE)
	{
		AddChild(new FileNodeInfoView(ref));
	}
	else if (roster->GetDormantNodeFor(ref->node(), &dormantNodeInfo) != B_OK) {
		port_info portInfo;
		app_info appInfo;
		if ((get_port_info(ref->node().port, &portInfo) == B_OK)
		 && (be_roster->GetRunningAppInfo(portInfo.team, &appInfo) == B_OK)) {
			app_info thisAppInfo;
			if ((be_app->GetAppInfo(&thisAppInfo) != B_OK)
			 || ((strcmp(appInfo.signature, thisAppInfo.signature) != 0)
			 && (strcmp(appInfo.signature, addon_host::g_appSignature) != 0))) {
				AddChild(new AppNodeInfoView(ref));
			}
			else {
				AddChild(new LiveNodeInfoView(ref));
			}
		}
		else {
			AddChild(new LiveNodeInfoView(ref));
		}
	}
	else {
		AddChild(new LiveNodeInfoView(ref));
	}

	_init();
}

InfoWindow::InfoWindow(
	const dormant_node_info &info)
	: BWindow(InfoView::M_DEFAULT_FRAME, "", B_DOCUMENT_WINDOW, 0)
{
	D_METHOD(("InfoWindow::InfoWindow(dormant_node)\n"));

	AddChild(new DormantNodeInfoView(info));
	_init();
}

InfoWindow::InfoWindow(
	const Connection &connection)
	: BWindow(InfoView::M_DEFAULT_FRAME, "", B_DOCUMENT_WINDOW, 0)
{
	D_METHOD(("InfoWindow::InfoWindow(connection)\n"));

	AddChild(new ConnectionInfoView(connection));
	_init();
}

InfoWindow::InfoWindow(
	const media_input &input)
	: BWindow(InfoView::M_DEFAULT_FRAME, "", B_DOCUMENT_WINDOW, 0)
{
	D_METHOD(("InfoWindow::InfoWindow(input)\n"));

	AddChild(new EndPointInfoView(input));
	_init();
}

InfoWindow::InfoWindow(
	const media_output &output)
	: BWindow(InfoView::M_DEFAULT_FRAME, "", B_DOCUMENT_WINDOW, 0)
{
	D_METHOD(("InfoWindow::InfoWindow(output)\n"));

	AddChild(new EndPointInfoView(output));
	_init();
}

InfoWindow::~InfoWindow()
{
	D_METHOD(("InfoView::~InfoView()\n"));

	// decrease window count
	s_windowCount--;
	if (s_windowCount == 0)
	{
		s_lastWindowPosition = M_INIT_POSITION;
	}
}

// -------------------------------------------------------- //
// *** internal operations (private)
// -------------------------------------------------------- //

void InfoWindow::_init()
{
	D_METHOD(("InfoWindow::_init()\n"));

	// increase window count
	s_windowCount++;

	// offset to a new position
	BScreen screen(this);
	BPoint windowPosition = s_lastWindowPosition + M_DEFAULT_OFFSET;
	if (!screen.Frame().Contains(windowPosition))
	{
		windowPosition = M_INIT_POSITION + BPoint(0.0, M_DEFAULT_OFFSET.y);
	}
	MoveTo(windowPosition);
	s_lastWindowPosition = windowPosition;
}

// END -- InfoWindow.cpp --
