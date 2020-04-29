#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
// #include "mapreduce.h"
#include <string>
#include <cmath>
#include <algorithm>
using namespace std;

std::vector<std::string> split(std::string strToSplit, char delimeter)
{
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(ss, item, delimeter))
    {
       splittedStrings.push_back(item);
    }
    return splittedStrings;
}

void calculateG(float** a,int n,float alpha){
	float c = (1.0-alpha)/((float)n);
	float *sum = new float[n];
	for(int i(0);i<n;i++){
		sum[i] = 0;
	}
	for(int j(0);j<n;j++){
		for (int i(0);i<n;i++){
			sum[j] += a[i][j];
		}
	}
	for(int j(0);j<n;j++){
		if(sum[j]==0){
            for(int i(0);i<n;i++){
                a[i][j] = alpha/((float)n) + c;
            }
        }
        else{
            for(int i(0);i<n;i++){
                a[i][j] = (alpha*a[i][j])/sum[j] + c;
            }
        }
	}
}


float difference(float* I_new, float* I_old, int n){
	float diff =0.0;
	for(int i(0);i<n;i++){
		diff += abs(I_new[i]-I_old[i]);
	}
	return diff;
}

void assignArray(float* I_new, float* I_old, int n){
	for(int i(0);i<n;i++){
		I_old[i] = I_new[i];
	}
}

void multiplyMatrix(float**G,float*I,int n){
	for(int i(0);i<n;i++){
		float ans = 0.0;
		for(int j(0);j<n;j++){
			ans += G[i][j]*I[j];
		}
		I[i] = ans;
	}
}

int computeSize(string file){
    cout<<"I am here";
    string line;
    int maxx(0);
    ifstream myfile(file);
	if(myfile.is_open()){
		while(getline(myfile,line)){
			std::vector<string> splittedString = split(line,' ');
			int k = max(stoi(splittedString[1]) , stoi(splittedString[0]));
            if(k>maxx)
                k = maxx;
		}
	}
    return maxx+1;
}

void computeI(float**G, float*I_new, float* I_old, float convergence,int n){
	float diff = 1.0;

	while (diff >convergence){
		assignArray(I_new,I_old,n);
		multiplyMatrix(G,I_new,n);
		diff = difference(I_new,I_old,n);
        cout<<diff<<endl;
	}
    float sum = 0.0;
    for (int i(0);i<n;i++){
        sum += I_new[i];
    }
    for (int i(0);i<n;i++){
        I_new[i]=I_new[i]/sum;
    }
}

int main(int argc, char *argv[]){
    cout<<"problem is taking command line arguement"<<endl;
	string line;
	float convergence = 0.0001;
	float alpha 	= 0.85;
	string file 	= argv[1];
    cout<<file<<endl;
    int maxx(0);
    cout<<"I took command line arguement"<<endl;
    ifstream infile(file);
    cout<<"c++ sucks"<<endl;
	if(infile.is_open()){
		while(getline(infile,line)){
			std::vector<string> splittedString = split(line,' ');
			int k = max(stoi(splittedString[1]) , stoi(splittedString[0]));
            if(k>maxx)
                maxx = k;
		}
        infile.close();
	}
    int size 		= maxx+1;
    cout<<size;
	float *I_new 	= new float[size];
	float *I_old	= new float[size];
	float **G		= new float*[size];
    // cout<<"problem is in assigning variables"<<endl;
	for(int i(0);i<size;i++){
		I_new[i]	= 1.0;
		I_old[i]	= 1.0;
		G[i]		= new float[size];
		for(int j(0);j<size;j++){
			G[i][j] 	= 0;
		}
	}
    // cout<<"problem is in reading file"<<endl;
	ifstream myfile(file);
	if(myfile.is_open()){
		while(getline(myfile,line)){
			std::vector<string> splittedString = split(line,' ');
			G[stoi(splittedString[1])][stoi(splittedString[0])] += 1.0;
		}
		myfile.close();
	}
    // cout<<"problem is in calculateG"<<endl;
	calculateG(G,size,alpha);
    // cout<<"problem is in computeI"<<endl;
	computeI(G,I_new,I_old,convergence,size);
	ofstream outfile ("aaa-cpp.txt");
	if (outfile.is_open()){
		for(int i(0);i<size;i++){
			outfile << i << " = " << I_new[i]<<endl;
		}
		outfile.close();
  	}
 	else
		cout << "Unable to open file";
}
