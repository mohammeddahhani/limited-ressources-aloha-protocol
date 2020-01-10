#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <cmath>
#define DBL_PREC 6   

using namespace std;
using namespace chrono;

typedef unordered_map<long long int, unsigned int> StateSuccDict;
typedef unordered_map<long long int, double> StateProbaDict;

unsigned int N 	;				
unsigned int NS;

vector<double> prob; 		// auxiliary array:
		 				 	// prob[NS] is the probability of beeing in a given intial state.	

int *n_k;		 			// number of stations in a slot for a given state

int *sum_n_k;	 		 	// auxiliary array: 
						 	// count the number of stations in a state 
							// if a valid state the last element of sum_n_k has N)

vector<double> prob_ones; 	// prob_ones[i] probability of having i success (i.e. ones) in a state

unsigned int *ones;		 	// auxiliary array:
						 	// counts number of ones in a state	

StateSuccDict nb_succ;		// number of success corresponding to a state
StateProbaDict prob_state; 	// probability of beeing in a state
StateProbaDict::iterator it, it1;

int *interm_state;	 		// a resulting state after distribution of colliding stations

vector<vector<double>> C; 	// pascal triangle
vector<long long unsigned> powof_2;
vector<vector<double>> pow_cache;
vector<vector<vector<vector<double>>>> powers_cache;

long long unsigned new_s_id, s_id, min_st, max_st;

double sum_pb, correction, prob_new;
double check, tmp_f, tmpf2 ;
double imprecision;

long long int t_state;
unsigned int t_index ;
long long int ii;

long long int id_;
long long int shift_s;
unsigned int sta_cnt_;

long long int tmp_i1,tmp_i2;
char tmp3[1];
int rm;



//--------------------------- Helper Functions-------------------------------//

long 
strtoi(const char *str){
	return strtol(str, NULL, 10);
}

void log_results(char *f_name, unsigned  NS_, unsigned  N_, double avg_){
	FILE *f = fopen(f_name, "a+");
	if (f == NULL)
	{
	    printf("Error opening file! %s\n",f_name);
	    exit(1);
	}
	double rate = avg_/(double) N_;
	fprintf(f, "%u\t%u\t%.20e\t%.20e\n", NS_, N_, rate, avg_);
	fclose(f);
}

int 
handle_args(int argc, char *argv[],  unsigned int *N, unsigned int *NS){
	if (argc >= 3){
		*N 	= strtoi(argv[1]);
		*NS = strtoi(argv[2]);
	} else{
		printf("\n[+] Usage:\n");
		printf("    %s <stations> <period size> \n\n", argv[0]);
		exit(0);
	}	
}

void 
print_state(int *state){
	for(int i=0;i<NS+1;i++){
		printf("%d\t",state[i]);
	}
	printf("\n");	
}

//--------------------------- Hashing Functions-------------------------------//

long long int 
hash_st(int *state){
	t_state = 0;
	t_index = 0;
	for (ii = 0; ii< NS+1; ii++){
		t_state += powof_2[t_index+state[ii]] - powof_2[t_index];
		t_index += state[ii] + 1;
	}
	return t_state - min_st;
}


void 
inv_hash_st(long long unsigned int hash_st_, int *state){
	id_= hash_st_ + min_st;
	shift_s = NS + N - 1;
	sta_cnt_;
	for(ii=NS;ii>-1;ii--){
		sta_cnt_ = 0;		
		while(id_ >= powof_2[shift_s]){
			id_ -= powof_2[shift_s];
			if(shift_s > 0) shift_s -= 1;
			sta_cnt_ += 1;
		}
		*(state+ii) = sta_cnt_;
		if (id_ > 0) shift_s -= 1;
	}
}


//--------------------------- Array Helpers-------------------------------//


void 
sum_array(int *res, int* arr1, int* arr2, int size){
	for(int i=0; i< size; i++)
		res[i] = arr1[i] + arr2[i];
}

