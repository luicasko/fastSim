#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <math.h>
#include "TFile.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3F.h"
#include "TGraph.h"
#include "TNtuple.h"
#include "TMath.h"
#include "TF1.h"
#include "TClonesArray.h"
#include "TPythia6.h"
#include "TPythia6Decayer.h"
#include "TRandom3.h"
#include "TParticle.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TGraph.h"
#include "TMath.h"
#include "phys_constants.h"
#include "SystemOfUnits.h"
#include "TStopwatch.h"
#include "StAnaCutsData.h"
// #include "TSystem.h"
// #include "TMemStat.h"

using namespace std;

void setDecayChannels(int const mdme);
void decayAndFill(int const kf, TLorentzVector* b, double const weight, TClonesArray& daughters);
void fill(int const kf, TLorentzVector* b, double weight, TLorentzVector const& kMom, TLorentzVector const& piMom, TVector3 v00);
void getKinematics(TLorentzVector& b, double const mass);
TLorentzVector smearMom(TLorentzVector const& b, TF1 const * const fMomResolution);
TVector3 smearPosData(int iParticleIndex, double vz, TLorentzVector const& rMom, TVector3 const& pos, int const centrality);
float dca(TVector3 const& p, TVector3 const& pos, TVector3 const& vertex);
float dcaSigned(TVector3 const& p, TVector3 const& pos, TVector3 const& vertex);
float dcaXY(TVector3 const& p, TVector3 const& pos, TVector3 const& vertex);
float dcaZ(TVector3 const& p, TVector3 const& pos, TVector3 const& vertex);
float dca1To2(TVector3 const& p1, TVector3 const& pos1, TVector3 const& p2, TVector3 const& pos2, TVector3& v0);
TVector3 getVertex(int centrality);
TVector3 getVertexWithError(int centrality);
int getIndex(const float x, const float* edge, const int size);
bool matchHft(int iParticleIndex, double vz, double multiplicity, TLorentzVector const& mom);
bool tpcReconstructed(int iParticleIndex, float charge, int cent, TLorentzVector const& mom);
bool matchTOF(int const iParticleIndex, TLorentzVector const& mom);
bool goodPID(int const iParticleIndex, TLorentzVector const& mom);
void bookObjects();
void write();

int getMultiplicityBinTPC(double);

TVector3 const smearVertex(TVector3);

TPythia6Decayer* pydecay;
TNtuple* nt;
TFile* result;

TF1* fKaonMomResolution = NULL;
TF1* fPionMomResolution = NULL;
TF1* fWeightFunction = NULL;
TF1* f1PidPi = NULL;
TF1* f1PidK = NULL;
TGraph* grEff[3];
const Int_t nParticles = 2;
const Int_t nCentHftRatio = 9;

const int nmultEdge = 7;
float const multEdge[nmultEdge+1] = {0, 4, 8, 12, 16, 20, 24, 200};

//tpc Eff.
const int nmultEdgeTPC = 1;
float const multEdgeTPC[nmultEdgeTPC+1] = {0, 200};

const int m_nZdc = 1;
float const m_zdcEdge[m_nZdc+1] = {0,210};

TH1D* h1Vz[vars::m_nmultEdge];
TH1D* h1ZdcX[vars::m_nmultEdge];
TH1D* h1VxError[vars::m_nmultEdge];
TH1D* h1VzError[vars::m_nmultEdge];

TH1F* hD0ptHIJING;
TH1F* hD0yHIJING;

TH1D* hHftRatio1[vars::m_nParticles][vars::m_nEtasRatio][vars::m_nVzsRatio][vars::m_nPhisRatio][vars::m_nmultEdge];
int const nCentDca = 9;
TH2D* h2Dca[vars::m_nParticles][vars::m_nEtasDca][vars::m_nVzsDca][vars::m_nmultEdge][vars::m_nPtsDca];

TH1D* hTpcPiPlus[nmultEdgeTPC]; //embedding
TH1D* hTpcPiMinus[nmultEdgeTPC]; //embedding
TH1D* hTpcKPlus[nmultEdgeTPC]; //embedding
TH1D* hTpcKMinus[nmultEdgeTPC]; //embedding
TH1D* hRefMult;
TH1D* h_k_tof_eff;//embedding
TH1D* h_pi_tof_eff;//embedding

string outFileName = "D0.toyMc";
std::pair<int, int> const decayChannels(747, 807);
std::pair<float, float> const momentumRange(0, 10);

