// K-means clustering using the SAS Fastclust algorithm,
// described on p. 352 of Numerical Ecology (1998).
// 
// // hoice of transformations for species abundance data,
// from Legendre and Gallagher (submitted).
// 
// Loop with different random starting configurations.
// 
// References:
// 
// Legendre, P. and E. Gallagher. 2001. Ecologically meaningful transformations 
// for ordination of species data. Oecologia (in press).
// 
//  Legendre, P. and L. Legendre, 1998. Numerical Ecology. 2nd English edition.
//  Elsevier Science BV, Amsterdam.
// 
//  Milligan, G. W. and M. C. Cooper. 1988. A study of standardization of 
//  variables in cluster analysis. Journal of Classification 5: 181-204.
// 
//                                           Pierre Legendre, August 1999
// 
//  345678901234567890123456789012345678901234567890123456789012345678901234567890
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <map>
#include <sstream>
#include <iostream>
#include <fstream>
 


/* FILE *Output2; //was Output1 */
FILE *Output4;
/* FILE *Output7; */


//--Read the data
void ReadData1(int &n,int &nmax,int &p,int &pmax,double** mat/* ,double* coord */,int* ishort,double* weight,double* colsum,int &ntran,char* namea, char* nameb, char* namew, int N, int pLeavesAbsent);
//--Calculate the kmeans
void CheckCentr(int &n,int &nmax,int &p,int &pmax,int &k1,int &kmax,double** mat,double** xbar,int* ishort,double** sx,int &idebug);
void Assign(int &iran,int &n,int &nmax,int &k1,int &kmax,int* list,int* howmany,int* no,int &idebug,int &iassign,int &iseed, int random_number);
//--Squared distances to group centroids. Assign objects to nearest one
void Centroids(int &n,int &nmax,int &p,int &pmax,int &kmax,double** mat,double** sx,double** xbar,double* mean,int* list,int* howmany,int &kk,int* ishort,int &idebug);
void CompSST(int &n,int &nmax,int &p,int &pmax,double** mat,double* weight,int* ishort,double &SST);
void Distances(int &n,int &nmax,int &p,int &pmax,int &kmax,double** mat,double** xbar,double* Dvec,int* list,int* howmany,double &SSE,int &kk,double* weight,int* ishort,int &idebug, double &W);
void Standard(int &n,int &nmax,int &kmax,int &p,int &pmax,double** mat,double** sx,double** sx2,double* vect,double** var,double* weight,int* ishort,int &istand);		
void Transform(int &n,int &nmax,int &p,int &pmax,double** mat,double* weight,double* colsum,int &ntran);
void Permute(int &iseed,int &n,int &nmax,int *iordre);
void Euclidean(int &i,int &k,int &nmax,int &kmax,int &p,int &pmax,double** mat,double** xbar,double* weight,int* ishort,double &D1);
void dist(int &i,int &k,int &nmax,int &kmax,int &p,int &pmax,double** mat,double** xbar,double* weight,int* ishort,double &D1,int &n,int* list,int* howmany,int &kk);
double Euclidean_point(int &i, int&j, int &p,int &pmax,double** mat,double* weight,int* ishort);
int Split(vector<string>& vecteur, string chaine, char separateur);
void listePartition(int Strouve[]);
int fact (int valeur);
double DistanceCH_old(vector <string> monTableau,string C,int &n,int &kmax,double** mat,int* list,double** Ww,double* resultats, char *N_especes);
double DistanceCH_old_final(vector <string> monTableau,string C,int &n,int &kmax,double** mat,int* list,double** Ww,double* resultats, char *N_especes);
double f_RI(int Strouve[],int Sref[],int N);
double f_ARI(int Strouve[],int Sref[],const char *K_real,int group,int N);
void outStat(int Strouve[],int Sref[],char *criteria,int N,char *N_especes,char *percent,const char *K_real,int group,double score,double* resultats_final,int **listr);
double NouvelleCallRF(double nb_esp_identique);

//  Parameters of the program:
//  n    = number of observations (ex. number of trees)   
//  nmax = maximum number of observations (default: 10000)
//  p    = number of variables     (ex. number of variables describing each trees)
//  pmax = maximum number of variables  (default: 10000)  
//  k    = number of groups (centroids)
//  kmax = maximum number of groups
//  kk   = ???
//  niter = maximum iteration for convergeance of centroid (fixed=100)
//  Parameter (nmax=100000,pmax=10,kmax=10)
//  critera = (0,1,2)
//            0: C-H  
//            1: logSS  
//            2: Silhouette   
//			  3: W
//   mat       =  double[nmax+1][pmax+1] (data matrix))
//   weight         =  double vector[p] of weigth vector for variable p
//   xbar      =  vector [k][i] = mean distance of i to centroid k
//   list      =  vector[i]=k contains group assignments for the objects.
//   howmany[k]=  contains the number of objects in each group centroid (k). 
//   ishort[p] = vector containing the position(p) of valide variables (containing non zero value for weight -- see ReadData). 
//   nobest
//   var       = double [kmax+1]
//   vect      = double [pmax+1]
//   mean[p]      = vector of mean value not weigthed for the variable [p] double [pmax+1] 
//   Dvec      = double [kmax+1]
//   CHr       = vector[k] of the best CH this k
//   BHr       = vector[k] of the best BH this k
//   Silr      = vector[k] of the best Silr this k
//   LogSSr    = vector[k] of the best LogSS this k
//   Wr        = vector[k] of the best W this k
//   SSEr      = double [kmax+1]  sum of squared error statistic (SSE) = within-group sum of squares
//   listr     = int[kmax+1][nmax+1];
//   howmanyr  = int[kmax+1][nmax+1];	
// sx = new double*[kmax+1];
//  sx2 = new double*[kmax+1];

//   no = new int [nmax+1]; 
//	iordre = new int [nmax+1];
//   nobest = new int [kmax+1];
//   nnitr = new int [kmax+1];
//
// Output file for summary results	
//
// Output2   = file.statistics.txt
// Output3   = file.sum.txt
// Output4   = stat.txt
// Output7    = file.groups.txt
//
// Note, we expect the input file to be a matrix in the form (below))
// were 5 is the n, and 4 is the p and the following line are the value (double))
//5	4
//0.0	0.0	0.0	1.0	0.0	
//0.0	1.0	0.0	0.0	1.0	
//1.0	1.0	1.0	0.0	1.0	
//0.0	0.0	0.0	0.0	1.0	
//
// IMPORTANT: See also the variables below

//Variables globales
/* map<int,int> nbNi; */
map<int,string>  mapIndicesTreesFinal;
vector <string> indicesTrees;
bool use_weight=false;

