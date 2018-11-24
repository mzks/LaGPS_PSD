
TH1D* sliceHist(int runnum = 5, double min = 0.07e-6, double max = 0.12e-6);

int projSI(){
    gROOT->SetStyle("ATLAS");

    auto hi5 = sliceHist(5, 0.6, 0.9);
    auto hi6 = sliceHist(6, 0.6, 0.9);
    hi5->Rebin(4);
    hi6->Rebin(4);
    hi6->SetLineColor(kRed);

    hi6->SetTitle(";Shape Indicator;Counts (a.u.)");
    cout << hi5->GetBinWidth(0);

    hi6->Draw();
    hi5->Draw("same");

    auto leg = new TLegend();
    leg->AddEntry(hi5,"Gamma","l");
    leg->AddEntry(hi6,"Alpha","l");
    leg->Draw("same");

    return 0;
}

TH1D* sliceHist(int runnum = 5, double min = 0.07e-6, double max = 0.12e-6){

    auto f = TFile::Open(Form("../areaVsSI/hist%01d.root",runnum));
    auto areaVsSI = dynamic_cast<TH2D*>(f->Get(Form("si%01d",runnum)));

    Int_t minBin = areaVsSI->GetXaxis()->FindBin(min);
    Int_t maxBin = areaVsSI->GetXaxis()->FindBin(max);
    auto hist = areaVsSI->ProjectionY(Form("slice%01d-%013f_%013f",runnum,min*1.e6,max*1.e6),minBin,maxBin,"");
    cout << areaVsSI->GetEntries() << "  " << hist->GetEntries() << endl;

    return hist;
}
