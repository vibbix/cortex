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
							BRect frame);

	virtual				~StatusView();

public:					// *** BScrollView impl.

	virtual void		Draw(
							BRect updateRect);

	virtual void		Pulse();

public:					// *** operations

	void				setErrorMessage(
							BString text,
							bool error = false);

private:				// *** data members

	BBitmap			   *m_icon;

	// from 0.0 to 1.0
	float				m_opacity;

	// untruncated string
	BString				m_fullText;
};

__END_CORTEX_NAMESPACE
#endif /* __StatusView_H__ */
