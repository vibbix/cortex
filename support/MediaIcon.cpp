// MediaIcon.cpp

#include "MediaIcon.h"
#include "MediaIconBits.h"
#include "MediaString.h"

// Application Kit
#include <Roster.h>
// Media Kit
#include <MediaDefs.h>
#include <MediaNode.h>
#include <MediaRoster.h>
#include <MediaAddOn.h>
// Storage Kit
#include <NodeInfo.h>
// Support Kit
#include <String.h>

__USE_CORTEX_NAMESPACE

#include <Debug.h>
#define D_METHOD(x) //PRINT (x)

// -------------------------------------------------------- //
// *** ctor/dtor
// -------------------------------------------------------- //

MediaIcon::MediaIcon(
	const live_node_info &nodeInfo,
	icon_size size)
	: BBitmap(BRect(0.0, 0.0, size - 1.0, size - 1.0), B_CMAP8),
	  m_size(size),
	  m_nodeKind(nodeInfo.node.kind)
{
	D_METHOD(("MediaIcon::MediaIcon(live_node_info)\n"));

	_findIconFor(nodeInfo);
}

MediaIcon::MediaIcon(
	const dormant_node_info &nodeInfo,
	icon_size size)
	: BBitmap(BRect(0.0, 0.0, size - 1.0, size - 1.0), B_CMAP8),
	  m_size(size),
	  m_nodeKind(0)
{
	D_METHOD(("MediaIcon::MediaIcon(dormant_node_info)\n"));

	_findIconFor(nodeInfo);
}

MediaIcon::~MediaIcon()
{
	D_METHOD(("MediaIcon::~MediaIcon()\n"));
}

// -------------------------------------------------------- //
// *** internal accessors (private)
// -------------------------------------------------------- //

bool MediaIcon::_isPhysicalInput() const
{
	D_METHOD(("MediaIcon::_isPhysicalInput()\n"));
	return ((m_nodeKind & B_PHYSICAL_INPUT)
	     && (m_nodeKind & B_BUFFER_PRODUCER));
}

bool MediaIcon::_isPhysicalOutput() const
{
	D_METHOD(("MediaIcon::_isPhysicalOutput()\n"));
	return ((m_nodeKind & B_PHYSICAL_OUTPUT)
	     && (m_nodeKind & B_BUFFER_CONSUMER));;
}

bool MediaIcon::_isProducer() const
{
	D_METHOD(("MediaIcon::_isProducer()\n"));
	return (!(m_nodeKind & B_BUFFER_CONSUMER) && 
			 (m_nodeKind & B_BUFFER_PRODUCER) && 
			!(m_nodeKind & B_PHYSICAL_INPUT)  &&
			!(m_nodeKind & B_PHYSICAL_OUTPUT));
}

bool MediaIcon::_isFilter() const
{
	D_METHOD(("MediaIcon::_isFilter()\n"));
	return ( (m_nodeKind & B_BUFFER_CONSUMER) && 
			 (m_nodeKind & B_BUFFER_PRODUCER) && 
		    !(m_nodeKind & B_PHYSICAL_INPUT)  &&
		    !(m_nodeKind & B_PHYSICAL_OUTPUT));
}

bool MediaIcon::_isConsumer() const
{
	D_METHOD(("MediaIcon::_isConsumer()\n"));
	return ( (m_nodeKind & B_BUFFER_CONSUMER) && 
		    !(m_nodeKind & B_BUFFER_PRODUCER) && 
		    !(m_nodeKind & B_PHYSICAL_INPUT)  &&
		    !(m_nodeKind & B_PHYSICAL_OUTPUT));
}

bool MediaIcon::_isSystemMixer() const
{
	D_METHOD(("MediaIcon::_isSystemMixer()\n"));
	return (m_nodeKind & B_SYSTEM_MIXER);
}

bool MediaIcon::_isTimeSource() const
{
	D_METHOD(("MediaIcon::_isTimeSource()\n"));
	return ((m_nodeKind & B_TIME_SOURCE) &&
		   !(m_nodeKind & B_PHYSICAL_INPUT) &&
		   !(m_nodeKind & B_PHYSICAL_OUTPUT));
}

// -------------------------------------------------------- //
// *** internal operations (private)
// -------------------------------------------------------- //