int main_kmeans(char **argv,vector <string> monTableau, double ** mat, string parametres, double ** n_identique,double ** Ww, vector<int> tabIndices, int pLeavesAbsent)
{
	//*****************Define variables******************************************//
	 // Variables
    time_t tbegin2,tend2;
	// Start timer
    time(&tbegin2);				// get the current calendar time
    double texec2=0.0;
	 
	double W = 0.0;
	double CH = 0.0;
	double silhouette=0.0;
	double LogSS=0.0;
	double BH=0.0;

	/* int kg = 0; */
	use_weight=false;
	int intParam = 0;

	double CHr_max=-1.0;
	int CHr_group=0;

	double BHr_min=1000000.0;
	int BHr_group=0;

	double Silr_max=-1.0;
	int Silr_group=0;

	double LogSS_min=1000000.0;
	int LogSS_group=0;

	double W_min=1000000.0;
	int W_group=0;


	
	
/* 	cout<<"Arbres en entrée "<<endl;
	for(int ij=0; ij<monTableau.size(); ij++){
		cout<<monTableau[ij]<<endl;
	}
	cout<<endl; */
	
	
	int N = monTableau.size(); //quantity of initial tree
	int i=0, j=0;		//Counters
	int n=0, p=0;		//,pmax,kmax; //      Integer p,pmax,kmax
	int ntran=0, itemp=0, iseed=0, niter=0, kk=0, nit=0;		//added declarations for variables
	int nnit=0, k=0, i1ref=0, i2ref=0, igr1=0, igr2=0;		//added declarations for variables
	int idebug=0 ; // 0, no debug, 1 debug
	int k1=0, k2=0;  //added declarations for variables
	int hard_max_k=0; //--Setting the max k1
        
    int random_number=5; //--Fixed random number
	int istand=0;   //--0 No standardization
	int iassign=2;  // 1 equal, 2 random
    int iran=5;   //--Number of random position
    int nran=1;  //--Number of Random start  
		
	int nmax=N;    //--Maximum number of object -Parameter (nmax=10000,pmax=250,kmax=100)
	int pmax=N;      //--Maximum data point (variable))
 	int kmax=N;	  // Maximum number of groups
	
	char *criteria = argv[0];
	char *N_especes = argv[2];
	const char *K_real = argv[3];
	char *percent = argv[4];
	
	int Strouve[N];
	
	for(int linej=0;linej<N;linej++){
		Strouve[linej]= 0;
	}
	
	
	double	**sx,**sx2,**xbar,**var;	//sx(kmax,pmax),sx2(kmax,pmax),xbar(kmax,pmax),var(kmax,pmax)
	sx = new double*[kmax+1];
	sx2 = new double*[kmax+1];
	xbar = new double*[kmax+1];
	var = new double*[kmax+1];

	for (i=0;i<=kmax;i++)
	{
		sx[i] = new double[pmax+1];
		sx2[i] = new double[pmax+1];
		xbar[i] = new double[pmax+1];
		var[i] = new double[pmax+1];
	}

		 
	for (i=0; i<=kmax; i++)
	{
		for (j=0; j<=pmax; j++)
		{
			sx[i][j] = 0.0;
			sx2[i][j] = 0.0;
			xbar[i][j] = 0.0;
			var[i][j] = 0.0;
		}
	}
	

	double *Dvec,*CHr, *BHr,*SSEr, *Silr, *LogSSr, *Wr, *diff_W, *V_W;
	Dvec = new double [kmax+1];
	SSEr = new double [kmax+1];

	for (i=0; i<=kmax; i++)
	{
		Dvec[i] = 0.0;
		SSEr[i] = 0.0;		
	}

	double *vect,*mean,*weight;		//vect(pmax),mean(pmax),weight(pmax),
	vect = new double [pmax+1];
	mean = new double [pmax+1];
	weight = new double [pmax+1];
	for (i=0; i<=pmax; i++)
	{
		vect[i] = 0.0;
		mean[i] = 0.0;
		weight[i] = 0.0;
	}


	double D1=0,Dref=0,SSE=0,SSEref=0,SST=0;		
    double temp=0;					

	int **listr;					//listr(kmax,nmax),
	listr = new int*[kmax+1];
	for (i=0;i<=kmax;i++)
	{
		listr[i] = new int [nmax+1];
	}

	for (i=0; i<=kmax; i++)
	{
		for (j=0; j<=nmax; j++)
		{
		  listr[i][j] = 1;
		}
	}


	int	**howmanyr;		//howmanyr(kmax,kmax)
	howmanyr = new int*[kmax+1];
	for (i=0;i<=kmax;i++)
	{
		howmanyr[i] = new int [kmax+1];
	}

	for (i=0; i<=kmax; i++)
	{
		for (j=0; j<=kmax; j++)
		{
		  howmanyr[i][j] = 0;
		}
	}

	
	int *list,*no, *iordre;		//list(nmax),no(nmax), iordre(nmax)
	list = new int [nmax+1];
	no = new int [nmax+1];
	iordre = new int [nmax+1];

	for (i=0; i<=nmax; i++)
	{
		list[i] = 0;
		no[i] = 0;
		iordre[i] = 0;
	}

	int *howmany,*nobest/* ,*nobestCH, *nobestBH, *nobestSilhouette, *nobestLogSS, *nobestW */, *nnitr;		//howmany(kmax),,nobest(kmax), nnitr(kmax);	
	howmany = new int [kmax+1];
	nobest = new int [kmax+1];
    nnitr = new int [kmax+1];
	
	for (i=0; i<=kmax; i++)
	{
		howmany[i] = 0;
		nobest[i] = 0;
		nnitr[i] = 0;
	}

	int *ishort;			//ishort(pmax);										
	ishort = new int [pmax+1];
	
	for (i=0; i<=pmax; i++)
	{
		ishort[i] = 0;
	}


//  Modification Centroids: add ",nameb" to next line
	char *namea,*nameb, *namew; 
	namea = new char [300];
	nameb = new char [300];
    namew = new char [300]; //--Weight matrix file
        

//***********************  Read data file  **********************************
  
                       
	strcpy(namea,argv[1]);
	
	
	// Ouvertute du fichier contenant les parametres du programme Consense
	ofstream myfile;
	myfile.open ("parametresConsense");
	myfile << argv[1];
	myfile << "\n";
	myfile << "C\n";
	myfile << "C\n";
	myfile << "Y\n";
	myfile.close();
 
	
	//appel du logiciel consense
	system("./consense <parametresConsense >outTmp");
    
	//Format consensus tree C for only one line
	system("cat outtree|tr \"\n\" \" \">tmp.txt");
	system("cp tmp.txt outtree >terminal.txt");

	//Recuperer le string du consensus des arbres de départ par la variable C
	ifstream fileC;
	fileC.open("outtree");
	string C;
	getline(fileC,C);
	fileC.close(); 
	
	system("rm outtree outfile outTmp >terminal.txt");
	
    //--Read the data from files
    ReadData1(n,nmax,p,pmax,mat/* ,coord */,ishort,weight,mean,ntran,namea, nameb, namew, N, pLeavesAbsent);	//Call ReadData11(n,nmax,p,pmax,mat,coord,ishort,w,mean,ntran,namea)

	int max_k1 = 5;
	
	if (max_k1>10) max_k1=5;
	
	k1=max_k1;
	
		if(parametres == "NewCH"){
		CHr = new double [kmax+1];
		for (i=0; i<=kmax; i++)
		{
			CHr[i] = -1.0;
		}
		
		intParam=1;
		k2=2;
		if (k1<=2) {
			printf("*** Warning, not enough trees (k1:%d) k1 set to 3\n",k1);
			k1=max_k1;      
		}
	}else if(parametres == "BH"){
		BHr = new double [kmax+1];
		for (i=0; i<=kmax; i++)
		{
			BHr[i] = 0.0;
		}
		
		intParam=2;
	}else if(parametres == "Silhouette"){
		Silr = new double [kmax+1];
		for (i=0; i<=kmax; i++)
		{
			Silr[i] = 0.0;
		}
		
		intParam=3;
	}else if(parametres == "LogSS"){
		LogSSr = new double [kmax+1];
		for (i=0; i<=kmax; i++)
		{
			LogSSr[i] = 0;
		}
		
		intParam=4;
	}else if(parametres == "OldCH"){
		CHr = new double [kmax+1];
		for (i=0; i<=kmax; i++)
		{
			CHr[i] = -1.0;
		}
		
		intParam=5;
		k2=2;
		if (k1<=2) {
			printf("*** Warning, not enough trees (k1:%d) k1 set to 3\n",k1);
			k1=max_k1;      
		}
	}else if(parametres == "NewCHcarre"){
		CHr = new double [kmax+1];
		for (i=0; i<=kmax; i++)
		{
			CHr[i] = 0.0;
		}
		
		intParam=6;
		
		k2=2;
		if (k1<=2) {
			printf("*** Warning, not enough trees (k1:%d) k1 set to 3\n",k1);
			k1=max_k1;      
		}
	}else if(parametres == "OldCHcarre"){
		CHr = new double [kmax+1];
		for (i=0; i<=kmax; i++)
		{
			CHr[i] = 0.0;
		}
		
		intParam=7;
		
		k2=2;
		if (k1<=2) {
			printf("*** Warning, not enough trees (k1:%d) k1 set to 3\n",k1);
			k1=max_k1;      
		}
	}
	
	  
	if (k1>kmax) {
	  printf("*** Warning, limiting groups to %d \n",kmax);
	  k1=max_k1-1;          
	}

	if (hard_max_k!=0) {
	  k1=max_k1;          
	}

	  
      CompSST(n,nmax,p,pmax,mat,weight,ishort,SST);

        // Compute vector 'mean' of overall means
      for (j=1;j<=p;j++)  { mean[j]=0;  }
	  
      for (i=1;i<=n;i++) 
	  {
		  for (j=1;j<=p;j++) 
		  {
			  mean[j]=mean[j]+mat[i-1][ishort[j]-1];
		  }
	  }
	  
      for (j=1;j<=p;j++)
	  {
		  mean[j]=mean[j]/(n*1.0);//18 mean(j)=mean(j)/dfloat(n)
	  }
       
	  double a=0;
	  double b=0;
	  double RI = 0.000; 
	  double ARI = 0.000;
	  iseed=0;
	  double CH_new = 0.0;
	  int taille_resultats = 4;
	  double *resultats = new double[taille_resultats];
	  double *resultats_final = new double[taille_resultats];
	  double W_new = 1000000000.0;
	  
	 	  
		//initialize Sref
		int Sref [N];
		for (int j=0; j<N; j++){
			Sref[j]=0;
		}
		
		
		//initialize resultats et resultats_final
		for (int j=0; j<taille_resultats; j++){
			resultats[j]=0.0;
			resultats_final[j]=0.0;
		}
		
	  
		int number_cluster = 0;
		int *nk = new int [kmax+1];
		int k_cluster = 0;
		 
		int nbInit =0;
		int nbFin =0;
		int comb = 1;
		map <int, int> CH_conversion;
		map <int, int> BH_conversion;
		map <int, int> Silhouette_conversion;
		map <int, int> LogSS_conversion;	
		map <int, int> W_conversion;

		int realk = 0;
		int unique = 0;
		int CHk = 0;
		int BHk = 0;
		int Silhouettek = 0;
		int LogSSk = 0;
		int wk = 0;
							
      for (iran=1;iran<=nran; iran++) {
			W_new = 1000000000.0;
			CH_new = 0.0;
		  if(iassign!=4) 		 
		  {
			  Assign(iran,n,nmax,k1,kmax,list,howmany,no,idebug,iassign,iseed, random_number);
			  /* printf ("Currently working on random start no. %d\n",iran);		 */
		  }

                  // Big loop on number of groups, downwards from k1 to k2 (k1>=k2) - - - - - -
		  niter=10;	

		  for (kk=k1;kk>=k2;kk--)			
		  {
			  /* cout<<"Test pour "<<kk<<" clusters"<<endl; */
			  SSEref=1.0e20;		
			  W = 1000000000.0;
			  CH = 0.0;
			  silhouette=0.0;
			  LogSS=0.0;
			  BH=0.0;
			  
			  for (nit=1;nit<=niter;nit++) 
			  {                     
				  if(idebug==1)    
				  {
					  printf ("Iteration = %d",nit);	
					  printf ("SSEref = %lf",SSEref);	
					  for (i=1;i<=n;i++)
					  {
						  printf ("%lf  ",list[i]);	
					  }
				  }					
				  nnit=nit;			

                  // If centroids are read, we want to use then during the first iteration
                  // (nit-1) of the first level of the cascade (kk=k1).
				  if(!((iassign==4)&&(kk==k1)&&(nit==1))) //      if(.not.((iassign.eq.4).and.(kk.eq.k1).and.(nit.eq.1))) then
				  {
                      // Compute centroids for all groups; write to matrix 'xbar'.
					  Centroids(n,nmax,p,pmax,kmax,mat,sx,xbar,mean,list,howmany,kk,ishort,idebug);	    
				  }	

                  // Compute distances to group centroids and assign objects to nearest one      
				  Distances(n,nmax,p,pmax,kmax,mat,xbar,Dvec,list,howmany,SSE,kk,weight,ishort,idebug, W);//Call Distances(n,nmax,p,pmax,kmax,mat,xbar,Dvec,list,howmany,SSE,kk,w,ishort,idebug)
				  number_cluster = 0;
				  		  
					switch (intParam) {
						 
						 case 5:
							if(CHr[kk]==10000000.0){
								nnitr[kk]=nnit;	//nnitr(kk)=nnit
								SSEr[kk]=SSE;		//SSEr(kk)=SSE
								nobest[kk]=iran;	//nobest(kk)=iran
								
								k_cluster = 0;
								
								for(int k=1;k<=kmax; k++){
									nk[k]=0;
								}
								
								for(int k=1;k<=kmax; k++){
									nk[list[k]]++;
								}
								
								for(int k=1;k<=kmax; k++){
									if(nk[k]!=0){
										k_cluster++;
									}
								}
								
								for (int i=0;i<taille_resultats;i++)			//do 65 i=1,n
								{
									resultats_final[i]=resultats[i];
									/* cout<<i<<" "<<resultats_final[i]<<endl; */
								}
								
								goto m60;
							}
							else{
								CH_new = DistanceCH_old(monTableau,C,n,kmax,mat,list,Ww,resultats,N_especes);
								
								if(CH_new>CH){
									SSEr[kk]=SSE;		//SSEr(kk)=SSE
									nobest[kk]=iran;	//nobest(kk)=iran
								
									nnitr[kk]=nnit;	//nnitr(kk)=nnit
									CH=CH_new;
									CHr[kk]=CH;  //CHr(kk)=CH
									for (int i=1;i<=n;i++)			//do 65 i=1,n
									{
										listr[kk][i]=list[i];
									}	//65    listr(kk,i)=list(i)
									
									for (int i=0;i<taille_resultats;i++)			//do 65 i=1,n
									{
										resultats_final[i]=resultats[i];
										/* cout<<i<<" "<<resultats_final[i]<<endl; */
									}
									
									for (int i=1;i<=kk;i++)				//do 67 i=1,kk
									  {howmanyr[kk][i]=howmany[i];}	//67    howmanyr(kk,i)=howmany(i)
								}
							}
							break;
							
					}
				  
				  // Compute sum of squared error statistic (SSE) = within-group sum of squares
				  if(fabs(SSEref-SSE)>(SSE/1000.0))			//if(dabs(SSEref-SSE).gt.SSE/1000.0) then
				  {
					  SSEref=SSE;//SSEref=SSE
				  }
				  else			
				  {
					goto m60;  
				  }	
					
			  }				
              // Compute the Calinski-Harabasz (1974) index 'CH' and
			  printf ("Convergence not reached in %d iterations.",niter);// write(*,*) 'Convergence not reached in ',niter,' iterations.'
m60:          
                         
      
              //--CALCUL DE CALINSKI HARABASS
           		
              // Concatenate the two closest groups before going to the next value of kk
			  Dref=1000000.0;	//Dref=1000000.0
			  D1=0.0;		//D1=0.0
			  i1ref=1;			//i1ref=1
			  i2ref=2;			//i1ref=1
			  for (igr1=1;igr1<=kk;igr1++)		//do 68 igr1=1,kk
			  {
				  for (igr2=(igr1+1);igr2<=kk;igr2++)		//do 68 igr2=(igr1+1),kk
				  {
					  D1=0.0;		//D1=0.0
					  for (j=1;j<=p;j++)	//do 66 j=1,p
					  {
						  D1=D1+( (xbar[igr1][j]-xbar[igr2][j])*(xbar[igr1][j]-xbar[igr2][j]));		//66 D1=D1+w(ishort(j))*( (xbar(igr1,j)-xbar(igr2,j))**2 )
					  }
					  if(D1<Dref) //if(D1.lt.Dref) then
					  {
						  i1ref=igr1;		//i1ref=igr1
						  i2ref=igr2;		//i2ref=igr2
						  Dref=D1;			//Dref=D1
					  }					//endif
				  }
              }		

              //Group "i2ref" disappears
			  for (i=1;i<=n;i++)		
			  {
				  if(list[i]==i2ref){
					list[i]=i1ref;	
				  }
				  if(list[i]==i2ref) list[i]=list[i]-1;			
			  }		//70 continue
			  
			  howmany[i1ref]=howmany[i1ref]+howmany[i2ref];		
			  
			  for (k=(i2ref+1);k<=kk;k++)			
			  {
				  howmany[k-1]=howmany[k];		
			  }	
		  }	
		//--------------------------------------------------------------------
        // Affichage des organisation des groupes pour chaque nran (random start)
        //--------------------------------------------------------------------
		nbInit =0;
		nbFin =0;

		for(int i=0;i<tabIndices.size();i++){
			nbFin+=tabIndices.at(i);
			for (int j=nbInit; j<nbFin; j++){
				Sref[j]=i+1;
			}
			nbInit+=tabIndices.at(i);
		}
		
		switch (intParam) {
			 
			 case 5:
			 {			
				for (k=k1;k>=k2;k--)		//do 92 k=k2,k1
				{
					if (CHr[k]>=CHr_max) {
						CHr_group=k;
						CHr_max=CHr[k];
						
						realk=0;
						unique=0;
						CHk = 0;
						
						for(int i=1; i<=k; i++){
							unique=0;
							for(int j=1; j<=n; j++){
								if(listr[CHr_group][j]==i && unique==0){
									CHk++;
									CH_conversion[i] = CHk;
									unique=1;
									realk++;
								}
							}
						}
						
						for(int iz=1; iz<=N; iz++){
							listr[CHr_group][iz]=CH_conversion[listr[CHr_group][iz]];
							Strouve[iz-1]=CH_conversion[listr[CHr_group][iz]];
						}
						CHr_group=realk;
					}	
				}
				
			}break;
		}
				
	} 
// Print results

	
		switch (intParam){

			case 5:
			{
				strcpy(criteria, "Old CH");
				DistanceCH_old_final(monTableau,C,n,kmax,mat,listr[CHr_group],Ww,resultats,N_especes);
				outStat(Strouve,Sref,criteria,N,N_especes,percent,K_real,CHr_group,CHr_max,resultats_final,listr);
			}break;
			
		}
		
	  

	
	
	// Print results
	
	for (i=0;i<=kmax;i++)
	{
		delete [] sx[i];
		delete [] sx2[i];
		delete [] xbar[i];
		delete [] var[i];
		delete [] listr[i];
		delete [] howmanyr[i];
	}
	
	delete [] sx;
	delete [] sx2;
	delete [] xbar;
	delete [] var;
	delete [] listr;
	delete [] howmanyr;
	
	delete [] Dvec;
	if(intParam==1 || intParam==6){
		delete [] CHr;
	}else if(intParam==2){
		delete [] BHr;
	}else if(intParam==3){
		delete [] LogSSr;
	}else if(intParam==4){
		delete [] Silr;
	}else if(intParam==5 || intParam==7){
		delete [] CHr;
	}

	delete [] SSEr;

	delete [] vect;
	delete [] mean;
	delete [] weight;


	delete [] list;
	delete [] no;
	delete [] iordre;

	delete [] howmany;
	delete [] nobest;
	delete [] nnitr;

	delete [] ishort;

	/* cout<<"QUITTTER !!"<<endl; */
	delete [] namea;
	delete [] nameb;
	delete [] namew;
	delete [] nk;
	delete [] resultats;
	delete [] resultats_final;
	
	// End timer
    tend2=time(NULL);				// get the current calendar time

	// Compute execution time
    texec2=difftime(tend2,tbegin2);	// tend-tbegin (result in second)
	fprintf (Output4,"%.3f;\n",texec2);
	// *********************Close output files ***************************
	fclose(Output4);
	//*********************** Remove matrix ******************************
	
	return 0;
}


 //      end
