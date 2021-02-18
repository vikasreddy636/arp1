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
#include "offset.h"
#include "arpnet.h"



char my_ip[] = "127.0.0.1";
struct timeval timeout;
timeout.tv_sec = 10;
timeout.tv_usec = 0;
int turnLeader_FLAG = 0;
int turnLeader = 0;


int main(){
	//Declaring variables to be used later //
	int my_id = iptab_get_ID_of(my_ip);
	int r_hsh, w_hsh, r_vot, w_vot, r_nor, w_nor;
	int sock_fd, newsock_fd, send_sock_fd, check1
	char next_ip[20];
	votation_t vote_msg;
	node_id winner_id;
	message_t norm_msg;
	
	
	//check I am the first node or not
	int znode = hsh_imfirst(my_ip);
	
	if(znode == 1){
		turnLeader_FLAG = 1;
	}
	
	if(znode==0){

	//--------------------------------------
    //  HANDSHAKE FIRST STEP
    //--------------------------------------
	
	handshake_t hsh_msg;
	hsh_init(&hsh_msg);
	
	//Accepting handshake msg from previous node
	sock_fd = net_server_init();
	newsock_fd = net_accept_client_timeout(sockfd, &timeout, NULL);
	
	if(newsock_fd<0){
		printf("Error in accepting client\n");
		return 0;
	}
	
	r_hsh = read(newsockfd, &hsh_msg, sizeof(handshake_t));
	if(r_hsh<0){
		printf("Error in reading from socket\n");
		return 0;
	}
	
	//Checking my version and availability
	check1 = hsh_check_availability(my_id, &hsh_msg );
	
	
	//Connecting to next node's server
	
	next_ip = iptab_getaddr(my_id + 1);
	send_sock_fd = net_client_connection_timeout(next_ip, &timeout);
	if(send_sock_fd <0){
		printf("Error in connecting to server\n");
		return 0;
	}
	
	//Sending handshake msg to next node
	w_hsh = write(send_sock_fd, &hsh_msg, sizeof(handshake_t));
	if(w_hsh<0){
		printf("Error in writing to socket\n");
		return 0;
	}
	
	close(sock_fd);
	close(newsock_fd);
	close(send_sock_fd);
	
	if(check1 ==0){
		printf("version is incorrect\n");
		return 0;
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////
	
	
	//--------------------------------------
    //  HANDSHAKE SECOND STEP
    //--------------------------------------
	
	
	//Accepting handshake msg from previous node
	sock_fd = net_server_init();
	newsock_fd = net_accept_client_timeout(sockfd, &timeout, NULL);
	
	if(newsock_fd<0){
		printf("Error in accepting client\n");
		return 0;
	}
	
	r_hsh = read(newsockfd, &hsh_msg, sizeof(handshake_t));
	if(r_hsh<0){
		printf("Error in reading from socket\n");
		return 0;
	}
	
	//Updating ip table
	hsh_update_iptab( &hsm )
	
	
	//Connecting to next node's server
	next_id = iptab_get_next( my_id );
	next_ip = iptab_getaddr(next_id);
	send_sock_fd = net_client_connection_timeout(next_ip, &timeout);
	if(send_sock_fd <0){
		printf("Error in connecting to server\n");
		return 0;
	}
	
	//Sending handshake msg to next node
	w_hsh = write(send_sock_fd, &hsh_msg, sizeof(handshake_t));
	if(w_hsh<0){
		printf("Error in writing to socket\n");
		return 0;
	}
	
	
	close(sock_fd);
	close(newsock_fd);
	close(send_sock_fd);
	}
	
	///////////////////////////////*********************///////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////********************///////////////////////////////////////////////////////////
	
	
	
	
	
	//Repeat 10 times	
	for(int i=0; i<10; i++){
	
	//--------------------------------------
    //  FIRST:: VOTATION STEP
    //--------------------------------------

	vote_init(&vote_msg);
	
	
	if(turnLeader_FLAG == 1){
		//I am the previous turn leader
		turnLeader_FLAG = 0;
		
		vote_do_votation(&vote_msg);
			
		//Connecting to next node's server
	    next_id = iptab_get_next( my_id );
	    next_ip = iptab_getaddr(next_id);
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
		
		
		
		//Accepting Votation msg from last voting node
	    sock_fd = net_server_init();
	    newsock_fd = net_accept_client_timeout(sockfd, &timeout, NULL);
	
	    if(newsock_fd<0){
		    printf("Votation: Error in accepting client\n");
		    return 0;
	    }
	
	    r_vot = read(newsockfd, &vote_msg, sizeof(votation_t));
	    if(r_vot<0){
		    printf("Error in reading from socket\n");
		    return 0;
	    }
		
		close(sock_fd);
	    close(newsock_fd);
	    close(send_sock_fd);
		
		//Getting the winner and informing him 
		winner_id = vote_getWinner( &vote_msg );
		
		if(winner_id != my_id){
			
			msg_init(&norm_msg);
			msg_set_ids( &norm_msg, my_id , winner_id );
			
			//Connecting to winner node's server
	        next_ip = iptab_getaddr(winner_id);
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
		}
		else{
			turnLeader_FLAG = 1;
			turnLeader = winner_id;
		}
		
		
	}
		
	else{
		///////////////////
		//NON-Leader node//
	    ///////////////////
		
		//Accepting Votation msg from previous voting node
	    sock_fd = net_server_init();
	    newsock_fd = net_accept_client_timeout(sockfd, &timeout, NULL);
	
	    if(newsock_fd<0){
		    printf("Votation: Error in accepting client\n");
		    return 0;
	    }
	
	    r_vot = read(newsockfd, &vote_msg, sizeof(votation_t));
	    if(r_vot<0){
		    printf("Error in reading from socket\n");
		    return 0;
	    }
		
		//Do my votation
		vote_do_votation(&vote_msg);
		
		
		//Connecting to next node's server
	    next_id = iptab_get_next( my_id );
	    next_ip = iptab_getaddr(next_id);
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
		close(sock_fd);
	    close(newsock_fd);
	    close(send_sock_fd);
		
	}
	
	//--------------------------------------
    //  SECOND:: Checking TurnLeader STEP
    //--------------------------------------
	
	//Check if I am not the new turnLeader
	if(turnLeader_FLAG ==0){
	    //Accepting Normal msg
	    msg_init(&norm_msg);
	    sock_fd = net_server_init();
	    newsock_fd = net_accept_client_timeout(sockfd, &timeout, NULL);
	
	    if(newsock_fd<0){
		    printf("Checking: Error in accepting client\n");
		    return 0;
	    }
	
	    r_nor = read(newsockfd, &norm_msg, sizeof(message_t));
	    if(r_nor<0){
	        printf("Checking: Error in reading from socket\n");
	        return 0;
	    }
	
	    turnLeader = msg_get_turnLeader(&norm_msg );
		
		close(sock_fd);
	    close(newsock_fd);
	}
	
	
	
	////************************////
	//I AM THE CURRENT TURNLEADER //
	////************************////
	if(my_id == turnLeader){
		turnLeader_FLAG = 1;
		
		//Repeat 10 times
		for(int j=0; j<10; j++){
			
			//** TO BE COMPLETED **//
			
			
			
			
		}
		
		
	
	}
	
	
	
	////****************************////
	//I AM NOT THE CURRENT TURNLEADER //
	////****************************////
	else{
		
		//Update msg info and sending it
		msg_set_recv(&norm_msg);
		msg_mark(&norm_msg, my_id);
		
		//Check if all nodes have been visited
		if(msg_all_visited(&norm_msg)){
			
			//If yes, Send the message only to the turnLeader
			msg_set_ids(&norm_msg, my_id, turnLeader);
			msg_set_sent(&norm_msg);
			//Connecting to next node's server
	        next_ip = iptab_getaddr(turnLeader);
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
		}
		else{
			//If no, Send the message twice; once for next random node and once to the turnLeader
			
			next_id = msg_rand(&norm_msg);
			msg_set_ids(&norm_msg, my_id, turnLeader);
			msg_set_sent(&norm_msg);
			
			//Connecting to next node's server
	        next_ip = iptab_getaddr(next_id);
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
	        next_ip = iptab_getaddr(turnLeader);
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
	        sock_fd = net_server_init();
	        newsock_fd = net_accept_client_timeout(sockfd, &timeout, NULL);
	
	        if(newsock_fd<0){
		        printf("Checking: Error in accepting client\n");
		        return 0;
	        }
	
	        r_nor = read(newsockfd, &norm_msg, sizeof(message_t));
	        if(r_nor<0){
	            printf("Checking: Error in reading from socket\n");
	            return 0;
	        }
			close(sock_fd);
	        close(newsock_fd);
			
			//Update msg info and sending it
		    //msg_init(&norm_msg);
		    msg_set_recv(&norm_msg);
		    msg_mark(&norm_msg, my_id);
		
		    //Check if all nodes have been visited
		    if(msg_all_visited(&norm_msg)){
			
			    //If yes, Send the message only to the turnLeader
			    msg_set_ids(&norm_msg, my_id, turnLeader);
			    msg_set_sent(&norm_msg);
			    //Connecting to next node's server
	            next_ip = iptab_getaddr(turnLeader);
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
		    }
		    else{
			    //If no, Send the message twice; once for next random node and once to the turnLeader
			
			    next_id = msg_rand(&norm_msg);
			    msg_set_ids(&norm_msg, my_id, turnLeader);
			    msg_set_sent(&norm_msg);
			
			    //Connecting to next node's server
	            next_ip = iptab_getaddr(next_id);
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
	            next_ip = iptab_getaddr(turnLeader);
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
			
		}
		
		
	
	}
	
	
	
	}
	
	
	
	return 0;
}