// InfoWindow.h (Cortex/InfoView)
//
// * PURPOSE
//   A window that chooses the appropriate InfoView implementation
//   by the argument passed into its constructor. It also has quite
//	 minimalistic management for positioning new InfoWindows through
//   the use of static members.
//
// * TODO
//   It is possible to open multiple InfoWindows on the same object
//   right now, which maybe should be avoided ?
//
// * HISTORY
//   c.lenz		5nov99		Begun
//

#ifndef __InfoWindow_H__
#define __InfoWindow_H__

#include "InfoView.h"

#include <Application.h>
#include <Window.h>

#include "cortex_defs.h"
__BEGIN_CORTEX_NAMESPACE

class Connection;
class NodeRef;

class InfoWindow : public BWindow
{

public:					// *** constants

	static const BPoint M_DEFAULT_OFFSET;
	static const BPoint M_INIT_POSITION;

public:					// *** ctor/dtor

	// creates a LiveNodeInfoView or a FileNodeInfoView (if the NodeRef
	// is a B_FILE_INTERFACE)
						InfoWindow(
							const NodeRef *ref);

	// creates a DormantNodeInfoView
						InfoWindow(
							const dormant_node_info &info);

	// creates a ConnectionInfoView
						InfoWindow(
							const Connection &connection);

	// create an EndPointInfoView
						InfoWindow(
							const media_input &input);
						InfoWindow(
							const media_output &output);

	virtual				~InfoWindow();

private:				// *** internal operations

	// is called from all ctors for window-positioning
	void				_init();

private:				// *** static data

	// number of currently open InfoWindows
	static int32		s_windowCount;

	// the BPoint at which the last InfoWindow was initially
	// opened
	static BPoint		s_lastWindowPosition;
};

__END_CORTEX_NAMESPACE
#endif /* __InfoWindow_H__ */
