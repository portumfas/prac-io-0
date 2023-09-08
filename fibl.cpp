#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/msg.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<string.h>
#include<sys/shm.h>
#include<sys/sem.h>

int main(int argc, char *argv[]) {
    long max_index;
    int shmid, semid, F_Ctrl;
    long *shmaddr;
    struct sembuf V1 = {0, 1, 0};
    struct sembuf P1 = {0, -1, 0};
    struct sembuf V2 = {1, 1, 0};
    struct sembuf P2 = {1, -1, 0};
    struct sembuf V3 = {2, 1, 0};
    struct sembuf P3 = {2, -1, 0};
    pid_t son1, son2;
    if ((F_Ctrl = creat("F_Ctrl", 0666)) < 0) {
        return 1;
    } else {
        close(F_Ctrl);
    }
    key_t key = ftok("F_Ctrl", 'h');
    semid = semget(key, 3, IPC_CREAT | 0666);
    scanf("%ld", &max_index);
    if ((son1 = fork()) < 0) {
        perror("fork");
        return 1;
    } else if (son1 == 0) {
//son1
        semop(semid, &P1, 1);
        int now_index = 2;
//now_index=sizeof(shmaddr)/sizeof(shmaddr[0])-1;
        shmid = shmget(key, max_index * sizeof(int), IPC_CREAT | 0666);
        shmaddr = shmat(shmid, NULL, 0);
        while (now_index < max_index) {
            semop(semid, &P1, 1);
            shmaddr[now_index] = shmaddr[now_index - 1] + shmaddr[now_index - 2];
            now_index = now_index + 2;
            semop(semid, &V2, 1);
        }

//to go son2 shutdown son 1
        semop(semid, &V3, 1);
        shmdt(shmaddr);
        return 0;
    } else {
        if ((son2 = fork()) < 0) {
            perror("fork");
            return 0;
        } else if (son2 == 0) {
//son2
            semop(semid, &P2, 1);
            int now_index = 3;
            shmid = shmget(key, max_index * sizeof(int), IPC_CREAT | 0666);
            shmaddr = shmat(shmid, NULL, 0);
            while (now_index < max_index) {
                semop(semid, &P2, 1);
                shmaddr[now_index] = shmaddr[now_index - 1] + shmaddr[now_index - 2];
                now_index = now_index + 2;
                semop(semid, &V1, 1);
            }
            semop(semid, &V3, 1);
            shmdt(shmaddr);
            return 0;
        } else {
//father
            shmid = shmget(key, max_index * sizeof(int), IPC_CREAT | 0666);
            if (shmid < 0) {
                perror("shmid");
                return 2;
            }
            shmaddr = shmat(shmid, NULL, 0);
            if (shmaddr == (void *) -1) {
                perror("shmat");
                return 3;
            }
            if (max_index <= 2) {
                if (max_index == 1) {
                    printf("0 ");
                    semop(semid, &V2, 1);
                    semop(semid, &V1, 1);
                    wait(NULL);
                    wait(NULL);
                    shmdt(shmaddr);
                    shmctl(shmid, IPC_RMID, NULL);
                    semctl(semid, 0, IPC_RMID, 0);
                    fflush(stdout);
                    return 0;
                } else {
                    printf("0 1 ");
                    fflush(stdout);
                    semop(semid, &V2, 1);
                    semop(semid, &V1, 1);
                    wait(NULL);
                    wait(NULL);
                    shmdt(shmaddr);
                    shmctl(shmid, IPC_RMID, NULL);
                    semctl(semid, 0, IPC_RMID, 0);
                    return 0;
                }
            } else {
                shmaddr[0] = 0;
                shmaddr[1] = 1;
                semop(semid, &V2, 1);
                semop(semid, &V1, 1);
                semop(semid, &V1, 1);
            }
            semop(semid, &P3, 1);
            int index = 0;
            while (index < max_index) {
		if (index==max_index-1){
			printf("%ld\n",shmaddr[index]);
		}
                else{
			printf("%ld ", shmaddr[index]);
		}
                index++;
            }
            fflush(stdout);
        }
    }
    wait(NULL);
    wait(NULL);
    shmdt(shmaddr);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID, 0);
    return 0;
}


////
#include <iostream>
#include <cstring>

using namespace std;

class mstring{
    char * string1;
    void right(int,int);
    void left(int,int);
public:
    mstring();
    mstring(const mstring & new_str);
    mstring(const char * str);
    ~mstring();

    int length() const;
    bool isempty() const;
    void add(char);
    void add(const char *);
    void insert(char,int);
    void insert(const char *, int);
    void print() const;
    void replace(const char *, const char *);
    int search(const char *) const;
    void del(int);
    void del(int, int);

