// AppNodeInfoView.cpp

#include "AppNodeInfoView.h"
// NodeManager
#include "NodeRef.h"
// Support
#include "MediaIcon.h"
#include "MediaString.h"

// Application Kit
#include <Roster.h>
// Media Kit
#include <MediaNode.h>
#include <MediaRoster.h>
// Storage Kit
#include <AppFileInfo.h>
#include <Entry.h>
#include <File.h>
#include <Path.h>

__USE_CORTEX_NAMESPACE

#include <Debug.h>
#define D_METHOD(x) //PRINT (x)

// -------------------------------------------------------- //
// *** ctor/dtor (public)
// -------------------------------------------------------- //

AppNodeInfoView::AppNodeInfoView(
	const NodeRef *ref)
	: LiveNodeInfoView(ref) {
	D_METHOD(("AppNodeInfoView::AppNodeInfoView()\n"));

	// adjust view properties
	setSideBarWidth(be_plain_font->StringWidth(" Application ") + 2 * InfoView::M_H_MARGIN);
	setSubTitle("Application-Owned Node");

	// add separator
	addField("", "");

	port_info portInfo;
	app_info appInfo;

	if ((get_port_info(ref->node().port, &portInfo) == B_OK)
	 && (be_roster->GetRunningAppInfo(portInfo.team, &appInfo) == B_OK)) {
		BEntry appEntry(&appInfo.ref);
		char appName[B_FILE_NAME_LENGTH];
		if ((appEntry.InitCheck() == B_OK)
		 && (appEntry.GetName(appName) == B_OK)) {
			addField("Application", appName);
		}
		BFile appFile(&appInfo.ref, B_READ_ONLY);
		if (appFile.InitCheck() == B_OK) {
			BAppFileInfo appFileInfo(&appFile);
			if (appFileInfo.InitCheck() == B_OK) {
				version_info appVersion;
				if (appFileInfo.GetVersionInfo(&appVersion, B_APP_VERSION_KIND) == B_OK) {
					BString versionStr("");
					if (strlen(appVersion.long_info) != 0) {
						versionStr << appVersion.long_info;
					}
					else if (strlen(appVersion.short_info) != 0) {
						versionStr << appVersion.short_info;
					}
					else {
						versionStr << appVersion.major << ".";
						versionStr << appVersion.middle << ".";
						versionStr << appVersion.minor;
						switch (appVersion.variety) {
							case 0: {
								versionStr << " Development " << appVersion.internal;
								break;
							}
							case 1: {
								versionStr << " Alpha " << appVersion.internal;
								break;
							}
							case 2: {
								versionStr << " Beta " << appVersion.internal;
								break;
							}
							case 3: {
								versionStr << " Gamma " << appVersion.internal;
								break;
							}
							case 4: {
								versionStr << " Golden Master " << appVersion.internal << "";
								break;
							}
						}
					}
					addField("Version", versionStr);
				}
			}
		}
	}
}

AppNodeInfoView::~AppNodeInfoView() {
	D_METHOD(("AppNodeInfoView::~AppNodeInfoView()\n"));
}

// END -- AppNodeInfoView.cpp --
