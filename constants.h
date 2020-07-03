//Contains all constants and changeable variable for a simulation

Int_t n_septant = 7;
Int_t n_sector = 13;
Int_t n_not_sector5 = 5;
Int_t n_ring = 7;

//std::vector<Double_t> scale = {0.700, 0.775, 0.850, 0.925, 1.000, 1.075, 1.150, 1.225, 1.300};
std::vector<Double_t> scale = {0.700, 0.750, 0.800, 0.850, 0.900, 0.950, 1.000, 1.050, 1.100, 1.150};

std::vector<TString> metric;
metric.push_back("rate");
//metric.push_back("asy");
//metric.push_back("fom");

std::vector<TString> gen;
gen.push_back("moller");
gen.push_back("elastic");
gen.push_back("inelastic");
//gen.push_back("beam");

//MUST CHANGE THE MAP IN rate/analysis.C TO WORK
std::vector<TString> p_type = {"primary", "electron", "positron", "photon", "other"};
std::vector<TString> p_nrg = {"gt_1", "lt_1", "1_to_10", "10_to_100", "100_to_1000", "gte_1000"};
//std::vector<TString> p_type = {"all", "secondary, "primary", "electron", "positron", "photon", "other_woph", "other"};
//std::vector<TString> p_nrg = {"all", "gt_1", "lt_1", "1_to_10", "10_to_100", "100_to_1000", "gte_1000"};



