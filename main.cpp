//
// Created by Elif Akalin on 13.02.2021.
//

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <pthread.h>
#include <unistd.h>

using namespace std;
//I created the input and output file streams as global so that every thread can access it anywhere.
ifstream infile;
ofstream outfile;

//Main matrix of client information. 300 is the maximum number of clients this simulation can receive according to the description.
int clients[300][4];
//Current status of the 3 tellers. -1 means they are available, otherwise holds the id of the client that is currently at the teller.
int tellerStatus[3] = {-1, -1, -1};
//clientCount is the total number of clients, while servedClientCount is the current number of clients that is served at any time.
int clientCount, servedClientCount = 0;
//List of seats, holding the availability of every seat.
vector<int> seatsList;
//The operation of creating all of the clients
bool doneCreating = false;

//printM is for locking the printing operation, tstatusM is for locking the tellerStatus array operations, seatM is for locking the seat operations
pthread_mutex_t printM = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tstatusM = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t seatM = PTHREAD_MUTEX_INITIALIZER;

/*
 * Receives the pointer of an int array, first element being the id of the client.
 * First, the clients wait for all of the clients to be created, to start the operations.
 * Then they wait for their arrival time using usleep. After that, there is a lock when the
 * client picks the available teller with the highest priority (A > B > C). This lock also
 * serves as a queue mechanism.
 * After a teller is picked, the client sleeps for its serving time.
 * The teller picks up the rest of the operation.
 */
void *client(void *param) {
    int *items = (int *) param;
    int id = *(items);
    while (!doneCreating);
    //Client waits until the arrival time
    usleep(1000 * clients[id - 1][1]);


    pthread_mutex_lock(&tstatusM);
    //Client waits until there is an available teller.
    while (tellerStatus[0] != -1 && tellerStatus[1] != -1 && tellerStatus[2] != -1);
    //Client picks the teller with highest priority, then sets that teller's status as its id.
    if (tellerStatus[0] == -1) {
        tellerStatus[0] = id;
    } else if (tellerStatus[1] == -1) {
        tellerStatus[1] = id;
    } else if (tellerStatus[2] == -1) {
        tellerStatus[2] = id;
    }
    pthread_mutex_unlock(&tstatusM);
    usleep(1000 * clients[id - 1][2]);
    return 0;
}

/*
 * Receives the pointer of a string, which is the label of the teller. For ease of operations, an integer called tellerID
 * is created accordingly. The tellers wait for some time so that they initialize in the correct order.
 *
 * Each teller waits for all of the clients to be created. After that, a loop continues until served client count is equal
 * to the total client count.  Each teller checks its status, if it's not -1, it means that there is a client waiting to
 * be processed. The seat the client wants is checked. If the seat is available, the client takes the seat without a problem,
 * the seat is marked as not available, and the teller waits for the client's serving time. If the seat is not available or
 * the client is asking for a nonexistent seat, seat is updated as the first available index. If the hall is full, the client
 * isn't given any seat. Either case the teller waits for the client's serving time.
 * Then, the teller is marked as available and the main loop of the teller continues.
 */
void *teller(void *param) {
    string label = *(string *) param;
    int tellerID;
    if (label == "A") tellerID = 0; else if (label == "B") tellerID = 1; else tellerID = 2;
    usleep(1000 * tellerID);
    pthread_mutex_lock(&printM);
    outfile << "Teller " << label << " has arrived." << endl;
    pthread_mutex_unlock(&printM);

    while (!doneCreating);
    while (servedClientCount < clientCount) {
        pthread_mutex_lock(&tstatusM);
        if (tellerStatus[tellerID] != -1) {
            int c = tellerStatus[tellerID];
            pthread_mutex_unlock(&tstatusM);
            int seat = clients[c - 1][3];
            if (servedClientCount >= seatsList.size()) {
                usleep(1000 * clients[c - 1][2]);
                pthread_mutex_lock(&printM);
                outfile << "Client" << c << " requests seat " << clients[c - 1][3]
                        << ", reserves None. Signed by Teller " << label << "." << endl;
                pthread_mutex_unlock(&printM);
            } else {
                pthread_mutex_lock(&seatM);
                if (seatsList[seat - 1] || seat > seatsList.size()) {
                    for (int i = 0; i < seatsList.size(); ++i) {
                        if (!seatsList[i]) {
                            seat = i + 1;
                            break;
                        }
                    }
                }
                seatsList[seat - 1] = 1;
                pthread_mutex_unlock(&seatM);
                usleep(1000 * clients[c - 1][2]);
                pthread_mutex_lock(&printM);
                outfile << "Client" << c << " requests seat " << clients[c - 1][3] << ", reserves seat " << seat
                        << ". Signed by Teller " << label << "." << endl;
                pthread_mutex_unlock(&printM);
            }
            tellerStatus[tellerID] = -1;
            pthread_mutex_lock(&tstatusM);
            servedClientCount++;
        }
        pthread_mutex_unlock(&tstatusM);
    }
    return 0;
}

/*
 * Input file is opened directly. Output file is opened for clearing, then closed and opened again in append mode.
 *
 * Using the input file, the hall name and the total number of clients is read. Then, two thread lists are created, one
 * for tellers and the other for clients. The three teller threads are created. Then all the data for the clients are
 * read from the input file and parsed, and the threads for the clients are created as soon as all the info is received.
 * After all of the clients are created, the boolean for being done with creating all the clients is set as true.
 * The size of the seat list is set according to the hall name. Then, the main thread waits for the teller and client
 * threads to finish their job.
 */
int main(int argc, char *argv[]) {
    infile.open(argv[1]);
    outfile.open(argv[2], ofstream::out);
    outfile.close();
    outfile.open(argv[2], ios_base::app);
    if (argc != 3) {
        cout << "wrong argument count" << endl;
        return 1;
    } else if (!infile) {
        cout << "input file problem" << endl;
        return 1;
    } else if (!outfile) {
        cout << "output file problem" << endl;
        return 1;
    }

    outfile << "Welcome to the Sync-Ticket!" << endl;
    string hall;
    infile >> hall >> clientCount;

    pthread_t tellerThreads[3];
    pthread_t clientThreads[clientCount];
    string tellerTypes[] = {"A", "B", "C"};
    pthread_create(&tellerThreads[0], NULL, teller, &tellerTypes[0]);
    pthread_create(&tellerThreads[1], NULL, teller, &tellerTypes[1]);
    pthread_create(&tellerThreads[2], NULL, teller, &tellerTypes[2]);

    for (int i = 0; i < clientCount; i++) {
        string line;
        infile >> line;
        unsigned long prev = line.find("Client") + 6;
        unsigned long current = line.find(',');
        for (int j = 0; j < 4; ++j) {
            current = line.find(',', prev);
            clients[i][j] = stoi(line.substr(prev, current - prev));
            prev = current + 1;
        }
        pthread_create(&clientThreads[i], NULL, client, &clients[i]);
    }
    doneCreating = true;
    switch (hall.size()) {
        case 12:
            seatsList.resize(60);
            break;
        case 18:
            seatsList.resize(80);
            break;
        case 10:
            seatsList.resize(200);
            break;
        default:
            cout << "-1" << endl;
    }

    for (int i = 0; i < clientCount; ++i) {
        pthread_join(clientThreads[i], NULL);
    }

    pthread_join(tellerThreads[0], NULL);
    pthread_join(tellerThreads[1], NULL);
    pthread_join(tellerThreads[2], NULL);

    outfile << "All clients received service." << endl;
}
