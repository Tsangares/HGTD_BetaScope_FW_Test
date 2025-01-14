#include "BetaScope_Driver/include/BetaScope_Class.h"

#include "Colorful_Cout/include/Colorful_Cout.h"

#include <string>
#include <iostream>

#include <TTreeReader.h>
#include <TTreeReaderArray.h>
#include <TTreeReaderValue.h>
#include <TThread.h>

bool BetaScope::rawTreeReader( const char* itreeName )
{
  std::string coutPrefix = "BetaScope::rawTreeReader => ";
  ColorCout::print(coutPrefix, "Entering", BOLDGREEN);
  ColorCout::print(coutPrefix, "Preparing raw tree reader.", YELLOW);

  this->treeReader = new TTreeReader( itreeName, this->iFile );
  this->numEvent = this->treeReader->GetEntries(true);

  ColorCout::print(coutPrefix, "Number of events: "+std::to_string(this->numEvent), YELLOW);

  ColorCout::print(coutPrefix, "Looping through raw scope channels.", YELLOW);

  //int branch_counter = 0;

  for( int b = 1, max = 5; b < max; b++ )
  {
    auto check_volName2 = ((TTree *) this->iFile->Get(iTreeName.c_str()))->GetListOfBranches()->FindObject(Form("t%i",b) );
    if(check_volName2!=NULL){
      auto br_check = readBranch<TTreeReaderArray<double>>( this->treeReader, Form("w%i", b ), Form("w%i", b ), &this->iTreeDoubleArrayMap, this->iTreeDoubleArray[this->iTreeBranchCounter], this->iTreeBranchCounter, &this->iTreeDoubleArrayMapIndex );
      br_check = readBranch<TTreeReaderArray<double>>( this->treeReader, Form("t%i", b ), Form("t%i", b ), &this->iTreeDoubleArrayMap, this->iTreeDoubleArray[this->iTreeBranchCounter], this->iTreeBranchCounter, &this->iTreeDoubleArrayMapIndex );
      this->channel.push_back(b);
    }
  }
  auto check_current = ((TTree *) this->iFile->Get(iTreeName.c_str()))->GetListOfBranches()->FindObject("i_current");
  if( check_current != NULL )
  {
    ColorCout::print("  > ", "Current branch from DAQ is found.", YELLOW);
    this->currentFromDAQ = true;
    auto br_check = readBranch<TTreeReaderValue<double>>( this->treeReader, "i_current", "i_current", &this->iTreeDoubleValueMap, this->iTreeDoubleValue[this->iTreeBranchCounter], this->iTreeBranchCounter, &this->iTreeDoubleValueMapIndex );
  }

  auto check_timestamp = ((TTree *) this->iFile->Get(iTreeName.c_str()))->GetListOfBranches()->FindObject("i_timestamp");
  if( check_timestamp != NULL )
  {
    ColorCout::print("  > ", "Timestamp branch from DAQ is found.", YELLOW);
    this->timestampFromDAQ = true;
    auto br_check = readBranch<TTreeReaderValue<double>>( this->treeReader, "i_timestamp", "i_timestamp", &this->iTreeDoubleValueMap, this->iTreeDoubleValue[this->iTreeBranchCounter], this->iTreeBranchCounter, &this->iTreeDoubleValueMapIndex );
  }

  auto check_ievent = ((TTree *) this->iFile->Get(iTreeName.c_str()))->GetListOfBranches()->FindObject("ievent");
  if( check_ievent != NULL)
  {
    ColorCout::print("  >", "ievent branch from DAQ is found.", YELLOW);
    this->ieventFromDAQ = true;
    auto br_check = readBranch<TTreeReaderValue<int>>( this->treeReader, "ievent", "ievent", &this->iTreeIntValueMap, this->iTreeIntValue[this->iTreeBranchCounter], this->iTreeBranchCounter, &this->iTreeIntValueMapIndex );
  }

  //this->treeReader->Next();
  //int npoint = this->iTreeDoubleArrayMap["w2"]->GetSize();
  //this->treeReader->Restart();
  //if ( npoint == 0 ){ std::cout << "there is no npoints, check your file" << std::endl; return false;}

  ColorCout::print(coutPrefix, "Finished, exiting", BOLDGREEN);

  return true;
}


bool BetaScope::setBranch( std::string typeName, std::string key, std::string branchName )
{
  if( typeName.compare("TTreeReaderArray<double>") ==0 ){
    this->iTreeDoubleArray[this->iTreeBranchCounter] = new TTreeReaderArray<double>( *this->treeReader, branchName.c_str() );
    this->iTreeDoubleArrayMap.insert( std::pair<std::string, TTreeReaderArray<double> * >(key, this->iTreeDoubleArray[this->iTreeBranchCounter]) );
    this->iTreeDoubleArrayMapIndex.insert( std::pair<std::string, int>(key, this->iTreeBranchCounter) );
    this->iTreeBranchCounter+=1;
    return true;
  }
  else if( typeName.compare("TTreeReaderArray<int>") ==0 ){
    this->iTreeIntArray[this->iTreeBranchCounter] = new TTreeReaderArray<int>( *this->treeReader, branchName.c_str() );
    this->iTreeIntArrayMap.insert( std::pair<std::string, TTreeReaderArray<int>*>(key, this->iTreeIntArray[this->iTreeBranchCounter]) );
    this->iTreeIntArrayMapIndex.insert( std::pair<std::string, int>(key, this->iTreeBranchCounter) );
    this->iTreeBranchCounter+=1;
    return true;
  }
  else if( typeName.compare("TTreeReaderValue<double>") ==0 ){
    this->iTreeDoubleValue[this->iTreeBranchCounter] = new TTreeReaderValue<double>( *this->treeReader, branchName.c_str() );
    this->iTreeDoubleValueMap.insert( std::pair<std::string, TTreeReaderValue<double>*>(key, this->iTreeDoubleValue[this->iTreeBranchCounter]) );
    this->iTreeDoubleValueMapIndex.insert( std::pair<std::string, int>(key, this->iTreeBranchCounter) );
    this->iTreeBranchCounter+=1;
    return true;
  }
  else if( typeName.compare("TTreeReaderValue<int>") ==0 ){
    this->iTreeIntValue[this->iTreeBranchCounter] = new TTreeReaderValue<int>( *this->treeReader, branchName.c_str() );
    this->iTreeIntValueMap.insert( std::pair<std::string, TTreeReaderValue<int>*>(key, this->iTreeIntValue[this->iTreeBranchCounter]) );
    this->iTreeIntValueMapIndex.insert( std::pair<std::string, int>(key, this->iTreeBranchCounter) );
    this->iTreeBranchCounter+=1;
    return true;
  }
  else{
    return false;
  }
}
