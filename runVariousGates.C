
TH2D* getSI(int, double);
int setDiffGate(double);

int runVariousGates(){

	for(double gateNs =10.;gateNs <700.;gateNs+=10.){
		setDiffGate(gateNs);
	}
	return 0;
}

int setDiffGate(double gateNs = 100.){

    gROOT->SetStyle("ATLAS");

	double gate = gateNs * 1.e-9;

    auto wf5 = getSI(5,gate);
    auto wf6 = getSI(6,gate);

    wf6->SetLineColor(kRed);
    wf6->SetMarkerColor(kRed);
    wf6->SetMarkerStyle(1);
    wf5->SetMarkerStyle(1);
    wf5->SetTitle(";Energy (MeV);Shape Indicator");
    wf5->Draw();
    wf6->Draw("same");

    //auto leg = new TLegend();
    //leg->AddEntry(wf5,"Gamma","p");
    //leg->AddEntry(wf6,"Alpha","p");
    //leg->Draw("same");

    return 0;
}

TH2D* getSI(int runnum = 5, double gate = 0.1e-6){
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

    auto f5 = TFile::Open("../stdPulse/hist5.root");
    auto f6 = TFile::Open("../stdPulse/hist6.root");
    auto std5 = dynamic_cast<TH1D*>(f5->Get("std5"));
    auto std6 = dynamic_cast<TH1D*>(f6->Get("std6"));

    auto P = new TH1D("P","P",wfBins, wfMin, wfMax); // P of Shape Indicator
    for(int i=0;i<P->GetNbinsX();++i){
        double alpha_i = std6->GetBinContent(i);
        double gamma_i = std5->GetBinContent(i);
        P->SetBinContent(i,(alpha_i - gamma_i)/(alpha_i + gamma_i) );
    }

    const Int_t totalEv = t->GetEntries();

    // Define Histgrams
    auto si = new TH2D(Form("si%01d",runnum),Form("si%01d",runnum),1000,-0.1,1.9,1000,0.0,1.);

    for(int iEntry=0;iEntry<totalEv;iEntry++){
    // Event Begin ===>
        if(iEntry%100 == 0) cout << "event# " << iEntry << endl;

        t->GetEntry(iEntry);

        Double_t pedestal = 0.;  // Pedestal (ave. first 1000 points)
        Double_t peak = 1.e9;   // Peak (inverted to positive);
        Double_t peakTime = 0.;  // Time of Peak
        Int_t    peakIndex = 0; // Index of Peak
        Double_t area = 0.;  //Integral over pedestal
        Double_t fast = 0.;  //Integral over pedestal

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
            if(i < std5->FindBin(gate)){
                fast += -1.*wfBinWidth*(data->at(i)-pedestal);
            }
        }

        auto wf = new TH1D(Form("wf%05d",iEntry),Form("wf%05d",iEntry),wfBins, wfMin, wfMax); //Raw wave form
        auto avwf = new TH1D(Form("avwf%05d",iEntry),Form("avwf%05d",iEntry),wfBins, wfMin, wfMax); //Normalized

        for(int i=0;i<time->size();++i){
            wf->SetBinContent(i,data->at(i)-pedestal);
            avwf->SetBinContent(i,-1.*(data->at(i)-pedestal)/area);
        }

        //Calc SI
        double SumWf = 0;
        double SumWfP = 0;
        for(int i=std5->FindBin(0.);i<std5->FindBin(1e-6);++i){
            SumWf += avwf->GetBinContent(i);
            SumWfP += avwf->GetBinContent(i)*P->GetBinContent(i);
        }
        
        si->Fill(area*calFac,fast/area);

        delete wf;
        delete avwf;

    } // <=== Event End

    si->SaveAs(Form("../diffGateHists2/gate%01d-%06d.root",runnum, (int)(gate*1.e9)));

    return si;
}
