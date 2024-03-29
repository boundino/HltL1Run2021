#include <TTree.h>
#include <TFile.h>
#include <TDirectory.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <TString.h>
#include "L1AnalysisEventDataFormat.h"
// #include "L1AnalysisL1CaloTowerDataFormat.h"

#include "maketree.h"

int evtmatching(TString infforest, TString infl1, TString outfile, Long64_t nentries = -1)
{
  TFile* fforest = new TFile(infforest);
  TFile* fl1 = new TFile(infl1);

  TTree* hiroot = (TTree*)fforest->Get("hiEvtAnalyzer/HiTree");
  TTree* skimroot = (TTree*)fforest->Get("skimanalysis/HltTree");
  TTree* zdcdigiroot = (TTree*)fforest->Get("zdcanalyzer/zdcdigi");
  TTree* zdcrechitroot = (TTree*)fforest->Get("zdcanalyzer/zdcrechit");
  TTree* l1EvtTree = (TTree*)fl1->Get("l1EventTree/L1EventTree");
  // TTree* l1TowerTree = (TTree*)fl1->Get("l1CaloTowerEmuTree/L1CaloTowerTree");
  TTree* l1ADC = (TTree*)fl1->Get("HFAdcana/adc");

  TFile* outf = new TFile(outfile, "recreate");
  EvtTowerInfoNTuple* nt = new EvtTowerInfoNTuple(l1ADC, hiroot, skimroot, zdcrechitroot, zdcdigiroot);

  hiroot->SetBranchStatus("*", 0);
  hiroot->SetBranchStatus("run", 1);
  UInt_t run; hiroot->SetBranchAddress("run", &run);
  hiroot->SetBranchStatus("evt", 1);
  ULong64_t evt; hiroot->SetBranchAddress("evt", &evt);
  hiroot->SetBranchStatus("lumi", 1);
  UInt_t lumi; hiroot->SetBranchAddress("lumi", &lumi);
  L1Analysis::L1AnalysisEventDataFormat *Event = new L1Analysis::L1AnalysisEventDataFormat();
  l1EvtTree->SetBranchAddress("Event", &Event);

  std::cout<<"---- Check event number"<<std::endl;
  Long64_t nmaptree = hiroot->GetEntries(), nmatchtree = l1EvtTree->GetEntries();
  // Long64_t nmaptree = l1EvtTree->GetEntries(), nmatchtree = hiroot->GetEntries();
  std::cout<<std::left<<std::setw(17)<<"[ hiroot"<<"] "<<hiroot->GetEntries()<<std::endl;
  std::cout<<std::left<<std::setw(17)<<"[ skimroot"<<"] "<<skimroot->GetEntries()<<std::endl;
  std::cout<<std::left<<std::setw(17)<<"[ zdcdigiroot"<<"] "<<zdcdigiroot->GetEntries()<<std::endl;
  std::cout<<std::left<<std::setw(17)<<"[ zdcrechitroot"<<"] "<<zdcrechitroot->GetEntries()<<std::endl;
  std::cout<<std::left<<std::setw(17)<<"[ l1EvtTree"<<"] "<<l1EvtTree->GetEntries()<<std::endl;
  std::cout<<std::left<<std::setw(17)<<"[ l1ADC"<<"] "<<l1ADC->GetEntries()<<std::endl;

  std::cout<<"---- Read HI tree events"<<std::endl;
  std::vector<UInt_t> hiroot_run;
  std::vector<ULong64_t> hiroot_evt;
  std::vector<UInt_t> hiroot_lumi;
  for(Long64_t k = 0; k<nmaptree; k++)
    {
      if(k % 10000 == 0) std::cout<<k<<" / "<<nmaptree<<"\r"<<std::flush; 
      hiroot->GetEntry(k); //
      hiroot_run.push_back(run);
      hiroot_evt.push_back(evt);
      hiroot_lumi.push_back(lumi);
    }
  std::cout<<std::endl<<"---- Matching events"<<std::endl;
  int countmatch = 0, lastmatch = -1;
  std::vector<Long64_t> matchingtable;
  Long64_t entries = (nentries<nmatchtree&&nentries>0)?nentries:nmatchtree;
  for(Long64_t j = 0; j<entries; j++)
    {
      if(j % 1000 == 0) std::cout<<j<<" / "<<entries<<"\r"<<std::flush;

      l1EvtTree->GetEntry(j); //
      Long64_t k = lastmatch+1, no = 0;
      for(; no < hiroot_run.size(); no++)
        {
          k = k%hiroot_run.size();
          if(hiroot_evt[k]==Event->event && hiroot_lumi[k]==Event->lumi && hiroot_run[k]==Event->run)
            break;
          k++;
        }
      bool matched = no<hiroot_run.size();
      if(!matched) k = -1;
      lastmatch = k;
      //
      matchingtable.push_back(k);
      if(j % 10000 == 0)
        {
          std::cout<<"check matching... ";
          std::cout<<std::left<<std::setw(8)<<Event->run<<std::setw(5)<<Event->lumi<<std::setw(10)<<Event->event<<" | ";
          if(matched)
            std::cout<<std::left<<std::setw(8)<<hiroot_run[k]<<std::setw(5)<<hiroot_lumi[k]<<std::setw(10)<<hiroot_evt[k];
          std::cout<<std::endl;
        }
      if(matched)
        { 
          countmatch++; 
          hiroot_run.erase(hiroot_run.begin() + k);
          hiroot_lumi.erase(hiroot_lumi.begin() + k);
          hiroot_evt.erase(hiroot_evt.begin() + k);
        }
    }
  std::cout<<std::endl;
  std::cout<<"countmatch = "<<countmatch<<std::endl;

  std::cout<<"---- Write to tree"<<std::endl;
  hiroot->SetBranchStatus("*", 1);
  for(Long64_t j = 0; j<entries; j++)
    {
      if(j % 10000 == 0) std::cout<<j<<" / "<<entries<<"\r"<<std::flush;
      Long64_t k = matchingtable[j];
      if(k<0) continue;
      hiroot->GetEntry(k); //
      skimroot->GetEntry(k); //
      zdcdigiroot->GetEntry(k); //
      zdcrechitroot->GetEntry(k); //
      l1ADC->GetEntry(j); //
      // l1EvtTree->GetEntry(j); //
      // l1TowerTree->GetEntry(j); //

      nt->calculate();

      nt->t->Fill();

      // dl1EvtTree->cd();
      // new_l1EvtTree->Fill();
      // dl1TowerTree->cd();
      // new_l1TowerTree->Fill();
      // dl1ADC->cd();
      // new_l1ADC->Fill();
      // dhiroot->cd();
      // new_hiroot->Fill();
      // dskimroot->cd();
      // new_skimroot->Fill();
      // dzdcroot->cd();
      // new_zdcdigiroot->Fill();
      // new_zdcrechitroot->Fill();
    }
  std::cout<<std::endl;

  outf->Write();
  std::cout<<"---- Writing tree done"<<std::endl;
  outf->Close();
  std::cout<<outfile<<std::endl;

  return 0;
}

int main(int argc, char* argv[])
{
  if(argc==5) { return evtmatching(argv[1], argv[2], argv[3], atoi(argv[4])); }
  if(argc==4) { return evtmatching(argv[1], argv[2], argv[3]); }
  return 1;
}
