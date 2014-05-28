#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <resolv.h>
#include <sys/epoll.h>
#include <list>
#include <set>

#define EPOLL_SIZE 10000
#define EPOLL_RUN_TIMEOUT -1
#define LENGTH 100 //максимальная длина запроса

using namespace std;

list<int> clients_list;
set<string> clients_nicks;

typedef struct {
	char a[100];
} char2send;

char2send ftoa (double num) {
	int ans = (int) num;
	int line1[100];
	char2send line2;
	int i = 0, j = 0;
	if (ans < 0) {
		line2.a[0] = '-';
		++j;
		ans *= -1;
		num *= -1.0;
	}
	while (ans/10 > 0) {
		line1[i] = ans % 10;
		ans = ans/10;
		++i;
	}
	int len = i;
	line1[i] = ans;
	for (i = 0; i <= len; ++i) 
		line2.a[j + i] = line1[len - i] + '0';
	line2.a[j + i] = '.';
	len = j + i + 1;
	num = num - (double) ans;
	for (i = 0; i < 2; ++i) {
		num *= 10;
		line2.a[len + i] = '0' + ((int) num % 10);
	}
	line2.a[len + i] = '\0';
	return line2;
}

char2send itoa (int ans) {
	char2send line;
	char line1[10];
	int i = 0;
	while (ans/10 > 0) {
                line1[i] = ans % 10;
                ans = ans/10;
                ++i;
        }
	line1[i] = ans;
	int len = i;
	for (i = 0; i <= len; ++i)
                line.a[i] = line1[len - i] + '0';
	return line;
}

double count (char *task) {
	char a[LENGTH];
	int b[LENGTH];
	int k, i = 0, j = 0;
	for (k = 0; k < strlen(task); ++k) {
		if (task[k] == '(') {
			a[i] = '(';
			++i;
		}
		if (task[k] == ')') {
			a[i] = ')';
			++i;
		}
		if (task[k] == '+') {
			a[i] = '+';
			++i;
		}
		if (task[k] == '-') {
			a[i] = '-';
			++i;
		}
		if (task[k] == '/') {
			a[i] = '/';
			++i;
		}
		if (task[k] == '*') {
			a[i] = '*';
			++i;
		}
		if ((task[k] >='0') && (task[k] <= '9')) {
			int tmp = task[k] - '0';
			++k;
			while ((task[k] >='0') && (task[k] <= '9'))	{
				tmp = 10 * tmp + task[k] - '0';
				++k;
			}
			--k;
			b[j] = tmp;
			++j;
			a[i] = 'a';
			++i;
		}
	}
	int aLen = i, bLen = j;
	

	char c[LENGTH], stek[LENGTH];
	double ans[LENGTH];
	int lStek = 0;
	j = 0, k = 0; 
	int p = 0;
	for (i = 0; i < aLen; ++i) {
		if (a[i] == 'a') {
			c[j] = 'a';
			++j;
			ans[k] = b[p];
			++k;
			++p;
		}
		if ((a[i] == '+') || (a[i] == '-') || (a[i] == '*') || (a[i] == '/')) {
			if (lStek > 0) {
				--lStek;
				while ((stek[lStek] == '*') || (stek[lStek] == '/') || (((stek[lStek] == '+') || (stek[lStek] == '-')) && ((a[i] == '+') || (a[i] == '-')))) {
					if (stek[lStek] == '+')
						ans[k - 2] = ans[k - 2] + ans [k - 1];
					if (stek[lStek] == '-')
						ans[k - 2] = ans[k - 2] - ans [k - 1];
					if (stek[lStek] == '*')
						ans[k - 2] = ans[k - 2] * ans [k - 1];
					if (stek[lStek] == '/')
						ans[k - 2] = ans[k - 2] / ans [k - 1];
					--k;
					c[j] = stek[lStek];
					++j;
					--lStek;
				}
				++lStek;
			}
			stek[lStek] = a[i];
			++lStek;
		}
		if (a[i] == '(') {
			stek[lStek] = a[i];
			++lStek;
		}
		if (a[i] == ')') {
			--lStek;
			while (stek[lStek] != '(') {
				if (lStek == -1) {
					printf ("Wrong expression!\n");
					return -1;
				}
				if (stek[lStek] == '+')
					ans[k - 2] = ans[k - 2] + ans [k - 1];
				if (stek[lStek] == '-')
					ans[k - 2] = ans[k - 2] - ans [k - 1];
				if (stek[lStek] == '*')
					ans[k - 2] = ans[k - 2] * ans [k - 1];
				if (stek[lStek] == '/')
					ans[k - 2] = ans[k - 2] / ans [k - 1];
				--k;
				c[j] = stek[lStek];
				++j;
				--lStek;
			}
		}
	}
	--lStek;
	while (lStek >= 0) {
		if (stek[lStek] == '+')
			ans[k - 2] = ans[k - 2] + ans [k - 1];
		if (stek[lStek] == '-')
			ans[k - 2] = ans[k - 2] - ans [k - 1];
		if (stek[lStek] == '*')
			ans[k - 2] = ans[k - 2] * ans [k - 1];
		if (stek[lStek] == '/')
			ans[k - 2] = ans[k - 2] / ans [k - 1];
		--k;
		c[j] = stek[lStek];
		++j;
		--lStek;
	}
	return ans[0];
}

