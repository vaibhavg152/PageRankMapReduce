#include<iostream>
#include <bits/stdc++.h>
#include "mpi.h"

using namespace std;
typedef std::vector< std::pair <int, float> > keyvalue_t;
typedef std::map< int, std::vector<float> > keymultivalue_t;

// keyvalue_t kv;
// keymultivalue_t keymultivalues;
std::vector<bool> node_exists;
std::vector<std::vector<int> > graph;
std::vector<bool> isDangling;
std::vector<float> importances;
int totalExistingNodes;
float TotalImportance;
float DanglingImportance;
float Alpha = 0.85;
float Convergence = 0.001;

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

void copyArray(std::vector<float> v){
    for(int i(0);i<v.size();i++)
        v.at(i)=importances.at(i);
}

float calculateDifference(std::vector<float> v){
    float diff(0.0);
    for(int i(0);i<v.size();i++){
        if(node_exists[i])
        diff += abs(v[i]-importances[i]);
    }
    return diff;
}

void normalize(){
    float sum = 0.0;
    for(int i(0);i<importances.size();i++){
        if(node_exists[i])
            sum += importances[i];
    }
    float dang = 0;
    for(int i(0);i<importances.size();i++){
        if(node_exists[i]){
            importances.at(i) = importances[i]/sum;
            if(isDangling[i]) dang+=importances.at(i);
        }
    }
    DanglingImportance = dang;
}

void print(std::vector<float> v){
    std::cout << "printing importances" << '\n';
    for(int i(0);i<v.size();i++){
        std::cout << i <<" = " <<v[i]<<'\n';
    }
    std::cout << '\n';
}

void function_for_map(keyvalue_t &key_values, int index){
    float importance = importances[index];
    int size = graph.at(index).size();
    float passedImportance = (size!=0)?(Alpha*(importance/(float)size)):(importance);
    if(size != 0){
        for(int i(0);i<size;i++){
            key_values.push_back( make_pair(graph.at(index).at(i), passedImportance));
        }
    }
    if(node_exists[index]){
        float dImportance = Alpha*(DanglingImportance/(float)totalExistingNodes)  +  (1-Alpha)/((float)totalExistingNodes);
        key_values.push_back( make_pair(index,dImportance));
    }
}

void function_for_reduce(int &key, std::vector<float> imp){
    float sum(0.0);
    for(int it1 = 0; it1<imp.size(); it1++)
        sum += imp[it1];
    importances.at(key) = sum;
}

class MyMapReduce{
public:
    MyMapReduce(MPI_Comm comm, int size){
        communicator = comm;
        map_size = size;
        MPI_Comm_rank(communicator, &my_rank);
        MPI_Comm_size(communicator, &num_procs);
        num_procs  = std::min(num_procs,map_size);
        size_per_process = 1 + size/num_procs;
    }

    void MAP(void (*func)(keyvalue_t&,int)){
        for(int j=my_rank*size_per_process; j<std::min(size_per_process*(my_rank+1),map_size); j++){
            func(kv,j);
        }
    }

    void COLLATE(){
        // print_keyvalue();
        for(int i=0; i<kv.size(); i++){
            std::pair <int, float> cur_pair = kv[i];
            int key = kv[i].first;
            std::map<int,std::vector<float> >::const_iterator it = keymultivalues.find(key);
            if(it==keymultivalues.end()){
                std::vector<float> v = {cur_pair.second};
                // v.push_back(cur_pair.second);
                keymultivalues.insert({key,v});
            }
            else{
                std::vector<float> v = it->second;
                v.push_back(cur_pair.second);
                keymultivalues[key] = v;
            }
        }
        // print_keymultivalue();
    }

    void REDUCE(void (*func)(int&,std::vector<float>)){
        for(int j=my_rank*size_per_process; j<std::min((my_rank+1)*size_per_process, map_size); j++){
            auto it = keymultivalues.find(j);
            if(it==keymultivalues.end()){
                if(node_exists[j])
                    std::cout << "something is wrong" << '\n';
                else continue;
            }
            func(j,it->second);
        }
    }

    void print_keymultivalue(){
        std::cout << "printing key-multi-values" << '\n';
        for(auto it=keymultivalues.begin(); it!=keymultivalues.end(); ++it){
            std::vector<float> v=it->second;
            std::cout << "\nmultivalue "<<it->first<<":\n";
            for(int i=0; i<v.size(); i++){
                std::cout <<v[i]<< '\t';
            }
            std::cout<<'\n';
        }
    }

    void print_keyvalue(){
        std::cout << "printing key-values" << '\n';
        for(int i=0; i<kv.size(); i++){
            std::cout << "key:"<<kv[i].first<<" value:"<<kv[i].second << '\n';
        }
    }

private:
    MPI_Comm communicator;
    keyvalue_t kv;
    keymultivalue_t keymultivalues;
    int map_size, num_procs, my_rank, size_per_process;
};

