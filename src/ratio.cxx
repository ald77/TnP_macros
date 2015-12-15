#include <stdexcept>

#include "TFile.h"
#include "TH2D.h"

using namespace std;

int main(int argc, char *argv[]){
  if(argc < 3) throw runtime_error("Need to supply two file names");
  TFile fa(argv[1],"read");
  TFile fb(argv[2],"read");
  if(!fa.IsOpen() || !fb.IsOpen()) throw runtime_error("Could not open files");
  TFile fout("ratio.root","recreate");
  fout.cd();

  TList *keys = fa.GetListOfKeys();
  if(keys == nullptr) throw runtime_error("Could not find list of keys");
  for(int i = 0; i < keys->GetSize(); ++i){
    TObject *key = keys->At(i);
    if(key == nullptr) continue;
    TString name = key->GetName();
    TObject *oa = fa.Get(name);
    TObject *ob = fb.Get(name);
    if(oa == nullptr || ob == nullptr) continue;
    if(!TString(oa->ClassName()).Contains("TH2")
       || !TString(ob->ClassName()).Contains("TH2")) continue;

    TH2 *ha = static_cast<TH2*>(oa);
    TH2 *hb = static_cast<TH2*>(ob);

    ha->Divide(hb);
    fout.cd();
    ha->Write();
  }
  fout.Close();
  fb.Close();
  fa.Close();
}
