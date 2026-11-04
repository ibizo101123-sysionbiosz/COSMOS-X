#include "PluginProcessor.h"
#include "PluginEditor.h"
namespace { class CosmosSound final:public juce::SynthesiserSound {
public: bool appliesToNote(int) override{return true;} bool appliesToChannel(int) override{return true;}
};}
class CosmosXAudioProcessor::CosmosVoice final:public juce::SynthesiserVoice {
public:
 explicit CosmosVoice(CosmosXAudioProcessor& p):processor(p){}
 bool canPlaySound(juce::SynthesiserSound* s) override{return dynamic_cast<CosmosSound*>(s)!=nullptr;}
 void startNote(int n,float v,juce::SynthesiserSound*,int) override{
  frequency=juce::MidiMessage::getMidiNoteInHertz(n); level=v*0.2f; phase=0; filterState=0; env.noteOn();
 }
 void stopNote(float,bool tail) override{env.noteOff();if(!tail)clearCurrentNote();}
 void pitchWheelMoved(int) override{} void controllerMoved(int,int) override{}
 void renderNextBlock(juce::AudioBuffer<float>& b,int start,int count) override{
  if(count<=0||!isVoiceActive())return;
  const double sr=juce::jmax(1.0,getSampleRate());
  auto val=[this](const char* id){return processor.parameters.getRawParameterValue(id)->load();};
  juce::ADSR::Parameters ep{val("attack"),val("decay"),val("sustain"),val("release")}; env.setParameters(ep);
  const float cutoff=juce::jlimit(30.0f,18000.0f,val("cutoff"));
  const float coeff=1.0f-std::exp(-juce::MathConstants<float>::twoPi*cutoff/(float)sr);
  const int wave=(int)val("wave");
  for(int i=0;i<count;++i){
   float raw=wave==1?(phase<0.5?1.0f:-1.0f):(wave==2?(float)std::sin(phase*juce::MathConstants<double>::twoPi):(float)(2.0*phase-1.0));
   phase+=frequency/sr;if(phase>=1.0)phase-=1.0;
   filterState+=coeff*(raw-filterState);
   float s=filterState*env.getNextSample()*level*val("gain");
   for(int ch=0;ch<b.getNumChannels();++ch)b.addSample(ch,start+i,s);
  }
  if(!env.isActive())clearCurrentNote();
 }
 void setSR(double sr){env.setSampleRate(sr);}
private: CosmosXAudioProcessor& processor; juce::ADSR env; double phase=0,frequency=440; float level=0,filterState=0;
};
CosmosXAudioProcessor::CosmosXAudioProcessor():AudioProcessor(BusesProperties().withOutput("Output",juce::AudioChannelSet::stereo(),true)),parameters(*this,nullptr,"COSMOSX_STATE",createParameterLayout()){
 for(int i=0;i<16;++i)synth.addVoice(new CosmosVoice(*this)); synth.addSound(new CosmosSound());
}
juce::AudioProcessorValueTreeState::ParameterLayout CosmosXAudioProcessor::createParameterLayout(){
 using F=juce::AudioParameterFloat; std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
 p.push_back(std::make_unique<F>("gain","Master",0.0f,1.0f,0.7f));
 p.push_back(std::make_unique<F>("cutoff","Cutoff",juce::NormalisableRange<float>(100,18000,1,0.35f),12000));
 p.push_back(std::make_unique<F>("attack","Attack",juce::NormalisableRange<float>(0.001f,5,0.001f,0.35f),0.01f));
 p.push_back(std::make_unique<F>("decay","Decay",juce::NormalisableRange<float>(0.01f,5,0.001f,0.35f),0.25f));
 p.push_back(std::make_unique<F>("sustain","Sustain",0.0f,1.0f,0.8f));
 p.push_back(std::make_unique<F>("release","Release",juce::NormalisableRange<float>(0.01f,8,0.001f,0.35f),0.3f));
 p.push_back(std::make_unique<juce::AudioParameterChoice>("wave","Wave",juce::StringArray{"Saw","Square","Sine"},0)); return {p.begin(),p.end()};
}
void CosmosXAudioProcessor::prepareToPlay(double sr,int){synth.setCurrentPlaybackSampleRate(sr);for(int i=0;i<synth.getNumVoices();++i)if(auto*v=dynamic_cast<CosmosVoice*>(synth.getVoice(i)))v->setSR(sr);}
void CosmosXAudioProcessor::releaseResources(){}
bool CosmosXAudioProcessor::isBusesLayoutSupported(const BusesLayout& l)const{auto o=l.getMainOutputChannelSet();return o==juce::AudioChannelSet::mono()||o==juce::AudioChannelSet::stereo();}
void CosmosXAudioProcessor::processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer& m){juce::ScopedNoDenormals n;b.clear();synth.renderNextBlock(b,m,0,b.getNumSamples());}
juce::AudioProcessorEditor* CosmosXAudioProcessor::createEditor(){return new CosmosXAudioProcessorEditor(*this);}
void CosmosXAudioProcessor::getStateInformation(juce::MemoryBlock& d){auto s=parameters.copyState();std::unique_ptr<juce::XmlElement>x(s.createXml());copyXmlToBinary(*x,d);}
void CosmosXAudioProcessor::setStateInformation(const void*d,int n){std::unique_ptr<juce::XmlElement>x(getXmlFromBinary(d,n));if(x&&x->hasTagName(parameters.state.getType()))parameters.replaceState(juce::ValueTree::fromXml(*x));}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new CosmosXAudioProcessor();}