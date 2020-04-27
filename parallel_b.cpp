#include <iostream>
#include <bits/stdc++.h>
#include <vector>
#include <string>
#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/stat.h"
#include <cmath>
#include <algorithm>
#include <map>

using namespace std;
typedef std::pair <char*, char*> keyvalue_object;

struct S_and_alpha{
    double danglingImportance;
    double totalImportance;
    int totalNodes;
};

struct multi_keyvalue_object{
    int nvalues;
    int *valuebytes;
    char* multivalue;
};

class KeyValue{
public:
    KeyValue(MPI_Comm caller){
        this->caller = caller;
        totalValues = 0;
        MPI_Comm_rank(caller,&myrank);
        MPI_Comm_size(caller,&nproc);
        totalValues = 0;
        currProc    = 0;
    }
    void add(char* key,int keybytes,char* value,int valuebytes){
        totalValues++;
        if(myrank == currProc){
            keyvalues.push_back(make_pair(key, value));
        }
        currProc = (currProc==nproc-1)?(0):(currProc+1);
    }
    int get_size(){
        return keyvalues.size();
    }
private:
    std::vector<keyvalue_object> keyvalues;
    MPI_Comm caller;
    int myrank;
    int nproc;
    int totalValues;
    int currProc;
};

class MultiKeyvalue{
private:
    map<char*,multi_keyvalue_object> key_multi_values;
};

void getLinks(int, char*, KeyValue*, void*);
void getNodes(int, char*, KeyValue*, void*);
void initializeImportance(char *, int, char *, int, int *, KeyValue *, void *);
void calculateImportances(char *, int, char *, int, int *, KeyValue *, void *);
void emitImportances(char *, int, char *, int, int *, KeyValue *, void *);
void calculateDifference(char *, int, char *, int, int *, KeyValue *, void *);
void initializeGraph(char *, int, char *, int, int *, KeyValue *, void *);
void outputValues(uint64_t, char*, int, char*, int, KeyValue*, void*);
void printValues(uint64_t, char*, int, char*, int, KeyValue*, void*);
void normalize(uint64_t, char*, int, char*, int, KeyValue*, void*);
void copyImportances(uint64_t, char*, int, char*, int, KeyValue*, void*);
void initializeDanglingImportance(uint64_t, char*, int, char*, int, KeyValue*, void*);
void addDanglingNode(uint64_t, char*, int, char*, int, KeyValue*, void*);

class MapReduce{
public:
    MapReduce(MPI_Comm caller){
        this->caller = caller;
        KeyValue kt(caller);
        *(this->kv) = kt;
        MPI_Comm_rank(caller,&myrank);
        MPI_Comm_size(caller,&nproc);
       };
    void map(int nstr, char *strings, int self, int recurse, int readfile, void (*func)(int, char *, KeyValue *, void *), void *ptr){
        func(nstr, strings, kv, ptr);
    };
    int collate(void*ptr){
        int individualTotalPairs =  kv->get_size();
        int *key_lengths = new int[individualTotalPairs];
        int *values_length = new int[individualTotalPairs];
        int *keys    = new int[individualTotalPairs];
        int *values  = new int[individualTotalPairs];
        for(int i(0);i<individualTotalPairs;i++){
            cout<<"Fuck off!!!"<<endl;
        }
        return 0;
    };
    // void map(MapReduce *mr2, void (*mymap)(uint64_t, char *, int, char *, int, KeyValue *, void *), void *ptr){
    //
    // }
private:
    KeyValue *kv;
    MultiKeyvalue mkv;
    MPI_Comm caller;
    int myrank;
    int nproc;
};




char* double_to_char(double var){
    std::string f_str = std::to_string(var);
    char char_array[f_str.length() + 1];
    strcpy(char_array,f_str.c_str());
    return char_array;
}
std::vector<std::string> split(std::string strToSplit, char delimeter){
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(ss, item, delimeter))
    {
       splittedStrings.push_back(item);
    }
    return splittedStrings;
}

