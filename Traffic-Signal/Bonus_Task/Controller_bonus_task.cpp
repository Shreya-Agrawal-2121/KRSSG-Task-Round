#include <arpa/inet.h>

#include <stdio.h>

#include <string.h>

#include <sys/socket.h>

#include <unistd.h>

#include <iostream>

#include <bits/stdc++.h>

using namespace std;

vector<int> waiting_queue(8, 0);

// FSM  class stores possible states of traffic lights as pair of ints and transitions as per input

// Input is expressed as an integer in the range of 0 to 255 each bit indicating

// presence or absence of traffic at that crossing.

class FSM

{

public:
    vector<pair<int, int>> states;

    int trans[13][256];

    // To ensure optimality for any input a state is formed such that traffic exists on both crossings

    // If it is not so then a state is found such that traffic is present at least in one of the crossings

    // otherwise the state doesn't changes or goes to state 0 (when all lights are off).

    FSM(vector<pair<int, int>> state)

    {

        bool possible2, possible1;

        states = state;

        for (int i = 0; i < 13; i++)

            trans[i][0] = 0;

        for (int i = 0; i < 13; i++)

        {

            for (int j = 1; j < 256; j++)

            {

                possible2 = possible1 = false;

                for (int k = 1; k < 13; k++)

                {

                    if (i == k)

                        continue;

                    else

                    {

                        int p1 = states[k].first;

                        int p2 = states[k].second;

                        if ((j & (1 << p1)) && (j & (1 << p2)))

                        {

                            possible2 = true;

                            trans[i][j] = k;

                            break;
                        }
                    }
                }

                if (possible2 == false)

                {

                    for (int k = 1; k < 13; k++)

                    {

                        if (i == k)

                            continue;

                        else

                        {

                            int p1 = states[k].first;

                            int p2 = states[k].second;

                            if ((j & (1 << p1)) || (j & (1 << p2)))

                            {

                                possible1 = true;

                                trans[i][j] = k;

                                break;
                            }
                        }
                    }

                    if (possible1 == false)

                    {

                        if (i == 0)

                            trans[i][j] = 0;

                        else

                        {

                            int p1 = states[i].first;

                            int p2 = states[i].second;

                            if ((j & (1 << p1)) || (j & (1 << p2)))

                                trans[i][j] = i;

                            else

                                trans[i][j] = 0;
                        }
                    }
                }
            }
        }
    }
};

// Function to convert the traffic input to a number in range of 0 to 255

int get_input()

{

    int sum = 0;

    for (int i = 0; i < 8; i++)

    {

        if (waiting_queue[i])

        {

            sum += ((1 << i));
        }
    }

    return (sum);
}

void modify_queue(pair<int, int> p)

{

    waiting_queue[p.second] = max(0, waiting_queue[p.second] - 1);

    waiting_queue[p.first] = max(0, waiting_queue[p.first] - 1);
}

int main(int argc, char *argv[])

{

    // States is a vector of pairs  indicating state of lights in  a predefined sequential order

    vector<pair<int, int>> states = {{-1, -1}, {0, 2}, {4, 6}, {1, 3}, {5, 7}, {0, 1}, {2, 3}, {4, 5}, {6, 7}, {0, 7}, {1, 4}, {2, 5}, {3, 6}};

    int num;

    int prev_state, next_state, t, i, j, time_step = 0, available;

    prev_state = 0;

    pair<int, int> p, new_p, status;

    //  Server socket is created and bound to a port

    //  Connection is established with a client and server is put in listening mode

    int socket_desc, client_sock, c, read_size;

    struct sockaddr_in server, client;

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;

    server.sin_addr.s_addr = INADDR_ANY;

    server.sin_port = htons(8880);

    bind(socket_desc, (struct sockaddr *)&server, sizeof(server));

    listen(socket_desc, 3);

    c = sizeof(struct sockaddr_in);

    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);

    FSM F(states);

    recv(client_sock, &t, sizeof(int), 0);

    while (true)

    {

        if (t > 0)

        {

            t--;

            for (i = 0; i < 8; i++)

            {

                // Server receives information of traffic flow

                // and modifies the queue of waiting cars accordingly.

                recv(client_sock, &num, sizeof(int), 0);

                waiting_queue[i] += num;
            }
        }

        int input = get_input();

        next_state = F.trans[prev_state][input];

        send(client_sock, &next_state, sizeof(int), 0);

        if (next_state != 0)

            modify_queue(states[next_state]);

        prev_state = next_state;

        if (next_state == 0 && t <= 0)

        {

            cout << "Traffic cleared !! \n";

            break;
        }
    }

    close(client_sock);

    close(socket_desc);

    return 0;
}