void MediaIcon::_findIconFor(
	const live_node_info &nodeInfo)
{
	D_METHOD(("MediaIcon::_findIconFor(live_node_info)\n"));

	BMediaRoster *roster = BMediaRoster::CurrentRoster();
	if (m_nodeKind & B_FILE_INTERFACE)
	{
		entry_ref ref;
		if ((roster && (roster->GetRefFor(nodeInfo.node, &ref) == B_OK))
		 && (BNodeInfo::GetTrackerIcon(&ref, this, m_size) == B_OK))
		{
			return;
		}
	}
	dormant_node_info dormantNodeInfo;
	if  (roster
	 && (roster->GetDormantNodeFor(nodeInfo.node, &dormantNodeInfo) == B_OK)
	 && (dormantNodeInfo.addon != 0)) // workaround because this method doesn't
									  // actually return an error for nodes that
									  // weren't instantiated from an addon
									  // in BeOS R4.5.2
	{
		if (dormantNodeInfo.addon > 0)
		{
			_findIconFor(dormantNodeInfo);
		}
		else
		{
			media_type type = _getMediaTypeFor(nodeInfo);
			_findDefaultIconFor(type);
		}
	}
	else
	{
		port_info portInfo;
		app_info appInfo;
		if ((get_port_info(nodeInfo.node.port, &portInfo) == B_OK)
		 && (be_roster->GetRunningAppInfo(portInfo.team, &appInfo) == B_OK))
		{
			BNodeInfo::GetTrackerIcon(&appInfo.ref, this, m_size);
		}
		else
		{
			media_type type = _getMediaTypeFor(nodeInfo);
			_findDefaultIconFor(type);
		}
	}
}

void MediaIcon::_findIconFor(
	const dormant_node_info &nodeInfo)
{
	D_METHOD(("MediaIcon::_findIconFor(dormant_node_info)\n"));

	dormant_flavor_info flavorInfo;
	BMediaRoster *roster = BMediaRoster::CurrentRoster();
	status_t error = roster->GetDormantFlavorInfoFor(nodeInfo, &flavorInfo);
	if (!error)
	{
		m_nodeKind = flavorInfo.kinds;
		media_type type = _getMediaTypeFor(flavorInfo);
		_findDefaultIconFor(type);
	}
}

media_type MediaIcon::_getMediaTypeFor(
	const live_node_info &nodeInfo)
{
	D_METHOD(("MediaIcon::_getMediaTypeFor(live_node_info)\n"));

	bool audioIn = false, audioOut = false;
	bool videoIn = false, videoOut = false;

	// get the media_types supported by this node
	const int32 numberOfInputs = 4;
	int32 numberOfFreeInputs, numberOfConnectedInputs;
	media_input inputs[numberOfInputs];
	const int32 numberOfOutputs = 4;
	int32 numberOfFreeOutputs, numberOfConnectedOutputs;
	media_output outputs[numberOfOutputs];
	BMediaRoster *roster = BMediaRoster::CurrentRoster();
	if (roster->GetFreeInputsFor(nodeInfo.node, inputs, numberOfInputs, &numberOfFreeInputs) == B_OK)
	{
		for (int32 i = 0; i < numberOfFreeInputs; i++)
		{
			if ((inputs[i].format.type == B_MEDIA_RAW_AUDIO)
			 || (inputs[i].format.type == B_MEDIA_ENCODED_AUDIO))
			{
				audioIn = true;
				continue;
			}
			if ((inputs[i].format.type == B_MEDIA_RAW_VIDEO)
			 || (inputs[i].format.type == B_MEDIA_ENCODED_VIDEO))
			{
				videoIn = true;
			}
		}
	}
	if (roster->GetConnectedInputsFor(nodeInfo.node, inputs, numberOfInputs, &numberOfConnectedInputs) == B_OK)
	{
		for (int32 i = 0; i < numberOfConnectedInputs; i++)
		{
			if ((inputs[i].format.type == B_MEDIA_RAW_AUDIO)
			 || (inputs[i].format.type == B_MEDIA_ENCODED_AUDIO))
			{
				audioIn = true;
				continue;
			}
			if ((inputs[i].format.type == B_MEDIA_RAW_VIDEO)
			 || (inputs[i].format.type == B_MEDIA_ENCODED_VIDEO))
			{
				videoIn = true;
			}
		}
	}
	if (roster->GetFreeOutputsFor(nodeInfo.node, outputs, numberOfOutputs, &numberOfFreeOutputs) == B_OK)
	{
		for (int32 i = 0; i < numberOfFreeOutputs; i++)
		{
			if ((outputs[i].format.type == B_MEDIA_RAW_AUDIO)
			 || (outputs[i].format.type == B_MEDIA_ENCODED_AUDIO))
			{
				audioOut = true;
				continue;
			}
			if ((outputs[i].format.type == B_MEDIA_RAW_VIDEO)
			 || (outputs[i].format.type == B_MEDIA_ENCODED_VIDEO))
			{
				videoOut = true;
			}
		}
	}
	if (roster->GetConnectedOutputsFor(nodeInfo.node, outputs, numberOfOutputs, &numberOfConnectedOutputs) == B_OK)
	{
		for (int32 i = 0; i < numberOfConnectedOutputs; i++)
		{
			if ((outputs[i].format.type == B_MEDIA_RAW_AUDIO)
			 || (outputs[i].format.type == B_MEDIA_ENCODED_AUDIO))
			{
				audioOut = true;
				continue;
			}
			if ((outputs[i].format.type == B_MEDIA_RAW_VIDEO)
			 || (outputs[i].format.type == B_MEDIA_ENCODED_VIDEO))
			{
				videoOut = true;
			}
		}
	}
	
	if ((audioIn || audioOut) && !(videoIn || videoOut))
	{
		return B_MEDIA_RAW_AUDIO;
	}
	if ((videoIn || videoOut) && !(audioIn || audioOut))
	{
		return B_MEDIA_RAW_VIDEO;
	}
	return B_MEDIA_UNKNOWN_TYPE;
}

