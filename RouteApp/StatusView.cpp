// StatusView.cpp

#include "StatusView.h"
#include "cortex_ui.h"
#include "TipManager.h"

// Application Kit
#include <Message.h>
// Interface Kit
#include <Bitmap.h>
#include <Font.h>
#include <ScrollBar.h>
#include <Window.h>
// Support Kit
#include <Beep.h>

__USE_CORTEX_NAMESPACE

#include <Debug.h>
#define D_ALLOC(x) PRINT (x)
#define D_HOOK(x) PRINT (x)
#define D_OPERATION(x) //PRINT (x)

// -------------------------------------------------------- //
// *** constants
// -------------------------------------------------------- //

const bigtime_t TEXT_DECAY_TIME = 40 * 1000 * 1000;

// width: 8, height:12, color_space: B_CMAP8
const unsigned char ERROR_ICON_BITS [] = {
	0xff,0xff,0x00,0x00,0xff,0xff,0xff,0xff,
	0xff,0x00,0xfa,0xfa,0x00,0xff,0xff,0xff,
	0x00,0x3f,0x3f,0xfa,0xfa,0x00,0xff,0xff,
	0x00,0xf9,0xf9,0x3f,0x5d,0x00,0xff,0xff,
	0x00,0xf9,0xf9,0x5d,0x5d,0x00,0xff,0xff,
	0x00,0xf9,0xf9,0x5d,0x5d,0x00,0xff,0xff,
	0x00,0x00,0xf9,0x5d,0x00,0x00,0xff,0xff,
	0xff,0x00,0x00,0x00,0x00,0xff,0xff,0xff,
	0x00,0xf9,0x00,0x00,0x5d,0x00,0xff,0xff,
	0x00,0xf9,0xf9,0x5d,0x5d,0x00,0xff,0xff,
	0x00,0x00,0xf9,0x5d,0x00,0x00,0x0f,0xff,
	0xff,0xff,0x00,0x00,0x00,0x0f,0x0f,0x0f,
};

// width: 8, height:12, color_space: B_CMAP8
const unsigned char INFO_ICON_BITS [] = {
	0xff,0xff,0x00,0x00,0x00,0xff,0xff,0xff,
	0xff,0x00,0x21,0x21,0x21,0x00,0xff,0xff,
	0xff,0x00,0x21,0x92,0x25,0x00,0xff,0xff,
	0xff,0x00,0x21,0x25,0x25,0x00,0xff,0xff,
	0xff,0xff,0x00,0x00,0x00,0xff,0xff,0xff,
	0xff,0x00,0x92,0x92,0x25,0x00,0xff,0xff,
	0xff,0x00,0x21,0x21,0x25,0x00,0xff,0xff,
	0xff,0x00,0x21,0x21,0x25,0x00,0xff,0xff,
	0x00,0x00,0x21,0x21,0x25,0x00,0x00,0xff,
	0x00,0xbe,0x21,0x21,0x92,0xbe,0x25,0x00,
	0x00,0x00,0x21,0x21,0x21,0x25,0x00,0x0f,
	0xff,0xff,0x00,0x00,0x00,0x00,0x0f,0x0f,
};

// -------------------------------------------------------- //
// *** ctor/dtor
// -------------------------------------------------------- //

StatusView::StatusView(
	BRect frame,
	BScrollBar *scrollBar)
	:	BStringView(frame, "StatusView", "", B_FOLLOW_LEFT | B_FOLLOW_BOTTOM,
					B_FRAME_EVENTS | B_WILL_DRAW),
		m_scrollBar(scrollBar),
		m_icon(0),
		m_opacity(1.0),
		m_dragging(false) {
	D_ALLOC(("StatusView::StatusView()\n"));

	SetViewColor(B_TRANSPARENT_COLOR);
	SetFont(be_plain_font);
}

StatusView::~StatusView() {
	D_ALLOC(("StatusView::~ParameterContainerView()\n"));

	// get the tip manager instance and reset
	TipManager *manager = TipManager::Instance();
	manager->removeAll(this);
}

