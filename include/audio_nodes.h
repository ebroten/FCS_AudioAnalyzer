#ifndef CIEQ_INCLUDE_AUDIO_NODES_H_
#define CIEQ_INCLUDE_AUDIO_NODES_H_

#include <memory>
#include <cinder/Timer.h>

namespace cinder 
{
namespace audio 
{
    class InputDeviceNode;
    class MonitorNode;
    class MonitorSpectralNode;
}
} //!ci::audio

namespace cieq
{

class AppGlobals;

/*!
 * \class AudioNodes
 * \brief holds and handles all Cinder audio nodes. Also takes care
 * of initialization of those nodes.
 * \note from my understanding, node in Cinder is a unit of audio
 * processing. For each operation (regardless of being input / output)
 * a node is required. I have three nodes here, one monitoring the raw
 * input and the other one performing FFT on it. The third is reading
 * the input.
 */
class AudioNodes
{
public:
	AudioNodes(AppGlobals&);

	// \brief initializes all nodes and connect them together
    void												setup(double userHopSize, size_t userWinSize, size_t fftSize, bool auto_enable = true);
	// \brief enables reading from input
	void												enableInput();
	// \brief disables reading from input
    void												disableInput();
    // \brief disconnects node from all inputs and/or outputs it may be connected to
    void												disconnectAll();
	// \brief toggles reading from input
	void												toggleInput();
    //Get the number of frequency bins in the current monitorSpectralNode
    size_t                                              getNumBins();
    //Get the FFT Size of the current monitorSpectralNode
    size_t                                              getFftSize();
    //Get the frequency of the last frequency bin in the current monitorSpectralNode
    size_t                                              getMaxFreqDisp(size_t binNumber);
    //Get the sample rate of the audio input device hardware on this machine
    size_t                                              getHardwareSampleRate();
    //Get the time stamp for when a given node was established and connected
    double                                              getTimeOfNode(size_t nodeNumber);

	// \brief returns a pointer to the node which is reading data from input
	cinder::audio::InputDeviceNode* const				getInputDeviceNode();
	// \brief returns a pointer to the node which is having raw data in it
	cinder::audio::MonitorNode* const					getMonitorNode();
	// \brief returns a pointer to the node which is performing FFT on data
    cinder::audio::MonitorSpectralNode* const			getMonitorSpectralNode(size_t nodeNumber);
    // Returns a pointer to the timer being used to track node start times (I hope)
    ci::Timer* const			                        getTimer();

private:
    std::shared_ptr<cinder::audio::InputDeviceNode>		mInputDeviceNode;
	std::shared_ptr<cinder::audio::MonitorNode>			mMonitorNode;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode2;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode3;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode4;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode5;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode6;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode7;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode8;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode9;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode10;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode11;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode12;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode13;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode14;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode15;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode16;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode17;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode18;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode19;
    std::shared_ptr<cinder::audio::MonitorSpectralNode>	mMonitorSpectralNode20;
    ci::Timer                                           mTimer;
    double                                              time;
    double                                              timeNode1;
    double                                              timeNode2;
    double                                              timeNode3;
    double                                              timeNode4;
    double                                              timeNode5;
    double                                              timeNode6;
    double                                              timeNode7;
    double                                              timeNode8;
    double                                              timeNode9;
    double                                              timeNode10;
    double                                              timeNode11;
    double                                              timeNode12;
    double                                              timeNode13;
    double                                              timeNode14;
    double                                              timeNode15;
    double                                              timeNode16;
    double                                              timeNode17;
    double                                              timeNode18;
    double                                              timeNode19;
    double                                              timeNode20;
    double                                              timeSec1Enter;
    double                                              timeSec1Exit;
    double                                              timeSec1Process;
    //double                                              userHopSizeMs;
    size_t                                              hardwareSampleRate;

private:
	AppGlobals&											mGlobals;
	bool												mIsEnabled;
};

} //!cieq

#endif //!CIEQ_INCLUDE_AUDIO_NODES_H_