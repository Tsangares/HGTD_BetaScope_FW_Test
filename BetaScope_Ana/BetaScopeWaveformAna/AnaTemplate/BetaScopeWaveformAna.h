#include "BetaScope_Driver/include/BetaScope_Class.h"
#include "BetaScope_Driver/include/BetaScope_AnaFramework.h"
#include "General/Colorful_Cout/include/Colorful_Cout.h"
#include <string>
#include <iostream>

class BetaScopeWaveformAna : public BetaScope_AnaFramework<BetaScope>
{
  //this is are required.
  std::string ifile;

  //user define class variables
  std::vector<int> invertChannels;
  std::vector<int> activeChannels;

  bool skipWaveform = false;

  bool limitPmaxSearchRange = true;
  double pmaxSearchMinRange = -1000.0; //unit of ps
  double pmaxSearchMaxRange = 1000.0;
  double voltageMultiFactor = 1000.0; //convert Volt to mili-Volt.
  double timeMultiFactor = 1.0e12; //convert second to pico-second
  double assistThreshold = 10.0; // unit of mili-Volt
  int triggerCh = 3;

  std::vector<double> *w[8];
  std::vector<double> *t[8];
  std::vector<double> *wRaw[8];
  std::vector<double> *tRaw[8];
  std::vector<double> *pmax[8];
  std::vector<double> *tmax[8];
  std::vector<double> *riseTime[8];
  std::vector<double> *cfd[8];
  std::vector<double> *cfd_fall[8];
  std::vector<double> *fineCFDRise[8];
  std::vector<double> *dvdt[8];
  std::vector<double> *thTime[8];
  std::vector<double> *neg_pmax[8];
  std::vector<double> *neg_tmax[8];
  std::vector<double> *rms[8];
  std::vector<double> *pulseArea_withUndershoot[8];
  std::vector<double> *pulseArea_withZeroCross[8];
  std::vector<double> *backBaselineInt_indepBaseCorr[8];
  std::vector<double> *frontBaselineInt_indepBaseCorr[8];

  std::vector<double> localW[8];
  std::vector<double> localT[8];

  double *current;
  double *timestamp;

  TTreeReaderArray<double> *i_w[8];
  TTreeReaderArray<double> *i_w2[8];
  TTreeReaderArray<double> *i_t[8];

  TTreeReaderValue<double> *i_current;
  TTreeReaderValue<double> *i_timestamp;


  public:
    BetaScopeWaveformAna(){};
    BetaScopeWaveformAna( std::string ifile)
    {
      std::cout << "Using user template: " << "BetaScopeWaveformAna" << std::endl;
      this->ifile = ifile;
    };
    ~BetaScopeWaveformAna(){};

    //required, user can add more to the existing methods;
    void initialize();
    void loopEvents();
    void finalize();

    //custom methods.
    void readWaveformConfig(std::string configName);
    static void generateWaveformConfig();
    void loadFile( std::string ifile){this->ifile = ifile;};
    void setWaveform( bool value ){ this->skipWaveform = value; };

    std::string rawFilesDir = "";

};
