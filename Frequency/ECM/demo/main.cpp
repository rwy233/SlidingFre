#include <fstream>
#include <iomanip>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <math.h>
#include "../sketch.cpp"
#include "../data.cpp"
#include "../hash_class.cpp"

unordered_map<Data, int, My_Hash> mp;
unordered_set<Data, My_Hash> st;
#define tpTest 1

//argv[1]:cycle  argv[2]:update_k
//argv[3]:hash_number  argv[4]:row_length
//argv[5]:file    argv[6]:input_num_max
//argv[7]:out_file
//argv[8]:out_model
//argv[9]:label_name
//argv[10]:memory_str
int main(int argc, char* argv[])
{
	int cycle = 1000000;
    Data *dat = new Data[cycle];
	int update_k = atoi(argv[1]);
	int hash_number =  5;
	int row_length =  atoi(argv[3]);
	int out_model =  1;

	
    ECM ecm(cycle, update_k, hash_number, row_length);

	unsigned long long int input_num_max =  1000000 * 8;
    unsigned long long int input_num = 0;
    FILE* file = fopen(argv[2],"rb");
    Data packet;
    double CM_dif = 0;
    double CM_ae = 0;
    double CM_re = 0;

    int Test_num = 0;
    cout << "ECM,Arrivals,ARE"<<endl;
    int clock_ = clock();
    while(fread(packet.str, DATA_LEN, 1, file) > 0){
        if(input_num >= input_num_max){
            break;
        }
#ifndef tpTest

        if(input_num >= cycle){
            mp[dat[input_num % cycle]] -= 1;
        }
        dat[input_num % cycle] = packet;
        if(mp.find(packet) == mp.end()){
            mp[packet] = 1;
        }
        else{
            mp[packet] += 1;
        }
#endif
        
        ecm.update(packet.str, DATA_LEN, input_num);
#ifndef tpTest
        
        if( input_num > 2*cycle){

            if(input_num%(cycle/5) ==0){
                st.clear();
                for(int i=0;i<cycle;++i){
                    if(st.find(dat[i]) != st.end()) continue;
                    st.insert(dat[i]);
                    Test_num ++;
                    CM_dif = fabs(ecm.query(dat[i].str,DATA_LEN,input_num) - mp[dat[i]]);
                    CM_ae += CM_dif; 
                    CM_re += CM_dif/mp[dat[i]]; 
                }
                
                // double tp = (double)num /1000000 / ((double)(clock()-clock_)/CLOCKS_PER_SEC);
                cerr << "SWCM mem" << "," << input_num << "," << ecm.q_memory(input_num)/1024.0/1024.0 << endl;
                cerr << "SWCM are" << "," << input_num << "," << CM_re / Test_num << endl;
                cerr << "SWCM aae" << "," << input_num << "," << CM_ae / Test_num << endl;
            }
        }
#endif

        input_num ++;                   
    }
#ifndef tpTest 

    cout << "SWCM mem" << "," << input_num << "," << ecm.q_memory(input_num)/1024.0/1024.0 << endl;
    cout << "SWCM are" << "," << input_num << "," << CM_re / Test_num << endl;
    cout << "SWCM aae" << "," << input_num << "," << CM_ae / Test_num << endl;
#else
    double tp = (double)input_num/1000000 / ((double)(clock()-clock_)/CLOCKS_PER_SEC);
    cout << "SWCM mem" << "," << input_num << "," << ecm.q_memory(input_num)/1024.0/1024.0 << endl;
    cout << "TP" << tpTest << "," << input_num<< "," << tp << endl;
#endif
}