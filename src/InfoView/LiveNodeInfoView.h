// LiveNodeInfoView.h (Cortex/InfoView)
//
// * PURPOSE
//   Defines fields to be displayed for all live MediaNodes,
//   which can be added to for special nodes like file-readers
//   (see FileNodeInfoView) etc.
//
// * HISTORY
//   c.lenz		5nov99		Begun
//

#ifndef __LiveNodeInfoView_H__
#define __LiveNodeInfoView_H__

#include "InfoView.h"

#include "cortex_defs.h"
__BEGIN_CORTEX_NAMESPACE

class NodeRef;

class LiveNodeInfoView : public InfoView
{

public:					// *** ctor/dtor

	// adds the live-node relevant fields the the
	// InfoView
						LiveNodeInfoView(
							const NodeRef *ref);

	virtual				~LiveNodeInfoView();

public:					// *** BView impl

	// register with the NodeRef
	virtual void		AttachedToWindow();
	
	// stop observing the NodeRef
	virtual void		DetachedFromWindow();

	// closes the window when the node is released
	virtual void		MessageReceived(
							BMessage *message);

private:

	// cached pointer to the NodeRef
	const NodeRef	   *m_ref;
};

__END_CORTEX_NAMESPACE
#endif /* __LiveNodeInfoView_H__ */
