
TH1D* sliceHist(int runnum = 5, double min = 0.07e-6, double max = 0.12e-6);

int fraction(){

    double width = 0.0001;
    for(double max =-0.01+width; max<0.03; max+=width ){
        auto hi5 = sliceHist(5,-0.01, max);
    }

    return 0;
}

TH1D* sliceHist(int runnum = 5, double min = 0.07e-6, double max = 0.12e-6){

    auto f = TFile::Open(Form("../areaVsSI/hist%01d.root",runnum));
    auto areaVsSI = dynamic_cast<TH2D*>(f->Get(Form("si%01d",runnum)));

    Int_t minBin = areaVsSI->GetYaxis()->FindBin(min);
    Int_t maxBin = areaVsSI->GetYaxis()->FindBin(max);

    auto hist = areaVsSI->ProjectionX(Form("slice%01d-%013f_%013f",runnum,min*1.e6,max*1.e6),minBin,maxBin,"");

    cout << max << "  " <<  areaVsSI->GetEntries() << "  " << hist->GetEntries() << endl;

    return hist;
}
