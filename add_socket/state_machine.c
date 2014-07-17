/*
 * Purpose: 实现MESI的状态机，加入socket通信
 * Author: 赵庶林
 * Date: 2014/7/9
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

int server_sockfd;
int client_sockfd;
struct sockaddr_in server_addr;
struct sockaddr_in client_addr;
char send_buf[BUFSIZ];
char recv_buf[BUFSIZ];

typedef int State;
typedef int Condition;
typedef void (* Func)(State state, Condition condition);

#define STATES 5
#define M 0
#define E 1
#define S 2
#define I 3
#define STATE_ERROR 4

#define CONDITIONS 16
#define M_LR 0
#define M_LW 1
#define M_RR 2
#define M_RW 3

#define E_LR 4
#define E_LW 5
#define E_RR 6
#define E_RW 7

#define S_LR 8
#define S_LW 9
#define S_RR 10
#define S_RW 11

#define I_LR 12
#define I_LW 13
#define I_RR 14
#define I_RW 15

typedef struct NEXT_STEP
{
	State next_state;
	Func func;
}NEXT_STEP;

//定义状态跳转之后被触发函数
void func_1(State state, Condition condition)
{
	printf("func_1: (1) p <- my_cache.\n");
}

void func_2(State state, Condition condition)
{
	printf("func_2: (1) p -> my_cache.\n");
}

void func_3(State state, Condition condition)
{
	printf("func_3: (1) memory <- my_cache; (2) memory -> other_cache; (3) p <- other_cache.\n");
}

void func_4(State state, Condition condition)
{
	printf("func_4: (1) memory <- my_cache; (2) p -> other_cache.\n");
}

void func_5(State state, Condition condition)
{
	printf("func_5: (1) p <- my_cache.\n");
}

void func_6(State state, Condition condition)
{
	printf("func_6: (1) p -> my_cache.\n");
}

void func_7(State state, Condition condition)
{
	printf("func_7: (1) memory -> other_cache; (2) p <- other_cache.\n");
}

void func_8(State state, Condition condition)
{
	printf("func_8: (1) p -> other_cache.\n");
}

void func_9(State state, Condition condition)
{
	printf("func_9: (1) p <- my_cache.\n");
}

void func_10(State state, Condition condition)
{
	printf("func_10: (1) p -> my_cache.\n");
}

void func_11(State state, Condition condition)
{
	printf("func_11: (1) p <- other_cache.\n");
}

void func_12(State state, Condition condition)
{
	printf("func_12: (1) p -> other_cache.\n");
}

void func_13(State state, Condition condition)
{
	printf("func_13: (1) other_cache -> memory; (2) my_cache <- memory; (3) p <- my_cache.\n");
}

void func_14(State state, Condition condition)
{
	printf("func_14: (1) other_cache -> memory; (2) p -> my_cache.\n");
}

void func_15(State state, Condition condition)
{
	printf("func_15: (1) p <- other_cache.\n");
}

void func_16(State state, Condition condition)
{
	printf("func_16: (1) other_cache -> memory; (2) p -> other_cache.\n");
}

void func_17(State state, Condition condition)
{
	printf("func_17: (1) memory -> my_cache; (2) p <- my_cache.\n");
}

void func_18(State state, Condition condition)
{
	printf("func_18: (1) p -> my_cache.\n");
}

void func_19(State state, Condition condition)
{
	printf("func_19: (1) p <- other_cache.\n");
}

void func_20(State state, Condition condition)
{
	printf("func_20: (1) p -> other_cache.\n");
}

void func_21(State state, Condition condition)
{
	printf("func_21: (1) memory -> my_cache; (2) my_cache -> p.\n");
}

void func_22(State state, Condition condition)
{
	printf("func_22: (1) p -> my_cache.\n");
}

void func_23(State state, Condition condition)
{
	printf("func_23: (1) memory -> other_cache; (2) other_cache -> p.\n");
}

void func_24(State state, Condition condition)
{
	printf("func_24: (1) p -> other_cache.\n");
}

void func_handle_error(State state, Condition condition)
{
	printf("should do sth to handle error!\n");
}


//关联下一跳状态和被触发函数
struct NEXT_STEP n1 = {
	M,
	func_1
};

struct NEXT_STEP n2 = {
	M,
	func_2
};

struct NEXT_STEP n3 = {
	S,
	func_3
};

struct NEXT_STEP n4 = {
	I,
	func_4
};

struct NEXT_STEP n5 = {
	E,
	func_5
};

struct NEXT_STEP n6 = {
	M,
	func_6
};

struct NEXT_STEP n7 = {
	S,
	func_7
};

struct NEXT_STEP n8 = {
	I,
	func_8
};

struct NEXT_STEP n9 = {
	S,
	func_9
};

struct NEXT_STEP n10 = {
	M,
	func_10
};

struct NEXT_STEP n11 = {
	S,
	func_11
};

struct NEXT_STEP n12 = {
	I,
	func_12
};

struct NEXT_STEP n13 = {
	S,
	func_13
};

struct NEXT_STEP n14 = {
	M,
	func_14
};

struct NEXT_STEP n15 = {
	I,
	func_15
};

struct NEXT_STEP n16 = {
	I,
	func_16
};

struct NEXT_STEP n17 = {
	S,
	func_17
};

struct NEXT_STEP n18 = {
	M,
	func_18
};

struct NEXT_STEP n19 = {
	I,
	func_19
};

struct NEXT_STEP n20 = {
	I,
	func_20
};

struct NEXT_STEP n21 = {
	E,
	func_21
};

struct NEXT_STEP n22 = {
	M,
	func_22
};

struct NEXT_STEP n23 = {
	I,
	func_23
};

struct NEXT_STEP n24 = {
	I,
	func_24
};

struct NEXT_STEP n0 = {
	STATE_ERROR,
	func_handle_error
};


//定义跳转表
struct NEXT_STEP *Jump_table[STATES][CONDITIONS] = {
	&n0, &n0, &n0, &n0, &n0, &n0, &n0, &n0, &n0, &n0, &n0, &n0, &n1, &n2, &n3, &n4,
	&n0, &n0, &n0, &n0, &n0, &n0, &n0, &n0, &n0, &n0, &n0, &n0, &n5, &n6, &n7, &n8,
	&n0, &n0, &n0, &n0, &n0, &n0, &n0, &n0, &n9, &n10, &n11, &n12, &n0, &n0, &n0, &n0,
	&n13, &n14, &n15, &n16, &n17, &n18, &n19, &n20, &n0, &n0, &n0, &n0, &n21, &n22, &n23, &n24,

};

//输入：条件流
//输出：下一个状态+调用被触发函数，直到STATE_ERROR
void roll(void)
{
	Condition condition;
	State current_state = I, next_state = -1; //初始状态为I
	
	while(current_state != STATE_ERROR)
	{
		printf("current_state is %d.\n Please type the next condition with Enter:", current_state);
		scanf("%d", &condition);

		(*(Jump_table[current_state][condition] ->func))(current_state, condition);
		current_state = Jump_table[current_state][condition] -> next_state;
		printf("condition is %d, and after jump, now the current_state is %d.\n", condition, current_state);
	}
}

int create_a_socket(void)
{
	int on;
	int sockaddr_in_size;

	memset(&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(8000);

	if((server_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error!\n");
		exit(EXIT_FAILURE);
	}
	printf("Now socked has been created successfully.\n");

	on = 1;
	if((setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) 
	{
		perror("setsockopt error!\n");
		exit(EXIT_FAILURE);
	}

	if((bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))) < 0)
	{
		perror("bind error!\n");
		exit(EXIT_FAILURE);
	}
	printf("Now bind succeessfully.\n");

	printf("Now start to listen...\n");
	listen(server_sockfd, 25);

	sockaddr_in_size = sizeof(struct sockaddr_in);
	if((client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &sockaddr_in_size)) < 0)
	{
		perror("accept error!\n");
		exit(EXIT_FAILURE);
	}

	 printf("Server has accepted client, whose IP is %s.\n", inet_ntoa(client_addr.sin_addr));

	 return 0;
}

int send_my_cache_state_to_another_cache(State state)
{
	memset(send_buf, state, sizeof(State));
	send(client_sockfd, send_buf, BUFSIZ, 0);
	return 0;
}

int recv_another_cache_state(void)
{
	memset(recv_buf, 0x00, sizeof(recv_buf));
	recv(client_sockfd, recv_buf, BUFSIZ, 0);
	return 0;
}

int main(void)
{
	printf("Testing...\n");
	create_a_socket();

	roll();
	return 0;
}
