
TH1D* getWf(int);

int makeStdPulse(){

    gROOT->SetStyle("ATLAS");

    auto wf5 = getWf(5);
    auto wf6 = getWf(6);
    wf6->SetLineColor(kRed);
    wf5->SetTitle(";Time (s);Pulse Height(a.u.)");
    wf5->Draw();
    wf6->Draw("same");

    auto leg = new TLegend();
    leg->AddEntry(wf5,"Gamma","l");
    leg->AddEntry(wf6,"Alpha","l");
    leg->Draw("same");

    return 0;
}

TH1D* getWf(int runnum = 5){
    //runnum 5 - gamma
    //runnum 6 - alpha

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
    auto stdPulse = new TH1D(Form("std%01d",runnum),Form("std%01d",runnum),wfBins,wfMin,wfMax);
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
        for(int i=0;i<data->size();++i){
            area += -1.*wfBinWidth*(data->at(i)-pedestal);
        }

        auto wf = new TH1D(Form("wf%05d",iEntry),Form("wf%05d",iEntry),wfBins, wfMin, wfMax); //Raw wave form
        auto avwf = new TH1D(Form("avwf%05d",iEntry),Form("avwf%05d",iEntry),wfBins, wfMin, wfMax); //Normalized

        for(int i=0;i<time->size();++i){
            wf->SetBinContent(i,data->at(i)-pedestal);
            avwf->SetBinContent(i,-1.*(data->at(i)-pedestal)/area);
        }

        if(0.5 <area*calFac && area*calFac<0.8){
            for(int i=0;i<time->size();++i){
                stdPulse->SetBinContent(i,stdPulse->GetBinContent(i)-(data->at(i)-pedestal)/area);
            }
            numOfAvwf++;
        }

        delete wf;
        delete avwf;

    } // <=== Event End

    for(int i=0;i<time->size();++i){
        stdPulse->SetBinContent(i,stdPulse->GetBinContent(i)/numOfAvwf);
    }

    stdPulse->Draw();
    stdPulse->SaveAs(Form("../stdPulse/hist%01d.root",runnum));

    cout << "ROI EVENTS#" << numOfAvwf << endl;

    return stdPulse;
}
