#include <vector>

#include "TColor.h"
#include "TStyle.h"
#include "TROOT.h"
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

void PrintPretty(TH2 *h_data, TH2 *h_mc,
		 const TString &measured, const TString &wp,
		 const TString &ext = ""){
  TCanvas c;
  TH2 *g_data = static_cast<TH2*>(h_data->Clone());
  TH2 *g_mc = static_cast<TH2*>(h_mc->Clone());
  g_data->SetMinimum(0.);
  g_mc->SetMinimum(0.);
  g_data->SetMaximum(1.0);
  g_mc->SetMaximum(1.0);
  g_data->Draw("colz");
  g_data->Draw("textesame");
  c.Print("plots/2d_data_"+measured+"_"+wp+"_"+ext+".pdf");
  g_mc->Draw("colz");
  g_mc->Draw("textesame");
  c.Print("plots/2d_mc_"+measured+"_"+wp+"_"+ext+".pdf");
  g_data->Divide(g_mc);
  g_data->SetMinimum(0.8);
  g_data->SetMaximum(1.25);
  c.SetLogz();
  g_data->GetZaxis()->SetNdivisions(16,8,0,false);
  g_data->Draw("colz");
  g_data->Draw("textesame");
  g_data->GetZaxis()->SetNdivisions(16,8,0,false);
  c.Print("plots/2d_sf_"+measured+"_"+wp+"_"+ext+".pdf");
}

void PrintComparison(TH2 *h_data, TH2 *h_mc,
		     const TString &measured, const TString &wp,
		     const TString &ext = ""){
  vector<TH1D*> rows_data(h_data->GetNbinsY());
  vector<TH1D*> rows_mc(rows_data);
  TCanvas c;
  for(size_t i = 0; i < rows_data.size(); ++i){
    TString app = "_px_";
    app = app + i;
    rows_data.at(i) = h_data->ProjectionX(app+"data", i+1, i+1, "e");
    rows_mc.at(i) = h_mc->ProjectionX(app+"_mc", i+1, i+1, "e");
    rows_data.at(i)->SetLineColor(i+1);
    rows_mc.at(i)->SetLineColor(i+1);
    rows_data.at(i)->SetMarkerColor(i+1);
    rows_mc.at(i)->SetMarkerColor(i+1);
    rows_data.at(i)->SetLineStyle(1);
    rows_mc.at(i)->SetLineStyle(2);
    rows_data.at(i)->SetMarkerStyle(20);
    rows_mc.at(i)->SetMarkerStyle(20);
    rows_data.at(i)->SetMarkerSize(1);
    rows_mc.at(i)->SetMarkerSize(1);
    if(i==0){
      rows_mc.at(i)->Draw();
    }else{
      rows_mc.at(i)->Draw("same");
    }
    rows_data.at(i)->Draw("same");
  }
  c.Print("plots/comp_"+measured+"_"+wp+"_"+ext+".pdf");
}

void PrintDirectory(TDirectory &mc_dir, TDirectory &data_dir,
		    const TString &measured, const TString &wp,
		    const TString &ext = ""){
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
	  if(class_name.Contains("TH2")){
	    PrintComparison(static_cast<TH2*>(h_data),
			    static_cast<TH2*>(h_mc),
			    measured, wp, ext+name+"_");
	    PrintPretty(static_cast<TH2*>(h_data),
			static_cast<TH2*>(h_mc),
			measured, wp, ext+name+"_");
	  }
	  if(h_data == NULL || h_mc == NULL) continue;
	  h_data->Divide(h_mc);
	  h_data->SetMinimum(0.5);
	  h_data->SetMaximum(2.0);
	  TCanvas c;
	  c.SetLogz();
	  h_data->Draw();
	  c.Print("plots/SF_"+measured+"_"+wp+"_"+ext+"_"+name+".pdf");
	}
      }
    }else if(class_name.Contains("TDirectory")){
      TDirectory *d_data = static_cast<TDirectory*>(data_dir.Get(name));
      TDirectory *d_mc = static_cast<TDirectory*>(mc_dir.Get(name));
      if(d_data == NULL || d_mc == NULL) continue;
      PrintDirectory(*d_mc, *d_data, measured, wp, ext+name+"_");
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
  gStyle->SetPaintTextFormat("2.3f");

  const unsigned num = 6;
  double red[num] =   {1.,0.,0.,0.,1.,1.};
  double green[num] = {0.,0.,1.,1.,1.,0.};
  double blue[num] =  {1.,1.,1.,0.,0.,0.};
  double stops[num] = {0.,0.2,0.4,0.6,0.8,1.};
  const int bands = 999;
  int fi = TColor::CreateGradientColorTable(num,stops,red,green,blue,bands);
  int palette[bands];
  for(int i = 0; i < bands; ++i){
    palette[i] = fi+i;
  }
  gStyle->SetNumberContours(bands);
  gStyle->SetPalette(bands, palette);
  
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