//************************End of Main

//******************************************************************************
//**********************************FUNCTIONS***********************************
//******************************************************************************

//stat output
void outStat(int Strouve[],int Sref[],char *criteria,int N,char *N_especes,char *percent,const char *K_real,int group/* ,double RI,double ARI */,double score,double* resultats_final,int **listr)
{
	
	double RI = f_RI(Strouve,Sref,N);
	double ARI = f_ARI(Strouve,Sref,K_real,group,N);
		
	fprintf (Output4,"%s;",criteria);
	fprintf (Output4,"%i;",N);
	fprintf (Output4,"%s;",N_especes);
	fprintf (Output4,"%s;",percent);
	fprintf (Output4,"%s;",K_real);
	fprintf (Output4,"%d;",group);
	int diff = atoi(K_real)-group;
	diff = fabs(diff);
	const int const_K_real = atoi(K_real);
	const int const_group = group;
	
	double max_k = max(const_K_real,const_group);
	double diff_norm = (diff*1.0)/(max_k*1.0);
	fprintf (Output4,"%i;",diff);
	fprintf (Output4,"%.3f;",diff_norm);
	if(atoi(K_real) == group){
		fprintf (Output4,"%i;",1);
	}else{
		fprintf (Output4,"%i;",0);
	}
	fprintf (Output4,"%.3f;",RI);
	fprintf (Output4,"%.3f;",ARI);	
	fprintf (Output4,"%.3f;",score);
	
	fprintf (Output4,"%.5f;",resultats_final[1]);
	fprintf (Output4,"%.5f;",resultats_final[2]);
	
	fprintf (Output4,"part(");
	for (int p=1; p<=N; p++){
		if(p==N){
			fprintf (Output4,"%i%s",listr[group][p]," ");
		}else{
			fprintf (Output4,"%i%s",listr[group][p]," <> ");
		}
		
	}
	fprintf (Output4,");");
	
/* 	string partition = "";
	
	for (int p=1; p<=N; p++){
		partition += std::to_string(listr[group][p]);
		partition += " <> ";
	}
	
	fprintf (Output4,"%s;",partition); */

}

//compute rand index
double f_RI(int Strouve[],int Sref[],int N)
{
	double comb = 1.0;

	for (int i=N; i>=(N-2+1); i--)
	{
		comb*=i;
	}

	comb/=2;
	 
	double a=0.0;
	double b=0.0; 
							
	for (int i=0; i<N-1; i++){
		for (int j=i+1; j<N; j++){
			if(Sref[i]!=Sref[j]){
				if(Strouve[i]!=Strouve[j]){
					b++;
				}
			}else{
				if(Strouve[i]==Strouve[j]){
					a++;
				}
			}
		}
	}						
	double RI = (a+b)/comb;
	
	return RI;
}

//compute adjusted rand index
double f_ARI(int Strouve[],int Sref[],const char *K_real,int group,int N)
{
	int kReal = atoi(K_real);
	int tabCongruence [kReal+1][group+1];
	int sumLigne [kReal+1];
	int sumColonne[group+1];
	
	//initialisation du tableau sumLigne à 0
	for(int i=0;i<=kReal;i++){
		sumLigne[i]=0;
	}
	
	//initialisation du tableau sumColonne à 0
	for(int i=0;i<=group;i++){
		sumColonne[i]=0;
	}
	
	//initialisation du tableau des congruence à 0
	for(int i=0;i<=kReal;i++){
		for(int j=0;j<=group;j++){
			tabCongruence[i][j]=0;
		}
	}
	
	
	for(int i=0;i<N;i++){
		tabCongruence[Sref[i]][Strouve[i]]++;
	}
	
	for(int i=1;i<=kReal;i++){
		for(int j=1;j<=group;j++){
			sumLigne[i]+=tabCongruence[i][j];
			sumColonne[j]+=tabCongruence[i][j];
		}
	}
	
	double a=0.0;
	double b=0.0; 
	double c=0.0;
	double d=0.0; 
	
	double comb = 1.0;

	for (int i=N; i>=(N-2+1); i--)
	{
		comb*=i;
	}

	comb/=2;
							
	for (int i=0; i<N-1; i++){
		for (int j=i+1; j<N; j++){
			if(Sref[i]!=Sref[j]){
				if(Strouve[i]!=Strouve[j]){
					d++;
				}else{
					c++;
				}
			}else{
				if(Strouve[i]==Strouve[j]){
					a++;
				}else{
					b++;
				}
			}
		}
	}
	
	double ARI = 0.0;

	if(a*2.0==((b+a)+(c+a))){
		ARI = 1.0;
	}else{
		ARI = a - ((b+a)*(c+a))/comb;
		ARI=ARI/((((b+a)+(c+a))/2.0)-(((b+a)*(c+a))/comb));
	}

	return ARI;
}


//////////////////////
//***********CheckCentr
//////////////////////

