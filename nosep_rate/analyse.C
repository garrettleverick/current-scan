using namespace ROOT;

int analyse(TString source, TString out, TString generator){

#include "/home/garrettl/projects/rrg-jmammei/garrettl/analysis/mag_over5/constants.h"

TChain T("T");
T.Add(Form("%s", source.Data())); // Adding source file
Int_t nEvents = T.GetEntries();  // Number of primary events
std::cout << "Analyzing "<< nEvents << " events" << std::endl;

Double_t weight= 0.0;
if(generator=="beam"){
    weight=1.0/(nEvents*1.602*1e-4*7);
}else {
    weight=1e-9/85/7; // Divide by current and number of septants. So, the Y-axis gets units of GHz/uA/sep. 
}

TFile f(Form("%s", out.Data()), "RECREATE");

std::map<TString, TH1D*> h;
std::map<TString, TH1D*> h_fom;
std::map<TString, TH1D*> h_asy;

Double_t size_septant=2.0*TMath::Pi()/n_septant;
std::vector<Double_t> off_septant;
TString part;

Float_t Rmin []= {640.0, 640.0, 680.0, 720.0, 785.0, 875.0, 1035.0, 1200.0}; //can be made into array so each sector's radii may be different
Float_t Rmax []= {1500.0, 680.0, 720.0, 785.0, 875.0, 1035.0, 1140.0, 1500.0};

//Setting up histograms
for(Int_t j=0; j<n_sector+1; j++){
    for(Int_t k=0; k<n_ring+1; k++){
        for(Int_t l=0; l<p_type.size(); l++){
            for(Int_t m=0; m<p_nrg.size(); m++){
                part= Form("%s_%s_%d_%d", p_type[l].Data(), p_nrg[m].Data(), j, k);
//                    std::cout<< part<< std::endl;
          
                h[part]=new TH1D(part, Form("%s rate, Generator=%s, Rrange= [%3.0f-%3.0f mm]", part.Data(), generator.Data(), Rmin[k], Rmax[k]), 400, Rmin[k], Rmax[k]);
                
                /*
                h_fom[part]=new TH1D(part+"_fom", Form("%s rate, Generator=%s, Rrange= [%3.0f-%3.0f mm]", part.Data(), generator.Data(), Rmin[k], Rmax[k]), 400, Rmin[k], Rmax[k]);

                if(generator=="moller"){
                    h_asy[part]=new TH1D(part+"_asy", Form("%s FOM, Generator=%s, Rrange =  [%3.0f-%3.0f mm]", part.Data(), generator.Data(), Rmin[k], Rmax[k]), 300, -60, 0);

                }else if(generator=="elastic"){
                    h_asy[part]=new TH1D(part+"_asy", Form("%s asy, Generator=%s, Rrange= [%3.0f-%3.0f mm]", part.Data(), generator.Data(), Rmin[k], Rmax[k]), 50, -150, 0);

                }else{
                    h_asy[part] = new TH1D(part+"_asy", Form("%s asy, Generator=%s, Rrange=  [%3.0f-%3.0f mm]", part.Data(), generator.Data(), Rmin[k], Rmax[k]), 400, -2000, 0);
                }*/
            }
        }        
    } 
}

Double_t fRate=0;
remollEvent_t *fEvent=0;
std::vector<remollGenericDetectorHit_t>  *fHit=0;
T.SetBranchAddress("ev", &fEvent);//what is likely happening here is SetBranchAddress is passing out the address as a pointer
T.SetBranchAddress("hit", &fHit);
T.SetBranchAddress("rate", &fRate);

//going through the data
for (size_t j=0;j< nEvents;j++){
    T.GetEntry(j);
    std::cout << j << std::endl;
    for (size_t i=0;i<fHit->size();i++){
        if(generator=="beam"){
            fRate=1.0;
        } 
        remollGenericDetectorHit_t hit=fHit->at(i);
       
        Bool_t hit_planedet = hit.det==28 ; 
        Int_t prim_track=0; // maximum track of primary
        if (generator=="moller"){
            prim_track=2;
        } 
        else{
            prim_track=1;
        }
        
        if (!hit_planedet || hit.r<Rmin[0] || hit.r>Rmax[0]) { continue; }
        
        std::map<TString, Bool_t> hit_type = {
            {"all", 1},
            {"primary", hit.trid<=prim_track},
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

        //sectioning off the septant
        Double_t sepdiv=2*TMath::Pi()/7.0;
        Int_t sec=0;
        Double_t phi=atan2(hit.y,hit.x);
        if (phi<0) {phi+=2*TMath::Pi();}
        Double_t secphi = fmod(phi, 2*TMath::Pi()/7);
        if      (secphi<TMath::Pi()/84.0)   {sec=1;}           
        else if (secphi<3*TMath::Pi()/84.0) {sec=2;}   
        else if (secphi<5*TMath::Pi()/84.0) {sec=3;}   
        else if (secphi<7*TMath::Pi()/84.0) {sec=4;}   
        else if (secphi<9*TMath::Pi()/84.0) {sec=5;}   
        else if (secphi<11*TMath::Pi()/84.0) {sec=6;} 
        else if (secphi<13*TMath::Pi()/84.0) {sec=7;}   
        else if (secphi<15*TMath::Pi()/84.0) {sec=8;} 
        else if (secphi<17*TMath::Pi()/84.0) {sec=9;}   
        else if (secphi<19*TMath::Pi()/84.0) {sec=10;} 
        else if (secphi<21*TMath::Pi()/84.0) {sec=11;}
        else if (secphi<23*TMath::Pi()/84.0) {sec=12;}   
        else {sec=13;}                                   

        Int_t ring=0;
        Double_t rad=hit.r;
        for(Int_t i=1; i<n_ring+1; i++){ //starts at 1 to skip 0 (which is all)
            if (rad<Rmax[i] && rad>=640.0){
                ring = i;
                break;
            }
        }

        TVector2 xy(hit.x, hit.y);
        Bool_t bool_septant;
        Bool_t bool_sector;
        Bool_t bool_ring;

        std::map<TString, Bool_t> hit_pid;

        for (Int_t j=0; j<n_sector+1 ;j++){
            for (Int_t k=0; k<n_ring+1; k++){
                for (Int_t l=0; l<p_type.size(); l++){
                    for (Int_t m=0; m<p_nrg.size(); m++){
                        part= Form("%s_%s_%d_%d", p_type[l].Data(), p_nrg[m].Data(), j, k);
                        if(j==0) {bool_sector=1;} else {bool_sector= sec==j;}; 
                        if(k==0) {bool_ring=1;} else {bool_ring= ring==k;}; 
                        hit_pid[part]= bool_sector && bool_ring && hit_type[p_type[l]] && hit_nrg[p_nrg[m]];
                    }
                }
            }
        }

        //fill the histograms
        for (Int_t j=0; j<n_sector+1 ;j++){
            for (Int_t k=0; k<n_ring+1; k++){
                for (Int_t l=0; l<p_type.size(); l++){
                    for (Int_t m=0; m<p_nrg.size(); m++){
                        part= Form("%s_%s_%d_%d", p_type[l].Data(), p_nrg[m].Data(), j, k);
                        if (hit_pid[part]){                             
                            h[part]->Fill(hit.r, (fRate)*weight);
                            //h_fom[part]->Fill(hit.r, (fRate)*(fEvent->A)*(fEvent->A)*weight);                
                            //h_asy[part]->Fill(fEvent->A, fRate*weight);
                        }
                    }
                }
            }  
        } 
    }
}


for (Int_t j=0; j<n_sector+1; j++){
    for (Int_t k=0; k<n_ring+1; k++){
        for (Int_t l=0; l<p_type.size(); l++){
            for (Int_t m=0; m<p_nrg.size(); m++){
                part= Form("%s_%s_%d_%d", p_type[l].Data(), p_nrg[m].Data(), j, k);
                //std::cout<< part << std::endl;
                h[part]->Write("", TObject::kOverwrite); 
                //h_fom[part]->Write("", TObject::kOverwrite);
                //h_asy[part]->Write("", TObject::kOverwrite);
            }
        }
    } 
}   


return 0;

}