// void MAP(void (*func)(keyvalue_t&,int), int size, int num){
//     int size_per_process = 1 + size/num;
//     for(int i=0;i<num; i++){
//         for(int j=i*size_per_process; j<std::min(size_per_process*(i+1),size); j++){
//             func(kv,j);
//         }
//     }
// }
//
// void COLLATE(){
//     // print_keyvalue();
//     for(int i=0; i<kv.size(); i++){
//         std::pair <int, float> cur_pair = kv[i];
//         int key = kv[i].first;
//         std::map<int,std::vector<float> >::const_iterator it = keymultivalues.find(key);
//         if(it==keymultivalues.end()){
//             std::vector<float> v = {cur_pair.second};
//             // v.push_back(cur_pair.second);
//             keymultivalues.insert({key,v});
//         }
//         else{
//             std::vector<float> v = it->second;
//             v.push_back(cur_pair.second);
//             keymultivalues[key] = v;
//         }
//     }
//     // print_keymultivalue();
// }
//
// void REDUCE(void (*func)(int&,std::vector<float>), int size, int num){
//     int size_per_process = size/num;
//     for(int i=0;i<num; i++){
//         for(int j=i*size_per_process; j<std::min((i+1)*size_per_process, size); j++){
//             auto it = keymultivalues.find(j);
//             func(j,it->second);
//         }
//     }
// }

int main(int narg, char** argv){
    TotalImportance = 1.0;
    std::string line;
    std::string file = argv[1];
    // int numProc      = atoi(argv[2]);

    auto start = std::chrono::high_resolution_clock::now();
    std::ifstream infile(file);
    int maxx(0);
    if(infile.is_open()){
        while(getline(infile,line)){
            std::vector<std::string> splittedString = split(line,' ');
            int k = std::max(std::stoi(splittedString[1]) , std::stoi(splittedString[0]));
            maxx = std::max(k,maxx);
        }
        infile.close();
    }
    int size = maxx + 1;
    std::vector<int> v;
    for(int i(0);i<size;i++){
        node_exists.push_back(false);
        graph.push_back(v);
        isDangling.push_back(true);
        importances.push_back(0);
    }

    std::ifstream myfile(file);
    if(myfile.is_open()){
        while(getline(myfile,line)){
            std::vector<std::string> splittedString = split(line,' ');
            int k1 = std::stoi(splittedString[0]);
            int k2 = std::stoi(splittedString[1]);
            node_exists[k1] = true;
            node_exists[k2] = true;
            (graph.at(k1)).push_back(k2);
            isDangling.at(k1) = false;
        }
        myfile.close();
    }
    int dangling(0);
    totalExistingNodes = 0;
    for(int i(0);i<size;i++){
        if(isDangling[i]) dangling++;
        if(node_exists[i]) totalExistingNodes++;
    }

    float initialImportance = 1/(float)totalExistingNodes;
    for(int i(0);i<size;i++)
        if(node_exists[i]) importances[i] = initialImportance;

    std::vector<float> prev_vals(size,0.0);
    float diff = 1.0;
    DanglingImportance = ((float)dangling)/((float)size);

    MPI_Init(&narg, &argv);
    int numProc = 0;
    MPI_Comm_size(MPI_COMM_WORLD,&numProc);

    MyMapReduce mp_object(MPI_COMM_WORLD, size);

    int iter=0;
    while(diff>Convergence){
        cout<<"iteration:"<<iter<<" diff:"<<diff<<endl;
        for(int i(0);i<importances.size();i++)
            prev_vals.at(i)=importances[i];

        // do map collate reduce
        // std::cout << "mapping" << '\n';
        mp_object.MAP(function_for_map);
        // std::cout << "collating" << '\n';
        mp_object.COLLATE();
        // std::cout << "reducing" << '\n';
        mp_object.REDUCE(function_for_reduce);

        // std::cout << "normalising" << '\n';
        normalize();
        // print(importances);
        diff = calculateDifference(prev_vals);
        iter++;
        // break;
    }
    MPI_Finalize();

    std::string outfile_name = file.substr(0,file.length()-4);
    outfile_name.append("-pr-mpi.txt");
    std::ofstream outputFile(outfile_name);
    double sum = 0.0;
    for(int i(0);i<importances.size();i++){
        if(node_exists[i]){
            outputFile << i <<" = "<<importances[i]<<std::endl;
            sum+=importances[i];
        }
    }
    outputFile<<"sum = "<<sum<<std::endl;

    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double,std::milli> duration = (stop - start);
    std::cout << duration.count()/1000.0<<" seconds taken for "<<iter<<" iterations." << '\n';

	return 0;

}