// Modification Centroids: this whole subroutine

void CheckCentr(int &n,int &nmax,int &p,int &pmax,int &k1,int &kmax,double** mat,double** xbar,int* ishort,double** sx,int &idebug)
{

		   double temp=0;		//      Real*8 mat(nmax,pmax),sx(kmax,pmax),xbar(kmax,pmax),temp
		   int j=0, i=0, kk=0;
//      Integer ishort(pmax)
// Check that the k1 centroids provided are within the range of data values.
// 'sx' is used to store the minimum and maximum values of the variables.
		   for (j=1;j<=p;j++)		//do j=1,p
		   {
			   sx[1][j]=mat[1-1][j-1];	//sx(1,j)=mat(1,j)
			   sx[2][j]=mat[1-1][j-1];	//sx(2,j)=mat(1,j)
		   }						//enddo


		   for (i=2;i<=n;i++)		//do i=2,n
		   {
			   for (j=1;j<=p;j++)		//do j=1,p
			   {
				   temp=mat[i-1][ishort[j]-1];				//temp=mat(i,ishort(j))
				   if(temp<sx[1][j]) sx[1][j]=temp;		//if(temp.lt.sx(1,j)) sx(1,j)=temp
				   if(temp<sx[2][j]) sx[2][j]=temp;		//if(temp.gt.sx(2,j)) sx(2,j)=temp
			   }										//enddo
		   }											//enddo

		   for (kk=1;kk<=k1;kk++)		//do kk=1,k1
		   {
			   for (j=1;j<=p;j++)		//do j=1,p
			   {
				   if((xbar[kk][j]<sx[1][j])||(xbar[kk][j]>sx[2][j]))		//if((xbar(kk,j).lt.sx(1,j)).or.(xbar(kk,j).gt.sx(2,j))) then
				   {
					   printf("Centroids not within the range of the (transformed?) data.");	//stop 'Centroids not within the range of the (transformed?) data.'
					   exit(1);
				   }				//endif
			   }					//enddo
		   }						//enddo
      return;
      
}//end
//************************End of CheckCentr

//////////////////////
//***********Assign
//////////////////////

void Assign(int &iran,int &n,int &nmax,int &k1,int &kmax,int* list,int* howmany,int* no,int &idebug,int &iassign,int &iseed, int random_number)
	  
{
      int k=0, i=0, ii=0, itemp=0, kk=0, how=0, isum=0;	
      char namea[255];
      double turn=0;
	  
        // Assign objects to groups.
        // On output, vector 'list' gives group assignments for the objects
        // whereas vector 'howmany' gives the number of objects in each group

   if ((iassign==1) || (iassign==2))   		
   {
	   how=n/k1;
	   for (k=1;k<=(k1-1);k++) {howmany[k]=how;}				   
	   howmany[k1]=n-(k1-1)*how;	
	   ii=0;			

	   for (k=1;k<=k1;k++)		
	   {
		   for (kk=1;kk<=howmany[k];kk++)	
		   {
			   ii++;			
			   list[ii]=k;
		   }
	   }						


	   if(iassign==1) return;	
           // Assign objects at random to the groups
	   if(iran==1)			
	   {
		  for (i=1;i<=(random_number+100);i++)  turn=rand()/32767.0;
	   }							//end if
           Permute(iseed,n,nmax,list);	
           return;
   }
   else if (iassign==3)
   {
// Read file of group assignments.
// First line: how many objects in each group?
// Then, read members of each group on one line (list of object numbers).
   
	   printf ("Name of file of group assignments?");		//60 write(*,*) 'Name of file of group assignments?'
	   scanf ("%s",namea);		//read(*,*) namea

	   FILE *Input3;
	   if ((Input3 = fopen(namea,"r"))==0) { printf("\n %s :Open Failed....",namea); exit(1); }   	
          
           printf ("File of group assignments: %s\n",namea);
		  		 
	  for (k=1;k<=k1;k++)				
	  {
		  fscanf(Input3,"%d",&howmany[k]);
	  }

      isum=0;							
	  for (k=1;k<=k1;k++)				
	  {
		  isum=isum+howmany[k];			
	  }

      if(isum!=n)					
	  {
		  printf("Objects assigned to groups do not sum to n.");
		  exit(1);
	  }
      	  
	  for (i=1;i<=n;i++) {
		  list[i]=-1;				
	  }

	  for (k=1;k<=k1;k++)			
	  {
		  for (i=1;i<=howmany[k];i++)				
		  {
			  fscanf(Input3, "%d", no[i]);
		  }
     	  for (i=1;i<=howmany[k];i++)		
		  {									
			  list[no[i]]=k;					
		  }
	  }

	  for (i=1;i<=n;i++)	
	  {	
		  if(list[i]==-1)	
		  {
			  printf("Overlapping assignments to groups.");
			  exit(1);
		  }									
	  }
      fclose(Input3);			
      return;
   }
   else 
   {
          printf("Wrong perameter <iassign> in function <Assign>.");
          exit(1);
   }									

} // end
//************************End of Assign


//////////////////////
//***********Centroids
//////////////////////
void Centroids(int &n,int &nmax,int &p,int &pmax,int &kmax,double** mat,double** sx,double** xbar,double* mean,int* list,int* howmany,int &kk,int* ishort,int &idebug)
{  
	int k=0, j=0, i=0, itemp=0;

// Compute centroids for all groups; write to matrix 'xbar'.
// Vector 'list' contains group assignments for the objects.
// Vector 'howmany' contains the number of objects in each group.

	
	//initialisation de sx[][] par 0.0
	for (k=1;k<=kk;k++)		//do 4 k=1,kk
	{
		for (j=1;j<=p;j++)		//do 4 j=1,p
		{
			sx[k][j]=0.0;		//    4 sx(k,j)=0.0
		}
	}

	for (i=1;i<=n;i++)		//do 5 i=1,n
	{
		for (j=1;j<=p;j++)		// do 5 j=1,p
		{
			itemp=list[i];		//itemp=list(i)
			sx[itemp][j]=sx[itemp][j]+mat[i-1][ishort[j]-1];		//5 sx(itemp,j)=sx(itemp,j)+mat(i,ishort(j))
		}
	}

	for (k=1;k<=kk;k++)		
	{
		itemp=howmany[k];	
		if(itemp==0)			//if(itemp.eq.0) then
		{
			for (j=1;j<=p;j++)		// do 6 j=1,p
			{
				xbar[k][j]=mean[j];		//6    xbar(k,j)=mean(j)
			}
		}
        else	
		{
			for (j=1;j<=p;j++)		// do 7 j=1,p
			{
				xbar[k][j]=sx[k][j]/static_cast<float>(itemp);		//7    xbar(k,j)=sx(k,j)/dfloat(itemp)
			}
		}			
	}
	

	return;
}			//  end
//************************End of Centroids


////////////////////
//***********CompSST
////////////////////
void CompSST(int &n,int &nmax,int &p,int &pmax,double** mat,double* weight,int* ishort,double &SST)
{
	double	sx=0,sx2=0,var=0,temp=0,dfln=0;	 //Real*8 mat(nmax,pmax),weight(pmax),sx,sx2,var,temp,dfln,SST
	int j=0, i=0;
      dfln=n;		//dfln=dfloat(n)
      SST=0.0;				//SST=0.0

	  for (j=1;j<=p;j++)		// do 22 j=1,p
	  {
		  sx=0.0;		//sx=0.0
		  sx2=0.0;		//sx2=0.0
		  for (i=1;i<=p;i++)		// do 20 i=1,n
		  {    
			  temp=mat[i-1][j-1];
			  sx=sx+temp;				//sx=sx+temp
			  sx2=sx2+temp*temp;		//20 sx2=sx2+temp*temp
		  }
		  var=sx2-(sx*sx/dfln);			//var=sx2-(sx*sx/dfln) 
		  SST=SST+var*weight[ishort[j]];		//22 SST=SST+var*weight(ishort(j))
	  }
      return;
}				//      end
//************************End of CompSST


//////////////////////
//***********Distances
//////////////////////
      
void Distances(int &n,int &nmax,int &p,int &pmax,int &kmax,double** mat,double** xbar,double* Dvec,int* list,int* howmany,double &SSE,int &kk,double* weight,int* ishort,int &idebug, double &W)
{

	  double Dref=0,D1=0;		//Real*8 Dref,D1,SSE,Dvec(kmax),weight(pmax)
      int	kref=0, k=0,i=0;		//Integer list(nmax),howmany(kmax),kref
      //Integer ishort(pmax)
        // Compute squared distances to group centroids. Assign objects to nearest one
      SSE=0;		//SSE=0.0
		
	  map<int,string>  mapIndicesTrees;

	  for (k=1;k<=kk;k++)		// do 10 k=1,kk
	  {
		  howmany[k]=0;			//10 howmany(k)=0
	  }

     
// 
      for (i=1;i<=n; i++)			//do 20 i=1,n
	  {

		  for (k=1;k<=kk;k++)			//do 12 k=1,kk
		  {
		  
			//Calcul de la distance eucledian de chaque point i
			// et assignation du point i au bon cluster
			// Compute a SQUARED (weighted) Euclidean distance to the centroid k
			Euclidean(i,k,nmax,kmax,p,pmax,mat,xbar,weight,ishort,D1);	
			Dvec[k]=D1;
			if(k==1) 			
			{
			  Dref=D1;		
			  kref=1;		
			}
			else		
			{
			  if(D1<Dref)		
			  {
				  Dref=D1;		
				  kref=k;		
			  }		
			}			
		}			  
		  
		  
		  
		SSE=SSE+Dref;         //SSE=SSE+Dref		 
		howmany[kref]++;	//howmany(kref)=howmany(kref)+1

		/* nbNi[kref]=howmany[kref];	//howmany(kref)=howmany(kref)+1 */

		list[i]=kref;		//  20 list(i)=kref

		  
		// créer un flux de sortie
		ostringstream oss;
		// écrire un nombre dans le flux
		oss << i;
		// récupérer une chaîne de caractères
		string result = oss.str();

		oss<<mapIndicesTrees[kref]<<","<<i;
		mapIndicesTrees[kref]=mapIndicesTrees[kref]+","+result;
	  }
	  
	  std::map<int,string>::iterator it = mapIndicesTrees.begin();
	  
	  int testClustErempty=0;
	  for (it=mapIndicesTrees.begin(); it!=mapIndicesTrees.end(); ++it){
		testClustErempty++;
		mapIndicesTreesFinal.insert ( std::pair<int,string>(testClustErempty,it->second) );
	 }	
	 
	int max=-1;//le nombre d'élément max pour un cluster
	int k_max=0;//le numero du cluster le plus gros
	for (int k1=1;k1<=kk;k1++){
		max=-1;
		k_max=0;
		
		if(howmany[k1]==0){
			//pour trouver le plus gros cluster
			for(int k2=1;k2<=kk;k2++){
				if(howmany[k2]>max){
					max=howmany[k2];
					k_max=k2;
				}
			}
			
			//mettre le premier element de ce groupe dans le cluster vide
			int trouvee=0;
			int k3=1;
			while(trouvee==0){
				if(list[k3]==k_max){
					list[k3]=k1;
					howmany[k1]++;
					howmany[k_max]--;
					trouvee=1;
				}
				k3++;
			}
		}
	}
	 	it = mapIndicesTreesFinal.begin();
				   
      return;
    
}//  end


