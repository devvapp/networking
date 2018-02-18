#include <assert.h>
#include <errno.h>
#include <libdill.h>
#include <stdio.h>
#include <string.h>

/*
* Author : Aneesh
* aget - implementation of wget. Network retriever and prints the data.
*/
int main(int argc, char *argv[]) {

    if(argc < 3 || argc > 4) {
        fprintf(stderr, "aget [protocol] [server] [optional resource]\n");
        return 1;
    }
    
    char *protocol = argv[1];
    char *server = argv[2];
    char *resource = NULL;

    if(argv[3] != NULL){
        resource = argv[3];
    } else {
        resource = "/";
    }

    int port;
    if(strcmp(protocol, "http") == 0) port = 80;
    else if(strcmp(protocol, "https") == 0) port = 443;
    else {
        fprintf(stderr, "Unsupported protocol.\n");
        return 1;
    }

    printf("Fetching IP...\n");
    //Query DNS Server for ip address
    struct ipaddr addr;
    int rc = ipaddr_remote(&addr, argv[2], port, 0, -1);
    if(rc != 0) {
        perror("Cannot resolve server addres");
        return 1;
    }

    //Create TCP connection
    int s = tcp_connect(&addr, -1);
    if(s < 0) {
        perror("Cannot connect to the remote server");
        return 1;
    }

    printf("Connection established...\n");

    //Attaching tls only if its an https connection - We are going to stack TLS protocal on top of underlying stack
    if(port == 443) {
        s = tls_attach_client(s, -1);
        assert(s >= 0);
    }

    //Attaching HTTP socket - We are going to stack HTTP protocal on top of underlying stack
    s = http_attach(s);
    assert(s >= 0);

    printf("Making request to server...\n"); 

    //Making a HTTP request
    rc = http_sendrequest(s, "GET", resource, -1);
    assert(rc == 0);   

    rc = http_sendfield(s, "Host", server, -1);
    assert(rc == 0);

    //We are asking server to close the connection after sending us the webpage
    rc = http_sendfield(s, "Connection", "close", -1);
    assert(rc == 0);

    //Tell the server we are done with the request and process it now.
    rc = hdone(s, -1);
    assert(rc == 0);

    //Reading the server status response
    char reason[256];
    rc = http_recvstatus(s, reason, sizeof(reason), -1);
    assert(rc >= 0);
    fprintf(stderr, "%s : %d : %s\n", "STATUS", rc, reason);

    fprintf(stderr, "\n");
    printf("Printing headers...\n");
    //Print the headers
    while(1) {
        char name[256];
        char value[256];
        rc = http_recvfield(s, name, sizeof(name), value, sizeof(value), -1);
        if(rc == -1 && errno == EPIPE) break;
        assert(rc == 0);
        fprintf(stderr, "%s: %s\n", name, value);
    }
    fprintf(stderr, "\n");

    //Detach HTTP socket
    s = http_detach(s, -1);
    assert(s >= 0);


    //Print the html page received
    while(1) {
        unsigned char c;
        rc = brecv(s, &c, 1, -1);
        if(rc == -1 && errno == EPIPE) break;
        //assert(rc == 0);
        fprintf(stdout, "%c", c);
    }
    fprintf(stderr, "\n");

    //Detach tls if an https connection
    if(port == 443) {
        s = tls_detach(s, -1);
        assert(s >= 0);
    }

    //Close TCP connection
    rc = tcp_close(s, now() + 1000);
    assert(rc == 0);
    printf("Connection closed...\n");

    return 0;
}


