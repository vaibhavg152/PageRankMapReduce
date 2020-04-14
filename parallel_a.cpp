#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <sstream>
#include "mapreduce.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/config.hpp>
#include <cmath>
#include <algorithm>

std::vector<std::vector<int> > graph;
std::vector<bool> isDangling;
std::vector<float> importances;
float TotalImportance;
float DanglingImportance;
float Alpha = 0.85;
float Convergence = 0.0001;
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
        diff += abs(v[i]-importances[i]);
    }
    return diff;
}

void normalize(){
    float sum = 0.0;
    for(int i(0);i<importances.size();i++){
        sum += importances[i];
    }
    float dang = 0;
    for(int i(0);i<importances.size();i++){
        importances.at(i) = importances[i]/sum;
        if(isDangling[i]) dang+=importances.at(i);
    }
    DanglingImportance = dang;
}

void print(std::vector<float> v){
    for(int i(0);i<v.size();i++){
        std::cout << i <<" = " <<v[i]<<'\n';
    }
    std::cout << '\n';
}

namespace pagerank {

template<typename MapTask>
class number_source : mapreduce::detail::noncopyable
{
  public:
    number_source(int first, int last, int step)
      : sequence_(0), first_(first), last_(last-1), step_(step)
    {
    }

    bool const setup_key(typename MapTask::key_type &key)
    {
        key = sequence_++;
        return (key * step_ <= last_);
    }

    bool const get_data(typename MapTask::key_type const &key, typename MapTask::value_type &value)
    {
        typename MapTask::value_type val;

        val.first  = first_ + (key * step_);
        val.second = std::min(val.first + step_ - 1, last_);

        std::swap(val, value);
        return true;
    }

  private:
    int sequence_;
    int const step_;
    int const last_;
    int const first_;
};

struct map_task : public mapreduce::map_task<int, std::pair<int, int> >
{
    template<typename Runtime>
    void operator()(Runtime &runtime, key_type const &/*key*/, value_type const &value) const
    {
        for (key_type loop=value.first; loop<=value.second; ++loop){
            float importance = importances[loop];
            int size = graph.at(loop).size();
            float passedImportance = (size!=0)?(Alpha*(importance/(float)size)):(importance);
            if(size != 0){
                for(int i(0);i<size;i++){
                    runtime.emit_intermediate(graph.at(loop).at(i), passedImportance);
                    // tImportance += passedImportance;
                }
            }
            float dImportance = Alpha*(DanglingImportance/((float)graph.size()))  +  (1-Alpha)/((float)graph.size());
            runtime.emit_intermediate(loop,dImportance);
        }
    }
};

struct reduce_task : public mapreduce::reduce_task<int , double>
{
    template<typename Runtime, typename It>
    void operator()(Runtime &runtime, key_type const &key, It it, It ite) const{
        float sum(0.0);
        for(It it1 = it; it1!=ite; ++it1){
            sum += *it1;
        }
        importances.at(key) = sum;
    }
};

typedef
mapreduce::job<pagerank::map_task,
               pagerank::reduce_task,
               mapreduce::null_combiner,
               pagerank::number_source<pagerank::map_task>
> job;

}

void printGraph(std::vector<std::vector<int> > v){
    for(int i(0);i<v.size();i++){
        std::cout<<i<<" -> ";
        for(int j(0);j<v[i].size();j++)
            std::cout<<v[i][j]<<", ";
        std::cout<<std::endl;
    }
}

int main(int argc, char *argv[]){
    mapreduce::specification spec;
    TotalImportance = 1.0;
    std::string line;
	std::string file = argv[1];
    int numProc      = atoi(argv[2]);
    std::ifstream infile(file);
    int maxx(0);
	if(infile.is_open()){
		while(getline(infile,line)){
			std::vector<std::string> splittedString = split(line,' ');
			int k = std::max(std::stoi(splittedString[1]) , std::stoi(splittedString[0]));
            if(k>maxx)
                maxx = k;
		}
        infile.close();
	}
    int size = maxx + 1;
    std::vector<int> v;
    float initialImportance = 1/(float)size;
    for(int i(0);i<size;i++){
        graph.push_back(v);
        isDangling.push_back(true);
        importances.push_back(initialImportance);
    }
    std::ifstream myfile(file);
	if(myfile.is_open()){
		while(getline(myfile,line)){
			std::vector<std::string> splittedString = split(line,' ');
			(graph.at(std::stoi(splittedString[0]))).push_back(std::stoi(splittedString[1]));
            isDangling.at(std::stoi(splittedString[0])) = false;
		}
		myfile.close();
	}
    int dangling = 0;
    for(int i(0);i<size;i++){
        if(isDangling[i]) dangling++;
    }
    std::vector<float> prev_vals(size,0.0);
    float diff = 1.0;
    DanglingImportance = ((float)dangling)/((float)size);
    spec.reduce_tasks = numProc;
    pagerank::job::datasource_type datasource(0, size, size/numProc);
    pagerank::job job(datasource, spec);
    mapreduce::results result;
    while(diff>Convergence){
        for(int i(0);i<importances.size();i++){
            prev_vals.at(i)=importances[i];
        }
        job.run<mapreduce::schedule_policy::cpu_parallel<pagerank::job> >(result);
        normalize();
        diff = calculateDifference(prev_vals);
    }
    for(int i(0);i<importances.size();i++){
        std::cout<<i<<" = "<<importances[i]<<std::endl;
    }
	return 0;
}
