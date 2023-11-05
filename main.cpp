#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

// function building

bool LTVAlgOk(int AV, float DP) // checks for LTV being <= 95%
{
    float ratio = 1.0 - DP/AV;  // gives percentage

    if (ratio <= .95) 
    {
        return true;
    }
    
    return false;
}

bool LTVAlgIdeal(int AV, float DP) // checks for LTV being <= 80%
{
    float ratio = 1.0 - DP/AV;

    if (ratio < .8)
    {
        return true;
    }

    return false;
}

bool PMIAlg(int AV, float DP) // checks for PMI payment requirements
{
    double ratio = 0;
    ratio = 1 - (DP/AV);

    if (ratio < .8)
    {
        return false;
    }

    return true;
}

bool DTIAlgOk(int CCP, int carPay, int SLP, float MMP, float GMI) // checks for DTI being <= 43% and 
{                                                                 // <= 28% of mortgage in the debt
    float debt = CCP + carPay + SLP + MMP;
    float ratio = debt / GMI;
    float mRatio = MMP / GMI; // mortgage debt ratio

    if (ratio <= .43 && mRatio <= .28) // most they LIKE to accept is .36 ratio, look into this later
    {
        return true;
    }
    
    return false;
}

bool DTIAlgIdeal(int CCP, int carPay, int SLP, float MMP, float GMI) // checks for DTI being <= 36%
{
    float debt = CCP + carPay + SLP + MMP;
    float ratio = debt / GMI;

    if (ratio <= .36)
    {
        return true;
    }
    
    return false;
}

bool FEDTIAlg(float MMP, int GMI) // checks for FEDTI of <= 28%
{
    float ratio = MMP / GMI;

    if (ratio <= .28)
    {
        return true;
    }

    return false;
}

bool acceptedAlg(bool cred, bool LTV, bool DTI, bool FEDTI) // checks if accepted
{
    if (cred == 1 && LTV == 1 && DTI == 1 && FEDTI == 1)
    {
        return true;
    }

    return false;
}

int main()
{
    ofstream report;    // file for compiling user reports
    ifstream data;      // customer data file
    
    // customer vars
    int ID = 0;     // int customer ID
    int GMI = 0;    // int gross monthly income
    int CCP = 0;    // int credit card payments
    int carPay = 0; // int car payments
    int SLP = 0;    // int student loan payments
    int AV = 0;     // int appraised value
    float DP = 0;  // float down payment
    float LA = 0;  // float loan amount
    float MMP = 0; // float monthly mortgage payment
    int CS = 0;     // int credit score

    // flags | must be reset after every customer
    bool credIsGood = false;  // bool credit score is > 640
    bool LTVisIdeal = false;   // bool LTV is ideal
    bool LTVisOk = false;      // bool LTV is acceptable
    bool DTIisIdeal = false;    // bool DTI is ideal
    bool DTIisOk = false;   // bool DTI is acceptable
    bool payPMI = false;      // bool true if DTI is <90 but >80
    bool FEDTIisGood = false; // bool true if FEDTI is acceptable
    bool accepted = false;    // bool true if accepted

    report.open("Report.txt");
    data.open("Data.txt");

    if (data.is_open())
    {
        while (!data.eof())
        {   
            // takes in all data from a single customer, stores in vars
            data >> ID;
            data >> GMI;
            data >> CCP;
            data >> carPay;
            data >> SLP;
            data >> AV;
            data >> DP;
            data >> LA;
            data >> MMP;
            data >> CS;
            
            if (data.eof())
            {
                break;
            }

            cout << ID << '\t' << GMI << '\t' << CCP << '\t' << carPay
                 << '\t' << SLP << '\t' << AV << '\t' << DP << '\t'
                 << LA << '\t' << MMP << '\t' << CS << endl;
            
            // checks for customer requirements
            if (CS >= 640)  // if credit score is >= to 640, true
            {
                credIsGood = true;
            }
            LTVisOk = LTVAlgOk(AV, DP); // checks if DTI is good
            if (LTVisOk)
            {
                LTVisIdeal = LTVAlgIdeal(AV, DP); // only runs if LTV is acceptable
                payPMI = PMIAlg(AV, DP);          // ^
            }
            DTIisOk = DTIAlgOk(CCP, carPay, SLP, MMP, GMI);
            if (DTIisOk)
            {
                DTIisIdeal = DTIAlgIdeal(CCP, carPay, SLP, MMP, GMI);
            }
            FEDTIisGood = FEDTIAlg(MMP, GMI);
            accepted = acceptedAlg(credIsGood, LTVisOk, DTIisOk, FEDTIisGood);
            

            // write data to report
            switch(accepted)
            {
                case 0:
                    report << ID << '\t' << "A: N" << endl;
                    break;
                case 1:
                    report << ID << '\t' << "A: Y" << '\t';
                    if (LTVisOk == 1 && LTVisIdeal == 0)
                    {
                        report << "LTV: N" << '\t';
                    }
                    if (DTIisOk == 1 && DTIisIdeal == 0)
                    {
                        report << "DTI: N" << '\t';
                    }
                    if (payPMI == 1 && LTVisOk == 1)
                    {
                        report << "PMI: Y";
                    }
                    report << endl;
                    break;
            }
            
            
            // reset flags
            LTVisOk = false;
            LTVisIdeal = false;
            payPMI = false;
            DTIisOk = false;
            DTIisIdeal = false;
            FEDTIisGood = false;
            credIsGood = false;
            accepted = false;
        }

        data.close();
        report.close();
    }

    return 0;
}