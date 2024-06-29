#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <omp.h>
using namespace std;

class Node{
    public:
    vector<int> state;
    int heurisitc;

    Node(){
        state={};
        heurisitc=0;
    }

    Node(vector<int> s){
        state=s;
        heurisitc=calculate_heuristic(state);
    }

    bool operator>(const Node &oth) const{
        return heurisitc<oth.heurisitc;
    }

    int calculate_heuristic(vector<int> &state){
        int n=state.size();
        int h=(n*(n-1))/2;
        for(int i=0; i<n; i++){
            for(int j=i+1; j<n; j++){
                if(state[i]==state[j]){
                    h--;
                }
                else if(state[i]+i==state[j]+j){
                    h--;
                }
                else if(state[i]-i==state[j]-j){
                    h--;
                }
            }
        }
        return h;
    }
};

vector<Node> initialize(){
    int size, n;
    cout<<"Input n: ";
    cin>>n;
    cout<<"Input Population Size: ";
    cin>>size;
    vector<Node> population(size);
    for(int i=0; i<size; i++){
        vector<int> s(n,0);
        for(int j=0; j<n; j++){
            s[j]=rand()%n;
        }
        population[i]=Node(s);
    }
    return population;
}

pair<vector<int>, vector<int>> reproduce(Node& parent1, Node& parent2){
    int n=parent1.state.size();
    int c=1+rand()%(n-1);
    // cout<<c;
    vector<int> s1(n), s2(n);
    for(int i=0; i<c; i++){
        s1[i]=parent1.state[i];
        s2[i]=parent2.state[i];
    }
    for(int i=c; i<n; i++){
        s1[i]=parent2.state[i];
        s2[i]=parent1.state[i];
    }
    return {s1,s2};
}

bool check_if_fit(Node& individual){
    int n=individual.state.size();
    int max_heuristic=(n*(n-1))/2;
    return individual.heurisitc==max_heuristic;
}

void mutation(vector<int> &state){
    int n=state.size();
    int position=rand()%n;
    state[position]=rand()%n;
}

vector<int> calculate_weights(vector<Node> &popu){
    vector<int> weights(popu.size());
    int max_h=0;
    for(int i=0; i<popu.size(); i++){
        weights[i]=popu[i].heurisitc;
        if(weights[i]>max_h) max_h=weights[i];
    }
    cout<<max_h<<'\n';
    return weights;
}

int pick_index(vector<int> &weights, int &sum){
    int n= weights.size();
    int rnd=rand()%sum;
    for(int i=0; i<n; i++){
        if(rnd<weights[i]) return i;
        rnd-=weights[i];
    }
    cout<<"Fail\n";
}

vector<int> get_parents(vector<int> &weights){
    int sum=0;
    for(int i=0; i<weights.size(); i++) sum+=weights[i];
    vector<int> parents(weights.size());
    // for(int i=0; i<weights.size(); i++){
    //     parents[i]=pick_index(weights, sum);
    // }
    for(int i=0; i<weights.size(); i+=2){
        parents[i]=pick_index(weights, sum);
        int ind=pick_index(weights, sum);
        while(ind==parents[i]) ind=pick_index(weights, sum);
        parents[i+1]=ind;
    }
    return parents;
}

Node genetic_algorithm(){
    double max_time;
    cout<<"Input Max Time Limit in seconds: ";
    cin>>max_time;
    int mutation_probability;
    vector<Node> population=initialize();
    // for(int i=0; i<population.size(); i++){
    //     for(int j=0; j<population[i].state.size(); j++){
    //         cout<<population[i].state[j];
    //     }
    //     cout<<' ';
    // }
    cout<<"Input Mutation Probability: ";
    cin>>mutation_probability;
    clock_t start=clock();
    clock_t present=clock();
    double time=((double)(present-start))/CLOCKS_PER_SEC;
    while(time<max_time){
        cout<<time<<" ";
        int size=population.size();
        vector<int> weights=calculate_weights(population);
        vector<int> parent_indices=get_parents(weights);
        // for(int i=0; i<size; i++){
        //     cout<<parent_indices[i]<<' ';
        // }
        vector<Node> popu(size);
        for(int i=0; i<size; i+=2){
        #pragma omp parallel for
            int index1=parent_indices[i], index2=parent_indices[i+1];
            Node parent1=population[index1], parent2=population[index2];
            pair<vector<int>, vector<int>> child_states=reproduce(parent1, parent2);
            double prob=((double)1)/ (rand()+1);
            if(prob<mutation_probability) {
                mutation(child_states.first);
                mutation(child_states.second);
            }
            Node child1=Node(child_states.first);
            Node child2=Node(child_states.second);
            int n=child_states.first.size();
            if(check_if_fit(child1)) {
                present=clock();
                time=((double)(present-start))/CLOCKS_PER_SEC;
                cout<<"Total Time: "<<time<<'\n';
                cout<<"Final Solution: \n";
                return child1;
            }
            else if(check_if_fit(child2)) {
                present=clock();
                time=((double)(present-start))/CLOCKS_PER_SEC;
                cout<<"Total Time: "<<time<<'\n';
                cout<<"Final Solution: \n";
                return child2;
            }
            popu[i]=child1;
            popu[i+1]=child2;
        }
        population=popu;
        present=clock();
        time=((double)(present-start))/CLOCKS_PER_SEC;
        // cout<<"Next Generation: ";
        // for(int i=0; i<popu.size(); i++){
        //     for(int j=0; j<popu[i].state.size(); j++){
        //         cout<<popu[i].state[j];
        //     }
        //     cout<<' ';
        // }
        cout<<'\n';
    }
    cout<<"Best result produced yet: \n";
    int h=0;
    int ind=0;
    for(int i=0; i<population.size(); i++){
        if(population[i].heurisitc>h){
            ind=i;
            h=population[i].heurisitc;
        }
    }
    return population[ind];
}

int main(){
    Node solution=genetic_algorithm();
    for(int i=0; i<solution.state.size(); i++) cout<<solution.state[i]+1;
    return 0;
}