float const gVzCut = 6.0;
float const acceptanceRapidity = 1.0;
float const sigmaVertexCent[nCentHftRatio] = {31., 18.1, 12.8, 9.3, 7.2, 5.9, 5., 4.6, 4.}; //not using

//============== main  program ==================
int jobindx;

//_______________________________________________________________________________________________________________
void toyMcEffZeroDecayLength(int npart = 1e5, int jobId=0)
//void toyMcEffZeroDecayLength(int npart = 500)
{
    jobindx = jobId;

    TStopwatch*   stopWatch = new TStopwatch();
    stopWatch->Start();

    gRandom->SetSeed(jobId);
    bookObjects();

    pydecay = TPythia6Decayer::Instance();
    pydecay->Init();
    setDecayChannels(763); // D0 --> Kpi
    TLorentzVector* b_d = new TLorentzVector;
    TClonesArray ptl("TParticle", 10);
    for (int ipart = 0; ipart < npart; ipart++)
    {
//      if (i((float)part/(float)npart*10) % 10 == 0)
//         cout << "____________ ipart = " << ipart / static_cast<float>(npart) << " ________________" << endl;

        getKinematics(*b_d, M_D_0); //random pt, y, phi, return b vector with correct prop.
        decayAndFill(421, b_d, fWeightFunction->Eval(b_d->Perp()), ptl);  //421 = D0, ptl = daughters array
        decayAndFill(-421, b_d, fWeightFunction->Eval(b_d->Perp()), ptl);
        if (ipart % 1000 == 1) nt->AutoSave("SaveSelf");
    }
    cout<<"lets write"<<endl;
    write();
    // mem.Show();
    stopWatch->Stop();
    stopWatch->Print();
}

//_______________________________________________________________________________________________________________
void setDecayChannels(int const mdme)
{
    for (int idc = decayChannels.first; idc < decayChannels.second + 1; idc++) TPythia6::Instance()->SetMDME(idc, 1, 0); //747-807 switching off
    TPythia6::Instance()->SetMDME(mdme, 1, 1);
}

//_______________________________________________________________________________________________________________
void decayAndFill(int const kf, TLorentzVector* b, double const weight, TClonesArray& daughters)
{
//    cout<<"decayAndFill start"<<endl;
    pydecay->Decay(kf, b);
    pydecay->ImportParticles(&daughters);

    TLorentzVector kMom;
    TLorentzVector pMom;
    TVector3 v00;

    int nTrk = daughters.GetEntriesFast();
    for (int iTrk = 0; iTrk < nTrk; ++iTrk)
    {
        TParticle* ptl0 = (TParticle*)daughters.At(iTrk);

        switch (abs(ptl0->GetPdgCode()))
        {
            case 321: //kaonplus kaon minus
                ptl0->Momentum(kMom); //seting momentum to kMom
                // v00.SetXYZ(0,0,0);
                v00.SetXYZ(ptl0->Vx() * 0.1, ptl0->Vy() * 0.1, ptl0->Vz() * 0.1); // converted to μm, production vertex
                break;
            case 211:   // pionplus
                ptl0->Momentum(pMom);
                break;
            default:
                break;
        }
    }
    daughters.Clear();
    fill(kf, b, weight, kMom, pMom, v00);
}

