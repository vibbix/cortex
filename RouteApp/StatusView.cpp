// StatusView.cpp

#include "StatusView.h"
#include "cortex_ui.h"
#include "TipManager.h"

// Interface Kit
#include <Bitmap.h>
#include <Font.h>
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
	BRect frame)
	:	BStringView(frame, "StatusView", "", B_FOLLOW_LEFT | B_FOLLOW_BOTTOM,
					B_FRAME_EVENTS | B_WILL_DRAW),
		m_icon(0),
		m_opacity(1.0) {
	D_ALLOC(("StatusView::StatusView()\n"));

	SetViewColor(B_TRANSPARENT_COLOR);
	SetFont(be_plain_font);
}

StatusView::~StatusView() {
	D_ALLOC(("ParameterContainerView::~ParameterContainerView()\n"));

	// get the tip manager instance and reset
	TipManager *manager = TipManager::Instance();
	manager->removeAll(this);
}

// -------------------------------------------------------- //
// *** BScrollView impl
// -------------------------------------------------------- //

void StatusView::Draw(
	BRect updateRect) {
	D_HOOK(("ParameterContainerView::Draw()\n"));

	BRect r(Bounds());
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

	if (m_icon) {
		SetBlendingMode(B_CONSTANT_ALPHA, B_ALPHA_OVERLAY);
		DrawBitmapAsync(m_icon, r.LeftTop());
	}
	r.left += 12.0;
	font_height fh;
	be_plain_font->GetHeight(&fh);
	r.bottom = Bounds().bottom - fh.descent - 1.0;
	MovePenTo(r.LeftBottom());
	DrawString(Text());
}

void StatusView::Pulse() {
	D_HOOK(("ParameterContainerView::Pulse()\n"));

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
	D_OPERATION(("ParameterContainerView::setErrorMessage(%s)\n",
				 text.String()));

	// get the tip manager instance and reset
	TipManager *manager = TipManager::Instance();
	manager->removeAll(this);

	// truncate if necessary
	m_fullText = text;
	if (be_plain_font->StringWidth(text.String()) > Bounds().Width() - 20.0) {
		be_plain_font->TruncateString(&text, B_TRUNCATE_END,
									  Bounds().Width() - 20.0);
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
		m_icon->SetBits(INFO_ICON_BITS, 96, 0, B_CMAP8);
	}
	else {
		// set icon
		if (m_icon) {
			delete m_icon;
			m_icon = 0;
		}
		BRect iconRect(0.0, 0.0, 7.0, 11.0);
		m_icon = new BBitmap(iconRect, B_CMAP8);
		m_icon->SetBits(ERROR_ICON_BITS, 96, 0, B_CMAP8);
	}
	m_opacity = 1.0;
	Invalidate();
	SetFlags(Flags() | B_PULSE_NEEDED);
}

// END -- ParameterContainerView.cpp --
