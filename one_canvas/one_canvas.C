
Int_t one_canvas(){
    
    #include "../constants.h"

    TCanvas *c = new TCanvas("c", "c", 1400, 600);
    TCanvas *c1;
    TCanvas *c2;

    TFile *f1 = new TFile("/home/garrettl/projects/rrg-jmammei/garrettl/analysis/current-scan/normal/multi_g_add.root");
    TFile *f2 = new TFile("/home/garrettl/projects/rrg-jmammei/garrettl/analysis/current-scan/asym/multi_g_add.root");

    for(Int_t i=0; i<metric.size(); i++){
        for(Int_t j=0; j<n_septant; j++){
            for(Int_t m=0; m<n_ring+1; m++){
                if(m!=5 && m!=0){
                    for(Int_t k=0; k<n_not_sector5+1; k++){
                        TString canvas_name = Form("c_%s_%d_%d_%d", metric[i].Data(), j+1, k, m);
                        c1 = (TCanvas*) f1->Get(canvas_name);
                        c2 = (TCanvas*) f2->Get(canvas_name);
                        
                        c->Clear();
                        c->Divide(2, 1);
                        c->cd(1);
                        c1->DrawClonePad();
                        c->cd(2);
                        c2->DrawClonePad();

                        c->Print(Form("current_scan_by_type/%s.png", canvas_name(2, canvas_name.Length()-1).Data()));
                    }
                } else{
                    for(Int_t k=0; k<n_sector+1; k++){//swtich this to 0 once formatting corected
                        TString canvas_name = Form("c_%s_%d_%d_%d", metric[i].Data(), j+1, k, m);
                        c1 = (TCanvas*) f1->Get(canvas_name);
                        c2 = (TCanvas*) f2->Get(canvas_name);
                        
                        c->Clear();
                        c->Divide(2, 1);
                        c->cd(1);
                        c1->DrawClonePad();
                        c->cd(2);
                        c2->DrawClonePad();
                        
                        c->Print(Form("current_scan_by_type/%s.png", canvas_name(2, canvas_name.Length()-1).Data()));
                    }
                }
            }
        }
    }


    return 0;
}



