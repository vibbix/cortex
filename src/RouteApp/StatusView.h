// StatusView.h (Cortex/ParameterWindow)
//
// * PURPOSE
//	 a small view to display a short message string with
//   an icon. for error messages, it will play a sound for
//   catching the users attention. messages will decay slowly
//   (become fully transparent). all this to avoid modal alert
//   boxes for error notification.
//
// * TODO
//   reduce flicker while message decay
//
// * HISTORY
//   c.lenz		21may00		Begun
//

#ifndef __StatusView_H__
#define __StatusView_H__

// Interface Kit
#include <StringView.h>
// Support Kit
#include <String.h>

#include "cortex_defs.h"

class BBitmap;

__BEGIN_CORTEX_NAMESPACE

class StatusView :
	public BStringView {
	
public:					// *** ctor/dtor

						StatusView(
							BRect frame,
							BScrollBar *scrollBar = 0);

	virtual				~StatusView();

public:					// *** BScrollView impl.

	virtual void		Draw(
							BRect updateRect);

	virtual void		FrameResized(
							float width,
							float height);

	virtual void		MouseDown(
							BPoint point);

	virtual void		MouseMoved(
							BPoint point,
							uint32 transit,
							const BMessage *message);

	virtual void		MouseUp(
							BPoint point);

	virtual void		Pulse();

public:					// *** operations

	void				setErrorMessage(
							BString text,
							bool error = false);

private:				// *** data members

	// the sibling scrollbar which should be resized by the 
	// status view
	BScrollBar		   *m_scrollBar;

	BBitmap			   *m_icon;

	// from 0.0 to 1.0
	float				m_opacity;

	// untruncated string
	BString				m_fullText;

	// is being resized
	bool				m_dragging;
};

__END_CORTEX_NAMESPACE
#endif /* __StatusView_H__ */