double DistanceCH_old_final(vector <string> monTableau,string C,int &n,int &kmax,double** mat,int* list,double** Ww,double* resultats,char *N_especes) {
	
double SSB=0.0;
	double SSW=0.0;
	double dist_all = 0.0;
	
	double distance_total = 0.0;
	int cluster_k=0;
	int *nk_CH = new int [kmax+1];
	int k_cluster = 0;
	/* const char * fileParaConsen = new char [100];	 */
	char *filename = new char [100];
	double *mat_distances = new double[4];
	double distancesRF;
	string str_Ci;	
	string str_xi;
	vector <string> Ci;
	vector <string> xi;
	int N_cluster_k = 0;
	double nb_esp_identique = 0;
	
	for(int k=1;k<=kmax; k++){
		nk_CH[k]=0;
	}
		
	for(int k=1;k<=kmax; k++){
		nk_CH[list[k]]++;
	}
	
	for(int k=1;k<=kmax; k++){
		if(nk_CH[k]!=0){
			k_cluster++;
		}
	}
	
	ofstream fichier("resultat_finaux.txt", ios::out | ios::trunc);
 
	if(fichier){       
		// instructions
			
			
		cout <<"ARBRES EN ENTREE "<<endl;
		fichier <<"ARBRES EN ENTREE "<<endl;
		for(int ij=0; ij<monTableau.size(); ij++){
			cout <<list[ij+1]<<" : "<<monTableau[ij]<<endl;
			fichier <<list[ij+1]<<" : "<<monTableau[ij]<<endl;
		}
		cout <<endl;
		cout <<endl;
		fichier <<endl;
		fichier <<endl;
		
		//Mettre tous les arbres de la meme repartition dans des mêmes fichier pour inférer leur consensus majoritaire
		for (int i=1;i<=monTableau.size();i++){
			cluster_k=list[i];
			
			sprintf(filename,"%s%d","outtree",cluster_k);
			
			/* Ouvertute du fichier qui contiendra tous les arbres de chaque partition */
			ofstream myfile;
			myfile.open (filename, ios::out | ios::app);
			myfile <<monTableau[i-1];
			myfile << "\n";
			myfile.close();
		}
		

		cout <<"NOMBRE D'ARBRES D'ENTREE "<<monTableau.size()<<endl;
		fichier <<"NOMBRE D'ARBRES D'ENTREE "<<monTableau.size()<<endl;
		//Inférer leur consensus majoritaire de chaque partition
		for (int i=1;i<=k_cluster;i++){
			
			sprintf(filename,"%s%d%s","cp outtree",i," intree >outTmp");
			// Ouvertute du fichier contenant les parametres du programme Consense
			system(filename);
			ofstream myfile;
			myfile.open ("parametresConsense");
			/* cout<<filename<<endl; */
			/* myfile << filename; */
			myfile << "intree";
			myfile << "\n";
			myfile << "C\n";
			myfile << "C\n";
			myfile << "Y\n";
			myfile.close();
		 
			
			//appel du logiciel consense
			system("./consense <parametresConsense >outTmp");
			
			//Format consensus tree Ci for only one line
			system("cat outtree|tr \"\n\" \" \">tmp.txt");
			system("cp tmp.txt outtree >terminal.txt");

			//Recuperer le string du consensus des arbres du cluster i par la variable Ci
			ifstream fileCi;
			fileCi.open("outtree");
			
			getline(fileCi,str_Ci);
			Ci.push_back(str_Ci);
			fileCi.close(); 
			
			system("rm outtree outfile outTmp intree >terminal.txt");
			
		}
		cout <<endl;
		cout <<"NOMBRE DE CLUSTERS = "<<k_cluster<<endl;
		fichier <<endl;
		fichier <<"NOMBRE DE CLUSTERS = "<<k_cluster<<endl;

		cout <<endl;
		fichier <<endl;
		for (int i=1;i<=k_cluster;i++){
			cout <<"\tLISTE DES ARBRES DU GROUPE "<<i<<endl;
			N_cluster_k = 0;
			sprintf(filename,"%s%d","outtree",i);
			ifstream fichierk(filename, ios::in);  // on ouvre en lecture

			if(fichierk)
			{
				string lignek;
				while(getline(fichierk, lignek))  // tant que l'on peut mettre la ligne dans "contenu"
				{
					cout  << "\t"<<lignek << endl;  // on l'affiche
					fichier  << "\t"<<lignek << endl;  // on l'affiche
					N_cluster_k++;
				}
				fichierk.close();
			}

			else
				cerr << "Impossible d'ouvrir le fichier !" << endl;

			cout <<endl;
			fichier <<endl;
			str_Ci=Ci.at(i-1);
			
			cout <<"\tARBRE CONSENSUS DU GROUPE "<<i<<" : "<<endl;
			cout <<"\t"<<str_Ci<<endl;
			cout <<"\tNOMBRE D'ARBRES PHYLOGENETIQUES APPARTENANT AU GROUPE "<<i<<" = "<<N_cluster_k<<endl;
			cout <<endl;
			cout <<endl;
			
			fichier <<"\tARBRE CONSENSUS DU GROUPE "<<i<<" : "<<endl;
			fichier <<"\t"<<str_Ci<<endl;
			fichier <<"\tNOMBRE D'ARBRES PHYLOGENETIQUES APPARTENANT AU GROUPE "<<i<<" = "<<N_cluster_k<<endl;
			fichier <<endl;
			fichier <<endl;
		}
		
		//compute SSB
		/* cout<<"\nCalcul de SSB:"<<endl; */
		for (unsigned ni=0; ni<Ci.size(); ++ni) {	
			N_cluster_k = 0;
			/* cout<<"Pour K = "<<ni+1<<endl; */
			// Appel des algorithmes des calcules des distances : RF
			str_Ci=Ci.at(ni);
	/* 		cout<<str_Ci<<endl;	
			cout<<C<<endl;	 */

			//Appel de main_hgt pour connaitre le nb d'especes identiques
	/* 		main_hgt(str_Ci,C,mat_distances);
			nb_esp_identique = mat_distances[3]; */
			
			nb_esp_identique = atoi(N_especes);
			ofstream myTrees;
			myTrees.open ("myTrees");
			myTrees << str_Ci<<"\n";
			myTrees << C<<"\n";
			myTrees.close();
				
			mat_distances[0]=NouvelleCallRF(nb_esp_identique);
			distancesRF=mat_distances[0];
			/* cout<<"RFnorm = "<<distancesRF; */
					
			sprintf(filename,"%s%d","outtree",ni+1);
			ifstream fichier(filename, ios::in);  // on ouvre en lecture
	 
			if(fichier){
				// si l'ouverture a fonctionné
				string ligne;
				// tant que l'on peut mettre la ligne dans "contenu"
				while(getline(fichier, ligne)){
					N_cluster_k ++;
				}

				fichier.close();
			}else{
				cerr << "Impossible d'ouvrir le fichier !" << endl;
			}
			
			/* cout<<" et Nk = "<<N_cluster_k<<endl; */
			distancesRF=N_cluster_k*distancesRF;
			SSB+=distancesRF;
		}
		
		resultats[2]=SSB;
		/* cout<<"Fonction SSB = "<<SSB<<endl;	 */
		
		double dist = 0.0;
		
		//compute SSW
		/* cout<<"\nCalcul de SSW:"<<endl; */
		for (int i=1;i<=k_cluster;i++){
			dist = 0;
			/* cout<<"Pour K = "<<i<<endl; */
			
			sprintf(filename,"%s%d","outtree",i);
			ifstream fichier(filename, ios::in);  // on ouvre en lecture
	 
			if(fichier){
				// si l'ouverture a fonctionné
				string ligne;
				// tant que l'on peut mettre la ligne dans "contenu"
				while(getline(fichier, ligne)){
					xi.push_back(ligne);
				}

				fichier.close();
			}else{
				cerr << "Impossible d'ouvrir le fichier !" << endl;
			}

			str_Ci=Ci.at(i-1);
			for (unsigned ni=0; ni<xi.size(); ++ni) {
				//Appel des algorithmes des calcules des distances : RF
				str_xi=xi.at(ni);
	/* 			cout<<str_xi<<endl;
				cout<<str_Ci<<endl;	 */

				//Appel de main_hgt pour connaitre le nb d'especes identiques
	/* 			main_hgt(str_xi,str_Ci,mat_distances);
				nb_esp_identique = mat_distances[3]; */
				
				nb_esp_identique = atoi(N_especes);
				ofstream myTrees;
				myTrees.open ("myTrees");
				myTrees << str_xi<<"\n";
				myTrees << str_Ci<<"\n";
				myTrees.close();
				
				distancesRF = NouvelleCallRF(nb_esp_identique);
				/* cout<<"RFnorm = "<<distancesRF<<endl; */
				SSW+=distancesRF;
			}
			
			//vider le vector xi
			xi.clear();
			
		}	
		
		resultats[1]=SSW;
		/* cout<<"Fonction SSW = "<<SSW<<endl; */
		
		
		if((fabs(SSW)>0.000001) && (k_cluster>1)){
			distance_total=(SSB/SSW)*((n-k_cluster)/(k_cluster-1.0));
		}
		else if(fabs(SSW)<0.000001){
			distance_total=10000000.0;
		}
		
	/* 	cout<<"dist_all "<<dist_all<<" K "<<k_cluster<<" SSB "<<SSB<<" SSW "<<SSW<<" distance_total "<<distance_total<<endl;
		for (int i=1;i<=n;i++)
		{
			cout<<list[i]<<" ";
		} */
		
		system("rm out*  >terminal.txt");
		fichier.close();  // on ferme le fichier
	}else{
        cerr << "Impossible d'ouvrir le fichier !" << endl;
	}
	delete [] mat_distances;
	delete [] filename;
	delete [] nk_CH;
	
	resultats[0]=distance_total;
	/* cout<<"\nOld CH = "<<distance_total<<endl; */
	
	//vider le vector Ci
	Ci.clear();
	
	return distance_total;
	
	
    
}//  end ************************End of Distances DistanceCH_old_final