// -------------------------------------------------------- //
// *** BScrollView impl
// -------------------------------------------------------- //

void StatusView::Draw(
	BRect updateRect) {
	D_HOOK(("StatusView::Draw()\n"));

	BRect r(Bounds());

	// draw border
	BeginLineArray(8);
	AddLine(r.LeftTop(), r.RightTop(), M_MED_GRAY_COLOR);
	AddLine(r.RightTop(), r.RightBottom(), M_MED_GRAY_COLOR);
	AddLine(r.RightBottom(), r.LeftBottom(), M_MED_GRAY_COLOR);
	AddLine(r.LeftBottom(), r.LeftTop(), M_MED_GRAY_COLOR);
	r.InsetBy(1.0, 1.0);
	AddLine(r.LeftTop(), r.RightTop(), M_LIGHT_GRAY_COLOR);
	AddLine(r.RightTop(), r.RightBottom(), tint_color(M_MED_GRAY_COLOR, B_LIGHTEN_1_TINT));
	AddLine(r.RightBottom(), r.LeftBottom(), tint_color(M_MED_GRAY_COLOR, B_LIGHTEN_1_TINT));
	AddLine(r.LeftBottom(), r.LeftTop(), M_LIGHT_GRAY_COLOR);
	EndLineArray();
	r.InsetBy(1.0, 1.0);
	SetLowColor(M_GRAY_COLOR);
	FillRect(r, B_SOLID_LOW);

	r.InsetBy(2.0, 0.0);
	SetDrawingMode(B_OP_ALPHA);
	SetHighColor(0, 0, 0, 255 * m_opacity);

	// draw icon
	if (m_icon) {
		SetBlendingMode(B_CONSTANT_ALPHA, B_ALPHA_OVERLAY);
		DrawBitmapAsync(m_icon, r.LeftTop());
	}

	// draw text
	r.left += 12.0;
	font_height fh;
	be_plain_font->GetHeight(&fh);
	r.bottom = Bounds().bottom - fh.descent - 1.0;
	MovePenTo(r.LeftBottom());
	DrawString(Text());

	// draw resize dragger
	SetDrawingMode(B_OP_OVER);
	r = Bounds();
	r.right -= 2.0;
	r.left = r.right - 2.0;
	r.InsetBy(0.0, 3.0);
	r.top += 1.0;
	for (int32 i = 0; i < r.IntegerHeight(); i += 3) {
		BPoint p = r.LeftTop() + BPoint(0.0, i);
		SetHighColor(M_MED_GRAY_COLOR);
		StrokeLine(p, p, B_SOLID_HIGH);
		p += BPoint(1.0, 1.0);
		SetHighColor(M_WHITE_COLOR);
		StrokeLine(p, p, B_SOLID_HIGH);
	}
}

void StatusView::FrameResized(
	float width,
	float height) {
	D_HOOK(("StatusView::FrameResized()\n"));

	// get the tip manager instance and reset
	TipManager *manager = TipManager::Instance();
	manager->removeAll(this);

	// re-truncate the string if necessary
	BString text = m_fullText;
	if (be_plain_font->StringWidth(text.String()) > Bounds().Width() - 25.0) {
		be_plain_font->TruncateString(&text, B_TRUNCATE_END,
									  Bounds().Width() - 25.0);
		manager->setTip(m_fullText.String(), this);
	}
	BStringView::SetText(text.String());

	float minWidth, maxWidth, minHeight, maxHeight;
	Window()->GetSizeLimits(&minWidth, &maxWidth, &minHeight, &maxHeight);
	minWidth = width + 6 * B_V_SCROLL_BAR_WIDTH;
	Window()->SetSizeLimits(minWidth, maxWidth, minHeight, maxHeight);
}