void 
init_array(int *arr, long long unsigned int size, int value){
	for (ii=0 ;ii< size;ii++)
		arr[ii] = value;
}
void 
init_uarray(unsigned int *arr, long long unsigned int size, unsigned int value){
	for (ii=0 ;ii< size;ii++)
		arr[ii] = value;
}

void 
fill_pascal(vector<vector<double>>&C, long long unsigned size){
	long unsigned i,j;
	C.reserve(size);

	for (i=0;i<size;i++){
		C[i].reserve(i+1);
		C[i][0] = (double) 1.0;
	}
	for(i=1;i<size;i++){
		C[i][i] = (double) 1.0;
		for(j=1;j<i;j++){
      		C[i][j]=C[i-1][j] + C[i-1][j-1];
		}
	}	
}

void 
fill_pow_cache(vector<vector<double>>& pow_cache){

	pow_cache.reserve(NS+1);
	for(int x = 0; x< NS+1; x++){
		pow_cache[x].reserve(N+1);
		for(int a = 0; a< N+1; a++){
			pow_cache[x][a] = powl((double)x, (double) a);
		}
	}
}

void 
fill_powers_cache(vector<vector<vector<vector<double>>>>& powers_cache,
				vector<vector<double>>& pow_cache){

	powers_cache.reserve(NS+1);	
	for(int x = 0; x< NS+1; x++){
		powers_cache[x].reserve(N+1);
		for(int a = 0; a< N+1; a++){
			powers_cache[x][a].reserve(NS+1);
			for(int y = 0; y< NS+1; y++){
				powers_cache[x][a][y].reserve(N+1);
				for(int b = 0; b< N+1; b++){
					powers_cache[x][a][y][b] = pow_cache[x][a] / pow_cache[y][b];
				}
			}
		}
	}	
}

void 
fill_powof_2(vector<long long unsigned>& powof_2){
	powof_2.reserve(N+NS+2);	
	for(int x = 0; x< NS+N+2; x++){
		powof_2[x] = (long long unsigned)pow(2,x);
	}	
}


//--------------------------- Model Implementation -------------------------------//

/*
 * Recursive function to compute probability of all intial states
 *
 * We distribute N stations among NS slots and compute the corresponding 
 * probability of each resulting state.
 * Probability of state n = (n_1, ..., n_s) is computed as follow:
 * 		P[n = (n_1, ..., n_s)] = N! / (n_1! x n_2! x .. x n_s!) / (NS)^N
 *
 * @params:
 * i 			current slot
 * n_k 		  	a possible weak composition of N_ into Ns_ parts
 *
 */
void 
next_slot(int i, int *n_k, int* sum_n_k){
	for (int k=0; k< N-sum_n_k[i-1]+1; k++){
		prob[i] 	= prob[i-1] * C[N-sum_n_k[i-1]][k];
		sum_n_k[i] 	= k + sum_n_k[i-1];
		n_k[i] 		= k;
		ones[i] 	= ones[i-1];
		if (k == 1) ones[i] += 1;

		if (i == NS - 1){				// last slot of a period
			n_k[i+1] = N - sum_n_k[i]; 	// nb of stations in virtual slot
			s_id = hash_st(n_k); 		// cache state
			nb_succ[s_id]= ones[i] ; 	// cache number of success in state n_k (up to slot NS)

			if (N-sum_n_k[i]==0){ 		// n_k is a valid state			
				tmp_f = prob[i-1]*powers_cache[1][1][NS][N];
				prob_state[s_id] 		 = tmp_f;
				sum_pb        			+= tmp_f;
				prob_ones[nb_succ[s_id]]+= tmp_f;
			}
		}else{
			next_slot(i+1, n_k, sum_n_k);
		}
	}
}

/*
 * Recursive function to compute probability of all final states
 *
 * @params:
 * i 			current slot
 * N_ 		  	number of colliding stations
 * coll_slot  	collision slot
 * Ns_ 		  	remaining slots including the virtual slot
 * state_ 	  	vector of the state beeing redistributed
 * s_id 	  	its hash
 * n_k 		  	a possible weak composition of N_ into Ns_ parts
 */
