#ifndef StANACUTS_H_INCLUDED
#define StANACUTS_H_INCLUDED
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TH1D.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TFile.h"
#include "TString.h"

//cuts
namespace anaCuts {
    float rcMinPtTrack = 0.15;
    float mcMinPtTrack = 0.15;
    float nHitsFitMin = 15;

    const int nDcaRatio=6;
    const float dcaCut[nDcaRatio+1]={0., 0.3, 0.5, 0.7, 1.0, 1.2, 1.5};

    const int nPtDca=6;
    const float ptCut[nPtDca+1]={0.15, 0.3, 0.7, 1.2, 2, 3, 5};

    const int nPtRatio=9;
    const float ptCutRatio[nPtRatio+1]={0.15, 0.3, 0.6, 0.8, 1.0, 1.2, 1.5, 2.0, 3.0, 5.0};

    const int nPar = 4;
    const char namePar[nPar][100] = {"PionPlus", "PionMinus", "KaonPlus", "KaonMinus"};

    const int nPtBins = 3;
    const float PtEdge[nPtBins + 1] = {1, 2, 3, 5};

    // BDT weight
    const int nPtBins_BDT = 3;
    const float PtEdge_BDT[nPtBins + 1] = {1, 2, 3, 5};

    // BDT response cuts
    const float bdt[nPtBins] = {0.7552, 0.645159, 0.531541};

    const float bdt_tight[nPtBins] = {0.1200, 0.1200, 0.120};
    const float bdt_loose[nPtBins] = {0.1200, 0.1200, 0.120};

    /////////////==  initial cuts for BDT
    const float dcaK_Init = 0.002;
    const float dcaPi_Init = 0.002;
    const float dcaD0_Init = 0.05;
    const float dca12_Init = 0.02;
    const float decayL_Init = 0.0005;
    const float cosTheta_Init = 0.7;
}

//SimInputs:
namespace vars {
    //histo stuff
    const int m_nPtTOF = 19;
    float const m_PtTOFedge[m_nPtTOF+1] = {0.15,0.3,0.4,0.5,0.6,0.8,1.,1.25,1.5,2.,3.0,4.,5.,6.,7,8,9,10,11,12.};

    const int m_nZdc = 1;
    float const m_zdcEdge[m_nZdc+1] = {0,210};

    const int m_nParticles = 2;
    const TString m_ParticleName[m_nParticles] = {"Pion", "Kaon"};

    //DCA:
    const int nZdcDCA = 1;
    float const zdcxBinsDCA[nZdcDCA+1] = {0,210};

//    const int m_nEtasDca = 4;
//    float const m_EtaEdgeDca[m_nEtasDca+1] = {0, 0.2, 0.4, 0.8, 1.0};

    const int m_nVzsDca = 4;
    float const m_VzEdgeDca[m_nVzsDca + 1] = { -6.0, -3.0, 0, 3.0, 6.0};

    const int m_nPtsDca = 11;
    float const m_PtEdgeDca[m_nPtsDca + 1] = {0.15, 0.3, 0.4, 0.5, 0.6, 0.8, 1., 1.25, 1.5, 2., 3., 12.};

    int const m_nEtasDca = 3;
    float const m_EtaEdgeDca[m_nEtasDca + 1] = { -1.0, -0.2, 0.2, 1.0 };
//    float const m_EtaEdgeDca[] = { -1.0, -0.2, 0.2, 1.0 };
//    const int m_nEtasDca = sizeof(m_EtaEdgeDca) / sizeof(float);

    const int m_nmultEdgeDCA = 1;
    float const m_multEdgeDCA[m_nmultEdgeDCA+1] = {0, 200};

    //HFT ratio:
//    const int m_nEtasRatio = 6;
//    float const m_EtaEdgeRatio[m_nEtasRatio + 1] = { -1.0, -0.6, -0.2, 0.0, 0.2, 0.6, 1.0};
//
    const int m_nEtasRatio = 10;
    float const m_EtaEdgeRatio[m_nEtasRatio + 1] = { -1.0, -0.8, -0.6, -0.4, -0.2, 0.0, 0.2, 0.4 , 0.6, 0.8, 1.0};

    const int m_nPhisRatio = 11;
    float const m_PhiEdgeRatio[m_nPhisRatio + 1] = { -3.14159, -2.80359, -2.17527, -1.54696, -0.918637, -0.290319, 0.338, 0.966319, 1.59464, 2.22296, 2.85127, 3.14159};