void StatusView::MouseDown(
	BPoint point) {
	D_HOOK(("StatusView::MouseDown()\n"));

	int32 buttons;
	if (Window()->CurrentMessage()->FindInt32("buttons", &buttons) != B_OK) {
		buttons = B_PRIMARY_MOUSE_BUTTON;
	}

	if (buttons == B_PRIMARY_MOUSE_BUTTON) {
		// drag rect
		BRect dragRect(Bounds());
		dragRect.left = dragRect.right - 10.0;
		if (dragRect.Contains(point)) {
			// resize
			m_dragging = true;
			SetMouseEventMask(B_POINTER_EVENTS,
							  B_LOCK_WINDOW_FOCUS | B_NO_POINTER_HISTORY);
		}
	}
}

void StatusView::MouseMoved(
	BPoint point,
	uint32 transit,
	const BMessage *message) {
	D_HOOK(("StatusView::MouseMoved()\n"));

	if (m_dragging) {
		float x = point.x - (Bounds().right - 5.0);
		if ((Bounds().Width() + x) <= 16.0) {
			return;
		}
		if (m_scrollBar
		 && ((m_scrollBar->Bounds().Width() - x) <= (6 * B_V_SCROLL_BAR_WIDTH))) {
			return;
		}
		ResizeBy(x, 0.0);
		BRect r(Bounds());
		r.left = r.right - 10.0;
		Invalidate(r);
		if (m_scrollBar) {
			m_scrollBar->ResizeBy(-x, 0.0);
			m_scrollBar->MoveBy(x, 0.0);
		}
	}
}

void StatusView::MouseUp(
	BPoint point) {
	D_HOOK(("StatusView::MouseUp()\n"));

	m_dragging = false;
}

void StatusView::Pulse() {
	D_HOOK(("StatusView::Pulse()\n"));

	if (m_opacity > 0.0) {
		float steps = static_cast<float>(TEXT_DECAY_TIME)
					  / static_cast<float>(Window()->PulseRate());
		m_opacity -= (1.0 / steps);
		if (m_opacity < 0.001) {
			m_opacity = 0.0;
		}
		Invalidate();
	}
	else if (Flags() & B_PULSE_NEEDED) {
		// turn off pulse for this view
		SetFlags(Flags() ^ B_PULSE_NEEDED);

		// get the tip manager instance and reset
		TipManager *manager = TipManager::Instance();
		manager->removeAll(this);
	}
}

// -------------------------------------------------------- //
// *** internal operations
// -------------------------------------------------------- //

void StatusView::setErrorMessage(
	BString text,
	bool error) {
	D_OPERATION(("StatusView::setErrorMessage(%s)\n",
				 text.String()));

	// get the tip manager instance and reset
	TipManager *manager = TipManager::Instance();
	manager->removeAll(this);

	// truncate if necessary
	m_fullText = text;
	if (be_plain_font->StringWidth(text.String()) > Bounds().Width() - 25.0) {
		be_plain_font->TruncateString(&text, B_TRUNCATE_END,
									  Bounds().Width() - 25.0);
		manager->setTip(m_fullText.String(), this);
	}
	BStringView::SetText(text.String());

	if (error) {
		beep();
		// set icon
		if (m_icon) {
			delete m_icon;
			m_icon = 0;
		}
		BRect iconRect(0.0, 0.0, 7.0, 11.0);
		m_icon = new BBitmap(iconRect, B_CMAP8);
		m_icon->SetBits(ERROR_ICON_BITS, 96, 0, B_CMAP8);
	}
	else {
		// set icon
		if (m_icon) {
			delete m_icon;
			m_icon = 0;
		}
		BRect iconRect(0.0, 0.0, 7.0, 11.0);
		m_icon = new BBitmap(iconRect, B_CMAP8);
		m_icon->SetBits(INFO_ICON_BITS, 96, 0, B_CMAP8);
	}
	m_opacity = 1.0;
	Invalidate();
	SetFlags(Flags() | B_PULSE_NEEDED);
}

// END -- ParameterContainerView.cpp --