int main () {

	int sockfd, newsockfd;
	int cllen;

	struct sockaddr_in servaddr, claddr;
	sockfd = socket (AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
        perror ("1");
		return -1;
	}
	bzero (&servaddr, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (51000);
	servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	
	if (bind (sockfd, (struct sockaddr *) &servaddr, sizeof (servaddr)) < 0) {
        perror ("2");
		close (sockfd);
		return -1;
	}
	if (listen (sockfd, 5) < 0) {
        perror ("3");
		close (sockfd);
		return -1;
	}
		
	int n;
	
	static struct epoll_event ev, events[EPOLL_SIZE];
	ev.events = EPOLLIN | EPOLLET;
	int epfd, epoll_events_count;
	epfd = epoll_create(EPOLL_SIZE);
	ev.data.fd = sockfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
	
	while (1) {
		epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, EPOLL_RUN_TIMEOUT);
		
		int i;
		for (i = 0; i < epoll_events_count; i++) {
			if(events[i].data.fd == sockfd) {
				cllen = sizeof (claddr);
                if ((newsockfd = accept (sockfd, (struct sockaddr *) &claddr, (socklen_t*)&cllen)) < 0) {
                    perror ("4");
					close (sockfd);
					return -1;
				}
				ev.data.fd = newsockfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, newsockfd, &ev);
                clients_list.push_back(newsockfd);
			}
			else {
				char line[1000];
				bzero(line, 1000);
				if ((n = read(events[i].data.fd, line, 999)) < 0) {
                    perror ("5");
					close (events[i].data.fd);
					return -1;
				}
				//printf("%s\n", line);
				if (bcmp (line, "exit:", 5) == 0) {
                    /*if ((n = write(events[i].data.fd, "echo:close%chat", strlen("echo:close%chat") + 1)) < 0)
                        perror("8");*/
					list<int>::iterator it;
					for(it = clients_list.begin(); it != clients_list.end(); it++)
						if (*it != events[i].data.fd)
							write(*it, line, strlen(line) + 1);	
                    clients_list.remove(events[i].data.fd);
					char nick[20];
					strcpy(nick, "newbie:");
					char buf[13];
					strncpy(buf, &line[5], strlen(line)-5);
					strcat(nick, buf);
					clients_nicks.erase(nick);
					close(events[i].data.fd);
				}
                else if (bcmp(line, "echo:", 5) == 0) {
                    list<int>::iterator it;
                    for(it = clients_list.begin(); it != clients_list.end(); it++){
                        write(*it, line, strlen(line) + 1);
                    }
                }
                else if (bcmp(line, "newbie:", 7) == 0) {
					list<int>::iterator it;
                    for(it = clients_list.begin(); it != clients_list.end(); it++){
						if (*it != events[i].data.fd)
							write(*it, line, strlen(line) + 1);
							else {
								set<string>::iterator iter;
								char clNicks[1000];
								strcpy(clNicks, "many:");
								for(iter = clients_nicks.begin(); iter != clients_nicks.end(); iter++){
									strcat(clNicks, (*iter).c_str());
								}
								write(*it, clNicks, strlen(clNicks)+1);
							}
                    }
                  	clients_nicks.insert(line);
				}
				else if (bcmp(line, "address", 7) == 0) {
					char strAddress[2000];
					strcpy(strAddress, "address:\"ulitsa Borisevicha, 4B, Krasnoyarsk, Krasnoyarskiy kray, Russia\"Internatsionalnaya ulitsa, 100A, Nizhniy Novgorod, Nizhegorodskaya oblast', Russia\"ulitsa Margolina, 22, Yaroslavl, Yaroslavskaya oblast', Russia\"Domostroitelnaya ulitsa, 4, Sankt-Peterburg, Russia\"bulvar Engelsa, 14a, Volgograd, Volgogradskaya oblast', Russia");
					char strMessage[20];
					strcpy(strMessage, "ids:00001");
					write(events[i].data.fd, strAddress, strlen(strAddress)+1);
					write(events[i].data.fd, strMessage, strlen(strMessage)+1);
				}
				else {
					double ans1 = count (line);
					char2send linetosend;
					linetosend = ftoa (ans1);

					if ((n = write(events[i].data.fd, linetosend.a, strlen(linetosend.a))) < 0) {
                        perror ("7");
						close (events[i].data.fd);
						return -1;
					}
				}						
			
			}
		}
	}
	close(sockfd);
	return 0;
}