    const int m_nVzsRatio = 3;
    float const m_VzEdgeRatio[m_nVzsRatio + 1] = { -6.0, -2.0, 2.0, 6.0};


    const int m_nmultEdge = 7;
    float const m_multEdge[m_nmultEdge + 1] = {0, 4, 8, 12, 16, 20, 24, 200};


//    const int m_nPtsRatio = 15;
//    float const m_PtEdgeRatio[m_nPtsRatio + 1] =
//            {
//                    0.15, 0.3, 0.4, 0.5, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 2.0, 2.5, 3.0, 4.0, 6.0, 12.0
//            };

    const int m_nPtsRatio = 30; ///tmp
    float const m_PtEdgeRatio[m_nPtsRatio + 1] = {0.15, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0, 3.2, 3.4, 3.6, 3.8, 4.0, 4.2, 4.4, 4.6, 4.8, 5.0, 6.0, 12.0};
//    const int m_nPtsRatio = sizeof(m_PtEdgeRatio) / sizeof(float);


//    const int m_nPtsRatio = 36;
//    float const m_PtEdgeRatio[m_nPtsRatio + 1] =  {
//            0.3, 0.4, 0.5, 0.6 , 0.7 , 0.8 , 0.9 ,
//            1. , 1.1 , 1.2 , 1.3 , 1.4 , 1.5 , 1.6 , 1.7 , 1.8 , 1.9 ,
//            2. , 2.2 , 2.4 , 2.6 , 2.8 , 3.0 ,
//            3.4 , 3.8 , 4.2 , 4.6 , 5.0 ,  5.5 ,
//            6. , 6.5 , 7.0 , 8.0 , 9.0 , 10. , 11,  12.0 };

//OLD
//    const int m_nDcasDca = 148;
//    float const  m_DcaEdgeDca[m_nDcasDca + 1] =   {
//            -1.5, -1.2, -1 , -0.96 , -0.92 , -0.88 , -0.84 , -0.8 , -0.76 , -0.72 , -0.68 , -0.64 , -0.6 , -0.56 , -0.52 , -0.48 , -0.44 , -0.4 , -0.36 , -0.32 , -0.28 , -0.24 , -0.2 , -0.16 , -0.12 ,  -0.08,
//            -0.078 , -0.075 , -0.072 , -0.069 , -0.066 , -0.063 , -0.06 , -0.057 , -0.054 , -0.051 , -0.048 , -0.045 , -0.042 , -0.039 , -0.036 , -0.033 , -0.03 , -0.027 , -0.024 , -0.021 , -0.018 , -0.015 , -0.012 ,
//            -0.01 , -0.0096 , -0.0092 , -0.0088 , -0.0084 , -0.008 , -0.0076 , -0.0072 , -0.0068 , -0.0064 , -0.006 , -0.0056 , -0.0052 , -0.0048 , -0.0044 , -0.004 , -0.0036 , -0.0032 , -0.0028 , -0.0024 , -0.002 , -0.0016 , -0.0012 , -0.0008 , -0.0004 , 0 , 0.0004 , 0.0008 , 0.0012 , 0.0016 , 0.002 , 0.0024 , 0.0028 , 0.0032 , 0.0036 , 0.004 , 0.0044 , 0.0048 , 0.0052 , 0.0056 , 0.006 , 0.0064 , 0.0068 , 0.0072 , 0.0076 , 0.008 , 0.0084 , 0.0088 , 0.0092 , 0.0096 , 0.01 ,
//            0.012 , 0.015 , 0.018 , 0.021 , 0.024 , 0.027 , 0.03 , 0.033 , 0.036 , 0.039 , 0.042 , 0.045 , 0.048 , 0.051 , 0.054 , 0.057 , 0.06 , 0.063 , 0.066 , 0.069 , 0.072 , 0.075 , 0.078 ,
//            0.08 , 0.12 , 0.16 , 0.2 , 0.24 , 0.28 , 0.32 , 0.36 , 0.4 , 0.44 , 0.48 , 0.52 , 0.56 , 0.6 , 0.64 , 0.68 , 0.72 , 0.76 , 0.8 , 0.84 , 0.88 , 0.92 , 0.96 , 1, 1.2, 1.5
//    };