//_______________________________________________________________________________________________________________
void fill(int const kf, TLorentzVector* b, double weight, TLorentzVector const& kMom, TLorentzVector const& pMom, TVector3 v00)
{
//    cout<<"Fill() start"<<endl;
    double refMult = hRefMult->GetRandom();
    int centralityTPC = getMultiplicityBinTPC(refMult);
    int centralityDCA = getIndex(refMult, vars::m_multEdge, vars::m_nmultEdge);

    TVector3 const vertex = getVertex(centralityDCA); //from hVz,  converted to um
//    TVector3 const vertex = getVertexWithError(centrality); //from hVz,  converted to um
//    TVector3 const vertexR = smearVertex(vertex); not implemented yet

    // smear primary vertex
    // float const sigmaVertex = sigmaVertexCent[cent];
    // TVector3 const vertex(gRandom->Gaus(0, sigmaVertex), gRandom->Gaus(0, sigmaVertex), gRandom->Gaus(0, sigmaVertex));

    v00 += vertex; //SV + z of vertex from data, in um => Z position of the SV, nothing changed in xy

    // smear momentum
    TLorentzVector const kRMom = smearMom(kMom, fKaonMomResolution); //fKaonMomResolution is TF1
    TLorentzVector const pRMom = smearMom(pMom, fPionMomResolution);

    // smear position
    TVector3 const kRPos = smearPosData(1, vertex.z(), kRMom, v00, centralityDCA); //particle dca smearing , transverse to its vector
    TVector3 const pRPos = smearPosData(0, vertex.z(), pRMom, v00, centralityDCA);

    // reconstruct
    TLorentzVector const rMom = kRMom + pRMom;
    float const kDca = dca(kMom.Vect(), v00, vertex);
    float const pDca = dca(pMom.Vect(), v00, vertex);
    float kRDca = dca(kRMom.Vect(), kRPos, vertex);
    float const kRSDca = dcaSigned(kRMom.Vect(), kRPos, vertex);
    float const kRDcaXY = dcaXY(kRMom.Vect(), kRPos, vertex);
    float const kRDcaZ = dcaZ(kRMom.Vect(), kRPos, vertex);
    float pRDca = dca(pRMom.Vect(), pRPos, vertex);
    float const pRSDca = dcaSigned(pRMom.Vect(), pRPos, vertex);
    float const pRDcaXY = dcaXY(pRMom.Vect(), pRPos, vertex);
    float const pRDcaZ = dcaZ(pRMom.Vect(), pRPos, vertex);

    TVector3 v0;
    float dca12 = dca1To2(kRMom.Vect(), kRPos, pRMom.Vect(), pRPos, v0); // v0 is reconstructed SV position
    float decayLength = (v0 - vertex).Mag();
    float dcaD0ToPv = dca(rMom.Vect(), v0, vertex);
    float const cosTheta = (v0 - vertex).Unit().Dot(rMom.Vect().Unit());
    float const angle12 = kRMom.Vect().Angle(pRMom.Vect());

    float cosThetaStar = kRMom.Vect().Unit().Dot(rMom.Vect().Unit());
    if (cosThetaStar!=cosThetaStar) cosThetaStar=-999;
    int const charge = kf > 0 ? 1 : -1;

    const int nNtVars = nt->GetNvar();
    float arr[nNtVars];
    int iArr = 0;
    arr[iArr++] = centralityDCA;
    arr[iArr++] = refMult;
    arr[iArr++] = vertex.X();
    arr[iArr++] = vertex.Y();
    arr[iArr++] = vertex.Z();

    arr[iArr++] = kf;
    arr[iArr++] = weight;
    arr[iArr++] = b->M();
    arr[iArr++] = b->Perp();
    arr[iArr++] = b->PseudoRapidity();
    arr[iArr++] = b->Rapidity();
    arr[iArr++] = b->Phi();
    arr[iArr++] = v00.X();
    arr[iArr++] = v00.Y();
    arr[iArr++] = v00.Z();

    arr[iArr++] = rMom.M();
    arr[iArr++] = rMom.Perp();
    arr[iArr++] = rMom.PseudoRapidity();
    arr[iArr++] = rMom.Rapidity();
    arr[iArr++] = rMom.Phi();
    arr[iArr++] = v0.X();
    arr[iArr++] = v0.Y();
    arr[iArr++] = v0.Z();

    arr[iArr++] = dca12;
    arr[iArr++] = decayLength;
    arr[iArr++] = dcaD0ToPv;
    arr[iArr++] = cosTheta;
    arr[iArr++] = angle12;
    arr[iArr++] = cosThetaStar;

    arr[iArr++] = kMom.M();
    arr[iArr++] = kMom.Perp();
    arr[iArr++] = kMom.PseudoRapidity();
    arr[iArr++] = kMom.Rapidity();
    arr[iArr++] = kMom.Phi();
    arr[iArr++] = kDca;

    arr[iArr++] = kRMom.M();
    arr[iArr++] = kRMom.Perp();
    arr[iArr++] = kRMom.PseudoRapidity();
    arr[iArr++] = kRMom.Rapidity();
    arr[iArr++] = kRMom.Phi();
    arr[iArr++] = kRPos.X();
    arr[iArr++] = kRPos.Y();
    arr[iArr++] = kRPos.Z();
    arr[iArr++] = kRDca;
    arr[iArr++] = kRSDca;
    arr[iArr++] = kRDcaXY;
    arr[iArr++] = kRDcaZ;
    arr[iArr++] = tpcReconstructed(1, -1 * charge, centralityTPC, kRMom);

    arr[iArr++] = pMom.M();
    arr[iArr++] = pMom.Perp();
    arr[iArr++] = pMom.PseudoRapidity();
    arr[iArr++] = pMom.Rapidity();
    arr[iArr++] = pMom.Phi();
    arr[iArr++] = pDca;

    arr[iArr++] = pRMom.M();
    arr[iArr++] = pRMom.Perp();
    arr[iArr++] = pRMom.PseudoRapidity();
    arr[iArr++] = pRMom.Rapidity();
    arr[iArr++] = pRMom.Phi();
    arr[iArr++] = pRPos.X();
    arr[iArr++] = pRPos.Y();
    arr[iArr++] = pRPos.Z();
    arr[iArr++] = pRDca;
    arr[iArr++] = pRSDca;
    arr[iArr++] = pRDcaXY;
    arr[iArr++] = pRDcaZ;
    arr[iArr++] = tpcReconstructed(0, charge, centralityTPC, pRMom);

    arr[iArr++] = goodPID(1, kRMom);
    arr[iArr++] = goodPID(0, pRMom);
    arr[iArr++] = matchHft(1, vertex.z(), refMult, kRMom); //kaon = 1, pion = 0
    arr[iArr++] = matchHft(0, vertex.z(), refMult, pRMom);

    nt->Fill(arr);
}

