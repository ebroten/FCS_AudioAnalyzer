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
    mMonitorSpectralNode3 = mGlobals.getAudioContext().makeNode(new ci::audio::MonitorSpectralNode(monitorSpectralFormat));
    mMonitorSpectralNode4 = mGlobals.getAudioContext().makeNode(new ci::audio::MonitorSpectralNode(monitorSpectralFormat));
    mMonitorSpectralNode5 = mGlobals.getAudioContext().makeNode(new ci::audio::MonitorSpectralNode(monitorSpectralFormat));
    mMonitorSpectralNode6 = mGlobals.getAudioContext().makeNode(new ci::audio::MonitorSpectralNode(monitorSpectralFormat));
    mMonitorSpectralNode7 = mGlobals.getAudioContext().makeNode(new ci::audio::MonitorSpectralNode(monitorSpectralFormat));
    mMonitorSpectralNode8 = mGlobals.getAudioContext().makeNode(new ci::audio::MonitorSpectralNode(monitorSpectralFormat));
    mMonitorSpectralNode9 = mGlobals.getAudioContext().makeNode(new ci::audio::MonitorSpectralNode(monitorSpectralFormat));
    mMonitorSpectralNode10 = mGlobals.getAudioContext().makeNode(new ci::audio::MonitorSpectralNode(monitorSpectralFormat));

    if (mTimer.isStopped())
    {
        mTimer.start();
    }
	mInputDeviceNode >> mMonitorNode;
    mInputDeviceNode >> mMonitorSpectralNode;
    //double time = 0.0;
    //time = mTimer.getSeconds();
    size_t i;
    timeNode1 = mTimer.getSeconds();
    for (i = 0; time < 0.05; i++)
    {
        time = mTimer.getSeconds() - timeNode1;
    }
    mInputDeviceNode >> mMonitorSpectralNode2;
    timeNode2 = mTimer.getSeconds();
    for (i = 0; time < 0.05; i++)
    {
        time = mTimer.getSeconds() - timeNode2;
    }
    mInputDeviceNode >> mMonitorSpectralNode3;
    timeNode3 = mTimer.getSeconds();
    for (i = 0; time < 0.05; i++)
    {
        time = mTimer.getSeconds() - timeNode3;
    }
    mInputDeviceNode >> mMonitorSpectralNode4;
    timeNode4 = mTimer.getSeconds();
    for (i = 0; time < 0.05; i++)
    {
        time = mTimer.getSeconds() - timeNode4;
    }
    mInputDeviceNode >> mMonitorSpectralNode5;
    timeNode5 = mTimer.getSeconds();
    for (i = 0; time < 0.05; i++)
    {
        time = mTimer.getSeconds() - timeNode5;
    }
    mInputDeviceNode >> mMonitorSpectralNode6;
    timeNode6 = mTimer.getSeconds();
    for (i = 0; time < 0.05; i++)
    {
        time = mTimer.getSeconds() - timeNode6;
    }
    mInputDeviceNode >> mMonitorSpectralNode7;
    timeNode7 = mTimer.getSeconds();
    for (i = 0; time < 0.05; i++)
    {
        time = mTimer.getSeconds() - timeNode7;
    }
    mInputDeviceNode >> mMonitorSpectralNode8;
    timeNode8 = mTimer.getSeconds();
    for (i = 0; time < 0.05; i++)
    {
        time = mTimer.getSeconds() - timeNode8;
    }
    mInputDeviceNode >> mMonitorSpectralNode9;
    timeNode9 = mTimer.getSeconds();
    for (i = 0; time < 0.05; i++)
    {
        time = mTimer.getSeconds() - timeNode9;
    }
    mInputDeviceNode >> mMonitorSpectralNode10;
    timeNode10 = mTimer.getSeconds();

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

cinder::audio::MonitorSpectralNode* const AudioNodes::getMonitorSpectralNode(size_t nodeNumber)
{
    //if (mTimer.isStopped())
    //{
    //    mTimer.start();
    //}
    //timeSec1Enter = mTimer.getSeconds();
    //cinder::audio::MonitorSpectralNode* const getMonitorSpectralNodeTemp = mMonitorSpectralNode.get();
    //timeSec1Exit = mTimer.getSeconds();
    //timeSec1Process = timeSec1Exit - timeSec1Enter;
    switch (nodeNumber)
    {
    case 1:
        return mMonitorSpectralNode.get(); //mMonitorSpectralNode.get();
    case 2:
        return mMonitorSpectralNode2.get();
    case 3:
        return mMonitorSpectralNode3.get();
    case 4:
        return mMonitorSpectralNode4.get();
    case 5:
        return mMonitorSpectralNode5.get();
    case 6:
        return mMonitorSpectralNode6.get();
    case 7:
        return mMonitorSpectralNode7.get();
    case 8:
        return mMonitorSpectralNode8.get();
    case 9:
        return mMonitorSpectralNode9.get();
    case 10:
        return mMonitorSpectralNode10.get();
    }
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

double AudioNodes::getTimeOfNode(size_t nodeNumber)
{
    //if (mTimer.isStopped())
    //{
    //    mTimer.start();
    //}
    //timeSec1Enter = mTimer.getSeconds();
    switch (nodeNumber)
    {
    case 1:
        return timeNode1;
    case 2:
        return timeNode2;
    case 3:
        return timeNode3;
    case 4:
        return timeNode4;
    case 5:
        return timeNode5;
    case 6:
        return timeNode6;
    case 7:
        return timeNode7;
    case 8:
        return timeNode8;
    case 9:
        return timeNode9;
    case 10:
        return timeNode10;
    }
    return timeNode1;
    //timeSec1Exit = mTimer.getSeconds();
    //timeSec1Process = timeSec1Exit - timeSec1Enter;
}

} //!cieq