double DistanceCH_old(vector <string> monTableau,string C,int &n,int &kmax,double** mat,int* list,double** Ww,double* resultats,char *N_especes) {
	
	double SSB=0.0;
	double SSW=0.0;
	double dist_all = 0.0;
	
	double distance_total = 0.0;
	int cluster_k=0;
	int *nk_CH = new int [kmax+1];
	int k_cluster = 0;
	/* const char * fileParaConsen = new char [100];	 */
	char *filename = new char [100];
	double *mat_distances = new double[4];
	double distancesRF;
	string str_Ci;	
	string str_xi;
	vector <string> Ci;
	vector <string> xi;
	int N_cluster_k = 0;
	double nb_esp_identique = 0;
	
	for(int k=1;k<=kmax; k++){
		nk_CH[k]=0;
	}
		
	for(int k=1;k<=kmax; k++){
		nk_CH[list[k]]++;
	}
	
	for(int k=1;k<=kmax; k++){
		if(nk_CH[k]!=0){
			k_cluster++;
		}
	}
	
		
/* 	cout<<"Arbres en entrée "<<endl;
	for(int ij=0; ij<monTableau.size(); ij++){
		cout<<list[ij+1]<<" : "<<monTableau[ij]<<endl;
	}
	cout<<endl; */
	
	
	//Mettre tous les arbres de la meme repartition dans des mêmes fichier pour inférer leur consensus majoritaire
	for (int i=1;i<=monTableau.size();i++){
		cluster_k=list[i];
		
		sprintf(filename,"%s%d","outtree",cluster_k);
		
		/* Ouvertute du fichier qui contiendra tous les arbres de chaque partition */
		ofstream myfile;
		myfile.open (filename, ios::out | ios::app);
		myfile <<monTableau[i-1];
		myfile << "\n";
		myfile.close();
	}
	

	/* cout<<"Nombre d'arbres d'entrée "<<monTableau.size()<<endl; */
	//Inférer leur consensus majoritaire de chaque partition
	for (int i=1;i<=k_cluster;i++){
		
		sprintf(filename,"%s%d%s","cp outtree",i," intree >outTmp");
		// Ouvertute du fichier contenant les parametres du programme Consense
		system(filename);
		ofstream myfile;
		myfile.open ("parametresConsense");
		/* cout<<filename<<endl; */
		/* myfile << filename; */
		myfile << "intree";
		myfile << "\n";
		myfile << "C\n";
		myfile << "C\n";
		myfile << "Y\n";
		myfile.close();
	 
		
		//appel du logiciel consense
		system("./consense <parametresConsense >outTmp");
		
		//Format consensus tree Ci for only one line
		system("cat outtree|tr \"\n\" \" \">tmp.txt");
		system("cp tmp.txt outtree >terminal.txt");

		//Recuperer le string du consensus des arbres du cluster i par la variable Ci
		ifstream fileCi;
		fileCi.open("outtree");
		
		getline(fileCi,str_Ci);
		Ci.push_back(str_Ci);
		fileCi.close(); 
		
		system("rm outtree outfile outTmp intree >terminal.txt");
		
	}
	
	/* cout<<"Nombre de clusters = "<<k_cluster<<endl; */

	for (int i=1;i<=k_cluster;i++){
		/* cout<<"LISTE DES ARBRES DU GROUPE "<<i<<endl; */
		N_cluster_k = 0;
		sprintf(filename,"%s%d","outtree",i);
		ifstream fichierk(filename, ios::in);  // on ouvre en lecture

		if(fichierk)
		{
			string lignek;
			while(getline(fichierk, lignek))  // tant que l'on peut mettre la ligne dans "contenu"
			{
				/* cout << lignek << endl;  // on l'affiche */
				N_cluster_k++;
			}
			fichierk.close();
		}

		else
			cerr << "Impossible d'ouvrir le fichier !" << endl;


		/* cout<<"ARBRE CONSENSUS DU GROUPE "<<i<<endl; */
		str_Ci=Ci.at(i-1);
/* 		cout<<"Arbre consensus du GROUPE "<<i<<" : "<<str_Ci<<endl;
		cout<<"Nombre d'arbres phylogénétiques appartenant au GROUPE "<<i<<" = "<<N_cluster_k<<endl;
		cout<<endl; */
	}
	
	//compute SSB
	/* cout<<"\nCalcul de SSB:"<<endl; */
	for (unsigned ni=0; ni<Ci.size(); ++ni) {	
		N_cluster_k = 0;
		/* cout<<"Pour K = "<<ni+1<<endl; */
		// Appel des algorithmes des calcules des distances : RF
		str_Ci=Ci.at(ni);
/* 		cout<<str_Ci<<endl;	
		cout<<C<<endl;	 */

		//Appel de main_hgt pour connaitre le nb d'especes identiques
/* 		main_hgt(str_Ci,C,mat_distances);
		nb_esp_identique = mat_distances[3]; */
		
		nb_esp_identique = atoi(N_especes);
		ofstream myTrees;
		myTrees.open ("myTrees");
		myTrees << str_Ci<<"\n";
		myTrees << C<<"\n";
		myTrees.close();
			
		mat_distances[0]=NouvelleCallRF(nb_esp_identique);
		distancesRF=mat_distances[0];
		/* cout<<"RFnorm = "<<distancesRF; */
				
		sprintf(filename,"%s%d","outtree",ni+1);
		ifstream fichier(filename, ios::in);  // on ouvre en lecture
 
        if(fichier){
			// si l'ouverture a fonctionné
			string ligne;
			// tant que l'on peut mettre la ligne dans "contenu"
			while(getline(fichier, ligne)){
				N_cluster_k ++;
			}

			fichier.close();
        }else{
			cerr << "Impossible d'ouvrir le fichier !" << endl;
		}
		
		/* cout<<" et Nk = "<<N_cluster_k<<endl; */
		distancesRF=N_cluster_k*distancesRF;
		SSB+=distancesRF;
    }
	
	resultats[2]=SSB;
	/* cout<<"Fonction SSB = "<<SSB<<endl;	 */
	
	double dist = 0.0;
	
	//compute SSW
	/* cout<<"\nCalcul de SSW:"<<endl; */
	for (int i=1;i<=k_cluster;i++){
		dist = 0;
		/* cout<<"Pour K = "<<i<<endl; */
		
		sprintf(filename,"%s%d","outtree",i);
		ifstream fichier(filename, ios::in);  // on ouvre en lecture
 
        if(fichier){
			// si l'ouverture a fonctionné
			string ligne;
			// tant que l'on peut mettre la ligne dans "contenu"
			while(getline(fichier, ligne)){
				xi.push_back(ligne);
			}

			fichier.close();
        }else{
			cerr << "Impossible d'ouvrir le fichier !" << endl;
		}

		str_Ci=Ci.at(i-1);
		for (unsigned ni=0; ni<xi.size(); ++ni) {
			//Appel des algorithmes des calcules des distances : RF
			str_xi=xi.at(ni);
/* 			cout<<str_xi<<endl;
			cout<<str_Ci<<endl;	 */

			//Appel de main_hgt pour connaitre le nb d'especes identiques
/* 			main_hgt(str_xi,str_Ci,mat_distances);
			nb_esp_identique = mat_distances[3]; */
			
			nb_esp_identique = atoi(N_especes);
			ofstream myTrees;
			myTrees.open ("myTrees");
			myTrees << str_xi<<"\n";
			myTrees << str_Ci<<"\n";
			myTrees.close();
			
			distancesRF = NouvelleCallRF(nb_esp_identique);
			/* cout<<"RFnorm = "<<distancesRF<<endl; */
			SSW+=distancesRF;
		}
		
		//vider le vector xi
		xi.clear();
		
    }	
	
	resultats[1]=SSW;
	/* cout<<"Fonction SSW = "<<SSW<<endl; */
	
	
	if((fabs(SSW)>0.000001) && (k_cluster>1)){
		distance_total=(SSB/SSW)*((n-k_cluster)/(k_cluster-1.0));
	}
	else if(fabs(SSW)<0.000001){
		distance_total=10000000.0;
	}
	
/* 	cout<<"dist_all "<<dist_all<<" K "<<k_cluster<<" SSB "<<SSB<<" SSW "<<SSW<<" distance_total "<<distance_total<<endl;
	for (int i=1;i<=n;i++)
	{
		cout<<list[i]<<" ";
	} */
	
	system("rm out*  >terminal.txt");
	delete [] mat_distances;
	delete [] filename;
	delete [] nk_CH;
	
	resultats[0]=distance_total;
	/* cout<<"\nOld CH = "<<distance_total<<endl; */
	
	//vider le vector Ci
	Ci.clear();
	
	return distance_total;
	
    
}//  end ************************End of Distances DistanceCH_old

double NouvelleCallRF (double nb_esp_identique){
	
	//Ancienne version
	/* system("./rf myTrees rf_output.txt rf_tmp.txt  rf_output.tmp rf_matrices.txt"); */
	
	//nouvelle version
	system("./rf myTrees rf_output.txt rf_tmp.txt rf_matrices.txt >terminal.txt");
	
	//Recuperer le string du consensus des arbres du cluster i par la variable Ci
	ifstream fileCi;
	fileCi.open("rf_output.txt", ios::in);
	string RF_new = "";
	
	if(fileCi){
		// si l'ouverture a fonctionné
		string ligne;
		int sixieme = 0;
		// tant que l'on peut mettre la ligne dans "contenu"
		while(getline(fileCi, ligne)){
			sixieme ++;
			if(sixieme==6){
				/* cout<<"LIGNE "<<ligne<<endl; */
				size_t pos = ligne.find(" = ");
				RF_new = ligne.substr(pos+3); 
				/* cout<<"=======>>>> RF "<<RF_new<<endl; */
			}
		}

		fileCi.close();
	}else{
		cerr << "Impossible d'ouvrir le fichier !" << endl;
	}
	
	system("rm rf_* >terminal.txt");
	//normalization de la distance RF par 2n - 6
	double RF_norm = atof(RF_new.c_str());
	RF_norm = RF_norm/(2*nb_esp_identique-6);
	return RF_norm;
}


int fact (int valeur) {
  int resultatFact = 1;
  for(int i = 1; i <= valeur; i++) {
    resultatFact *= i;
  }
  return resultatFact;
}

void listePartition(int Strouve []) {
	
    
	std::map<int,string>::iterator it = mapIndicesTreesFinal.begin();
	int indiceGroupe=1;
	
	for ( it = mapIndicesTreesFinal.begin(); it != mapIndicesTreesFinal.end(); it++)
	{
		string ListeIndicesGroupe = it->second;
		int nbTabl = Split(indicesTrees, ListeIndicesGroupe, ',');
		
		for(int i(1); i<indicesTrees.size(); ++i){
			//conversion un string en int
			istringstream iss(indicesTrees[i]);
			// convertir en un int
			int nombre;
			iss >> nombre; // nombre vaut indicesTrees[i]
			
			Strouve[nombre-1]=indiceGroupe;
		}
		indiceGroupe++;
	}

    
}//  end ************************End listePartition