//_______________________________________________________________________________________________________________
void getKinematics(TLorentzVector& b, double const mass)
{
    float const pt = gRandom->Uniform(momentumRange.first, momentumRange.second);
    float const y = gRandom->Uniform(-acceptanceRapidity, acceptanceRapidity);
    float const phi = TMath::TwoPi() * gRandom->Rndm();

    float const mT = sqrt(mass * mass + pt * pt);
    float const pz = mT * sinh(y);
    float const E = mT * cosh(y);

    b.SetPxPyPzE(pt * cos(phi), pt * sin(phi) , pz, E);
}

//_______________________________________________________________________________________________________________
float dca(TVector3 const& p, TVector3 const& pos, TVector3 const& vertex)
{
    TVector3 posDiff = pos - vertex;
    return fabs(p.Cross(posDiff.Cross(p)).Unit().Dot(posDiff));
}

//_______________________________________________________________________________________________________________
float dcaSigned(TVector3 const& p, TVector3 const& pos, TVector3 const& vertex)
{
    TVector3 posDiff = pos - vertex;
    float sign = posDiff.x() * p.y() - posDiff.y() * p.x() > 0 ? +1 : -1;

    return sign * p.Cross(posDiff.Cross(p)).Unit().Dot(posDiff);
}

//_______________________________________________________________________________________________________________
float dcaXY(TVector3 const& p, TVector3 const& pos, TVector3 const& vertex)
{
    TVector3 newPos(pos);
    newPos.SetZ(0);

    TVector3 newP(p);
    newP.SetZ(0);

    TVector3 newVertex(vertex);
    newVertex.SetZ(0);

    TVector3 posDiff = newPos - newVertex;
    float sign = posDiff.x() * p.y() - posDiff.y() * p.x() > 0 ? +1 : -1;
    return sign * newP.Cross(posDiff.Cross(newP)).Unit().Dot(posDiff);
}

//_______________________________________________________________________________________________________________
float dcaZ(TVector3 const& p, TVector3 const& pos, TVector3 const& vertex)
{
    TVector3 posDiff = pos - vertex;
    if (sin(p.Theta()) == 0) return 0;
    else return (-posDiff.x() * cos(p.Phi()) - posDiff.y() * sin(p.Phi())) * cos(p.Theta()) / sin(p.Theta()) + posDiff.z();
}

//_______________________________________________________________________________________________________________
float dca1To2(TVector3 const& p1, TVector3 const& pos1, TVector3 const& p2, TVector3 const& pos2, TVector3& v0)
{
    TVector3 posDiff = pos2 - pos1;
    TVector3 pu1 = p1.Unit();
    TVector3 pu2 = p2.Unit();
    double pu1Pu2 = pu1.Dot(pu2);
    double g = posDiff.Dot(pu1);
    double k = posDiff.Dot(pu2);
    double s2 = (k - pu1Pu2 * g) / (pu1Pu2 * pu1Pu2 - 1.);
    double s1 = g + s2 * pu1Pu2;
    TVector3 posDca1 = pos1 + pu1 * s1;
    TVector3 posDca2 = pos2 + pu2 * s2;
    v0 = 0.5 * (posDca1 + posDca2);
    return (posDca1 - posDca2).Mag();
}

