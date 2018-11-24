
TH1D* sliceHist(int runnum = 5, int gate = 100, double min = 0.07e-6, double max = 0.12e-6);
int calcFval(int gate = 100, double min = 0.6, double max = 0.9);

int projGate(){
    gROOT->SetStyle("ATLAS");

    const double energyWidth = 0.1;
    double min = 0.1;

    while(min<2.0){
        for(int iGate=10;iGate<700;iGate+=10){
            calcFval(iGate, min, min+energyWidth);
        }
        min += energyWidth;
    }


    return 0;
}


int calcFval(int gate = 100, double min = 0.6, double max = 0.9){
    //min and max for energy (MeV)


    auto hi5 = sliceHist(5, gate, min, max);
    auto hi6 = sliceHist(6, gate, min, max);
    hi5->Rebin(4);
    hi6->Rebin(4);
    hi6->SetLineColor(kRed);

    hi6->SetTitle(";Fast/Total;Counts (a.u.)");
    //cout << hi5->GetBinWidth(0);

    //hi6->Draw();
    //hi5->Draw("same");

    auto leg = new TLegend();
    leg->AddEntry(hi5,"Gamma","l");
    leg->AddEntry(hi6,"Alpha","l");
    //leg->Draw("same");

    double f_measure = 0.;
    int divLineBin = 0.;
    const int totalBin = hi5->GetNbinsX();
    const double a_total = hi6->GetEntries();
    const double g_total = hi5->GetEntries();

    // calc F val 
    for(int iDiv=0;iDiv<totalBin;++iDiv){
        double TP=0., TN=0., FP=0., FN=0.;
        for(int i=0;i<totalBin;++i){
            if(iDiv>i){
                TP += hi6->GetBinContent(i);
                FP += hi5->GetBinContent(i);
            }
            if(iDiv<=i){
                FN += hi6->GetBinContent(i);
                TN += hi5->GetBinContent(i);
            }
        }
        double precision = TP/(TP+FP);
        double recall    = TP/(TP+FN);
        double iF_measure= 2*recall*precision/(recall+precision);

        if(f_measure < iF_measure){
            f_measure = iF_measure;
            divLineBin = iDiv;
        }
    }

    // cout << "F_measure:" << f_measure <<endl;
    // cout << "Div Line :" << hi5->GetBinLowEdge(divLineBin) <<endl;
    cout <<gate << " " << min << " " << max << " " << f_measure << " " << hi5->GetBinLowEdge(divLineBin) <<endl;


    return 0;
}

TH1D* sliceHist(int runnum = 5, int gate = 100, double min = 0.07e-6, double max = 0.12e-6){

    auto f = TFile::Open(Form("../diffGateHists2/gate%01d-%06d.root",runnum,gate));
    auto areaVsSI = dynamic_cast<TH2D*>(f->Get(Form("si%01d",runnum)));

    Int_t minBin = areaVsSI->GetXaxis()->FindBin(min);
    Int_t maxBin = areaVsSI->GetXaxis()->FindBin(max);
    auto hist = areaVsSI->ProjectionY(Form("slice%01d-%013f_%013f",runnum,min*1.e6,max*1.e6),minBin,maxBin,"");
    //cout << areaVsSI->GetEntries() << "  " << hist->GetEntries() << endl;

    return hist;
}
