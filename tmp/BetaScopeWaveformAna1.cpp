#include "BetaScopeWaveformAna.h"

#include <fstream>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "BetaScopeWaveformAna/include/Waveform_Analysis.hpp"
#include "BetaScopeWaveformAna/include/general.hpp"

#include <ctime>

void BetaScopeWaveformAna::initialize()
{
  //required
  this->beta_scope.fileIO_Open( ifile.c_str() );
  BetaScope_AnaFramework::initialize( this->beta_scope, "BetaScope_Ana/BetaScopeWaveformAna/AnaTemplate/myOwnTree.ini" );

  //do your own stuffs here
  for(int chh = 0; chh < this->activeChannels.size(); chh ++)
  {
    int ch = this->activeChannels.at(chh);
    this->w[ch] = &this->beta_scope.iTreeDoubleArrayMap["w"+std::to_string(ch)];
    this->t[ch] = &this->beta_scope.iTreeDoubleArrayMap["t"+std::to_string(ch)];
    this->wRaw[ch] = &this->beta_scope.iTreeDoubleArrayMap["wRaw"+std::to_string(ch)];
    this->tRaw[ch] = &this->beta_scope.iTreeDoubleArrayMap["tRaw"+std::to_string(ch)];
    this->pmax[ch] = &this->beta_scope.iTreeDoubleArrayMap["pmax"+std::to_string(ch)];
    this->tmax[ch] = &this->beta_scope.iTreeDoubleArrayMap["tmax"+std::to_string(ch)];
    this->neg_pmax[ch] = &this->beta_scope.iTreeDoubleArrayMap["neg_pmax"+std::to_string(ch)];
    this->neg_tmax[ch] = &this->beta_scope.iTreeDoubleArrayMap["neg_tmax"+std::to_string(ch)];

    this->riseTime[ch] = &this->beta_scope.iTreeDoubleArrayMap["riseTime"+std::to_string(ch)];
    this->dvdt[ch] = &this->beta_scope.iTreeDoubleArrayMap["dvdt"+std::to_string(ch)];
    this->cfd_fall[ch] = &this->beta_scope.iTreeDoubleArrayMap["cfd_fall"+std::to_string(ch)];
    this->cfd[ch] = &this->beta_scope.iTreeDoubleArrayMap["cfd"+std::to_string(ch)];
    this->fineCFDRise[ch] = &this->beta_scope.iTreeDoubleArrayMap["fineCFDRise"+std::to_string(ch)];
    this->thTime[ch] = &this->beta_scope.iTreeDoubleArrayMap["thTime"+std::to_string(ch)];
    this->rms[ch] = &this->beta_scope.iTreeDoubleArrayMap["rms"+std::to_string(ch)];
    this->pulseArea_withUndershoot[ch] = &this->beta_scope.iTreeDoubleArrayMap["pulseArea_withUndershoot"+std::to_string(ch)];
    this->pulseArea_withZeroCross[ch] = &this->beta_scope.iTreeDoubleArrayMap["pulseArea_withZeroCross"+std::to_string(ch)];
  }

}