//_______________________________________________________________________________________________________________
TLorentzVector smearMom(TLorentzVector const& b, TF1 const * const fMomResolution)
{
    float const pt = b.Perp();
    float const sPt = gRandom->Gaus(pt, pt * fMomResolution->Eval(pt));

    TLorentzVector sMom;
    sMom.SetXYZM(sPt * cos(b.Phi()), sPt * sin(b.Phi()), sPt * sinh(b.PseudoRapidity()), b.M());
    return sMom;
}

//_______________________________________________________________________________________________________________
int getMultiplicityBinTPC(double mult)
{
    for (int i = 0; i < nmultEdgeTPC; i++) {
        if ((mult >= multEdgeTPC[i]) && (mult < multEdgeTPC[i+1]))
            return i;
    }
    return -1 ;
}

//_______________________________________________________________________________________________________________
TVector3 smearPosData(int const iParticleIndex, double const vz, TLorentzVector const& rMom, TVector3 const& pos, int const centrality) //pos is SV
{
    int const iEtaIndex = getIndex(rMom.PseudoRapidity(), vars::m_EtaEdgeDca, vars::m_nEtasDca);
    int const iVzIndex = getIndex(vz, vars::m_VzEdgeDca, vars::m_nVzsDca);
    int const iPtIndex = getIndex(rMom.Perp(), vars::m_PtEdgeDca, vars::m_nPtsDca);

    double sigmaPosZ = 0;
    double sigmaPosXY = 0;

    if (h2Dca[iParticleIndex][iEtaIndex][iVzIndex][centrality][iPtIndex]->GetEntries()>0) h2Dca[iParticleIndex][iEtaIndex][iVzIndex][centrality][iPtIndex]->GetRandom2(sigmaPosXY,sigmaPosZ);

    TVector3 newPos(pos);
    newPos.SetZ(0);
    TVector3 momPerp(-rMom.Vect().Y(), rMom.Vect().X(), 0.0); //momentum perp. to the original one in xy (transverse) plane
    newPos -= momPerp.Unit() * sigmaPosXY;

    return TVector3(newPos.X(), newPos.Y(), pos.Z() + sigmaPosZ);
}

//_______________________________________________________________________________________________________________
TVector3 getVertex(int const centrality)
{
    double rdmVz;

    if (h1Vz[centrality]->GetEntries() == 0) rdmVz = 0.;
    else
    {
        do {
            rdmVz = h1Vz[centrality]->GetRandom();
        }
        while (fabs(rdmVz) > gVzCut);
    }

    return TVector3(0., 0., rdmVz);
}

//_______________________________________________________________________________________________________________
TVector3 getVertexWithError(int const centrality)
{
    double rdmVz;

    if (h1Vz[centrality]->GetEntries() == 0) rdmVz = 0.;
    else
    {
        do {
            rdmVz = h1Vz[centrality]->GetRandom();
        }
        while (fabs(rdmVz) > gVzCut);
    }

    double xError = h1VxError[centrality]->GetRandom();
    double yError = h1VxError[centrality]->GetRandom();
    double zError = h1VzError[centrality]->GetRandom();

    float rand;
    do {
        rand = gRandom->Uniform(-1, 1);
    } while ( rand==0 );
    if (rand<0) rand=-1;
    if (rand>0) rand=1;
    return TVector3(0.+rand*xError, 0.+rand*yError, rdmVz+rand*zError);
}
//_______________________________________________________________________________________________________________
int getIndex(const float x, const float* edge, const int size) {
    int bin = -1;
    if(size<=0) return bin;
    for (int i = 0; i < size; i++) {
        if ((x >= edge[i]) && (x < edge[i + 1]))
            bin = i;
    }
    return bin;
}

//_______________________________________________________________________________________________________________
bool tpcReconstructed(int iParticleIndex, float charge, int cent, TLorentzVector const& mom)
{
    TH1D* h = NULL;

    if (iParticleIndex == 0)
    {
        if (charge > 0) h = hTpcPiPlus[cent];
        else h = hTpcPiMinus[cent];
    }
    else
    {
        if (charge > 0) h = hTpcKPlus[cent];
        else h = hTpcKMinus[cent];
    }

    int const bin = h->FindBin(mom.Perp());

    return gRandom->Rndm() < h->GetBinContent(bin);
}

