// DormantNodeWindow.h
// e.moon 2jun99

#ifndef __DormantNodeWindow_H__
#define __DormantNodeWindow_H__

// Interface Kit
#include <Window.h>

#include "cortex_defs.h"
__BEGIN_CORTEX_NAMESPACE

class DormantNodeView;

class DormantNodeWindow :
	public	BWindow {
	typedef	BWindow _inherited;
	
public:					// *** ctor/dtor

						DormantNodeWindow(BWindow* parent);

	virtual				~DormantNodeWindow();

public:					// *** BHandler impl.

	virtual void		MessageReceived(BMessage* pMsg);
	
public:					// *** BWindow impl.

	bool				QuitRequested();

private:				// *** data

	BWindow*			m_parent;
	
	DormantNodeView *m_listView;
	
	static const BRect	s_initFrame;
};

__END_CORTEX_NAMESPACE
#endif /*__DormantNodeWindow_H__*/
