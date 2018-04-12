/*************************************************************************
  > File Name: process_sample.c
  > Author: Sues
  > Mail: sumory.kaka@foxmail.com 
  > Created Time: Fri 30 Mar 2018 10:31:22 AM CST
 ************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <strings.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "print_color.h"
#include <string.h>
#include "list.h"
#include "common.h"
#include "shm.h"
#include <pthread.h>
#include "function.h"
#include "sha1.h"



process_type_t process_type = -1;

void init(void * arg){//每个进程都有的一个初始化.
    printf(TODO"specific init\n"NONE);
}

void call_back_SEND(int count,char state){                                          

    printf(TODO"call_back_SEND  count :%d, state:%d\n"NONE,count,state);          
    return ;                                                                        
}                                                                                   

void call_back_ACK(int count,char state){                                           

    printf(TODO"call_back_ACK  count :%d, state:%d\n"NONE,count,state);        
    return ;                                                                     
}      

void call_back_PASSIVE(int count,char state){                                     

    printf(TODO"call_back_ACTIVE  count :%d, state:%d\n"NONE,count,state);     
    return ;                                                                     
}    


void todel(list_xxx_t* list_todel_head){//websocket 独有的 发送消息的过程

    printf(TODO"todel fuction\n"NONE);     
    printf("sws : %s,%s,line = %d\n",__FILE__,__func__,__LINE__);
}

void waitfor(data_t * data){//websocket 独有的 接收的过程
    printf(TODO"waitfor fuction\n"NONE);     
}



int main(int argc,char ** argv){

    int count;
    int i ,j ;
    //要发送的数据
    data_t data;

    //要注册的信息
    process_msg_t process_msg;

    int ret = 0;

    if (argc == 1){

        printf(RED"use the sample like\n./main 1\n"NONE);
        return -1;

    }

    ret = process_init(NULL);
    if(ret < 0){
        printf(ERROR"process_init failed\n"NONE);
        return ret;
    }

    process_type = strtoul(argv[1],NULL, 10);
    printf(INFO"my name is %s\n"NONE,whoami(process_type));

    //填充要注册的信息
    process_msg.pid = getpid();
    process_msg.process_type  = process_type;
    process_msg.msg_del_method.callback_send = call_back_SEND;
    process_msg.msg_del_method.callback_ack = call_back_ACK;
    process_msg.msg_del_method.callback_passive = call_back_PASSIVE;
    process_msg.msg_del_method.init = init;
    process_msg.msg_del_method.todel = NULL;
    process_msg.msg_del_method.waitfor = NULL;

    ret = register_process(&process_msg);
    if(ret == -2){
        printf(ERROR"the process type has been registed,please change a process type\n"NONE);
        raise(SIGINT);
    }


    //遍历存在的进程
    traverse_process();

    //printf("BLUETOOTH is_existed :%d\n",is_existed(BLUETOOTH));

	alarm(1);
    while(1){

		printf("\n\n\n\n\n\n");
		// 包,发送 ,发送给 AUDIO的包
		bzero(&data,sizeof(data));
		strcpy((char *)&(data.context),"JSON package");
		strcpy((char *)&(data.msg),"Extra string information");
		data.count = ++count;
		data.pid_from = getpid();
		ret = get_str_sha1((char *)&data,sizeof(data),data.sha1);
		if(ret < 0){
			printf("get file sha1 error\n");
			exit(-89);
		}

		while(findpidbyname(AUDIO) <=0 || findpidbyname(AUDIO) == getpid()){
			if(findpidbyname(AUDIO) <=0)
				printf(WARN"AUDIO is not on line\n"NONE);
			else if(findpidbyname(AUDIO) == getpid()){
				printf(WARN"can't send msg to myself, i am %s,going to block\n"NONE,whoami(process_type));
				while(1);
			}
			sleep(1);
		}

		data.pid_to = findpidbyname(AUDIO);
		data.data_state = (process_type == WEBSOCKET ? SEND_WEBSOCKET:SEND_NORMAL);
		printf(INFO"%s is  on-line\n\n\n"NONE,whoami(AUDIO));

		ret = pkt_send(&data,sizeof(data));
		if( ret < 0 ){
			printf("error happed\n");
		}

		for(i = 0;i<30000;i++)
			for(j = 0; j < 10000;j++);
	}

	return 0;
}
