
int plot_save(TString metric, Int_t septant, Int_t sector, Int_t ring, std::vector<Int_t> colour, std::vector<Int_t> m_style, TFile *f, TCanvas *c, std::vector<Double_t> scale){
 
    TMultiGraph *mg = new TMultiGraph(); 

    std::vector<std::vector<std::vector<TString>>> graphs;
    //graphs.push_back({{list of gen}, {list of p_type}, {list of p_nrg}, {title}, {scale}}); is the format
    graphs.push_back({{"moller"}, {"primary"}, {"1_to_10", "10_to_100", "100_to_1000", "gte_1000"}, {"moller_prim_>1MeV"}, {"1"}});
    graphs.push_back({{"elastic"}, {"primary"}, {"1_to_10", "10_to_100", "100_to_1000", "gte_1000"}, {"elastic_prim_>1MeV"}, {"1"}});
    graphs.push_back({{"inelastic"}, {"primary"}, {"1_to_10", "10_to_100", "100_to_1000", "gte_1000"}, {"inelastic_prim_>1MeV"}, {"10"}});
    graphs.push_back({{"elastic", "moller", "inelastic"}, {"primary"}, {"1_to_10", "10_to_100", "100_to_1000", "gte_1000"}, {"all_prim_>1MeV"}, {"1"}});
    graphs.push_back({{"beam"}, {"primary"}, {"1_to_10", "10_to_100", "100_to_1000", "gte_1000"}, {"beam_prim_>1MeV"}, {"1"}});

    TString title;

    Double_t * tmp_y_val;
    Double_t * tmp_y_err;
    std::vector<Double_t> y_val(scale.size(), 0.0);
    std::vector<Double_t> y_err_sq(scale.size(), 0.0);
    std::vector<Double_t> y_err(scale.size(), 0.0);
    std::vector<Double_t> scale_err(scale.size(), 0.0);    
    
    for(Int_t i=0; i<graphs.size(); i++){ //loop over things on same TMultiGraph
        std::fill(y_val.begin(), y_val.end(), 0.0);
        std::fill(y_err.begin(), y_err.end(), 0.0); 
        std::fill(y_err_sq.begin(), y_err_sq.end(), 0.0); 
        Double_t scale_plot = graphs[i][4][0].Atof();
        for(Int_t j=0; j<graphs[i][0].size(); j++){
            for(Int_t k=0; k<graphs[i][1].size(); k++){
                for(Int_t l=0; l<graphs[i][2].size(); l++){
                    TString g_name = Form("%s_%s_%s_%s_%d_%d_%d", graphs[i][0][j].Data(), metric.Data(), graphs[i][1][k].Data(), graphs[i][2][l].Data(), septant, sector, ring);
//                    std::cout << g_name << std::endl;
                    TGraphErrors *g1 = (TGraphErrors*) f->Get(g_name);
                    tmp_y_val = g1->GetY();
                    tmp_y_err = g1->GetEY();                    
                    for(Int_t m=0; m<scale.size(); m++){
                        y_val[m] += scale_plot*tmp_y_val[m];
                        y_err_sq[m] += scale_plot*scale_plot*tmp_y_err[m]*tmp_y_err[m];
                    }
                    for(Int_t m=0; m<scale.size(); m++){
                        y_err[m] = sqrt(y_err_sq[m]);
                    }

                }
            }
        }   
        
        TGraphErrors *g_out = new TGraphErrors(scale.size(), &scale[0], &y_val[0], &scale_err[0], &y_err[0]);
        title = Form("%s X %s", graphs[i][3][0].Data(), graphs[i][4][0].Data());
        g_out->SetTitle(title);
        g_out->SetMarkerColor(colour[i]);
        g_out->SetLineColor(colour[i]);
        g_out->SetMarkerStyle(m_style[i]);
        mg->Add(g_out);
    
    }
                        
    mg->SetTitle(Form("%s vs Scale, Septant: %d, Sector: %d, Ring: %d; Magnetic Field Scaling Factor; Rate [GHz]", metric.Data(), septant, sector, ring)); 
    mg->GetXaxis()->CenterTitle(1);
    mg->GetYaxis()->CenterTitle(1);
    mg->GetXaxis()->SetLimits(0.65, 1.35);
    mg->Draw("AP");
    
    TString out_name = Form("%s_%d_%d_%d", metric.Data(), septant, sector, ring);

    c->Update(); 
    c->BuildLegend(0.7, 0.1, 0.9, 0.25);
    c->Print(Form("curscan_prims_w_newbeam/%s.png", out_name.Data()));

//    f_out.cd();
    mg->Write(out_name);
    return 0;
}



int multi_g_add(){
    #include "../constants.h"
    TFile *f = new TFile("/home/garrettl/projects/rrg-jmammei/garrettl/analysis/current-scan/collect.root");
    TFile f_out("multi_g_add.root", "RECREATE");   
    
    std::vector<Int_t> colour = {1, 2, 3, 4, 6, 46};
    std::vector<Int_t> m_style = {20, 21, 22, 23, 29, 33, 43, 31, 34};

    TCanvas *c = new TCanvas("c", "c", 800, 600);
    
    f_out.cd();

    //loop over each TMultiGraph you want to make
    for(Int_t l=0; l<metric.size(); l++){
        for(Int_t j=0; j<n_septant; j++){
            for(Int_t m=0; m<n_ring+1; m++){
                if(m!=5 && m!=0){
                    for(Int_t k=0; k<n_not_sector5+1; k++){
                        plot_save(metric[l], j+1, k, m, colour, m_style, f, c, scale);
                    }
                } else{
                    for(Int_t k=0; k<n_sector+1; k++){//swtich this to 0 once formatting corected
                        plot_save(metric[l], j+1, k, m, colour, m_style, f, c, scale);
                    }
                } 
            }
        }
    }

    return 0;

}




