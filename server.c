//3EP4-43 MizukiFurusawa
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

int vals[32];
int vals_len = 0;
char exps[32];
int exps_len = 0;
void push_value(int exp);
void pop_value(int *exp);
void push_exp(char exp);
void pop_exp(char *exp);
int calc(char *str);
int exec_calc(char exp, int val1, int val2);
int val_strlen(char *str);

#define PORT 9876

int main() {
	int i;
	int result;
	int srcSocket;
	int dstSocket;
	struct sockaddr_in srcAddr;
	struct sockaddr_in dstAddr;
	int dstAddrSize = sizeof(dstAddr);
	int status;
	int numrcv;
	char buf[1024];

	while (1) {
		bzero((char *)&srcAddr, sizeof(srcAddr));
		srcAddr.sin_port = htons(PORT);
		srcAddr.sin_family = AF_INET;
		srcAddr.sin_addr.s_addr = INADDR_ANY;
		srcSocket = socket(AF_INET, SOCK_STREAM, 0);
		bind(srcSocket, (struct sockaddr *)&srcAddr, sizeof(srcAddr));
		listen(srcSocket, 1);
		printf("接続を待っています\nクライアントプログラムを動かして下さい\n");
		dstSocket = accept(srcSocket, (struct sockaddr *)&dstAddr, &dstAddrSize);
		printf("%s から接続を受けました\n", (char *)inet_ntoa(dstAddr.sin_addr));
		close(srcSocket);

		while (1) {
			numrcv = read(dstSocket, buf, 1024);
			if (numrcv == 0 || numrcv == -1) {
				close(dstSocket); break;
			}
			printf("受信した文字列 %s\n", buf);
			result = calc(buf);
			printf("送信する整数値 %d\n", result);
			sprintf(buf, "%d", result);
			write(dstSocket, buf, 1024);
			printf("結果を送信しました.\n");
		}
	}
	return(0);
}


int calc(char *str) {
	int result;
	char *pos;
	int ret;
	int len;
	int tmp, val1, val2;
	char exp;

	pos = str;
	while (*pos) {
		switch (*pos) {
		case '(':
			push_exp(*pos);
			pos++;
			break;
		case ')':
			while (0 < exps_len) {
				pop_exp(&exp);
				if (exp == '(') {
					break;
				}
				pop_value(&val2);
				pop_value(&val1);
				ret = exec_calc(exp, val1, val2);
				push_value(ret);
			}
			pos++;
			break;
		case '*':
		case '/':
		case '+':
		case '-':
			push_exp(*pos);
			pos++;
			break;
		case ' ':
			pos++;
			break;
		default:
			if (isdigit((int)*pos)) {
				len = val_strlen(pos);
				tmp = atoi(pos);
				push_value(tmp);
				if (exps_len) {
					if ((exps[exps_len - 1] == '*') || (exps[exps_len - 1] == '/')) {
						pop_value(&val2);
						pop_value(&val1);
						pop_exp(&exp);
						ret = exec_calc(exp, val1, val2);
						push_value(ret);
					}
				}
				pos += len;
				continue;
			}
			else {
				fprintf(stderr, "syntax error [%c]\n", *pos);
				return -1;
			}
		}
	}

	while (0 < exps_len) {
		pop_exp(&exp);
		if (exp == '(') {
			continue;
		}
		pop_value(&val2);
		pop_value(&val1);
		ret = exec_calc(exp, val1, val2);
		push_value(ret);
	}
	pop_value(&result);
	return result;
}

int exec_calc(char exp, int val1, int val2) {
	int result;

	switch (exp) {
	case '+':
		result = val1 + val2;
		break;
	case '-':
		result = val1 - val2;
		break;
	case '*':
		result = val1 * val2;
		break;
	case '/':
		result = val1 / val2;
		break;
	default:
		fprintf(stderr, "Unkown exp [%c]\n", exp);
		break;
	}
	return result;
}

int val_strlen(char *str) {
	int len = 0;
	while (isdigit((int)*str)) {
		len++;
		str++;
	}
	return len;
}

void push_value(int val) {
	vals[vals_len] = val;
	vals_len++;
	return;
}

void pop_value(int *val) {
	*val = vals[vals_len - 1];
	vals_len--;
	return;
}


void push_exp(char exp) {
	exps[exps_len] = exp;
	exps_len++;
	return;
}

void pop_exp(char *exp) {
	*exp = exps[exps_len - 1];
	exps_len--;
	return;
}
