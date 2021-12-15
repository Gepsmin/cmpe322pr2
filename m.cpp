//
// Created by onursefa on 31.01.2021.
//

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <fstream>
#include <string>
#include <stdio.h>
#include <vector>
#include <sstream>
#include <queue>

using namespace std;

struct thread_info{
    string name;
    int arrival;
    int service;
    int seat;
};


fstream write_file;
char* file_address;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t que_mutex = PTHREAD_MUTEX_INITIALIZER;
int available[3] = {1, 1, 1};
thread_info processing[3];
vector<int> chairs_pointer;
pthread_mutex_t reserve_mutex = PTHREAD_MUTEX_INITIALIZER;
int wait = 0;
int chair_number = 0;
pthread_mutex_t arriiv = PTHREAD_MUTEX_INITIALIZER;
bool done = false;
int mult = 1000;


int reserve(int seat){

    pthread_mutex_lock(&reserve_mutex);
    if (chairs_pointer[seat]==0&&seat <= chair_number){
        chairs_pointer[seat]=1;
        pthread_mutex_unlock(&reserve_mutex);
        return seat;
    }else{
        for(int i=1; i<=chair_number; i++){
            if(chairs_pointer[i]==0){
                chairs_pointer[i]=1;
                pthread_mutex_unlock(&reserve_mutex);
                return i;
            }
        }
    }
    pthread_mutex_unlock(&reserve_mutex);
    return -1;
}

void* teller(void* arg){
    char name = *(char *) arg;
    int teller = (int)name - 65;

    pthread_mutex_lock(&mutex);
    write_file << "Teller " << name << " has arrived." << endl;
    pthread_mutex_unlock(&mutex);


    while(!done){
        pthread_mutex_lock(&arriiv);
        int a = processing[teller].arrival;
        processing[teller].arrival = -1;
        pthread_mutex_unlock(&arriiv);
        
        if(a != -1){
            string cl_name = processing[teller].name;
            int seat = processing[teller].seat;
            int chair = reserve(seat);
            int service = processing[teller].service;
            
            usleep(service*mult);


            pthread_mutex_lock(&mutex);
            write_file << cl_name << " requests seat " << seat << ", reserves ";
            if(chair==-1){
                write_file << "None";
            }else{
                write_file << "seat " << chair;
            }
            write_file << ". Signed by Teller " << name << "."<<endl;
            pthread_mutex_unlock(&mutex);

        }
    }


    return 0;
}

void* client(void* arg){
    struct thread_info info = *(thread_info *)arg;
    while(wait==0);
    usleep(info.arrival*mult);
    

    pthread_mutex_lock(&que_mutex);
    int teller = -1;
    while(teller==-1){
        if(available[0]==1){
            teller = 0;
        }else if(available[1]==1){
            teller = 1;
        }else if(available[2]==1){
            teller = 2;
        }
    }
    available[teller] = 0;
    pthread_mutex_unlock(&que_mutex);

    pthread_mutex_lock(&arriiv);
    processing[teller] = info;
    pthread_mutex_unlock(&arriiv);

    usleep(info.service*mult);

    available[teller] = 1;
        

    return 0;
}


int main(int argc, char* argv[]){
    pthread_t client_threads[300];
    thread_info client_info[300];
    fstream read_file;
    file_address = argv[2];
    read_file.open(argv[1], ios_base::in);
    write_file.open(file_address, ios_base::out);
    write_file.close();
    write_file.open(file_address, ios_base::app);

    write_file << "Welcome to the Sync-Ticket!" << endl;

    string theater_name = "";
    int client_number = 0;
    getline(read_file, theater_name);
    string temp;
    getline(read_file, temp);
    client_number = stoi(temp);

    if(theater_name.compare("OdaTiyatrosu")==0){
        chair_number = 60;
    }else if(theater_name.compare("UskudarStudyoSahne")==0){
        chair_number = 80;
    }else{
        chair_number = 200;
    }

    for(int i=0; i<3; i++){
        processing[i].arrival = -1;
    }
    
    for(int i=0; i<=chair_number; i++){
        chairs_pointer.push_back(0);
    }


    pthread_t a_thread_t;
    pthread_t b_thread_t;
    pthread_t c_thread_t;

    char teller_name_a = 'A';
    char teller_name_b = 'B';
    char teller_name_c = 'C';

    pthread_create(&a_thread_t, NULL, teller, &teller_name_a);
    pthread_create(&b_thread_t, NULL, teller, &teller_name_b);
    pthread_create(&c_thread_t, NULL, teller, &teller_name_c);


    string line;
    for(int i=0; i< client_number; i++){
        getline(read_file, line);
        stringstream ss(line);
        vector<string> s_vector;
        while (ss.good()){
            string substr;
            getline(ss, substr, ',');
            s_vector.push_back(substr);
        }
        client_info[i].name = s_vector[0];
        client_info[i].arrival = stoi(s_vector[1]);
        client_info[i].service = stoi(s_vector[2]);
        client_info[i].seat = stoi(s_vector[3]);
        pthread_create(&client_threads[i], NULL, client, &client_info[i]);
    }
    wait = 1;

    for(int i=0; i< client_number; i++){
        pthread_join(client_threads[i], NULL);
    }

    done = true;

    pthread_join(a_thread_t, NULL);
    pthread_join(b_thread_t, NULL);
    pthread_join(c_thread_t, NULL);


    write_file << "All clients received service." << endl;

    write_file.close();
    read_file.close();


    cout << "Hello World" << endl;




    return 0;
}

