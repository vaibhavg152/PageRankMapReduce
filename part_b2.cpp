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
float Convergence = 0.0005;

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

void function_for_reduce(keymultivalue_t kmv){
    for(auto it=kmv.begin(); it!=kmv.end(); ++it){
        int key = it->first;
        std::vector<float> imp = it->second;
        float sum(0.0);
        for(int i = 0; i<imp.size(); i++)
            sum += imp[i];
        importances[key] = sum;
    }
}

class MyMapReduce{
public:
    MyMapReduce(MPI_Comm comm, int size){
        root = 0;
        communicator = comm;
        map_size = size;
        MPI_Comm_rank(communicator, &my_rank);
        MPI_Comm_size(communicator, &num_procs);
        num_procs  = std::min(num_procs,map_size);
        size_per_process = ceil(map_size/(double)num_procs);
    }

    void MAP(void (*func)(keyvalue_t&,int)){
        for(int j=my_rank*size_per_process; j<std::min(size_per_process*(my_rank+1),map_size); j++){
            func(kv,j);
        }
    }

    void COLLATE(){
        // print_keyvalue();
        if(my_rank!=root){
            // send all keyvalues to root
            int kv_size = kv.size();
            MPI_Send(&kv_size, 1, MPI_INT, root, 0, communicator);
            int keys[kv_size];
            float values[kv_size];
            for(int i=0; i<kv.size(); i++){
                keys[i] = kv[i].first;
                values[i] = kv[i].second;
            }
            MPI_Send(&keys, kv_size, MPI_INT, root, 0, communicator);
            MPI_Send(&values, kv_size, MPI_FLOAT, root, 0, communicator);

            //receive keymultivalues from root
            int kmv_size;
            MPI_Recv(&kmv_size,1,MPI_INT,root,0,communicator,MPI_STATUS_IGNORE);
            for(int i=0; i<kmv_size; i++){
                int mv_size,key;
                MPI_Recv(&mv_size,1,MPI_INT,root,0,communicator,MPI_STATUS_IGNORE);
                MPI_Recv(&key,1,MPI_INT,root,0,communicator,MPI_STATUS_IGNORE);
                float mv[mv_size];
                MPI_Recv(mv,mv_size,MPI_FLOAT,root,0,communicator,MPI_STATUS_IGNORE);
                keymultivalues.insert({key,std::vector<float>(mv, mv + mv_size)});
            }
        }
        else{
            // receiving key values from all other processors
            keymultivalue_t all_keymultivalues;
            for(int sender=1; sender<num_procs; sender++){
                int kv_size;
                MPI_Recv(&kv_size, 1, MPI_INT, sender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int keys[kv_size];
                float values[kv_size];
                MPI_Recv(keys, kv_size, MPI_INT, sender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(values, kv_size, MPI_FLOAT, sender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                // collating the key values received by the processor
                for(int i=0; i<kv_size; i++){
                    auto it = all_keymultivalues.find(keys[i]);
                    if(it==all_keymultivalues.end()){
                        std::vector<float> v = {values[i]};
                        all_keymultivalues.insert({keys[i],v});
                    }
                    else{
                        std::vector<float> v = it->second;
                        v.push_back(values[i]);
                        all_keymultivalues[keys[i]] = v;
                    }
                }
            }
            // collating the key values of the root processors
            for(int i=0; i<kv.size(); i++){
                std::pair <int, float> cur_pair = kv[i];
                int key = kv[i].first;
                auto it = all_keymultivalues.find(key);
                if(it==all_keymultivalues.end()){
                    std::vector<float> v = {cur_pair.second};
                    all_keymultivalues.insert({key,v});
                }
                else{
                    std::vector<float> v = it->second;
                    v.push_back(cur_pair.second);
                    all_keymultivalues[key] = v;
                }
            }
            // sending keymultivalues

            int kmv_size = ceil(map_size/(double)num_procs);
            for(int receiver=0; receiver<num_procs; receiver++){
                int start = receiver*kmv_size;
                int end = min(start + kmv_size, map_size);
                kmv_size = end-start;
                if(kmv_size<=0) continue;
                if(receiver!=root) MPI_Send(&kmv_size,1,MPI_INT,receiver,0,communicator);
                for(int key=start; key<end; key++){
                    auto it = all_keymultivalues.find(key);
                    if(it!=all_keymultivalues.end()){
                        if(receiver==root){
                            keymultivalues.insert({it->first,it->second});
                        }
                        else{
                            std::vector<float> multivalues = it->second;
                            int mv_size = multivalues.size();
                            MPI_Send(&mv_size,1,MPI_INT,receiver,0,communicator);
                            MPI_Send(&key,1,MPI_INT,receiver,0,communicator);
                            float* mv = &multivalues[0];
                            MPI_Send(mv,mv_size,MPI_FLOAT,receiver,0,communicator);
                        }
                    }
                }
            }
        }
        // print_keymultivalue();
    }

    void REDUCE(void (*func)(keymultivalue_t)){
        func(keymultivalues);
        for(int i=0; i<importances.size();i++){
            MPI_Bcast(&importances[i],1,MPI_FLOAT,i/size_per_process,communicator);
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
    int map_size, num_procs, my_rank, size_per_process, root;
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

    int iter=0, root=0;

    MPI_Init(&narg, &argv);
    int numProc = 0, my_rank;
    MPI_Comm_size(MPI_COMM_WORLD,&numProc);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);


    while(diff>Convergence){
        if(my_rank==root) cout<<"\niteration:"<<iter<<" diff:"<<diff<<endl;
        MyMapReduce mp_object(MPI_COMM_WORLD, size);
        for(int i(0);i<importances.size();i++)
            prev_vals.at(i)=importances[i];
        MPI_Barrier(MPI_COMM_WORLD);

        mp_object.MAP(function_for_map);
        MPI_Barrier(MPI_COMM_WORLD);

        mp_object.COLLATE();
        MPI_Barrier(MPI_COMM_WORLD);

        mp_object.REDUCE(function_for_reduce);
        MPI_Barrier(MPI_COMM_WORLD);

        normalize();
        if(my_rank==root){
            diff = calculateDifference(prev_vals);
            // print(importances);
        }
        MPI_Bcast(&diff,1,MPI_FLOAT,root,MPI_COMM_WORLD);
        MPI_Bcast(&DanglingImportance,1,MPI_FLOAT,root,MPI_COMM_WORLD);
        iter++;
    }

    if(my_rank==root){
        std::string outfile_name = file.substr(0,file.length()-4);
        outfile_name.append("-pr-mpi.txt");
        std::ofstream outputFile(outfile_name);
        double sum = 0.0;
        std::cout << "writing" << '\n';
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
    }
    MPI_Finalize();

	return 0;

}