int main(int narg, char** args){
    std::string file = args[1];
    std::vector<std::string > v = split(file,'.');
    std::string filename = v[0];
    MPI_Init(&narg,&args);
    double convergence = 0.0001;
    int me,nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD,&me);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

    if(narg<=1){
        std::cout<<"File arguement not given"<<std::endl;
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    MapReduce *nodes            = new MapReduce(MPI_COMM_WORLD);
    MapReduce *links            = new MapReduce(MPI_COMM_WORLD);
    MapReduce *danglingNode     = new MapReduce(MPI_COMM_WORLD);
    MapReduce *prevImportances  = new MapReduce(MPI_COMM_WORLD);
    MapReduce *newImportances   = new MapReduce(MPI_COMM_WORLD);
    MapReduce *difference       = new MapReduce(MPI_COMM_WORLD);
    std::cout << "Error is not in mapreduce construction" << '\n';
    MPI_Barrier(MPI_COMM_WORLD);
    // std::cout << "Error is in MapReduce construction" << '\n';
    nodes -> map(narg-1,args[1],0,1,0,getNodes,NULL);
    links -> map(narg-1,args[1],0,1,0,getLinks,NULL);
    int outgoingNodes = links -> collate(NULL);
    // int totalNodes = nodes -> collate(NULL);
    // double *finalIndividualImportances = new double[totalNodes];
    // double *finalCollectiveImportances = new double[totalNodes];
    // for(int i(0);i<totalNodes;i++)finalIndividualImportances[i] = 0.0;
    // int danglingNodes = totalNodes - outgoingNodes;
    // double danglingImportance = 0.85*((double)danglingNodes/(((double)totalNodes)*((double)totalNodes))) + (0.15/((double)totalNodes));
    // nodes -> reduce(initializeImportance,(void*) &totalNodes);
    // newImportances ->map(nodes, copyImportances, NULL);
    // links -> reduce(initializeGraph,(void*) &totalNodes);
    // danglingNode -> map(nodes, initializeDanglingImportance, &danglingImportance);
    // links -> add(danglingNode);
    // links -> collate(NULL);
    // S_and_alpha curr;
    // curr.totalImportance    = 0;
    // curr.danglingImportance = 0;
    // curr.totalNodes = totalNodes;
    // double collectiveTotalImportance    = 1.0;
    // double individualTotalImportance    = 1.0;
    // double individualDanglingImportance = 1.0;
    // double collectiveDanglingImportance = 1.0;
    // double individualDifference         = 0.0;
    // double collectiveDifference         = 1.0;
    // while(true){;
    //     links           -> reduce(calculateImportances,(void*) &curr);
    //     individualTotalImportance    = curr.totalImportance;
    //     individualDanglingImportance = curr.danglingImportance;
    //     MPI_Reduce(&individualTotalImportance,&collectiveTotalImportance,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
    //     MPI_Reduce(&individualDanglingImportance,&collectiveDanglingImportance,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
    //     MPI_Bcast(&collectiveTotalImportance,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
    //     MPI_Bcast(&collectiveDanglingImportance,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
    //     curr.danglingImportance     = collectiveDanglingImportance;
    //     curr.totalImportance        = collectiveTotalImportance;
    //     links           -> map(links,normalize,(void*) &curr);
    //
    //                 //checking if the system converged or not
    //     prevImportances -> map(newImportances, copyImportances, NULL);
    //     newImportances  -> map(links, copyImportances, NULL);
    //     difference      -> map(newImportances, copyImportances, NULL);
    //     difference      -> add(prevImportances);
    //     difference      -> collate(NULL);
    //     difference      -> reduce(calculateDifference, (void*) &individualDifference);
    //     MPI_Reduce(&individualDifference,&collectiveDifference,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
    //     MPI_Bcast(&collectiveDifference, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    //     if(me == 0) std::cout<<collectiveDifference<<std::endl;
    //     if(collectiveDifference<convergence){
    //         newImportances -> map(newImportances, printValues, (void*)finalIndividualImportances);
    //         MPI_Reduce(finalIndividualImportances,finalCollectiveImportances,totalNodes,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
    //         if(me == 0){
    //             std::string textfile = "";
    //             textfile.append(filename);
    //             textfile.append("-pr-mpi-base.txt");
    //             std::ofstream outputFile;
    //             outputFile.open(textfile);
    //             double sum = 0.0;
    //             for(int i(0);i<totalNodes;i++){
    //                 outputFile << i <<" = "<<finalCollectiveImportances[i]<<std::endl;
    //                 sum+=finalCollectiveImportances[i];
    //             }
    //             outputFile<<"sum = "<<sum<<std::endl;
    //         }
    //         break;
    //     }
    //     individualDifference = 0.0;
    //     collectiveDifference = 0.0;
    //                 //done checking
    //
    //     links           -> collate(NULL);
    //     links           -> reduce(emitImportances,NULL);
    //     danglingNode    -> map(nodes, addDanglingNode, (void*) &curr);
    //     links           -> add(danglingNode);
    //     links           -> collate(NULL);
    //     curr.danglingImportance     = 0.0;
    //     curr.totalImportance        = 0.0;
    //     collectiveTotalImportance   = 0.0;
    //     collectiveDanglingImportance= 0.0;
    // }
    MPI_Finalize();
}


