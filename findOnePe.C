//  ____                      ____                  _                        
// |  _ \ _ __ __ ___      __/ ___| _ __   ___  ___| |_ _ __ _   _ _ __ ___  
// | | | | '__/ _` \ \ /\ / /\___ \| '_ \ / _ \/ __| __| '__| | | | '_ ` _  
// | |_| | | | (_| |\ V  V /  ___) | |_) |  __/ (__| |_| |  | |_| | | | | | |
// |____/|_|  \__,_| \_/\_/  |____/| .__/ \___|\___|\__|_|   \__,_|_| |_| |_|
//                                 |_|                                       
//  K. Mizukoshi 2018 Nov. 20
//  Usage: $root drawSpectrum.C

TH1D* getSpectrum(int);

int findOnePe(){

    auto sp5 = getSpectrum(5);
    auto sp6 = getSpectrum(6);
    sp6->SetLineColor(kRed);
    sp5->SetTitle(";Gamma energy (MeV);Consts/2 keV");
    sp5->Draw();
    sp6->Draw("same");

    auto leg = new TLegend();
    leg->AddEntry(sp5,"Gamma","l");
    leg->AddEntry(sp6,"Alpha","l");
    leg->Draw("same");

    return 0;
}

TH1D* getSpectrum(int runnum = 5){
    //runnum 5 - gamma source
    //runnum 6 - alpha source

    gROOT->SetStyle("ATLAS");

    TString filename = Form("../../rootfile/run%03d.root",runnum);
    auto f = TFile::Open(filename.Data());
    auto t = dynamic_cast<TTree*>(f->Get("tree"));

    Int_t           ev;
    vector<double>  *time = nullptr;
    vector<double>  *data = nullptr;

    t->SetBranchAddress("ev", &ev);
    t->SetBranchAddress("time", &time);
    t->SetBranchAddress("data", &data);

    const Double_t wfMin = -2.5e-6;
    const Double_t wfMax =  2.5e-6;
    const Int_t    wfBins=  12496;
    const Double_t wfBinWidth = (wfMax - wfMin)/wfBins;
    const Double_t calFac = 7.726488e6;

    const Int_t totalEv = t->GetEntries();

    // Define Histgrams
    auto areah = new TH1D(Form("area%02d",runnum),Form("area%02d",runnum),1000,-1e-8,3e-7);
    auto energyHist = new TH1D(Form("energy%02d",runnum),Form("energy%02d",runnum),1000,-0.1,1.9);
    Int_t numOfAvwf = 0;

    for(int iEntry=0;iEntry<totalEv;iEntry++){
    // Event Begin ===>
        if(iEntry%100 == 0) cout << "event# " << iEntry << endl;

        t->GetEntry(iEntry);

        Double_t pedestal = 0.;  // Pedestal (ave. first 1000 points)
        Double_t peak = 1.e9;   // Peak (inverted to positive);
        Double_t peakTime = 0.;  // Time of Peak
        Int_t    peakIndex = 0; // Index of Peak
        Double_t area = 0.;  //Integral over pedestal
        Double_t energy = 0.; // area * cal. fac. 7.726488e6
    

        // Set pedestal
        for(int index=0; index<1000;++index){
            pedestal += data->at(index);
        }
        pedestal = pedestal/1000.;  //pedestal

        // Set peak
        for(int index=0; index<data->size();++index){
            if(peak>data->at(index)){
                peak = data->at(index);
                peakTime = time->at(index);
                peakIndex= index;
            }
        }
        peak = -1.*(peak - pedestal); //peak

        // Set area
        for(int i=0;i<data->size()/2;++i){
            area += -1.*wfBinWidth*(data->at(i)-pedestal);
        }
        areah->Fill(area);
        energyHist->Fill(area*calFac);

    } // <=== Event End

    return energyHist;
}
