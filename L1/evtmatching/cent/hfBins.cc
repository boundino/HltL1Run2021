#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCut.h>

#include <vector>
#include <algorithm>    // std::sort
#include <string>

#include "xjjrootuti.h"
#include "xjjcuti.h"

int macro(std::string inputname, std::string sel, std::string tag, std::string var="HFAdcana/adc::hft")
{
  auto vars = xjjc::str_divide(var, "::");
  auto t = (TTree*)TFile::Open(inputname.c_str())->Get(vars[0].c_str());

  float hft; t->SetBranchAddress(vars[1].c_str(), &hft);
  int nhfp; t->SetBranchAddress("nhfp", &nhfp);
  int nhfn; t->SetBranchAddress("nhfn", &nhfn);
  int mMaxL1HFAdcPlus; t->SetBranchAddress("mMaxL1HFAdcPlus", &mMaxL1HFAdcPlus);
  int mMaxL1HFAdcMinus; t->SetBranchAddress("mMaxL1HFAdcMinus", &mMaxL1HFAdcMinus);

  TH1D* hhft = new TH1D("hhft", ";HF Energy;", 1000, 0, 6.e+3);
  
  TCut cutsel = sel.c_str();
  t->Project("hhft", vars[1].c_str(), cutsel);

  std::vector<float> vhft, vhibins(1, 0);
  for(int i=0; i<t->GetEntries(); i++)
    {
      t->GetEntry(i);
      if(nhfp <= 1 || nhfn <= 1 || mMaxL1HFAdcPlus <= 14 || mMaxL1HFAdcMinus <= 14) continue;
      vhft.push_back(hft);
    }
  int n = vhft.size();
  // std::sort(vhft.begin(), vhft.end(), std::greater<float>());
  std::sort(vhft.begin(), vhft.end());

  for(int i=1; i<200; i++)
    vhibins.push_back(vhft[std::ceil(i/200.*n)-1]);
  vhibins.push_back(1.e+4);

  std::cout<<"const Double_t binTable[nBins+1] = {0";
  for(int i=1; i<=200; i++)
    std::cout<<", "<<vhibins[i];
  std::cout<<"};"<<std::endl;

  std::cout<<"const Double_t binTable[nBins+1] = {0.00";
  for(int i=1; i<=20; i++)
    std::cout<<", "<<vhibins[i*10];
  std::cout<<"};"<<std::endl;

  auto outf = xjjroot::newfile(Form("rootfiles/hft_%s.root", tag.c_str()));
  xjjroot::writehist(hhft);
  outf->Close();

  return 0;
}

int main(int argc, char* argv[])
{
  if(argc==5) return macro(argv[1], argv[2], argv[3], argv[4]);
  if(argc==4) return macro(argv[1], argv[2], argv[3]);
  return 1;
}