//_______________________________________________________________________________________________________________
bool matchHft(int const iParticleIndex, double const vz, double const multiplicity, TLorentzVector const& mom)
{
    int const iEtaIndex = getIndex(mom.PseudoRapidity(), vars::m_EtaEdgeRatio, vars::m_nEtasRatio);
    int const iVzIndex = getIndex(vz, vars::m_VzEdgeRatio, vars::m_nVzsRatio);
    int const iPhiIndex = getIndex(mom.Phi(), vars::m_PhiEdgeRatio, vars::m_nPhisRatio);
    int const iMult = getIndex(multiplicity, vars::m_multEdge, vars::m_nmultEdge);
    if (iEtaIndex<0 || iVzIndex<0 || iPhiIndex<0 || iMult<0) return false;
    if (mom.Perp()>12) return false;
    if (mom.Perp()<0.15) return false;
    int bin = -1;
    if (hHftRatio1[iParticleIndex][iEtaIndex][iVzIndex][iPhiIndex][iMult]->GetEntries()>0) bin = hHftRatio1[iParticleIndex][iEtaIndex][iVzIndex][iPhiIndex][iMult]->FindBin(mom.Perp());
    else return false;
    if (bin<1) return false;
    return gRandom->Rndm() < hHftRatio1[iParticleIndex][iEtaIndex][iVzIndex][iPhiIndex][iMult]->GetBinContent(bin);
}

//_______________________________________________________________________________________________________________
bool matchTOF(int const iParticleIndex, TLorentzVector const& mom)
{
    if (iParticleIndex == 0) { // pion
        return gRandom->Rndm() < h_pi_tof_eff->GetBinContent(h_pi_tof_eff->FindBin(mom.Perp())); //from histogram
    }
    else if (iParticleIndex == 1) { // kaon
        return gRandom->Rndm() < h_k_tof_eff->GetBinContent(h_k_tof_eff->FindBin(mom.Perp())); //from histogram
    } else {
        return false;
    }
}

//_______________________________________________________________________________________________________________
bool goodPID(int const iParticleIndex, TLorentzVector const& mom)
{
//    cout<<"pid"<<endl;
    if (iParticleIndex == 0) { // pion
        return gRandom->Rndm() < f1PidPi->Eval(mom.Perp()); //from histogram
    }
    else if (iParticleIndex == 1) { // kaon
        return gRandom->Rndm() < f1PidK->Eval(mom.Perp()); //from histogram
    } else {
        return false;
    }
}

//_______________________________________________________________________________________________________________
TVector3 const smearVertex(TVector3 vertex){
    return vertex;
}

