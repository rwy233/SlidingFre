#include <fstream>
#include <iomanip>
#include <map>
#include <math.h>
#include <unordered_map>
#include <unordered_set>
#include "../splitter.cpp"
#include "../hash_class.cpp"
#include "../data.cpp"

#define tpTest 1
unordered_map<Data, int, My_Hash> mp;
unordered_set<Data, My_Hash> st;
//argv[1]:cycle
//argv[2]:hash_number
//argv[3]:row_length
//argv[4]:file
//argv[5]:input_num_max
//argv[6]:out_file
//argv[7]:out_model
//argv[8]:label_name
int main(int argc, char* argv[])
{
    double tau = atof(argv[3]);
    double mu = 1.5;
    int cycle = 1000000;
    int hash_number = atoi(argv[4]);
    int row_length = atoi(argv[1]);
    int input_num_max = 1000000 * 8 + 1;
    int out_model = 2;

    Bucket cm(cycle, tau, mu, hash_number, row_length);
    //Bucket cu(cycle, tau, mu, hash_number, row_length);
    //Bucket co(cycle, tau, mu, hash_number, row_length);
    int input_num = 0;

    Data *dat = new Data[cycle];
    memset(dat, 0, cycle * sizeof(Data));

    FILE* file = fopen(argv[2],"rb");
    Data packet;
 
    double CM_dif = 0;
    double CM_ae = 0;
    double CM_re = 0;

    int Test_num = 0;
    cout <<"SWCM,Arrivals,ARE"<<endl;
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

        cm.update(packet.str, DATA_LEN, input_num);
#ifndef tpTest
        
        if( input_num > 2*cycle){

            if(input_num%(cycle/5) ==0){
                st.clear();
                for(int i=0;i<cycle;++i){
                    if(st.find(dat[i]) != st.end()) continue;
                    st.insert(dat[i]);
                    Test_num ++;
                    CM_dif = fabs(cm.query(dat[i].str,DATA_LEN) - mp[dat[i]]);
                    CM_ae += CM_dif; 
                    CM_re += CM_dif/mp[dat[i]]; 
                }
                
                // double tp = (double)num /1000000 / ((double)(clock()-clock_)/CLOCKS_PER_SEC);
                cerr << "SWCM mem" << "," << input_num << "," << cm.q_memory()/1024.0/1024.0 << endl;
                cerr << "SWCM are" << "," << input_num << "," << CM_re / Test_num << endl;
                cerr  << "SWCM aae" << "," << input_num << "," << CM_ae / Test_num << endl;
            }
        }
#endif

        input_num ++;                   
    }
#ifndef tpTest 
                cout << "SWCM mem" << "," << input_num << "," << cm.q_memory()/1024.0/1024.0 << endl;
                cout << "SWCM are" << "," << input_num << "," << CM_re / Test_num << endl;
                cout << "SWCM aae" << "," << input_num << "," << CM_ae / Test_num << endl;
#else
    double tp = (double)input_num/1000000 / ((double)(clock()-clock_)/CLOCKS_PER_SEC);
    cout << "SWCM mem" << "," << input_num << "," << cm.q_memory()/1024.0/1024.0 << endl;
    cout << "TP" << tpTest << "," << input_num << "," << tp << endl;
#endif


}