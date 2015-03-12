#include "audio_nodes.h"
#include "app_globals.h"

#include <cinder/audio/Context.h>
#include <cinder/audio/MonitorNode.h>
#include <cinder/app/App.h>
#include <cinder/Timer.h>

namespace cieq
{

AudioNodes::AudioNodes(AppGlobals& globals)
    : mGlobals(globals)
    , mIsEnabled(false)
{}

void AudioNodes::setup(size_t userWinSize, size_t fftSize, bool auto_enable /*= true*/)
{
    hardwareSampleRate = 0;
    if (mInputDeviceNode == NULL)
    {
        mInputDeviceNode = mGlobals.getAudioContext().createInputDeviceNode();
        ci::app::getWindow()->setTitle(ci::app::getWindow()->getTitle() + " (" + mInputDeviceNode->getDevice()->getName() + ")");
    }
    //Get the current sample rate the audio hardware is configured for:
    hardwareSampleRate = mInputDeviceNode->getSampleRate();
    //Convert desired window size from ms to nearest number of samples:
    size_t userWinSizeSamples = floor((((float)userWinSize) / 1000) * hardwareSampleRate);
	
    auto monitorFormat = ci::audio::MonitorNode::Format().windowSize(userWinSizeSamples); // was originally windowSize(1024)
	mMonitorNode = mGlobals.getAudioContext().makeNode(new ci::audio::MonitorNode(monitorFormat));
	
    auto monitorSpectralFormat = ci::audio::MonitorSpectralNode::Format().fftSize(fftSize).windowSize(userWinSizeSamples); // was originally windowSize(1024), 
    //I also changed fftSize to 131072 to ensure we get a minimum of 273 frequency bins at the minimum display frequency range setting of 0 - 100Hz

    mMonitorSpectralNode = mGlobals.getAudioContext().makeNode(new ci::audio::MonitorSpectralNode(monitorSpectralFormat));
    mMonitorSpectralNode2 = mGlobals.getAudioContext().makeNode(new ci::audio::MonitorSpectralNode(monitorSpectralFormat));

	mInputDeviceNode >> mMonitorNode;
    mInputDeviceNode >> mMonitorSpectralNode;
    double time = 0.0;
    time = mTimer.getSeconds();
    mTimer.start();
    for (size_t i = 0; time < 0.02; i++)
    {
        time = mTimer.getSeconds();
    }
    mTimer.stop();
    mInputDeviceNode >> mMonitorSpectralNode2;


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
    //if (mTimer.isStopped())
    //{
    //    mTimer.start();
    //}
    //timeSec1Enter = mTimer.getSeconds();
    //cinder::audio::MonitorSpectralNode* const getMonitorSpectralNodeTemp = mMonitorSpectralNode.get();
    //timeSec1Exit = mTimer.getSeconds();
    //timeSec1Process = timeSec1Exit - timeSec1Enter;
    return mMonitorSpectralNode.get(); //mMonitorSpectralNode.get();
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

void AudioNodes::disconnectAll()
{
    mInputDeviceNode->disconnectAll();
    mMonitorNode->disconnectAll();
    mMonitorSpectralNode->disconnectAll();
    mGlobals.getAudioContext().disconnectAllNodes();
    //delete &mMonitorNode;
    //delete &mMonitorSpectralNode;
    //delete &mInputDeviceNode;
}

size_t AudioNodes::getNumBins()
{
    return mMonitorSpectralNode->getNumBins();
}

size_t AudioNodes::getFftSize()
{
    return mMonitorSpectralNode->getFftSize();
}

size_t AudioNodes::getMaxFreqDisp(size_t binNumber)
{
    return mMonitorSpectralNode->getFreqForBin(binNumber);//getNumBins() - 1);
}

size_t AudioNodes::getHardwareSampleRate()
{
    return hardwareSampleRate;
}

} //!cieq