    const mstring operator+ (const mstring &mstr) const;
    const mstring operator* (int n) const;
    const mstring & operator= (const mstring &mstr);
    bool operator> (const mstring &mstr) const;
    bool operator< (const mstring &mstr) const;
    bool operator<= (const mstring &mstr) const;
    bool operator>= (const mstring &mstr) const;
    bool operator== (const mstring &mstr) const;
    char & operator[] (int i) const;
    friend ostream & operator<< (ostream & out, const mstring &mstr);
    friend istream & operator>> (istream & in, mstring &mstr);

    friend const mstring operator* (int n,const mstring &mstr);
    friend const mstring operator+ (const char * str,const mstring &mstr);

};

//////////////////////////////////////////

const mstring operator+ (const char * str,const mstring &mstr){
    char * new_str = new char[strlen(str) + obj_str.length() + 1];
    memcpy(new_str,str,strlen(str) + 1);
    memcpy(new_str + strlen(str),obj_str.string1,obj_str.length() + 1);
    mstring new_obj_str(new_str);
    return new_obj_str;
}



/////////////////////////////////////////

mstring::mstring(){
    string1 = new char;
    string1[0] = '\0';
}

mstring::mstring(const mstring & new_str){
    string1 = new char[new_str.length() + 1];
    strcpy(string1,new_str.string1);
}

mstring::mstring(const char * str){
    int length = 0;
    while (str[length] != '\0')
    {
        length++;
    }
    length++;
    string1 = new char[length];
    strcpy(string1,str);
}

mstring::~mstring(){
    delete [] string1;
}

/////////////////////////////////////////

void mstring::right(int curr, int new_str_len = 1){ //передвигаем строку вправо(для insert)
    int len = length();
    new_str_len-=1;
    string1[len + 1 + new_str_len] = '\0';
    while (len != curr)
    {
        string1[len + new_str_len] = string1[len - 1];
        len--;
    }
}

void mstring::left(int curr, int new_str_len = 1){//передвигаем строку влево (для delete)
    new_str_len--;
    while (string1[curr] != '\0')
    {
        string1[curr] = string1[curr + 1 + new_str_len];
        curr++;
    }
}

int mstring::length() const{
    int len = 0;
    while (string1[len] != '\0')
    {
        len++;
    }
    return len;
}

bool mstring::isempty() const{
    if (string1[0] == '\0'){
        return true;
    }
    return false;
}

void mstring::add(char c){
    char * new_str = new char[length() + 2];
    memcpy(new_str,string1,length() + 1);
    new_str[length()] = c;
    new_str[length() + 1] = '\0';
    delete [] string1;
    string1 = new_str;
}

void mstring::add(const char *c){
    char * new_str = new char[length() + 2 + strlen(c)];
    memcpy(new_str,string1,length() + 1);
    memcpy(new_str + length(), c,strlen(c) + 1);
    delete [] string1;
    string1 = new_str;
}

void mstring::insert(char c,int i){
    char * new_str = new char[length() + 2];
    memcpy(new_str,string1,length() + 1);
    delete [] string1;
    string1 = new_str;
    right(i);
    string1[i] = c;
}

void mstring::insert(const char *c, int i){
    char * new_str = new char[length() + 2 + strlen(c)];
    memcpy(new_str,string1,length() + 1);
    delete [] string1;
    string1 = new_str;
    right(i,(int)strlen(c));
    memcpy(string1 + i,c,(int)strlen(c));
}

void mstring::del(int i){
    left(i);
    char * new_str = new char[length()];
    memcpy(new_str,string1,length());
    delete [] string1;
    string1 = new_str;
}

void mstring::del(int i, int j){
    left(i,j-i+1);
    char * new_str = new char[length() + j - i];
    memcpy(new_str,string1,length() + j - i);
    delete [] string1;
    string1 = new_str;
}

int mstring::search(const char *str) const{
    int len = length();
    for (int i = 0; i<len; i++)
    {
        if (string1[i] == str[0])
        {
            int j = i;
            int k = 0;
            while(str[k] == string1[j])
            {
                j++;
                k++;
                if (str[k] == '\0'){ //возвращаем, если нашлась строка до окончания строки string1
                    return i;
                }
            }
            if (str[k] == '\0'){//возвращаем, если нашлась строка вместе с окончанием строки string1
                return i;
            }
        }
    }
    return -1;
}

void mstring::replace(const char *sub_str, const char *new_str){
    int curr;
    if ((curr = search(sub_str)) == -1)
        return;
    del(curr,curr + strlen(sub_str) - 1);
    insert(new_str,curr);
}

void mstring::print() const{
    int i = 0;
    while (string1[i] != '\0')
    {
        cout << string1[i];
        i++;
    }
    cout << endl;
}

