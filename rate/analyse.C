using namespace ROOT;

int analyse(TString source, TString out, TString generator){

//the particle type and energy partitions are stored in constants.h
#include "/home/garrettl/projects/rrg-jmammei/garrettl/analysis/current-scan/constants.h"

TChain T("T");
T.Add(Form("%s", source.Data())); // Adding source file
Int_t nEvents = T.GetEntries();  // Number of primary events
std::cout << "Analyzing "<< nEvents << " events" << std::endl;

Double_t weight= 0.0; //the weighting is different based on the generator
if(generator=="beam"){
    weight=1.0/(nEvents*1.602*1e-4*7);
} else{
    weight=1e-9/85/7; 
    // Divide by current and number of septants. So, the Y-axis gets units of GHz/uA/sep. 
}

TFile f(Form("%s", out.Data()), "RECREATE");

std::map<TString, TH1D*> h;

Double_t size_septant = 2.0*TMath::Pi()/n_septant;
std::vector<Double_t> off_septant;
TString part;

//Setting up histograms, 
for(Int_t l=0; l<p_type.size(); l++){
    for(Int_t m=0; m<p_nrg.size(); m++){
        part= Form("%s_%s", p_type[l].Data(), p_nrg[m].Data());
        h[part]=new TH1D(part, Form("Theta Distribution from hits in Ring 5, %s, Generator=%s", part.Data(), generator.Data()), 100, 0, 0.030);
    }        
}

Double_t fRate=0;
remollEvent_t *fEvent=0;
std::vector<remollGenericDetectorHit_t>  *fHit=0;
std::vector<remollEventParticle_t> *fPart=0;
T.SetBranchAddress("ev", &fEvent); //getting (the locations of) fEvent, fHit, and fRate from raw data. An event is a stack of hits
T.SetBranchAddress("hit", &fHit);
T.SetBranchAddress("rate", &fRate);
T.SetBranchAddress("part", &fPart);


Int_t prim_track=0; // maximum track of primary, we can't tell which moller e- is the original so they are both given trid 2
if(generator=="moller"){
    prim_track=2;
} else{
    prim_track=1;
}

//Initialise variables for processing the data
Double_t acceptance_angle = 0.06;
Double_t acceptance_rad = 38.0;
std::vector<Int_t> good_track; 
//^ is where tracks we want to keep are stored. The first loop finds the good tracks, the second loop find the hits on the main detector with said tracks
Int_t display_percent = 1;
std::vector<int>::iterator iter;

//going through the data
for (size_t event=0; event<nEvents; event++){
    T.GetEntry(event);
    
    Float_t percent = float(event+1)/nEvents*100;
    if(percent > display_percent){
        std::cout << display_percent << std::endl;
        display_percent+=1;
    }
    
    good_track.clear();

    //we want to keep secondaries from beam gen under a certain angle, and keep secondaries from physics above. Want all prims from physics
    for(size_t k=0; k<fHit->size(); k++){
        remollGenericDetectorHit_t hit = fHit->at(k);
        if(hit.det!=26){continue;} //ignore if hit is not at det 27 (z = 750)
        Float_t theta = abs(atan(sqrt(hit.px*hit.px+hit.py*hit.py)/hit.pz));
        Double_t rad = hit.r;
        //        cout << theta << endl;
        if(theta > acceptance_angle && find(good_track.begin(), good_track.end(), hit.trid) == good_track.end()){
            good_track.push_back(hit.trid);
        }
    }
    
    for(size_t i=0; i<fHit->size(); i++){
        remollGenericDetectorHit_t hit=fHit->at(i);
       
        //continue if not a hit at det 28 or outside the main det radii
        if(hit.det!=28){
            continue;
        }

        iter = find(good_track.begin(), good_track.end(), hit.trid);
        if(iter==good_track.end()){continue;}
        
        Bool_t primary_cond;
        if(generator=="beam"){
            fRate=1.0;
            primary_cond = hit.vz<=-3875 && hit.pid==11;
        } else{
            primary_cond = hit.trid<=prim_track;
        }
        
        //below are maps which evaluate the particle type and energy, these maps are called when filling the histograms
        std::map<TString, Bool_t> hit_type = {
            {"all", 1},
            {"primary", primary_cond},
            {"electron", hit.trid>prim_track && hit.pid==11},
            {"positron", hit.trid>prim_track && hit.pid==-11},
            {"photon", hit.trid>prim_track && hit.pid==22},
            {"secondary_woph", hit.trid>prim_track && hit.pid!=22},
            {"secondary", hit.trid>prim_track},
            {"other", hit.trid>prim_track && hit.pid!=11 && hit.pid!=-11 && hit.pid!=22}
        };

        std::map<TString, Bool_t> hit_nrg = {
            {"all", 1},
            {"gt_1", hit.p>1},
            {"lt_1", hit.p<1},
            {"1_to_10", hit.p>=1 && hit.p<10},
            {"10_to_100", hit.p>=10 && hit.p<100},
            {"100_to_1000", hit.p>=100 && hit.p<1000},
            {"gte_1000", hit.p>=1000}
        };

        std::map<TString, Bool_t> hit_pid;
        for (Int_t l=0; l<p_type.size(); l++){
            for (Int_t m=0; m<p_nrg.size(); m++){
                part= Form("%s_%s", p_type[l].Data(), p_nrg[m].Data());
                hit_pid[part]=hit_type[p_type[l]] && hit_nrg[p_nrg[m]] && hit.r<=1035 && hit.r>=875;
            }
        }
        
        //fill the histograms
        for (Int_t l=0; l<p_type.size(); l++){
            for (Int_t m=0; m<p_nrg.size(); m++){
                part= Form("%s_%s", p_type[l].Data(), p_nrg[m].Data());
                if (hit_pid[part]){        
//                    cout << part << endl;                    
//                    cout << fPart->at(0).th << endl; 
                    h[part]->Fill(fPart->at(hit.trid-1).th, weight);
                }
            }  
        } 
    }    
}

for (Int_t l=0; l<p_type.size(); l++){
    for (Int_t m=0; m<p_nrg.size(); m++){
        part= Form("%s_%s", p_type[l].Data(), p_nrg[m].Data());
        //std::cout<< part << std::endl;
        h[part]->Write("", TObject::kOverwrite); 
    }
}

return 0;

}