void BetaScopeWaveformAna::loopEvents()
{
  //fill up your own analysis in the while loop
  bool limiting_search_region_OnOff = this->limitPmaxSearchRange;
  double pmaxSearchRange[2] = {this->pmaxSearchMinRange, this->pmaxSearchMaxRange};

  WaveformAnalysis WaveAna;

  while( this->beta_scope.treeReader->Next() )
  {
    std::time_t t1 = std::time(nullptr);
    //loop through all the possible channels
    for(int chh = 0; chh < this->activeChannels.size(); chh ++)
    {
      int ch = this->activeChannels.at(chh);
      //if(std::find(this->activeChannels.begin(), this->activeChannels.end(), ch) != this->activeChannels.end())
      if(true)
      {
        //if( this->beta_scope.iTreeDoubleArrayMap.count("w"+std::to_string(ch) ))
        if(true)
        {

          for(std::size_t i=0, max = this->beta_scope.iTreeDoubleArrayMap["w"+std::to_string(ch)]->GetSize(); i<max; i++)
          {
            //if( std::find(this->invertChannels.begin(), this->invertChannels.end(), ch) != this->invertChannels.end() )
            if(this->invertChannels.at(chh) != 0)
            {
              this->w[ch]->push_back( this->beta_scope.iTreeDoubleArrayMap["w"+std::to_string(ch)]->At(i) * -1.0 * this->voltageMultiFactor );
            }
            else
            {
              this->w[ch]->push_back( this->beta_scope.iTreeDoubleArrayMap["w"+std::to_string(ch)]->At(i) * this->voltageMultiFactor );
            }
            this->t[ch]->push_back( this->beta_scope.iTreeDoubleArrayMap["t"+std::to_string(ch)]->At(i) * this->timeMultiFactor );
            this->wRaw[ch]->push_back( this->beta_scope.iTreeDoubleArrayMap["w"+std::to_string(ch)]->At(i) * this->voltageMultiFactor );
            this->tRaw[ch]->push_back( this->beta_scope.iTreeDoubleArrayMap["t"+std::to_string(ch)]->At(i) * this->timeMultiFactor );
          }

          // Base line
          std::vector<double> front_temp_voltage = *this->w[ch];
          std::vector<double> front_temp_time = *this->t[ch];

          std::vector<double> back_temp_voltage = *this->w[ch];
          std::vector<double> back_temp_time = *this->t[ch];


          std::pair<double,unsigned int> pmax_before_baseline = WaveAna.Find_Singal_Maximum( *this->w[ch], *this->t[ch], limiting_search_region_OnOff, pmaxSearchRange);
          double tmax_for_baseline = this->t[ch]->at(pmax_before_baseline.second);
          double temp_riseTime = WaveAna.Find_Rise_Time( *this->w[ch], *this->t[ch], pmax_before_baseline, 0.1, 0.9 );

          double temp_front_searchRange[2] = {tmax_for_baseline-temp_riseTime-15000.0, tmax_for_baseline-temp_riseTime-10000.0};
          std::pair<double,unsigned int> front_baseline_pmax = WaveAna.Find_Singal_Maximum( *this->w[ch], *this->t[ch], true, temp_front_searchRange );
          double front_baseline_tmax = this->t[ch]->at(front_baseline_pmax.second);
          double front_baseline_riseTime = WaveAna.Find_Rise_Time( *this->w[ch], *this->t[ch], front_baseline_pmax, 0.1, 0.9 );
          double temp_baselineRange[2] = {front_baseline_tmax-front_baseline_riseTime-5000.0, front_baseline_tmax+5000.0};
          WaveAna.Correct_Baseline3(front_temp_voltage, front_temp_time, temp_baselineRange );
          std::pair<double,unsigned int> front_baseline_pmax_corr = WaveAna.Find_Singal_Maximum( front_temp_voltage, front_temp_time, true, temp_front_searchRange );
          //this->frontBaselineInt_indepBaseCorr[b]->push_back( WaveAna.Pulse_Integration_with_Fixed_Window_Size( front_temp_voltage, front_temp_time, front_baseline_pmax_corr, "Simpson", 1000.0, 3000.0) );


          double temp_back_searchRange[2] = {tmax_for_baseline+10000.0, tmax_for_baseline+15000.0 };
          std::pair<double,unsigned int> back_baseline_pmax = WaveAna.Find_Singal_Maximum ( *this->w[ch], *this->t[ch], true, temp_back_searchRange);
          double back_baseline_tmax = this->t[ch]->at(back_baseline_pmax.second);
          double back_baseline_riseTime = WaveAna.Find_Rise_Time( *this->w[ch], *this->t[ch], back_baseline_pmax, 0.1, 0.9 );
          double temp_back_baselineRange[2] = {back_baseline_tmax-back_baseline_riseTime-5000.0, back_baseline_tmax+5000.0 };
          WaveAna.Correct_Baseline3(back_temp_voltage, back_temp_time, temp_back_baselineRange );
          std::pair<double,unsigned int> back_baseline_pmax_corr = WaveAna.Find_Singal_Maximum( back_temp_voltage, back_temp_time, true, temp_back_searchRange );
          //this->backBaselineInt_indepBaseCorr[b]->push_back( WaveAna.Pulse_Integration_with_Fixed_Window_Size( back_temp_voltage, back_temp_time, back_baseline_pmax_corr, "Simpson", 1000.0, 3000.0) );

          double baselineRange[2] = {tmax_for_baseline-temp_riseTime-3000.0, tmax_for_baseline-temp_riseTime-1000.0 };
          WaveAna.Correct_Baseline3( *this->w[ch], *this->t[ch], baselineRange);

          if( ch == this->triggerCh )limiting_search_region_OnOff=false;
          else limiting_search_region_OnOff = true;

          std::pair<double,unsigned int> pmaxHolder = WaveAna.Find_Singal_Maximum( *this->w[ch], *this->t[ch], limiting_search_region_OnOff, pmaxSearchRange );
          std::pair<double,unsigned int> neg_pmaxHolder = WaveAna.Find_Negative_Signal_Maximum( *this->w[ch], *this->t[ch], limiting_search_region_OnOff, pmaxSearchRange);

          this->pmax[ch]->push_back( pmaxHolder.first );
          this->neg_pmax[ch]->push_back( neg_pmaxHolder.first );

          for(int k =0; k<101; k++){ double percentage = 1.0*k; this->cfd[ch]->push_back( WaveAna.Rising_Edge_CFD_Time( percentage, *this->w[ch], *this->t[ch], pmaxHolder ) ); }

          for(int k =0; k<101; k++){ this->cfd_fall[ch]->push_back( WaveAna.Falling_Edge_CFD_Time( k, *this->w[ch], *this->t[ch], pmaxHolder ) ); }

          for( int step = 0; step < 500; step++ ){ double percentage = 0.2 * step; this->fineCFDRise[ch]->push_back( WaveAna.Rising_Edge_CFD_Time( percentage, *this->w[ch], *this->t[ch], pmaxHolder ) ); }

          //========================================================================
          //Filling Tmax
          this->tmax[ch]->push_back( WaveAna.Get_Tmax( *this->t[ch], pmaxHolder) );
          this->neg_tmax[ch]->push_back( WaveAna.Get_Tmax( *this->t[ch], neg_pmaxHolder) );


          this->rms[ch]->push_back( WaveAna.Find_Noise( *this->w[ch], 0.25*this->w[ch]->size() ) );

          this->pulseArea_withUnderShoot[ch]->push_back( WaveAna.Pulse_Integration_with_Fixed_Window_Size( *this->w[ch], *this->t[ch], pmaxHolder, "Simpson", 1000.0, 3000.0) );
          this->pulseArea_withZeroCross[ch]->push_back( WaveAna.Find_Pulse_Area( *this->w[ch], *this->t[ch], pmaxHolder) );

          //this->frontBaselineInt[b]->push_back( WaveAna.Pulse_Integration_with_Fixed_Window_Size( this->w[ch], this->t[ch], pmaxHolder, "Simpson", 12000.0, -8000.0) );
          //this->backBaselineInt[b]->push_back( WaveAna.Pulse_Integration_with_Fixed_Window_Size( this->w[ch], this->t[ch], pmaxHolder, "Simpson", -8000.0, 12000.0) );

          //------------------Rise Time 10 to 90---------------------------------------------------------

          this->riseTime[ch]->push_back( WaveAna.Find_Rise_Time( *this->w[ch], *this->t[ch], pmaxHolder, 0.1, 0.9 ) );

          //--------------------------------------------------------------------------------------------

          for(int d =0; d<101; d++){ this->dvdt[ch]->push_back( WaveAna.Find_Dvdt( d, 0, *this->w[ch], *this->t[ch], pmaxHolder ) ); }

          for(int k = 0; k < 2000; k++){ this->thTime[ch]->push_back( WaveAna.Find_Time_At_Threshold( double(k), *this->w[ch], *this->t[ch], pmaxHolder ) ); }

          for( unsigned int invI = 0; invI < this->w[ch]->size(); invI++)
          {
            this->w[ch]->at(invI) = -1.0 * this->w[ch]->at(invI);
          }

        }
      }
    }

    //filling value that's indep of scope channels
    if( this->beta_scope.currentFromDAQ )*this->beta_scope.oTreeDoubleMap["current"] = **this->beta_scope.iTreeDoubleValueMap["i_current"];
    if( this->beta_scope.timestampFromDAQ )*this->beta_scope.oTreeDoubleMap["timestamp"] = **this->beta_scope.iTreeDoubleValueMap["i_timestamp"];


    //required
    BetaScope_AnaFramework::filldata( this->beta_scope );
    std::time_t t2 = std::time(nullptr);
    //std::cout << t2-t1 << std::endl;
    //std::cout << this->beta_scope.oTreeVecDoubleMap["thTime2"]->size() <<std::endl;
  }
}

