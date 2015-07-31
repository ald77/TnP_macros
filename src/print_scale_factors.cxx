#include <vector>

#include "TString.h"
#include "TDirectory.h"
#include "TList.h"
#include "TKey.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TH2D.h"
#include "TH2F.h"
#include "TCanvas.h"

using namespace std;

void PrintDirectory(TDirectory &mc_dir, TDirectory &data_dir,
		    const TString &measured, const TString &wp){
  TString lower_wp = wp;
  lower_wp.ToLower();

  TList * keys = data_dir.GetListOfKeys();
  if(keys == NULL) return;
  for(int i = 0; i < keys->GetSize(); ++i){
    TObject *key = keys->At(i);
    if(key == NULL) continue;
    TString name = key->GetName();
    if(name == "w") continue;
    TObject *obj = data_dir.Get(name);
    if(obj == NULL) continue;
    TString class_name = obj->ClassName();
    if(class_name.Contains("TCanvas")){
      TCanvas *c_data = static_cast<TCanvas*>(data_dir.Get(name));
      TCanvas *c_mc = static_cast<TCanvas*>(mc_dir.Get(name));
      if(c_data == NULL || c_mc == NULL) continue;
      TList *list = c_data->GetListOfPrimitives();
      for(int j = 0; j < list->GetSize(); ++j){
	TObject *prim = list->At(j);
	class_name = prim->ClassName();
	name = prim->GetName();
	if(class_name.Contains("TH1") || class_name.Contains("TH2")){
	  TH1 *h_data = static_cast<TH1*>(c_data->GetPrimitive(name));
	  TH1 *h_mc = static_cast<TH1*>(c_mc->GetPrimitive(name));
	  if(h_data == NULL || h_mc == NULL) continue;
	  h_data->Divide(h_mc);
	  h_data->SetMinimum(0.5);
	  h_data->SetMaximum(2.0);
	  TCanvas c;
	  c.SetLogz();
	  h_data->Draw();
	  c.Print("plots/SF_"+measured+"_"+wp+"_"+name+".pdf");
	}
      }
    }else if(class_name.Contains("TDirectory")){
      TDirectory *d_data = static_cast<TDirectory*>(data_dir.Get(name));
      TDirectory *d_mc = static_cast<TDirectory*>(mc_dir.Get(name));
      if(d_data == NULL || d_mc == NULL) continue;
      PrintDirectory(*d_mc, *d_data, measured, wp);
    }
  }
}

void PrintScaleFactors(const TString &measured, const TString &wp){
  TString lower_wp = wp;
  lower_wp.ToLower();

  TString file_name = measured == "id" ? "data/eff_mc_id.root"
    : "data/eff_mc_"+lower_wp+".root";
  TFile mc(file_name, "read");
  file_name.ReplaceAll("mc","data");
  TFile data(file_name, "read");

  TString path = "GsfElectronToID/"+wp;
  if(measured != "id"){
    path.ReplaceAll("GsfElectronToID",wp+"ElectronToMiniIso");
  }

  TDirectory *mc_dir = static_cast<TDirectory*>(mc.Get(path));
  TDirectory *data_dir = static_cast<TDirectory*>(data.Get(path));

  if(mc_dir == NULL || data_dir == NULL) return;

  PrintDirectory(*mc_dir, *data_dir, measured, wp);
}

int main(){
  vector<TString> measured, wp;
  measured.push_back("id");
  measured.push_back("iso");
  wp.push_back("Veto");
  wp.push_back("Loose");
  wp.push_back("Medium");
  wp.push_back("Tight");

  for(size_t imeasured = 0; imeasured < measured.size(); ++imeasured){
    for(size_t iwp = 0; iwp < wp.size(); ++iwp){
      PrintScaleFactors(measured.at(imeasured), wp.at(iwp));
    }
  }
}
