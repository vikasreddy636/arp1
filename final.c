#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
//#include "offset.h"
#include "arpnet.h"



char my_ip[20] = "23.97.253.253";
int turnLeader_FLAG = 0;
int turnLeader = 0;
int available_nodes;

int main(){
	//Declaring variables to be used later //
        srand(time(NULL));
        struct timeval timeout;
        timeout.tv_sec = 50;
        timeout.tv_usec = 0;
	iptab_reset_flags();
	int my_id = iptab_get_ID_of(my_ip);
	int r_hsh, w_hsh, r_vot, w_vot, r_nor, w_nor;
	int sock_fd, newsock_fd, send_sock_fd, check1;
	char next_ip[20];
        int next_id;
	votation_t vote_msg;
	int winner_id;
	message_t norm_msg;
	int n_nodes = iptab_len();  // number of nodes
	handshake_t hsh_msg;
	
	printf("I am starting\n");
	printf("My id is %d\n", my_id);
	printf("My ip is %s\n", my_ip);
	
	
	///////////////////////////////*********************///////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////********************///////////////////////////////////////////////////////////
	
	//--------------------------------------
        //  FIRST:: HANDSHAKE
        //--------------------------------------
	sock_fd = net_server_init();	
	//check I am the first node or not
	int znode = hsh_imfirst(my_ip);
	
	if(znode == 1){
		printf("I am the zero node\n");
		printf("starting handshake\n");
		turnLeader_FLAG = 1;
		
		hsh_init(&hsh_msg);
		//Checking my version and availability
	    check1 = hsh_check_availability(my_id, &hsh_msg );
		next_id = iptab_get_next(my_id);
		strcpy(next_ip, iptab_getaddr(next_id));
		
		printf("Next id is %d\n", next_id);
		
		//Connecting to next node's server
	    //next_id = iptab_get_next(my_id);
	    //strcpy(next_ip, iptab_getaddr(next_id ));
	    send_sock_fd = net_client_connection_timeout(next_ip, &timeout);
	    while(send_sock_fd <0){
		    printf("Error in connecting to server\n");
		    //Connecting to next node's server
    	    next_id = iptab_get_next(next_id);
	        strcpy(next_ip, iptab_getaddr(next_id));
	        send_sock_fd = net_client_connection_timeout(next_ip, &timeout);
		    //return 0;
		}
		printf("I connected to the server\n");
	
	
	    //Sending handshake msg to next node
	    w_hsh = write(send_sock_fd, &hsh_msg, sizeof(handshake_t));
	    if(w_hsh<0){
		    printf("Error in writing to socket\n");
		    return 0;
	    }
	    printf("I sent the first handshake msg\n");
	
	    //Accepting handshake msg from previous node
	    hsh_init(&hsh_msg);
	    //sock_fd = net_server_init();
	    newsock_fd = net_accept_client_timeout(sock_fd, &timeout, NULL);
	
	    if(newsock_fd<0){
		    printf("Error in accepting client\n");
		    return 0;
	    }
	
	    r_hsh = read(newsock_fd, &hsh_msg, sizeof(handshake_t));
	    if(r_hsh<0){
		    printf("Error in reading from socket\n");
		    return 0;
	    }
            printf("I received the turned back handshake msg\n");
	
	    hsh_update_iptab(&hsh_msg);
	    printf("I updated the ip_table\n");
            close(newsock_fd);
            close(send_sock_fd);
		
		////////////////////////////
		//// Second handshake step ////
		/////////////////////////////
		printf("Handshake second step\n");
		
		//hsh_init(&hsh_msg);
		
		next_id = iptab_get_next(my_id);
		strcpy(next_ip, iptab_getaddr(next_id));
		
		printf("Next id is %d\n", next_id);
		
		//Connecting to next node's server
	    send_sock_fd = net_client_connection_timeout(next_ip, &timeout);
	    if(send_sock_fd <0){
		    printf("Error in connecting to server\n");
		    return 0;
		}
		printf("I connected to the server\n");
	
	
	    //Sending handshake msg to next node
	    w_hsh = write(send_sock_fd, &hsh_msg, sizeof(handshake_t));
	    if(w_hsh<0){
		    printf("Error in writing to socket\n");
		    return 0;
	    }
	    printf("I sent the second handshake msg\n");
	
	    //Accepting handshake msg from previous node
	    hsh_init(&hsh_msg);
	    //sock_fd = net_server_init();
	    newsock_fd = net_accept_client_timeout(sock_fd, &timeout, NULL);
	
	    if(newsock_fd<0){
		    printf("Error in accepting client\n");
		    return 0;
	    }
	
	    r_hsh = read(newsock_fd, &hsh_msg, sizeof(handshake_t));
	    if(r_hsh<0){
		    printf("Error in reading from socket\n");
		    return 0;
	    }
	    close(newsock_fd);
            close(send_sock_fd);
	    
	    printf("I finished handshake\n");
	
	
	}
	
	if(znode==0){//else

	//--------------------------------------
        //  HANDSHAKE FIRST STEP
        //--------------------------------------
	printf("I am not the zero node\n");
	//hsh_msg;
	hsh_init(&hsh_msg);
	
	//Accepting handshake msg from previous node
	//sock_fd = net_server_init();
	newsock_fd = net_accept_client_timeout(sock_fd, &timeout, NULL);
	
	if(newsock_fd<0){
		printf("Error in accepting client\n");
		return 0;
	}
	
	r_hsh = read(newsock_fd, &hsh_msg, sizeof(handshake_t));
	if(r_hsh<0){
		printf("Error in reading from socket\n");
		return 0;
	}
        printf("I received the handshake first msg\n");
	close(newsock_fd);
	//Checking my version and availability
	check1 = hsh_check_availability(my_id, &hsh_msg );
	
	
	//Connecting to next node's server
	next_id = iptab_get_next(my_id);
	strcpy(next_ip, iptab_getaddr(next_id ));
	send_sock_fd = net_client_connection_timeout(next_ip, &timeout);
	while(send_sock_fd <0){
		printf("Handshake:: Error in connecting to server\n");
		//Connecting to next node's server
    	next_id = iptab_get_next(next_id);
	    strcpy(next_ip, iptab_getaddr(next_id));
	    send_sock_fd = net_client_connection_timeout(next_ip, &timeout);
		//return 0;
	}
        //close(send_sock_fd);
	
	//Sending handshake msg to next node
	w_hsh = write(send_sock_fd, &hsh_msg, sizeof(handshake_t));
	if(w_hsh<0){
		printf("Error in writing to socket\n");
		return 0;
	}
        close(send_sock_fd);
        printf("I sent the handshake msg\n");
	
	//close(sock_fd);
	//close(newsock_fd);
	//close(send_sock_fd);
	
	if(check1 ==0){
		printf("version is incorrect\n");
		return 0;
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////
	
	
	//--------------------------------------
    //  HANDSHAKE SECOND STEP
    //--------------------------------------
	printf("Handshake Second step\n"); 
	
	//Accepting handshake msg from previous node
	//sock_fd = net_server_init();
	newsock_fd = net_accept_client_timeout(sock_fd, &timeout, NULL);
	
	if(newsock_fd<0){
		printf("Error in accepting client\n");
		return 0;
	}
	
	r_hsh = read(newsock_fd, &hsh_msg, sizeof(handshake_t));
	if(r_hsh<0){
		printf("Error in reading from socket\n");
		return 0;
	}
	printf("I accepted the second handshake msg\n");
	//Updating ip table
	hsh_update_iptab( &hsh_msg );
	close(newsock_fd);
	
	//Connecting to next node's server
	next_id = iptab_get_next( my_id );
	strcpy(next_ip, iptab_getaddr(next_id));
	send_sock_fd = net_client_connection_timeout(next_ip, &timeout);
	if(send_sock_fd <0){
		printf("Handshake:: Error in connecting to server\n");
		return 0;
	}
	
	//Sending handshake msg to next node
	w_hsh = write(send_sock_fd, &hsh_msg, sizeof(handshake_t));
	if(w_hsh<0){
		printf("Error in writing to socket\n");
		return 0;
	}
	printf("I sent the seconde handshake msg\n");
	
	//close(sock_fd);
	
	close(send_sock_fd);
	}
	
	
	
	///////////////////////////////*********************///////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////********************///////////////////////////////////////////////////////////
	
	
	// get the number of available nodes
	available_nodes = iptab_len_av();
	printf("Number of available nodes: %d\n", available_nodes);
	
	// Arrays to store the sending and receiving times for each message in the 10 turns of each leader
	struct timeval sending_times[10][available_nodes-1];
	struct timeval receiving_times[10][available_nodes-1];
	
	int counter = 0; // counter to store received messages for each turnleader
	
	//Repeat 10 times	
	for(int i=0; i<10; i++){
	
	//--------------------------------------
        //  SECOND:: VOTATION STEP
        //--------------------------------------

	vote_init(&vote_msg);
	printf("Started votation step\n");
	
	if(turnLeader_FLAG == 1){
		printf("Past TurnLeader:: I will hold the votation\n");
		//I am the previous turn leader
		turnLeader_FLAG = 0;
		
		vote_do_votation(&vote_msg);
			
		//Connecting to next node's server
	    next_id = iptab_get_next( my_id );
	    strcpy(next_ip, iptab_getaddr(next_id));
	    send_sock_fd = net_client_connection_timeout(next_ip, &timeout);
	    if(send_sock_fd <0){
		    printf("Votation: Error in connecting to server\n");
		    return 0;
	    }
	
	    //Sending votation msg to next node
	    w_vot = write(send_sock_fd, &vote_msg, sizeof(votation_t));
	    if(w_vot<0){
		    printf("Votation: Error in writing to socket\n");
		    return 0;
	    }
		
		
		printf("I voted and sent votation msg to %d\n",next_id);
                close(send_sock_fd);
		//Accepting Votation msg from last voting node
	    //sock_fd = net_server_init();
	    newsock_fd = net_accept_client_timeout(sock_fd, &timeout, NULL);
	
	    if(newsock_fd<0){
		    printf("Votation: Error in accepting client\n");
		    return 0;
	    }
	    printf("I connected to votation client\n");
	    r_vot = read(newsock_fd, &vote_msg, sizeof(votation_t));
	    if(r_vot<0){
		    printf("Error in reading from socket\n");
		    return 0;
	    }
		
		//close(sock_fd);
	    close(newsock_fd);
	    //close(send_sock_fd);
		printf("I received votation msg\n");
		//Getting the winner and informing him 
		winner_id = vote_getWinner( &vote_msg );
		printf("Winner id is: %d\n",winner_id);
		
		if(winner_id != my_id){
                        printf("I am not the new turnleader\n");
			turnLeader_FLAG=0;
			msg_init(&norm_msg);
			msg_set_ids( &norm_msg, my_id , winner_id );
			
			//Connecting to winner node's server
	        strcpy(next_ip, iptab_getaddr(winner_id));
	        send_sock_fd = net_client_connection_timeout(next_ip, &timeout);
	        if(send_sock_fd <0){
		        printf("Votation: Error in connecting to server\n");
		        return 0;
	        }
	
	        //Sending votation msg to winner node
	        w_vot = write(send_sock_fd, &norm_msg, sizeof(message_t));
	        if(w_vot<0){
		        printf("Votation: Error in writing to socket\n");
		        return 0;
	        }
			close(send_sock_fd);
                        printf("I sent notification msg to the winner%d\n", winner_id);
		}
		else{
			turnLeader_FLAG = 1;
			turnLeader = winner_id;
                        printf("I am also the new turnleader\n");
		}
		
		
	}
		
	else{
		///////////////////
		//NON-Leader node//
	    ///////////////////
		printf("I am not the previous TL\n Started votation step\n");
		//Accepting Votation msg from previous voting node
	    //sock_fd = net_server_init();
	    newsock_fd = net_accept_client_timeout(sock_fd, &timeout, NULL);
	
	    if(newsock_fd<0){
		    printf("Votation: Error in accepting client\n");
		    return 0;
	    }
	
	    r_vot = read(newsock_fd, &vote_msg, sizeof(votation_t));
	    if(r_vot<0){
		    printf("Error in reading from socket\n");
		    return 0;
	    }
		close(newsock_fd);
		//Do my votation
		vote_do_votation(&vote_msg);
		printf("I voted\n");
		
		//Connecting to next node's server
	    next_id = iptab_get_next( my_id );
	    strcpy(next_ip, iptab_getaddr(next_id));
	    send_sock_fd = net_client_connection_timeout(next_ip, &timeout);
	    if(send_sock_fd <0){
		    printf("Votation: Error in connecting to server\n");
		    return 0;
	    }
	
	    //Sending votation msg to next node
	    w_vot = write(send_sock_fd, &vote_msg, sizeof(votation_t));
	    if(w_vot<0){
		    printf("Votation: Error in writing to socket\n");
		    return 0;
	    }
		//close(sock_fd);
	    //close(newsock_fd);
	    close(send_sock_fd);
            printf("I sent votation msg\n");
		
	}
	
	///////////////////////////////*********************///////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////********************///////////////////////////////////////////////////////////
	
	
	
	//--------------------------------------
    //  THIRD:: Checking The Current TurnLeader 
    //--------------------------------------
	
	//Check if I am not the new turnLeader
	if(turnLeader_FLAG ==0){
            printf("I am checking the winner turnLeader\n");
	    //Accepting Normal msg
	    msg_init(&norm_msg);
	    //sock_fd = net_server_init();
	    newsock_fd = net_accept_client_timeout(sock_fd, &timeout, NULL);
	
	    if(newsock_fd<0){
		    printf("Checking: Error in accepting client\n");
		    return 0;
	    }
	
	    r_nor = read(newsock_fd, &norm_msg, sizeof(message_t));
	    if(r_nor<0){
	        printf("Checking: Error in reading from socket\n");
	        return 0;
	    }
	    //printf("I am not turnLeader\n");
	    turnLeader = msg_get_turnLeader(&norm_msg );
		printf("The new turnLeader: %d\n",turnLeader);
		//close(sock_fd);
	    close(newsock_fd);
	}
	
	///////////////////////////////*********************///////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////********************///////////////////////////////////////////////////////////
	
	
	//--------------------------------------
    //  Third:: Starting The Telephone Loop
    //--------------------------------------


	
	//I AM THE CURRENT TURNLEADER //
	
	if(my_id == turnLeader){
		turnLeader_FLAG = 1;
		printf("I am LEADER: Started the turn\n");
		counter =0;
		//Repeat 10 times
		for(int j=0; j<10; j++){
			
			//message_t turn_msg;
			msg_init(&norm_msg);
			msg_set_ids( & norm_msg, my_id, my_id );
			
			// Mark the unavailable nodes as visited
			for(int g = 0; g<n_nodes; g++){
				if(iptab_is_available(g) == 0){
					msg_mark(&norm_msg, g);
				}	
			}
			// Mark myself as visited
			msg_mark(&norm_msg, my_id);
			
			// Choose a next random node
			next_id = msg_rand(&norm_msg);
			printf("Next id is: %d\n", next_id);
			
			//Set the sending time
			msg_set_sent(&norm_msg);
			
			// Save the sending time for later use
			sending_times[j][0] = msg_get_sent(&norm_msg);
			
			// Send the msg to the random node
			
			//******//
			//Connecting to next node's server
	        strcpy(next_ip, iptab_getaddr(next_id));
	        send_sock_fd = net_client_connection_timeout(next_ip, &timeout);
	        if(send_sock_fd <0){
		        printf("Error in connecting to server\n");
		        return 0;
	        }
	
	        //Sending norm msg to next node
	        w_nor = write(send_sock_fd, &norm_msg, sizeof(message_t));
	        if(w_nor<0){
		        printf("Error in writing to socket\n");
		        return 0;
	        }
	        
	        close(send_sock_fd);
                printf("Sent the first msg as a Leader\n");
			//*****//
			
			// Receive as many number of messages as there are available nodes -1 (myself)
			//sock_fd = net_server_init();
			
			for(int h=1; h< available_nodes; h++){
				//Accept the message from the previous node
			    msg_init(&norm_msg);
	            
				//******//
				//Accept the connection 
	            newsock_fd = net_accept_client_timeout(sock_fd, &timeout, NULL);
	
	            if(newsock_fd<0){
		            printf("Checking: Error in accepting client\n");
		            return 0;
	            }
	            
				// Read the mesg
	            r_nor = read(newsock_fd, &norm_msg, sizeof(message_t));
	            if(r_nor<0){
	                printf("Checking: Error in reading from socket\n");
	                return 0;
	            }
				//******//
				
				// Update info stored in the received msg
				sending_times[h][counter] = msg_get_sent(&norm_msg);
				receiving_times[h][counter] = msg_get_recv(&norm_msg);
				
				close(newsock_fd);
				
				counter++;
			}
			//close(sock_fd);
			
			//**// Compute Statistics
	        
			
			
		}
		
		
	
	}
	
	
	
	////****************************////
	//I AM NOT THE CURRENT TURNLEADER //
	////****************************////
	else{
		printf("Normal NODE: Starting\n");
		//Update msg info and sending it
		msg_set_recv(&norm_msg);
		msg_mark(&norm_msg, my_id);
		
		//Check if all nodes have been visited
		if(msg_all_visited(&norm_msg)){
			printf("All nodes are visited\n");
			//If yes, Send the message only to the turnLeader
			msg_set_ids(&norm_msg, my_id, turnLeader);
			msg_set_sent(&norm_msg);
			
			//Connecting to next node's server
	        strcpy(next_ip, iptab_getaddr(turnLeader));
	        send_sock_fd = net_client_connection_timeout(next_ip, &timeout);
	        if(send_sock_fd <0){
		        printf("Error in connecting to server\n");
		        return 0;
	        }
	
	        //Sending norm msg to next node
	        w_nor = write(send_sock_fd, &norm_msg, sizeof(message_t));
	        if(w_nor<0){
		        printf("Error in writing to socket\n");
		        return 0;
	        }
	        printf("I sent the mesg to turnleader\n");
	        close(send_sock_fd);
		}
		else{
			//If no, Send the message twice; once for next random node and once to the turnLeader
			
			next_id = msg_rand(&norm_msg);
			
			printf("Next id is: %d\n", next_id);
			msg_set_ids(&norm_msg, my_id, turnLeader);
			msg_set_sent(&norm_msg);
			
			//Connecting to next node's server
	        strcpy(next_ip, iptab_getaddr(next_id));
	        send_sock_fd = net_client_connection_timeout(next_ip, &timeout);
	        if(send_sock_fd <0){
		        printf("Error in connecting to server\n");
		        return 0;
	        }
	
	        //Sending norm msg to next node
	        w_nor = write(send_sock_fd, &norm_msg, sizeof(message_t));
	        if(w_nor<0){
		        printf("Error in writing to socket\n");
		        return 0;
	        }
	        
	        close(send_sock_fd);
			
			//Connecting to turnLeader's server
	        strcpy(next_ip, iptab_getaddr(turnLeader));
	        send_sock_fd = net_client_connection_timeout(next_ip, &timeout);
	        if(send_sock_fd <0){
		        printf("Error in connecting to server\n");
		        return 0;
	        }
	
	        //Sending norm msg to turnLeader node
	        w_nor = write(send_sock_fd, &norm_msg, sizeof(message_t));
	        if(w_nor<0){
		        printf("Error in writing to socket\n");
		        return 0;
	        }
			
	        close(send_sock_fd);
		}
		
		//Repeat 9 times
		for(int k=0; k<9; k++){
			
			//Accept the message from the previous node
			msg_init(&norm_msg);
	        //sock_fd = net_server_init();
	        newsock_fd = net_accept_client_timeout(sock_fd, &timeout, NULL);
	
	        if(newsock_fd<0){
		        printf("Checking: Error in accepting client\n");
		        return 0;
	        }
	
	        r_nor = read(newsock_fd, &norm_msg, sizeof(message_t));
	        if(r_nor<0){
	            printf("Checking: Error in reading from socket\n");
	            return 0;
	        }
			//close(sock_fd);
	        close(newsock_fd);
			
			//Update msg info and sending it
		    //msg_init(&norm_msg);
		    msg_set_recv(&norm_msg);
		    msg_mark(&norm_msg, my_id);
		
		    //Check if all nodes have been visited
		    if(msg_all_visited(&norm_msg)){
			    printf("All nodes are visited\n");
			    //If yes, Send the message only to the turnLeader
			    msg_set_ids(&norm_msg, my_id, turnLeader);
			    msg_set_sent(&norm_msg);
			    //Connecting to next node's server
	            strcpy(next_ip, iptab_getaddr(turnLeader));
	            send_sock_fd = net_client_connection_timeout(next_ip, &timeout);
	            if(send_sock_fd <0){
		            printf("Error in connecting to server\n");
		            return 0;
	            }
	
	            //Sending norm msg to next node
	            w_nor = write(send_sock_fd, &norm_msg, sizeof(message_t));
	            if(w_nor<0){
		            printf("Error in writing to socket\n");
		            return 0;
	            }
	            
	            close(send_sock_fd);
                    printf("Sent msg to the turnLaeder\n");
		    }
		    else{
			    //If no, Send the message twice; once for next random node and once to the turnLeader
			
			    next_id = msg_rand(&norm_msg);
				printf("Next id is: %d\n", next_id);
			    msg_set_ids(&norm_msg, my_id, turnLeader);
			    msg_set_sent(&norm_msg);
			
			    //Connecting to next node's server
	            strcpy(next_ip, iptab_getaddr(next_id));
	            send_sock_fd = net_client_connection_timeout(next_ip, &timeout);
	            if(send_sock_fd <0){
		            printf("Error in connecting to server\n");
		            return 0;
	            }
	
	            //Sending norm msg to next node
	            w_nor = write(send_sock_fd, &norm_msg, sizeof(message_t));
	            if(w_nor<0){
		            printf("Error in writing to socket\n");
		            return 0;
	            }
	            
	            close(send_sock_fd);
                    printf("Sent msg to the next random node, with id: %d\n", next_id);
			
			    //Connecting to turnLeader's server
	            strcpy(next_ip, iptab_getaddr(turnLeader));
	            send_sock_fd = net_client_connection_timeout(next_ip, &timeout);
	            if(send_sock_fd <0){
		            printf("Error in connecting to server\n");
		            return 0;
	            }
	
	            //Sending norm msg to turnLeader node
	            w_nor = write(send_sock_fd, &norm_msg, sizeof(message_t));
	            if(w_nor<0){
		            printf("Error in writing to socket\n");
		            return 0;
	            }
			    
	            close(send_sock_fd);
                    printf("Sent msg to the current turnLeader %d\n",turnLeader);
		    }	
			
		}
		
		
	
	}
	
	
	
	}
	
	
	
	return 0;
}
