#include "audio_nodes.h"
#include "app_globals.h"

#include <cinder/audio/Context.h>
#include <cinder/audio/MonitorNode.h>
#include <cinder/app/App.h>

namespace cieq
{

AudioNodes::AudioNodes(AppGlobals& globals)
    : mGlobals(globals)
    , mIsEnabled(false)
{}

void AudioNodes::setup(size_t userWinSize, bool auto_enable /*= true*/)
{
    size_t hardwareSampleRate = 0;
    mInputDeviceNode = mGlobals.getAudioContext().createInputDeviceNode();
    //Get the current sample rate the audio hardware is configured for:
    hardwareSampleRate = mInputDeviceNode->getSampleRate();
    //Convert desired window size from ms to nearest number of samples:
    size_t userWinSizeSamples = floor((((float)userWinSize) / 1000) * hardwareSampleRate);
	
    auto monitorFormat = ci::audio::MonitorNode::Format().windowSize(userWinSizeSamples); // was originally windowSize(1024)
	mMonitorNode = mGlobals.getAudioContext().makeNode(new ci::audio::MonitorNode(monitorFormat));
	
	auto monitorSpectralFormat = ci::audio::MonitorSpectralNode::Format().fftSize(2048).windowSize(1024);
	mMonitorSpectralNode = mGlobals.getAudioContext().makeNode(new ci::audio::MonitorSpectralNode(monitorSpectralFormat));

	mInputDeviceNode >> mMonitorNode;
	mInputDeviceNode >> mMonitorSpectralNode;

	ci::app::getWindow()->setTitle(ci::app::getWindow()->getTitle() + " (" + mInputDeviceNode->getDevice()->getName() + ")");

	if (auto_enable)
	{
		enableInput();
	}
}

cinder::audio::InputDeviceNode* const AudioNodes::getInputDeviceNode()
{
	return mInputDeviceNode.get();
}

cinder::audio::MonitorNode* const AudioNodes::getMonitorNode()
{
	return mMonitorNode.get();
}

cinder::audio::MonitorSpectralNode* const AudioNodes::getMonitorSpectralNode()
{
	return mMonitorSpectralNode.get();
}

void AudioNodes::enableInput()
{
	if (mIsEnabled) return;

	mGlobals.getAudioContext().enable();
	mInputDeviceNode->enable();

	mIsEnabled = true;
}

void AudioNodes::disableInput()
{
	if (!mIsEnabled) return;

	mGlobals.getAudioContext().disable();
	mInputDeviceNode->disable();

	mIsEnabled = false;
}

void AudioNodes::toggleInput()
{
	if (mIsEnabled)
	{
		disableInput();
	}
	else
	{
		enableInput();
	}
}

} //!cieq