//////////////////////
//***********Euclidean
//////////////////////

// Compute a SQUARED (weighted) Euclidean distance to the centroid
void Euclidean(int &i,int &k,int &nmax,int &kmax,int &p,int &pmax,double** mat,double** xbar,double* weight,int* ishort,double &D1)
{
// Compute a SQUARED (weighted) Euclidean distance to the centroid
	int j=0;
      D1=0.0;			//D1=0.0
	  for (j=1;j<=p;j++)		// do 10 j=1,p
	  {
		  D1=D1+weight[ishort[j]]*((mat[i-1][ishort[j]-1]-xbar[k][j])*(mat[i-1][ishort[j]-1]-xbar[k][j]));		//   10 D1=D1+weight(ishort(j))*((mat(i,ishort(j))-xbar(k,j))**2)
	 }
      return;
}	//end
//************************End of Euclidean

void dist(int &i,int &k,int &nmax,int &kmax,int &p,int &pmax,double** mat,double** xbar,double* weight,int* ishort,double &D1,int &n,int* list,int* howmany,int &kk){
	
	 D1=0.0;	
	int cluster_k = list[i];
	for (int j=i+1;j<=n;j++){
		if (list[j]==cluster_k) {
			D1+=mat[i-1][j-1];
		}
	}
    return;
}	//end
//************************End of dist

//////////////////////
//***********Euclidean
//////////////////////

// Compute a Eucledian distance between point i and j 
double Euclidean_point(int &i, int&j, int &p,int &pmax,double** mat,double* weight,int* ishort)
{

    if (i==j) return 0.0;
    
          double sumSq=0.0;			//D1=0.0
	  for (int a=1;a<=p;a++)		// do 10 j=1,p
	  {
		  sumSq+=pow((weight[ishort[a]]*mat[i-1][ishort[a]-1]-weight[ishort[a]]*mat[j-1][ishort[a]-1]),2);		                  
	  }
      return sqrt(sumSq);
}	//end
//************************End of Euclidean


//////////////////////
//***********Permute
//////////////////////

void Permute(int &iseed,int &n,int &nmax,int *iordre)
{
// This subroutine permutes the first 'n' values of vector 'iordre' at random
// in an equiprobable way. This property has been checked through intensive
// simulations.
      int i=0,j=0,itemp=0,km1=0,m=0;			//Integer iseed,n,i,j,itemp,km1,m
      //Integer iordre(nmax)
      m=n;		//m=n
      km1=n-1;	//km1=n-1

	  for (i=1;i<=km1;i++)			//      do 10 i=1,km1
	  {
m8:		// j = 1 + rand(iseed)*m;		//    8    j = 1 + rand(iseed)*m
		  j=1+(rand()/32767.0)*m;

         if(j>m) goto m8;			//if(j.gt.m) goto 8
         itemp = iordre[m];			//itemp = iordre(m)
         iordre[m] = iordre[j];		//iordre(m) = iordre(j)
         iordre[j] = itemp;			//iordre(j) = itemp
         m = m-1;					//m = m-1
	  }			//   10 continue
	  
	  
      return;
}			//end
//************************End of Permute


//////////////////////
//***********ReadData1
//////////////////////

void ReadData1(int &n,int &nmax,int &p,int &pmax,double** mat/* ,double* coord */,int* ishort,double* weight,double* colsum,int &ntran,char* namea, char* nameb, char* namew, int N, int pLeavesAbsent)
{
     int p1=0,p2=0;
     double rowsum=0,tsum=0;
     
     int i=0, j=0, nlines=0;
       int   nmat=2; // (orientation of data))
	 int iflag=0;//iflag=0
      
	  //Read matrix parameters 
	  n = N;
	  p = N;	
      //printf("\nData:\nn:%d p:%d\n", n,p);
      
      if(n>nmax)    //if(n.gt.nmax) Stop 
	  {	
		  printf ("Too many objects. Use a sample of objects or recompile program to increase nmax.");				//     +'Too many objects. Use a sample of objects or recompile program.'
		  exit(1);
	  }
		       
     if(p>pmax)    // if(p.gt.pmax) Stop 'Too many variables. Recompile the program.'
        {
                printf ("Too many variables. Use a sample of objects or recompile program to increase pmax.");				//     +'Too many objects. Use a sample of objects or recompile program.'
                exit(1);
        }

	
   switch (nmat)   //goto (10,14,18) nmat
   {
   case 1:
	   {
		  
		   break;//goto 22;
	   }
   case 2:
	   {
		   
		   break;//goto 22;
	   }

//        To read the file of QTC variables:
   case 3:
	   {
		   printf ("How many position (p1) and QTC variables (p2)?\n");			
		   printf ("E.g., p1 = 0 or 1 or 2; p2 = 166.  Answer with 2 numbers:");      
		   scanf("%d %d",&p1, &p2);		
 
		   if(p2>pmax)   
		   {
			 printf ("Too many variables. Recompile the program to increase pmax.");			
			 exit(1);
		   }

		   p=p2;
		   printf ("\n"); 
 
		   break;
	   }
   } 
   
   
   //fclose(Input1); 
		  
	  for (j=1;j<=p;j++)		
	  {
		  ishort[j]=j;		
		  weight[j]=1.0;			
	  }
	 //--Load weight matrix
             if (use_weight) {
			 FILE *Input4; //was Input2
			 if ((Input4 = fopen(namew,"r"))==0) 
			 { printf("Error: Weight vector open failed... (%s)\n ",namew); exit(1); }   			 
				printf ("Input weight file: %s\n",namew);		
			 for (j=1;j<=p;j++)
			 {
				 fscanf (Input4,"%lf",&weight[j]);		
			 }
			 fclose(Input4);//close(4)         
		 }//endif
   
/* 		char * statOuput  = new char[50];
		strcpy(nameb, namea);
		
		sprintf (statOuput, "stat%d.csv",pLeavesAbsent);
		strcpy(nameb, statOuput);
	   
	  	delete [] statOuput; */
		
		strcpy(nameb, namea);
		strcpy(nameb, "stat.csv");
		
	  if((Output4 = fopen(nameb,"a"))==NULL)
	  {
		  printf("\n%s: result file open failed...",nameb);
		  exit(1);
	  } 
      
}//end
//************************End of ReadData1


//////////////////////
//***********Standard
//////////////////////    
void Standard(int &n,int &nmax,int &kmax,int &p,int &pmax,double** mat,double** sx,double** sx2,double* vect,double** var,double* weight,int* ishort,int &istand)
{
	  double temp=0, dfln=0, dflnm1=0;
	  int nlines=0, j=0,i=0;
	  switch (istand)         //goto (10,30,50) istand
	  {
	  case 1:
		  {
                  // (1) Standardize: y(i,j)'=(y(i,j)-Mean(y))/StDev(y)
			  dfln=static_cast<float>(n);		//10 dfln=dfloat(n)
			  dflnm1=static_cast<float>(n-1);	//dflnm1=dfloat(n-1)
			  for (j=1;j<=p;j++)			//do j=1,p
			  {
				  sx[1][j]=0.0;			//sx(1,j)=0.0
				  sx2[1][j]=0.0;			//sx2(1,j)=0.0
			  }			//enddo

			  for (i=1;i<=n;i++)		// do i=1,n
			  {
				  for (j=1;j<=p;j++)		//do j=1,p
				  {
					  temp=mat[i-1][ishort[j]-1];		//temp=mat(i,ishort(j))
					  sx[1][j]=sx[1][j]+temp;			//sx(1,j)=sx(1,j)+temp
					  sx2[1][j]=sx2[1][j]+temp*temp;	//sx2(1,j)=sx2(1,j)+temp*temp
				  }		//enddo
			  }			//enddo
			  
			  for (j=1;j<=p;j++)			//do j=1,p
			  {
				  vect[j]=sx[1][j]/dfln;		//vect(j)=sx(1,j)/dfln
				  var[1][j]=sx2[1][j]-(sx[1][j]*sx[1][j]/dfln);		//var(1,j)=sx2(1,j)-(sx(1,j)*sx(1,j)/dfln)
				  var[1][j]=sqrt(var[1][j]/(dflnm1));				//var(1,j)=dsqrt(var(1,j)/dflnm1)
			  }			//enddo
                        //     Standardize the data      
			  for (i=1;i<=n;i++)			//do i=1,n
			  {
				  for (j=1;j<=p;j++)		//do j=1,p
				  {
					  mat[i-1][ishort[j]-1]=(mat[i-1][ishort[j]-1]-vect[j])/var[1][j];		//mat(i,ishort(j))=(mat(i,ishort(j))-vect(j))/var(1,j)
				  }		//            enddo
			  }		//         enddo

		  break;			//goto 24
		  }
	  case 2:
		  {
// 
// (2) Range: y' =  y(i,j)/yMax
// 'sx(1,j)' is used to store the maximum values of the variables.
   
			  for (j=1;j<=p;j++)			//30 do j=1,p
			  {
				  sx[1][j]=mat[1-1][j-1];			//sx(1,j)=mat(1,j)
			  }		//enddo
			  
			  for (i=2;i<=n;i++)			//do i=2,n
			  {
				  for (j=1;j<=p;j++)		//do j=1,p
				  {
					  temp=mat[i-1][ishort[j]-1];			//temp=mat(i,ishort(j))
					  if(temp>sx[1][j]) sx[1][j]=temp;		//if(temp.gt.sx(1,j)) sx(1,j)=temp
				  }			//enddo
			  }				//enddo

			  for (i=1;i<=n;i++)			//do i=1,n
			  {
				  for (j=1;j<=p;j++)		//do j=1,p
				  {
					  mat[i-1][ishort[j]-1]=mat[i-1][ishort[j]-1]/sx[1][j];		//mat(i,ishort(j))=mat(i,ishort(j))/sx(1,j)
				  }		//enddo
			  }			//enddo
		  break;			//goto 24
		  }
	  case 3:
		  {
// 
// (3) Range: y' = (y(i,j)-yMin)/(yMax-yMin)
// Matrix 'sx' is used to store the minimum and maximum values of the variables.
			  for (j=1;j<=p;j++)			//50 do j=1,p
			  {
				  sx[1][j]=mat[1-1][j-1];		//sx(1,j)=mat(1,j)
				  sx[2][j]=mat[1-1][j-1];		//sx(2,j)=mat(1,j)
			  }			//enddo

			  for (i=2;i<=n;i++)			//do i=2,n
			  {
				  for (j=1;j<=p;j++)		//do j=1,p
				  {
					  temp=mat[i-1][ishort[j]-1];		//temp=mat(i,ishort(j))
					  if(temp<sx[1][j]) sx[1][j]=temp;			//if(temp.lt.sx(1,j)) sx(1,j)=temp
					  if(temp>sx[2][j]) sx[2][j]=temp;			//if(temp.gt.sx(2,j)) sx(2,j)=temp
				  }			//enddo
			  }			//enddo
			  
			  for (i=1;i<=n;i++)			//do i=1,n
			  {
				  for (j=1;j<=p;j++)		//do j=1,p
				  {
					  mat[i-1][ishort[j]-1]=(mat[i-1][ishort[j]-1]-sx[1][j])/(sx[2][j]-sx[1][j]);		//mat(i,ishort(j))=(mat(i,ishort(j))-sx(1,j))/(sx(2,j)-sx(1,j))
            	  }		//enddo
			  }			//enddo
		  break;			//goto 24
		  }
	  }
// 
// Print a few lines of data
   
m24:	  printf ("Print how many lines of transformed data? (None: type 0)");		//24 write(*,*) 'Print how many lines of transformed data? (None: type 0)'
		  scanf	("%d", &nlines);			//read(*,*) nlines
		  
		  if((nlines<0)||(nlines>n)) goto m24;			//		  if((nlines.lt.0).or.(nlines.gt.n)) goto 24
		  if(nlines>0)									//if(nlines.gt.0) then
		  {
			  for (i=1;i<=nlines;i++)		// do 26 i=1,nlines
			  {
				  for (j=1;j<=p;j++)			//26    write(*,101) (mat(i,ishort(j)), j=1,p)
				  {
					  printf("%lf ", mat[i-1][ishort[j]-1]);
				  }
			  }
		  }			//endif
      
		  printf ("\n"); // write(*,*)
      return;

}//   end
//************************End of Standard