void 
next_slot_redist(int i, int N_, int Ns_, int *n_k, int* sum_n_k, 
					 int* state_, int coll_slot, long long unsigned s_id){
	for (int k=0; k< N_-sum_n_k[i-1]+1; k++){
		prob[i] 	= prob[i-1] * C[N_-sum_n_k[i-1]][k];// C_.binom(N-n_k[i-1], k);
		sum_n_k[i] 	= k + sum_n_k[i-1];
		n_k[i] 		= k;

		if (i == coll_slot + Ns_ - 1){ // reached virtual slot
			
			if (N_-sum_n_k[i]==0){ 	   // valid state
			    it 		= prob_state.find(s_id);
				tmp_f 	= it->second 
						  * prob[i-1] 
						  * powers_cache[coll_slot][n_k[i]][NS][N_];
				
				sum_array(interm_state, state_, n_k, Ns_+coll_slot); 
												   //Ns_+coll_slot is NS + 1
				new_s_id = hash_st(interm_state);	
				prob_state[new_s_id] += tmp_f;
				check 				 -= tmp_f;
			}
		}else{
			next_slot_redist(i+1, N_, Ns_, n_k, sum_n_k, state_,
								 coll_slot, s_id);
		}
	}
}


int 
main(int argc, char *argv[]){
	long long unsigned i,j, slot;
	int k_0;
	double avg_succ;

	cout.precision(DBL_PREC);
	handle_args(argc, argv, &N, &NS);

	// allocate memory
	n_k 		= (int *) 		malloc((NS+1)*sizeof(int));
	sum_n_k 	= (int *) 		malloc((NS+1)*sizeof(int));
	interm_state= (int *) 		malloc((NS+1)*sizeof(int));	
	ones 		= (unsigned  *) malloc((NS+1)*sizeof(unsigned));
	prob.resize(NS+1);
    prob_ones.resize(NS+1);

	// initialize containers
	init_array		  (n_k, 		NS+1, 	0);
	init_array		  (sum_n_k, 	NS+1, 	0);
	init_uarray		  (ones, 		NS+1, 	0);
    fill(prob.begin(), prob.end(), (double) 0.0);
    fill(prob_ones.begin(), prob_ones.end(), (double)0.0);

    // caching computations
	fill_pascal(C, (long long unsigned)NS+N+1);
	fill_pow_cache(pow_cache);
	fill_powers_cache(powers_cache, pow_cache);
	fill_powof_2(powof_2);

	min_st = powof_2[N] - 1;
	max_st = min_st*powof_2[NS];
	const long long unsigned MAX= max_st - min_st + 1;

	printf("\n\n[+] Model parameters:\n");
	printf("    ---------------- \n");

	printf("\nstations  = %d\n",N);
	printf("slots     = %d\n",NS);	
//	printf("Hashs space = %lld\n",MAX);


	////////////////////////////////////// 	
	// 									//     	 		
	// 									//     	 	
	//    Initial states probabilities 	//   	 
	// 									//     	 				 					
	// 									//     	 		
	//////////////////////////////////////  

	printf("\n\n[+] Intial states:\n");
	printf("    ------------- \n\n");

	nb_succ.reserve(C[N+NS][N]);
	prob_state.reserve(C[N+NS][N]);
	for (k_0=0; k_0<N+1; k_0++){
		prob[0] 	= C[N][k_0];
		n_k[0] 		= k_0;
		sum_n_k[0]	= k_0;
		ones[0]		= 0;
		if (k_0 == 1) ones[0] += 1;

		next_slot(1, n_k, sum_n_k);
	}

	cout << "inital states  = " << prob_state.size() << endl<< endl;
	cout << "including \\phi = " << nb_succ.size() << endl<< endl;

	// Average number of success among inital states
	avg_succ = 0;
	for(j=0; j< NS+1; j++)
		avg_succ += j*prob_ones[j];
	
//	cout << "\nProbability sum = " << sum_pb << endl;
 	cout << "Average number of success (initial states) = " << avg_succ  << endl<< endl;
 	cout << "Rate of success           (initial states) = " << avg_succ/N  << endl;
    


	////////////////////////////////////// 	
	// 									//     	 		
	// 									//     	 	
	//    Final states probabilities 	//   	 
	// 									//     	 				 					
	// 									//     	 		
	//////////////////////////////////////  

	printf("\n\n[+] Final states:\n");
	printf("    ------------\n\n");

	int state_[NS+1], state_v_[NS+1];
	int k_i;
    duration<double> seconds;
    unsigned mins ;
    unsigned secs ;

    /*
     * Iterate over slots 1 to NS -1 
	 * We do not distribute stations in the slot NS
	 */
	for(i=0; i< NS-1;i++){ 
	    auto start 	= system_clock::now();
		rm 			= 0;
		imprecision = (double) 0.0;

 		for(j=0; j< MAX; j++){ 				// all states
	    	it1= prob_state.find(j);

	    	if(it1 != prob_state.end()){
				inv_hash_st(j, state_); 	// state n'
				
				if(state_[i] > 1){			// A collision occured in slot i+1
					check = it1->second;	// proba to be in state n'

					for(k_i=0; k_i < state_[i]+1; k_i++){						
                        fill(prob.begin(), prob.end(), (double) 0.0);
						init_array(n_k, NS+1, 0);
						init_array(sum_n_k, NS+1, 0);
						prob[i + 1] 	= C[state_[i]][k_i];
						n_k[i]			= -state_[i];																
						n_k[i + 1] 		= k_i;
						sum_n_k[i + 1]	= k_i;	

						// Distribute state_[i] stations among NS-i+1 slots 
						// starting from slot i+2
						next_slot_redist(i+2, state_[i], NS-i, n_k, 
											sum_n_k, state_, i+1, j);
					}				
					imprecision += check;
					prob_state.erase(j);
					rm++;		
				}
			}
		}
		cout << "Slot " << setw(2)<< i+1 <<
				" -- interm: " << setprecision(0) << scientific << prob_state.size() << 
				" -- resid. error: " << scientific << imprecision <<
				" -- removed: " << rm << endl;		

	    auto end = system_clock::now();
	    seconds = end-start;
	    mins 	= seconds.count()/60;
	    secs 	= (unsigned) seconds.count() % 60;
	    cout 	<< "        -- "   << 
	    			mins << "m"   << 
	    			secs << "s\n" << endl;
	}

	/*
	 * Computing the law of number of success in a period
	 *
	 */
	printf("\n[+] Results:\n    ------- \n\n");

	avg_succ 	= (double)0.0;
	sum_pb 		= (double) 0.0;
    fill(prob_ones.begin(), prob_ones.end(), (double) 0.0);            

	for(auto& x: prob_state)
		prob_ones[nb_succ[x.first]] += x.second;
	
	cout << "Final states = " << prob_state.size() << endl<<endl;

	for(i=0; i< NS+1; i++){
		sum_pb 		+= prob_ones[i];
		avg_succ 	+= i*prob_ones[i];
	}
	cout.precision(DBL_PREC);
	cout << fixed;
    cout << "Average number ofsuccess (final states) = "<< avg_succ  <<endl<<endl;
    cout << "Rate of success          (final states) = "<< avg_succ/(double)N <<endl<<endl;
//  cout << "\nProbability sum= "<< sum_pb  <<endl;


    for (int i =0;i< NS+1;i++)
    	cout << "PR[" << i<<"] = "<<prob_ones[i] <<endl;


	/*
	 * Log average number of success and success rate in a period
	 *
	 */
	char* f_name;
	if(0 > asprintf(&f_name, "success_%u_slots.proba", NS)){
		printf("formatting log file's name went wrong");
		return 1;
	} 
	log_results(f_name, NS, N, avg_succ);


	free(n_k);
	free(sum_n_k);
	free(ones);
	free(interm_state);
	vector<vector<double>>().swap(C);
	vector<long long unsigned>().swap(powof_2);
	vector<vector<double>>().swap(pow_cache);
	vector<vector<vector<vector<double>>>>().swap(powers_cache);

   	return 0;
}