media_type MediaIcon::_getMediaTypeFor(
	const dormant_flavor_info &flavorInfo)
{
	D_METHOD(("MediaIcon::_getMediaTypeFor(dormant_flavor_info)\n"));

	bool audioIn = false, audioOut = false;
	bool videoIn = false, videoOut = false;
	for (int32 i = 0; i < flavorInfo.in_format_count; i++)
	{
		if ((flavorInfo.in_formats[i].type == B_MEDIA_RAW_AUDIO)
		 || (flavorInfo.in_formats[i].type == B_MEDIA_ENCODED_AUDIO))
		{
			audioIn = true;
			continue;
		}
		if ((flavorInfo.in_formats[i].type == B_MEDIA_RAW_VIDEO)
		 || (flavorInfo.in_formats[i].type == B_MEDIA_ENCODED_VIDEO))
		{
			videoIn = true;
		}
	}
	for (int32 i = 0; i < flavorInfo.out_format_count; i++)
	{
		if ((flavorInfo.out_formats[i].type == B_MEDIA_RAW_AUDIO)
		 || (flavorInfo.out_formats[i].type == B_MEDIA_ENCODED_AUDIO))
		{
			audioOut = true;
			continue;
		}
		if ((flavorInfo.out_formats[i].type == B_MEDIA_RAW_VIDEO)
		 || (flavorInfo.out_formats[i].type == B_MEDIA_ENCODED_VIDEO))
		{
			videoOut = true;
		}
	}
	
	if ((audioIn || audioOut) && !(videoIn || videoOut))
	{
		return B_MEDIA_RAW_AUDIO;
	}
	if ((videoIn || videoOut) && !(audioIn || audioOut))
	{
		return B_MEDIA_RAW_VIDEO;
	}
	return B_MEDIA_UNKNOWN_TYPE;
}