void BetaScopeWaveformAna::finalize()
{
  //do your own stuffs here


  //required
  BetaScope_AnaFramework::finalize(this->beta_scope);
}











//custom class methods start from here
void BetaScopeWaveformAna::readWaveformConfig( std::string configName )
{
  boost::property_tree::ptree pt;
  boost::property_tree::ini_parser::read_ini(configName, pt);

  this->voltageMultiFactor = pt.get<double>("General.voltageMultiFactor");
  this->timeMultiFactor = pt.get<double>("General.timeMultiFactor");
  this->voltageMultiFactor = pt.get<double>("General.voltageMultiFactor");
  this->limitPmaxSearchRange = pt.get<bool>("General.limitPmaxSearchRange");
  this->pmaxSearchMinRange = pt.get<double>("General.pmaxSearchMinRange");
  this->pmaxSearchMinRange = pt.get<double>("General.pmaxSearchMinRange");
  this->assistThreshold = pt.get<double>("General.assistThreshold");
  this->triggerCh = pt.get<int>("General.triggerCh");

  for( int i = 1; i < 7; i++)
  {
    std::string ch = "channel_"+std::to_string(i);
    if( pt.get<int>("Channel_Activation."+ch) !=0 )
    {
      this->activeChannels.push_back(i);
      if( pt.get<int>("Channel_Invertion."+ch) !=0 )this->invertChannels.push_back(1);
      else this->invertChannels.push_back(0);
    }

  }

}

void BetaScopeWaveformAna::generateWaveformConfig()
{
  std::ofstream configFile("WaveformAnaConfig.ini");

  configFile << "[General]" << std::endl;
  configFile << "voltageMultiFactor = 1000.0" << std::endl;
  configFile << "timeMultiFactor = 1.0e12" << std::endl;
  configFile << "limitPmaxSearchRange = true" << std::endl;
  configFile << "pmaxSearchMinRange = -2200.0 " << std::endl;
  configFile << "pmaxSearchMinRange = 2200.0 " << std::endl;
  configFile << "assistThreshold = 10.0 " << std::endl;
  configFile << "triggerCh = 3 " << std::endl;

  configFile << std::endl;

  configFile << "[Channel_Activation]" << std::endl;
  for( int i = 1; i < 7; i++)
  {
    configFile << "channel_" << i << " = 0" << std::endl;
  }

  configFile << std::endl;

  configFile << "[Channel_Invertion]" << std::endl;
  for( int i = 1; i < 7; i++)
  {
    configFile << "channel_" << i << " = 0" << std::endl;
  }
}
