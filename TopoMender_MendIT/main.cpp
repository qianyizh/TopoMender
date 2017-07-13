#include <iostream>
#include "Topomender.h"
using namespace std;

//ofstream fout(".\\test.txt");

//Usage:
//Topomender.exe 
//arg1:MeshFile(.obj)
//arg2:SOGFile(.sog)
//arg3:SktFile(.axs)
//arg4:OutputFile(.obj)
void PrintUsage() {
	cout << "Usage:" << endl;
	cout << "Topomender[.exe]"
		<< "MeshFile.obj"
		<< "SOGFile.sog"
		<< "SktFile.axs"
		<< "OutputFile.obj"
		<< endl;
	cout << "Attention!"
		<< "AllExternName:OnlyLowercaseAllowed!";
}

int main(int argc, char* argv[]) {
	cout << "Process Topomender Begin..." << endl;
	if(argc != 5) {
		PrintUsage();
		return -1;
	}
	Topomender* topomender;
	topomender = new Topomender(argv[1],argv[2],argv[3],argv[4]);
	if(!topomender->testValid()) {
		PrintUsage();
		return -1;
	}
	// main procedure
	// Step1: Initialize
	if(!topomender->Initialize()) {
		cerr << "FailedtoInitialize!" << endl;
		return -1;
	}
	// Step2: Go
	topomender->Go();
	// Step3: Contour
	topomender->Contour();
	//fout.close();
	return 0;
}