    int const m_nDcasDca = 290;
    float const m_DcaEdgeDca[m_nDcasDca + 1] =
            {
                    -1, -0.95, -0.9, -0.85, -0.8, -0.75, -0.7, -0.65, -0.6, -0.55,  // 10: 500um
                    -0.5, -0.46, -0.42, -0.38, -0.34, // 5: 400um
                    -0.3, -0.28, -0.26, -0.24, -0.22, // 5: 200um
                    -0.2, -0.19, -0.18, -0.17, -0.16, -0.15, -0.14, -0.13, // 8: 100um
                    -0.12, -0.114, -0.108, -0.102, -0.096, -0.09, -0.084, // 7: 60um
                    -0.078, -0.075, -0.072, -0.069, -0.066, -0.063, -0.06, -0.057, -0.054, -0.051, // 10: 30um
                    -0.048, -0.0464, -0.0448, -0.0432, -0.0416, -0.04, -0.0384, -0.0368, -0.0352, -0.0336, // 10: 16um
                    -0.032, -0.0312, -0.0304, -0.0296, -0.0288, -0.028, -0.0272, -0.0264, -0.0256, -0.0248, -0.024, -0.0232, -0.0224, -0.0216, -0.0208, // 15: 8um
                    -0.02, -0.0196, -0.0192, -0.0188, -0.0184, -0.018, -0.0176, -0.0172, -0.0168, -0.0164, -0.016, -0.0156, -0.0152, -0.0148, -0.0144, -0.014, -0.0136, -0.0132, -0.0128, -0.0124, -0.012, -0.0116, -0.0112, -0.0108, -0.0104, // 25: 4um
                    -0.01, -0.0098, -0.0096, -0.0094, -0.0092, -0.009, -0.0088, -0.0086, -0.0084, -0.0082, -0.008, -0.0078, -0.0076, -0.0074, -0.0072, -0.007, -0.0068, -0.0066, -0.0064, -0.0062, -0.006, -0.0058, -0.0056, -0.0054, -0.0052, -0.005, -0.0048, -0.0046, -0.0044, -0.0042, -0.004, -0.0038, -0.0036, -0.0034, -0.0032, -0.003, -0.0028, -0.0026, -0.0024, -0.0022, -0.002, -0.0018, -0.0016, -0.0014, -0.0012, -0.001, -0.0008, -0.0006, -0.0004, -0.0002, 0, 0.0002, 0.0004, 0.0006, 0.0008, 0.001, 0.0012, 0.0014, 0.0016, 0.0018, 0.002, 0.0022, 0.0024, 0.0026, 0.0028, 0.003, 0.0032, 0.0034, 0.0036, 0.0038, 0.004, 0.0042, 0.0044, 0.0046, 0.0048, 0.005, 0.0052, 0.0054, 0.0056, 0.0058, 0.006, 0.0062, 0.0064, 0.0066, 0.0068, 0.007, 0.0072, 0.0074, 0.0076, 0.0078, 0.008, 0.0082, 0.0084, 0.0086, 0.0088, 0.009, 0.0092, 0.0094, 0.0096, 0.0098, 0.01, // 101: 2um
                    0.0104, 0.0108, 0.0112, 0.0116, 0.012, 0.0124, 0.0128, 0.0132, 0.0136, 0.014, 0.0144, 0.0148, 0.0152, 0.0156, 0.016, 0.0164, 0.0168, 0.0172, 0.0176, 0.018, 0.0184, 0.0188, 0.0192, 0.0196, 0.02, // 25: 4um
                    0.0208, 0.0216, 0.0224, 0.0232, 0.024, 0.0248, 0.0256, 0.0264, 0.0272, 0.028, 0.0288, 0.0296, 0.0304, 0.0312, 0.032, // 15: 8um
                    0.0336, 0.0352, 0.0368, 0.0384, 0.04, 0.0416, 0.0432, 0.0448, 0.0464, 0.048, // 10: 16um
                    0.051, 0.054, 0.057, 0.06, 0.063, 0.066, 0.069, 0.072, 0.075, 0.078, // 10: 30um
                    0.084, 0.09, 0.096, 0.102, 0.108, 0.114, 0.12, // 7: 60um
                    0.13, 0.14, 0.15, 0.16, 0.17, 0.18, 0.19, 0.2, // 8: 100um
                    0.22, 0.24, 0.26, 0.28, 0.3, // 5: 200um
                    0.34, 0.38, 0.42, 0.46, 0.5, // 5: 400um
                    0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 1 // 10: 500um
            };

}

// get array index
int getIndex(const float x, const float* edge, const int size) {
    int bin = -1;
    if(size<=0) return bin;
    for (int i = 0; i < size; i++) {
        if ((x >= edge[i]) && (x < edge[i + 1]))
            bin = i;
    }

    return bin;
}



#endif // VARIABLE_H_INCLUDED