//_______________________________________________________________________________________________________________
void bookObjects()
{
   cout << "Loading input momentum resolution ..." << endl;
    TFile fPionMom("pion_momentum_resolution.root");
    TFile fKaonMom("kaon_momentum_resolution.root");
    fPionMomResolution = (TF1*)fPionMom.Get("fct_gaus_HFT_1.00")->Clone();
    fKaonMomResolution = (TF1*)fKaonMom.Get("fct_gaus_HFT_1.00")->Clone();

//    fPionMomResolution = (TF1*)fPionMom.Get("fct_gaus_HFT_1.05")->Clone();
//    fKaonMomResolution = (TF1*)fKaonMom.Get("fct_gaus_HFT_1.05")->Clone();

//    fPionMomResolution = (TF1*)fPionMom.Get("fct_gaus_HFT_0.95")->Clone();
//    fKaonMomResolution = (TF1*)fKaonMom.Get("fct_gaus_HFT_0.95")->Clone();

//    fPionMomResolution = (TF1*)fPionMom.Get("pion_MomResFit")->Clone("pion_MomResFit");
//    fKaonMomResolution = (TF1*)fKaonMom.Get("kaon_MomResFit")->Clone("kaon_MomResFit");

    fPionMom.Close();
    fKaonMom.Close();

    cout << "Loading input spectra ..." << endl;
    TFile fPP("published_run10_D0_AuAu_data.root");
    fWeightFunction = (TF1*)fPP.Get("Levy_pp")->Clone("f1Levy");
    fPP.Close();

    TFile filePidK("totalEff_K.root");
    f1PidK = (TF1*)filePidK.Get("fTotalGraphEffPid_K")->Clone("f1PidK");
    filePidK.Close();

    TFile filePidPi("totalEff_pi.root");
    f1PidPi = (TF1*)filePidPi.Get("fTotalGraphEffPid_pi")->Clone("f1PidPi");
    filePidPi.Close();

    TFile fileHijing("HIJING_D0_pt_y.root");
    hD0ptHIJING = (TH1F*)fileHijing.Get("hMcD0Pt")->Clone("hD0ptHIJING");
    hD0ptHIJING->SetDirectory(0);
    hD0yHIJING = (TH1F*)fileHijing.Get("hMcD0Y")->Clone("hD0yHIJING");
    hD0yHIJING->SetDirectory(0);
    fileHijing.Close();

    cout<<"Loading TOF eff."<<endl;
    TFile f_tof("eff_tof.root");
    h_pi_tof_eff = (TH1D*)f_tof.Get("eff_TOF_p0_nsigma1");
    h_pi_tof_eff->SetDirectory(0);
    h_k_tof_eff = (TH1D*)f_tof.Get("eff_TOF_p1_nsigma1");
    h_k_tof_eff->SetDirectory(0);
    f_tof.Close();

    char name[500];
    //getting VZ histogram for each multiplicity
   cout<<"Loading Vz and ZDCs..."<<endl;
    TFile fEvent("inputs.event.root");
    TH3F* mh3VzZdcMult = new TH3F();
    mh3VzZdcMult = (TH3F*)fEvent.Get("mh3VzZdcMult");
//    hRefMult = (TH1D*)fEvent.Get("hrefMult");

    TFile fVertexReso("vertexReso.root");
    TH2F* mh2VxRefMult = new TH2F();
    mh2VxRefMult = (TH2F*)fVertexReso.Get("picoDstVErrX_vs_refMult");

    TH2F* mh2VzRefMult = new TH2F();
    mh2VzRefMult = (TH2F*)fVertexReso.Get("picoDstVErrZ_vs_refMult");

    int binVzmin = 1;
    int binVzup = mh3VzZdcMult->GetXaxis()->GetNbins();
    int binZDCmin = 1;
    int binZDCmax = mh3VzZdcMult->GetYaxis()->GetNbins();

    for (int ii = 0; ii < vars::m_nmultEdge; ++ii)   {
        int binMultmin = mh3VzZdcMult->GetZaxis()->FindBin(multEdge[ii]);
        int binMultmax = mh3VzZdcMult->GetZaxis()->FindBin(multEdge[ii+1]);
        h1Vz[ii] = mh3VzZdcMult -> ProjectionX("_px",binZDCmin, binZDCmax, binMultmin, binMultmax, ""); //vz zdc
        h1Vz[ii]->SetDirectory(0);
        h1ZdcX[ii] = mh3VzZdcMult -> ProjectionY("_py",binVzmin, binVzup, binMultmin, binMultmax, ""); //vz zdc
        h1ZdcX[ii]->SetDirectory(0);

        h1VzError[ii] = mh2VzRefMult-> ProjectionY("_py", mh2VzRefMult->GetXaxis()->FindBin(multEdge[ii]), mh2VzRefMult->GetXaxis()->FindBin(multEdge[ii+1]), "");
        h1VzError[ii]->SetDirectory(0);
        h1VxError[ii] = mh2VxRefMult-> ProjectionY("_py", mh2VxRefMult->GetXaxis()->FindBin(multEdge[ii]), mh2VxRefMult->GetXaxis()->FindBin(multEdge[ii+1]), "");
        h1VxError[ii]->SetDirectory(0);

    }
    hRefMult = (TH1D*) mh3VzZdcMult -> ProjectionZ("_pz",binVzmin, binVzup, binZDCmin, binZDCmax, "");
    hRefMult->SetDirectory(0);
    fEvent.Close();

    cout << "Loading input HFT ratios and DCA ..." << endl;
    TFile fDca1("dcaxy_vs_dcaz.root");
    TFile fHftRatio1Pion("hftratio_vs_pt_dAu_pion.root");
    TFile fHftRatio1Kaon("hftratio_vs_pt_dAu_kaon.root");
    for (int iParticle = 0; iParticle < vars::m_nParticles; ++iParticle) {
        //DCA
        for (int iEta = 0; iEta < vars::m_nEtasDca; ++iEta) {
            for (int iVz = 0; iVz < vars::m_nVzsDca; ++iVz) {
                for (int iCent = 0; iCent < vars::m_nmultEdge; ++iCent) {
                    for (int iPt = 0; iPt < vars::m_nPtsDca; ++iPt) {
                        TString h2dName=Form("mh3DcaXyZPt_p%d_eta%d_vz%d_m%d_pt%d", iParticle, iEta, iVz, iCent, iPt);
                        h2Dca[iParticle][iEta][iVz][iCent][iPt] = new TH2D();
                        h2Dca[iParticle][iEta][iVz][iCent][iPt] = (TH2D* )((fDca1.Get(h2dName)));
                        h2Dca[iParticle][iEta][iVz][iCent][iPt]->SetDirectory(0);
                    }
                }
            }
                        }
        cout<<"dcas lodad"<<endl;

        for (int iEta = 0; iEta < vars::m_nEtasRatio; iEta++) {
            for (int iVz = 0; iVz < vars::m_nVzsRatio; iVz++) {
                for (int iPhi = 0; iPhi < vars::m_nPhisRatio; iPhi++) {
                    for (int iMult = 0; iMult < vars::m_nmultEdge; ++iMult) {
                        if (iParticle == 0) hHftRatio1[iParticle][iEta][iVz][iPhi][iMult] = (TH1D * )(fHftRatio1Pion.Get(Form("h_hftratio_p%d_eta%d_vz%d_phi%d_m%d", iParticle, iEta, iVz, iPhi, iMult)));
                        if (iParticle == 1) hHftRatio1[iParticle][iEta][iVz][iPhi][iMult] = (TH1D * )(fHftRatio1Kaon.Get(Form("h_hftratio_p%d_eta%d_vz%d_phi%d_m%d", iParticle, iEta, iVz, iPhi, iMult)));
                        hHftRatio1[iParticle][iEta][iVz][iPhi][iMult]->SetDirectory(0);
                    }
                    }
                }
            }

        cout<<"hft loaded"<<endl;


    }
    cout << "Finished loading Dca: " <<  endl;

    fHftRatio1Pion.Close();
    fHftRatio1Kaon.Close();
    fDca1.Close();

   cout << " Loading TPC tracking efficiencies " << endl;
    TFile fTpcPiPlus("piplus_tpc_eff_embedding.root");
    TFile fTpcPiMinus("piminus_tpc_eff_embedding.root");
    TFile fTpcKPlus("kplus_tpc_eff_embedding.root");
    TFile fTpcKMinus("kminus_tpc_eff_embedding.root");

    for (int iCent = 0; iCent < nmultEdgeTPC; ++iCent) {
        hTpcPiPlus[iCent] = (TH1D*)fTpcPiPlus.Get(Form("TrackEffMult%i", iCent));
        hTpcPiPlus[iCent]->SetDirectory(0);
        hTpcPiMinus[iCent] = (TH1D*)fTpcPiMinus.Get(Form("TrackEffMult%i", iCent));
        hTpcPiMinus[iCent] ->SetDirectory(0);
        hTpcKPlus[iCent] = (TH1D*)fTpcKPlus.Get(Form("TrackEffMult%i", iCent));
        hTpcKPlus[iCent]->SetDirectory(0);
        hTpcKMinus[iCent] = (TH1D*)fTpcKMinus.Get(Form("TrackEffMult%i", iCent));
        hTpcKMinus[iCent]->SetDirectory(0);
    }

    fTpcPiPlus.Close();
    fTpcPiMinus.Close();
    fTpcKPlus.Close();
    fTpcKMinus.Close();

//   cout << "Done with loading all files ..." << endl;

    std::stringstream ss_indx; ss_indx << jobindx;
    result = new TFile((outFileName + "_c" + "_job" + ss_indx.str() + ".root").c_str(), "recreate");
    result->SetCompressionLevel(1);
    result->cd();

    int BufSize = (int)pow(2., 16.);
// int Split = 1;
    nt = new TNtuple("nt", "", "cent:refMult:vx:vy:vz:"
                               "pid:w:m:pt:eta:y:phi:v0x:v0y:v0z:" // MC D0
                               "rM:rPt:rEta:rY:rPhi:rV0x:rV0y:rV0z:" // Rc D0
                               "dca12:decayLength:dcaD0ToPv:cosTheta:angle12:cosThetaStar:" // Rc pair
                               "kM:kPt:kEta:kY:kPhi:kDca:" // MC Kaon
                               "kRM:kRPt:kREta:kRY:kRPhi:kRVx:kRVy:kRVz:kRDca:kRSDca:kRDcaXY:kRDcaZ:kTpc:" // Rc Kaon
                               "pM:pPt:pEta:pY:pPhi:pDca:" // MC Pion1
                               "pRM:pRPt:pREta:pRY:pRPhi:pRVx:pRVy:pRVz:pRDca:pRSDca:pRDcaXY:pRDcaZ:pTpc:" // Rc Pion1
                               "kPID:pPID:kHft:pHft", BufSize);
// nt->SetAutoSave(-500000); // autosave every 1 Mbytes

    cout<<"files loaded"<<endl;
}

//___________
void write() {
    result->cd();
    nt->Write(nt->GetName(), TObject::kOverwrite);
    result->Close();
}