void MediaIcon::_findDefaultIconFor(
	media_type type)
{
	D_METHOD(("MediaIcon::_findDefaultIcon()\n"));

	if (_isTimeSource())
	{
		if (m_size == B_LARGE_ICON)
			SetBits(M_TIME_SOURCE_ICON.large, 1024, 0, B_CMAP8);
		else if (m_size == B_MINI_ICON)
			SetBits(M_TIME_SOURCE_ICON.small, 256, 0, B_CMAP8);
	}
	else if (_isSystemMixer())
	{
		if (m_size == B_LARGE_ICON)
			SetBits(M_AUDIO_MIXER_ICON.large, 1024, 0, B_CMAP8);
		else if (m_size == B_MINI_ICON)
			SetBits(M_AUDIO_MIXER_ICON.small, 256, 0, B_CMAP8);
	}
	else if (m_nodeKind & B_FILE_INTERFACE)
	{
		if (_isProducer())
		{
			if (m_size == B_LARGE_ICON)
				SetBits(M_FILE_READER_ICON.large, 1024, 0, B_CMAP8);
			else if (m_size == B_MINI_ICON)
				SetBits(M_FILE_READER_ICON.small, 256, 0, B_CMAP8);
		}
		else
		{
			if (m_size == B_LARGE_ICON)
				SetBits(M_FILE_WRITER_ICON.large, 1024, 0, B_CMAP8);
			else if (m_size == B_MINI_ICON)
				SetBits(M_FILE_WRITER_ICON.small, 256, 0, B_CMAP8);
		}
	}
	else if (type & B_MEDIA_RAW_AUDIO)
	{
		if (_isPhysicalInput())
		{
			if (m_size == B_LARGE_ICON)
				SetBits(M_AUDIO_INPUT_ICON.large, 1024, 0, B_CMAP8);
			else if (m_size == B_MINI_ICON)
				SetBits(M_AUDIO_INPUT_ICON.small, 256, 0, B_CMAP8);
		}
		else if (_isPhysicalOutput())
		{
			if (m_size == B_LARGE_ICON)
				SetBits(M_AUDIO_OUTPUT_ICON.large, 1024, 0, B_CMAP8);
			else if (m_size == B_MINI_ICON)
				SetBits(M_AUDIO_OUTPUT_ICON.small, 256, 0, B_CMAP8);
		}
		else if (_isProducer())
		{
			if (m_size == B_LARGE_ICON)
				SetBits(M_AUDIO_PRODUCER_ICON.large, 1024, 0, B_CMAP8);
			else if (m_size == B_MINI_ICON)
				SetBits(M_AUDIO_PRODUCER_ICON.small, 256, 0, B_CMAP8);
		}
		else if (_isFilter())
		{
			if (m_size == B_LARGE_ICON)
				SetBits(M_AUDIO_FILTER_ICON.large, 1024, 0, B_CMAP8);
			else if (m_size == B_MINI_ICON)
				SetBits(M_AUDIO_FILTER_ICON.small, 256, 0, B_CMAP8);
		}
		else if (_isConsumer())
		{
			if (m_size == B_LARGE_ICON)
				SetBits(M_AUDIO_CONSUMER_ICON.large, 1024, 0, B_CMAP8);
			else if (m_size == B_MINI_ICON)
				SetBits(M_AUDIO_CONSUMER_ICON.small, 256, 0, B_CMAP8);
		}
		else
		{
			if (m_size == B_LARGE_ICON)
				SetBits(M_GENERIC_ICON.large, 1024, 0, B_CMAP8);
			else if (m_size == B_MINI_ICON)
				SetBits(M_GENERIC_ICON.small, 256, 0, B_CMAP8);
		}
	}
	else if (type & B_MEDIA_RAW_VIDEO)
	{
		if (_isPhysicalInput())
		{
			if (m_size == B_LARGE_ICON)
				SetBits(M_VIDEO_INPUT_ICON.large, 1024, 0, B_CMAP8);
			else if (m_size == B_MINI_ICON)
				SetBits(M_VIDEO_INPUT_ICON.small, 256, 0, B_CMAP8);
		}
		else if (_isPhysicalOutput())
		{
			if (m_size == B_LARGE_ICON)
				SetBits(M_VIDEO_OUTPUT_ICON.large, 1024, 0, B_CMAP8);
			else if (m_size == B_MINI_ICON)
				SetBits(M_VIDEO_OUTPUT_ICON.small, 256, 0, B_CMAP8);
		}
		else if (_isProducer())
		{
			if (m_size == B_LARGE_ICON)
				SetBits(M_VIDEO_PRODUCER_ICON.large, 1024, 0, B_CMAP8);
			else if (m_size == B_MINI_ICON)
				SetBits(M_VIDEO_PRODUCER_ICON.small, 256, 0, B_CMAP8);
		}
		else if (_isFilter())
		{
			if (m_size == B_LARGE_ICON)
				SetBits(M_VIDEO_FILTER_ICON.large, 1024, 0, B_CMAP8);
			else if (m_size == B_MINI_ICON)
				SetBits(M_VIDEO_FILTER_ICON.small, 256, 0, B_CMAP8);
		}
		else if (_isConsumer())
		{
			if (m_size == B_LARGE_ICON)
				SetBits(M_VIDEO_CONSUMER_ICON.large, 1024, 0, B_CMAP8);
			else if (m_size == B_MINI_ICON)
				SetBits(M_VIDEO_CONSUMER_ICON.small, 256, 0, B_CMAP8);
		}
		else
		{
			if (m_size == B_LARGE_ICON)
				SetBits(M_GENERIC_ICON.large, 1024, 0, B_CMAP8);
			else if (m_size == B_MINI_ICON)
				SetBits(M_GENERIC_ICON.small, 256, 0, B_CMAP8);
		}
	}
	else
	{
		if (m_size == B_LARGE_ICON)
			SetBits(M_GENERIC_ICON.large, 1024, 0, B_CMAP8);
		else if (m_size == B_MINI_ICON)
			SetBits(M_GENERIC_ICON.small, 256, 0, B_CMAP8);
	}
}
