
void xscal(){

	double BR = 0.58* 0.1;
	double lumi = 41.48; // fb^-1 (1e-15)
	double nevt = 9934000.00;
	double wgt = 0.00000109;
	double xs = 0.00;

	// wgt = xs*BR*lumi / nevt
	xs = wgt*nevt / (BR*lumi);

	cout << "The xs : " << xs << " [ fb ]" << endl;

}