void getNodes(int itask, char* fname, KeyValue *kv, void *ptr){
    struct stat stbuf;
    int flag = stat(fname,&stbuf);
    if (flag < 0) {
        printf("ERROR: Could not query file size\n");
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    int filesize = stbuf.st_size;

    FILE *fp = fopen(fname,"r");
    char *text = new char[filesize+1];
    int nchar = fread(text,1,filesize,fp);
    text[nchar] = '\0';
    fclose(fp);

    char *whitespace = " \t\n\f\r\0";
    char *word = strtok(text,whitespace);
    while (word) {
        std::string str = word;
        int k = std::stoi(str) + 2;
        std::string f_str = std::to_string(k);
        char char_array[f_str.length() + 1];
        strcpy(char_array,f_str.c_str());
        kv->add(char_array,strlen(char_array)+1,NULL,0);
        word = strtok(NULL,whitespace);
    }
  delete [] text;
}
void getLinks(int itask, char* fname, KeyValue *kv, void *ptr){
    struct stat stbuf;
    int flag = stat(fname,&stbuf);
    if (flag < 0) {
        printf("ERROR: Could not query file size\n");
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    int filesize = stbuf.st_size;
    FILE *fp = fopen(fname,"r");
    char *text = new char[filesize+1];
    int nchar = fread(text,1,filesize,fp);
    text[nchar] = '\0';
    fclose(fp);
    bool isReceivingNode = false;
    char *whitespace = " \t\n\f\r\0";
    char *word = strtok(text,whitespace);
    std::string inNode;
    while (word) {
        std::string str = word;
        int k = std::stoi(str) + 2;
        std::string f_str = std::to_string(k);
        char char_array[f_str.length() + 1];
        strcpy(char_array,f_str.c_str());
        if(isReceivingNode){
            char chararray[inNode.length() + 1];
            strcpy(chararray,inNode.c_str());
            kv->add(chararray,strlen(chararray)+1,char_array,strlen(char_array)+1);
            isReceivingNode = false;
        }
        else{
            inNode = char_array;
            isReceivingNode = true;
        }
        word = strtok(NULL,whitespace);
    }
  delete [] text;
}
void initializeImportance(char *key, int keybytes, char *multivalue, int nvalues, int *valuebytes, KeyValue *kv, void *ptr){
    int *totalNodes = (int*) ptr;
    double initialImportance = 1.0/((double) *totalNodes);
    std::string f_str = std::to_string(initialImportance);
    char char_array[f_str.length() + 1];
    strcpy(char_array,f_str.c_str());
    kv->add(key, keybytes, char_array, strlen(char_array)+1);
}
void initializeGraph(char *key, int keybytes, char *multivalue, int nvalues, int *valuebytes, KeyValue *kv, void *ptr){
    int *totalNodes = (int*) ptr;
    double initialImportance = 1.0/((double) *totalNodes);
    double passedImportance = 0.85*(initialImportance/((double)nvalues)); // This wont work if anything extra is added to the keyvalue pair in the beginning
    std::string f_str = std::to_string(passedImportance);
    std::string add = "0000000";
    f_str = add.append(f_str);
    if(f_str.length()>15){
        int extraZeros = f_str.length() - 15;
        f_str = f_str.substr(extraZeros, f_str.length()-1);
    }
    char char_array[f_str.length() + 1];
    strcpy(char_array,f_str.c_str());
    std::string something = key;
    int keyy = std::stoi(something);
    double incomingNode = (double) keyy;//0.1 to change all doubles to have double values
    std::string str = std::to_string(incomingNode);
    std::string zeros = "0000000";
    str = zeros.append(str);
    if(str.length()>15){
        int extraZeros = str.length() - 15;
        str = str.substr(extraZeros, str.length()-1);
    }
    char chararray[str.length() + 1];
    strcpy(chararray,str.c_str());
    for(int i(0);i<nvalues;i++){
        char currValue[nvalues];
        for(int j(0);j< *valuebytes;j++){
            currValue[j] = multivalue[j+i*(*valuebytes)];
        }
        kv->add(currValue,strlen(currValue)+1,char_array,16);
        kv->add(currValue,strlen(currValue)+1,chararray,16);
    }
    double has_outLinks = 1.5;
    std::string f = std::to_string(has_outLinks);
    std::string additionalZeros = "0000000";
    f = additionalZeros.append(f);
    char charArray[f.length() + 1];
    strcpy(charArray,f.c_str());
    kv->add(key,keybytes,charArray,strlen(charArray)+1);
}
void emitImportances(char *key, int keybytes, char *multivalue, int nvalues, int *valuebytes, KeyValue *kv, void *ptr){
    // std::cout<<"I reached here!!!!"<<std::endl;
    std::vector<int> v;
    bool twice = false;
    double importance;
    std::string something = key;
    int keyy = std::stoi(something);
    double incomingNode = (double) keyy;
    std::string str = std::to_string(incomingNode);
    std::string zz = "0000000";
    str = zz.append(str);
    if(str.length()>15){
        int extraZeros = str.length() - 15;
        str = str.substr(extraZeros, str.length()-1);
    }
    char chararray[str.length() + 1];
    strcpy(chararray,str.c_str());
    // std::cout << "The value of bytes are" <<*valuebytes <<'\n';
    for(int i(0);i<nvalues;i++){
        char currValue[nvalues];
        for(int j(0);j< *valuebytes;j++){
            currValue[j] = multivalue[j+i*(*valuebytes)];
        }
        std::string val = currValue;
        // std::cout << "The value of key is" << key<< " and the value with it is "<<currValue<<'\n';
        double link = std::stod(val);
        if(link > 1.8){
            int node = (int)(link+0.3);
            v.push_back(node);
        }
        else{
            if(twice){
                std::cout<<"There is some error. Got two different importance values for same key"<<std::endl;
            }
            importance = link;
            twice = true;
        }
    }
    importance =(v.size()!=0)?((0.85*importance)/v.size()):(importance);
    std::string imp = std::to_string(importance);
    std::string zeros = "0000000";
    imp = zeros.append(imp);
    if(imp.length()>15){
        int extraZeros = imp.length() - 15;
        imp = imp.substr(extraZeros, imp.length()-1);
    }
    char passedImportance[imp.length()+1];
    strcpy(passedImportance, imp.c_str());
    for(int i(0);i<v.size();i++){
        std::string ss= std::to_string(v[i]);
        char char_array[ss.length()+1];
        strcpy(char_array,ss.c_str());
        kv->add(char_array,strlen(char_array)+1,chararray,16);
        kv->add(char_array,strlen(char_array)+1,passedImportance,16);
    }
    if(v.size()!=0){
        double has_outLinks = 1.5;
        std::string f = std::to_string(has_outLinks);
        std::string z = "0000000";
        f = z.append(f);
        char charArray[f.length() + 1];
        strcpy(charArray,f.c_str());
        kv->add(key,keybytes,charArray,16);
    }
}
void calculateDifference(char *key, int keybytes, char *multivalue, int nvalues, int *valuebytes, KeyValue *kv, void *ptr){
    double* totalDiff = (double*) ptr;
    if(nvalues!=2) std::cerr<<"Your logic sucks!!! The number of values are "<<nvalues<<std::endl;
    double d1 = 0;
    double d2 = 0;
    for(int i(0);i<nvalues;i++){
        char currValue[nvalues];
        for(int j(0);j< *valuebytes;j++){
            currValue[j] = multivalue[j+i*(*valuebytes)];
        }
        if(i == 0){
            std::string val = currValue;
            d1 = std::stod(val);
        }
        else{
            std::string val = currValue;
            d2 = std::stod(val);
        }
    }
    double diff = abs(d1 - d2);
    *totalDiff += diff;
}
void calculateImportances(char *key, int keybytes, char *multivalue, int nvalues, int *valuebytes, KeyValue *kv, void *ptr){
    S_and_alpha* s_and_alpha = (S_and_alpha*) ptr;
    std::string current_key = key;
    double inNode = (double)std::stoi(current_key);
    std::string ff = std::to_string(inNode);
    std::string zeros = "0000000";
    ff = zeros.append(ff);
    if(ff.length()>15){
        int extraZeros = ff.length() - 15;
        ff = ff.substr(extraZeros, ff.length()-1);
    }
    char char_array[ff.length() + 1];
    strcpy(char_array,ff.c_str());
    double importance = 0;
    bool isDangling = true;
    std::vector<double> v;
    for(int i(0);i<nvalues;i++){
        char currValue[nvalues];
        for(int j(0);j< *valuebytes;j++){
            currValue[j] = multivalue[j+i*(*valuebytes)];
        }
        // std::cout<<"problem is here"<<std::endl;
        std::string curr = currValue;
        // std::cout << currValue<<std::endl;
        double current = std::stod(curr);
        // std::cout << current << '\n';
        // std::cout<<"problem was there"<<std::endl;
        if(current>1.8){
            int keyy = (int)(current+0.3);
            std::string f = std::to_string(keyy);
            char charArray[f.length() + 1];
            strcpy(charArray,f.c_str());
            kv->add(charArray, strlen(charArray)+1, char_array, 16);
        }
        else if(current > 1.3){
            isDangling = false;
        }
        else{
            std::cout<<"The value of key is "<<key<<" and the value with it is "<<currValue<<std::endl;
            importance += current;
        }
    }
    std::string calculatedImportance = std::to_string(importance);
    std::string z = "0000000";
    calculatedImportance = z.append(calculatedImportance);
    if(calculatedImportance.length()>15){
        int extraZeros = calculatedImportance.length() - 15;
        calculatedImportance = calculatedImportance.substr(extraZeros, calculatedImportance.length()-1);
    }
    char imp[calculatedImportance.length() + 1];
    strcpy(imp,calculatedImportance.c_str());
    kv->add(key, keybytes, imp, 16);
    // std::cout<<"The value of key is "<<key<<" and the value with it is "<<imp<<std::endl;
    if(isDangling){
        s_and_alpha->danglingImportance = s_and_alpha->danglingImportance+importance;
    }
    s_and_alpha->totalImportance = s_and_alpha->totalImportance+importance;
}
void outputValues(uint64_t itask, char* key, int keybytes, char* value, int valuebytes, KeyValue* kv, void* ptr){
    std::cout<<key <<", "<<value;
    std::cout<<std::endl;
    kv->add(key,keybytes,value,valuebytes);
}
void printValues(uint64_t itask, char* key, int keybytes, char* value, int valuebytes, KeyValue* kv, void* ptr){
    double* importances  = (double*) ptr;
    std::string keyy = key;
    // std::cout<<"I've reached in print values"<<std::endl;
    double node = std::stod(keyy) - 2.0;
    int index = (int)(node+0.3);
    std::string val = value;
    double imp = std::stod(val);
    importances[index] = imp;
}
void normalize(uint64_t itask, char* key, int keybytes, char* value, int valuebytes, KeyValue* kv, void* ptr){
    S_and_alpha* s_and_alpha = (S_and_alpha*) ptr;
    double normalizingFactor = 1/(s_and_alpha->totalImportance);
    std::string val = value;
    double currValue = std::stod(val);
    if(currValue<1.8){
        double normalizedValue = currValue*normalizingFactor;
        std::string new_val = std::to_string(normalizedValue);
        std::string z = "0000000";
        new_val = z.append(new_val);
        if(new_val.length()>15){
            int extraZeros = new_val.length() - 15;
            new_val = new_val.substr(extraZeros, new_val.length()-1);
        }
        char char_array[new_val.length()+1];
        strcpy(char_array,new_val.c_str());
        kv->add(key,keybytes,char_array,16);
    }
    else{
        kv->add(key,keybytes,value,valuebytes);
    }
}
void copyImportances(uint64_t itask, char* key, int keybytes, char* value, int valuebytes, KeyValue* kv, void* ptr){
    std::string val = value;
    double currValue = std::stod(val);
    if(currValue<1.3){
        kv->add(key,keybytes,value,valuebytes);
    }
}
void addDanglingNode(uint64_t itask, char* key, int keybytes, char* value, int valuebytes, KeyValue* kv, void* ptr){
    S_and_alpha* s_and_alpha = (S_and_alpha*) ptr;
    double importance = 0.85*((s_and_alpha->danglingImportance)/((s_and_alpha->totalImportance)*((double)s_and_alpha->totalNodes))) + (0.15/((double)s_and_alpha->totalNodes));
    std::string f_str = std::to_string(importance);
    std::string z = "0000000";
    f_str = z.append(f_str);
    if(f_str.length()>15){
        int extraZeros = f_str.length() - 15;
        f_str = f_str.substr(extraZeros, f_str.length()-1);
    }
    char char_array[f_str.length() + 1];
    strcpy(char_array,f_str.c_str());
    kv -> add(key, keybytes, char_array, 16);
}
void initializeDanglingImportance(uint64_t itask, char* key, int keybytes, char* value, int valuebytes, KeyValue* kv, void* ptr){
    double *importance = (double*) ptr;
    std::string f_str = std::to_string(*importance);
    std::string extraZeros = "0000000";
    f_str = extraZeros.append(f_str);
    if(f_str.length()>15){
        int zeros = f_str.length() - 15;
        f_str = f_str.substr(zeros, f_str.length()-1);
    }
    char char_array[f_str.length() + 1];
    strcpy(char_array,f_str.c_str());
    kv -> add(key, keybytes, char_array, strlen(char_array)+1);
}