//////////////////////
//***********Transform
//////////////////////
void Transform(int &n,int &nmax,int &p,int &pmax,double** mat,double* weight,double* colsum,int &ntran)
		//      Subroutine Transform(n,nmax,p,pmax,mat,weight,colsum,ntran)
{      
    //Integer n,nmax,p,pmax
      double rowsum=0,tsum=0;		//Real*8 mat(nmax,pmax),weight(pmax),colsum(pmax),rowsum,tsum
	  int iflag=0,i=0,j=0;
// Transformation of species data before K-means partitioning?
      printf ("\n"); // write(*,*)
m2:   printf ("Transform species abundance data, in order to base\n"); // write(*,*) 'Transform species abundance data, in order to base'
      printf ("the analysis on a different distance measure?\n"); // write(*,*) 'the analysis on a different distance measure?'
      printf ("(see Legendre & Gallagher, manuscript)\n"); // write(*,*) '(see Legendre & Gallagher, manuscript)'
      printf ("\n"); // write(*,*)
      printf ("(0) No transformation (Euclidean distance preserved)\n"); // write(*,*) '(0) No transformation (Euclidean distance preserved)'
      printf ("(1) Chord distance\n"); // write(*,*) '(1) Chord distance'
      printf ("(2) Chi-square metric\n"); // write(*,*) '(2) Chi-square metric'
      printf ("(3) Chi-square distance\n"); // write(*,*) '(3) Chi-square distance'
      printf ("(4) Distance between species profiles\n"); // write(*,*) '(4) Distance between species profiles'
      printf ("(5) Hellinger distance\n"); // write(*,*) '(5) Hellinger distance'

	  scanf	("%d",&ntran);			//read(5,*) ntran


      if((ntran<0)||(ntran>5)) goto m2;		//      if((ntran.lt.0).or.(ntran.gt.5)) goto 2

      printf ("\n"); // write(*,*)
// Computing the transformed variables.
// The code is written in such a way as to avoid computing in advance
// a vector 'rowsum(i)' of length 'n'

      if(ntran==0) return;			//if(ntran.eq.0) return


      if((ntran==2)||(ntran==3)) 		//      if((ntran.eq.2).or.(ntran.eq.3)) then
	  {
		  tsum=0.0;			//tsum=0.0
		  for (j=1;j<=p;j++)		//do 14 j=1,p
		  {
			  colsum[j]=0.0;		//colsum(j)=0.0
			  for (i=1;i<=n;i++)		//do 14 i=1,n
			  {
				  tsum=tsum+mat[i-1][j-1];				//tsum=tsum+mat(i,j)
				  colsum[j]=colsum[j]+mat[i-1][j-1];		//   14    colsum(j)=colsum(j)+mat(i,j)
			  }
		  }

		  for (j=1;j<=p;j++)					//do 16 j=1,p
		  {
			  if(colsum[j]==0.0) weight[j]=0.0;		//if(colsum(j).eq.0.0) weight(j)=0.0
		  }			//   16    continue
	  }				//endif


	  switch (ntran)         //      goto (20,30,40,50,60) ntran
	  {
	  case 1:
		  {
// Chord distance
			  for (i=1;i<=n;i++)		//20 do 26 i=1,n
			  {
				  rowsum=0.0;			//rowsum=0.0
				  for (j=1;j<=p;j++)	//do 22 j=1,p
				  {
					  rowsum=rowsum+mat[i-1][j-1]*mat[i-1][j-1];			//22 rowsum=rowsum+mat(i,j)**2
				  }
				  
				  if(rowsum==0.0)							//if(rowsum.eq.0.0) then
				  {
					  printf ("Line	%d has total species abundance = 0", i);			//write(*,100) i
					  iflag=1;		//iflag=1
					  break;			//goto 26
				  }		//    endif
				  rowsum=sqrt(rowsum);		//rowsum=dsqrt(rowsum)
				
				  for (j=1;j<=p;j++)			//do 24 j=1,p
				  {
					  mat[i-1][j-1]=mat[i-1][j-1]/rowsum;			//24 mat(i,j)=mat(i,j)/rowsum
				  }
			  }			//26 continue
			  break;		//      goto 68
		  }

	  case 2:
		  {
// Chi-square metric
			  for (i=1;i<=n;i++)				//30 do 36 i=1,n
			  {
				  rowsum=0.0;					//rowsum=0.0
				  for (j=1;j<=p;j++)			//do 32 j=1,p
				  {
					  rowsum=rowsum+mat[i-1][j-1];	//32 rowsum=rowsum+mat(i,j)
				  }
				  
				  if(rowsum==0.0)				//if(rowsum.eq.0.0) then
				  {
					  printf ("Line	%d has total species abundance = 0", i);			//write(*,100) i
					  iflag=1;		//iflag=1
					  break;			//goto 36
				  }		//         endif

				  for (j=1;j<=p;j++)			//do 34 j=1,p
				  {
					  if(colsum[j]==0.0) mat[i-1][j-1]=mat[i-1][j-1]/(rowsum*sqrt(colsum[j]));	//if(colsum(j).ne.0.0) mat(i,j)=mat(i,j)/(rowsum*dsqrt(colsum(j)))
				  }			//   34 continue
			  }				//   36 continue
			  break;		//      goto 68
		  }
	  case 3:
		  {
// Chi-square distance
			  for (i=1;i<=n;i++)				//   40 do 46 i=1,n
			  {
				  rowsum=0.0;					//     rowsum=0.0
				  for (j=1;j<=p;j++)			//     do 42 j=1,p
				  {
					  rowsum=rowsum+mat[i-1][j-1];	//   42 rowsum=rowsum+mat(i,j)
				  }
				  
				  if(rowsum==0.0) 			//      if(rowsum.eq.0.0) then
				  {
					  printf ("Line	%d has total species abundance = 0", i);			// write(*,100) i
					  iflag=1;		//iflag=1
					  break;			//goto 46
				  }			//endif

				  for (j=1;j<=p;j++)			//do 44 j=1,p
				  {
					  if(colsum[j]==0.0) mat[i-1][j-1]=sqrt(tsum)*mat[i-1][j-1]/(rowsum*sqrt(colsum[j]));		// if(colsum(j).ne.0.0) mat(i,j)=dsqrt(tsum)*mat(i,j)/(rowsum*dsqrt(colsum(j)))
				  }		//   44 continue
			  }			//   46 continue
			  break;		//      goto 68
		  }
	  case 4:
		  {
// Distance between species profiles
			  for (i=1;i<=n;i++)			//50 do 56 i=1,n
			  {
				  rowsum=0.0;					//   rowsum=0.0
				  for (j=1;j<=p;j++)			//do 52 j=1,p
				  {
					  rowsum=rowsum+mat[i-1][j-1];	// 52 rowsum=rowsum+mat(i,j)
				  }

				  if(rowsum==0.0)				// if(rowsum.eq.0.0) then
				  {
					  printf ("Line	%d has total species abundance = 0", i);			// write(*,100) i
					  iflag=1;		//iflag=1
					  break;			//goto 56
				  }			//         endif

				  for (j=1;j<=p;j++)      //do 54 j=1,p
				  {
					  mat[i-1][j-1]=mat[i-1][j-1]/rowsum;			//54 mat(i,j)=mat(i,j)/rowsum
				  }
			  }		//   56 continue
			  break;		//   goto 68
		  }
	  case 5:
		  {
// Hellinger distance
			  for (i=1;i<=n;i++)			// 60 do 66 i=1,n
			  {
				  rowsum=0.0;					//    rowsum=0.0
				  for (j=1;j<=p;j++)			//do 62 j=1,p
				  {
					  rowsum=rowsum+mat[i-1][j-1];	//  62 rowsum=rowsum+mat(i,j)
				  }

				  if(rowsum==0.0)				//  if(rowsum.eq.0.0) then
				  {
					  printf ("Line	%d has total species abundance = 0", i);			//   write(*,100) i
					  iflag=1;		//iflag=1
					  break;			//   goto 66
				  }			//           endif
  
				  for (j=1;j<=p;j++)			// do 64 j=1,p
				  {
					  mat[i-1][j-1]=sqrt(mat[i-1][j-1]/rowsum);		//64 mat(i,j)=dsqrt(mat(i,j)/rowsum)
				  }
			  }							//66 continue
			  break;		//      68 continue
		  }
		  
}//???
		  

		  if(iflag==1) //if(iflag.eq.1) stop 'This is incompatible with the selected transformation.'
		  {
			  printf ("This is incompatible with the selected transformation.");
			  exit (1);
		  }

      return;			//return
      
}//************************End of Transform


int Split(vector<string>& vecteur, string chaine, char separateur)
{
	vecteur.clear();

	string::size_type stTemp = chaine.find(separateur);
	
	while(stTemp != string::npos)
	{
		vecteur.push_back(chaine.substr(0, stTemp));
		chaine = chaine.substr(stTemp + 1);
		stTemp = chaine.find(separateur);
	}
	
	vecteur.push_back(chaine);

	return vecteur.size();
}


///* 
// * Code from Found in http://cran.r-project.org/web/packages/cluster/
// * Original code by Francois Romain 
// */

