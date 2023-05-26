/* A simplified community detection algorithm:
 *
 * Skeleton code written by Jianzhong Qi, May 2023
 * Edited by: [Add your name and student ID here for automatic recognition]
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define STAGE_NUM_ONE 1              /* stage numbers */
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4
#define STAGE_HEADER "Stage %d\n==========\n" /* stage header format string */

#define yes printf("yes\n")
#define no printf("no\n")

typedef struct {
  /* add your user_t struct definition */
  int year;
  int number_of_hashtags;
  char* hashtag[10];
} user_t;

typedef char* data_t;                /* to be modified for Stage 4 */

/* linked list type definitions below, from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c 
*/
typedef struct node node_t;

struct node {
  data_t data;
  node_t *next;
};

typedef struct {
  node_t *head;
  node_t *foot;
} list_t;

/****************************************************************/

/* function prototypes */
list_t *make_empty_list(void);
void free_list(list_t *list);
void print_list(list_t *list);
list_t *insert_unique_in_order(list_t *list, data_t value);

void print_stage_header(int stage_num, int substage_num);

void stage_one(user_t* users[], int* number_of_users, int* thc, float* ths, int areFriends[][50]);
void stage_two(int number_of_users, int areFriends[50][50], float strength_of_connection[50][50]);
void stage_three(int number_of_users, int areFriends[50][50], float strength_of_connection[50][50]);
void stage_four(user_t* users[], int number_of_users, int thc, float ths, float strength_of_connection[50][50]);

/* add your own function prototypes here */
float computeSOC(int user1, int user2, int number_of_users, int areFriends[50][50]);
int areCloseFriends(float ths, int user_i, int user_j, float strength_of_connection[50][50]);
int isCoreUser(int user_i, int number_of_users, int thc, float ths, float strength_of_connection[50][50]);
void find_close_friends(user_t* users[], int user_i, int number_of_users, int thc, float ths, float strength_of_connection[50][50]);

/****************************************************************/

/* main function controls all the action; modify if needed */
int main(int argc, char *argv[]) {
  /* add variables to hold the input data */
  user_t* users[50];
  int number_of_users, thc, areFriends[50][50];
  float ths, strength_of_connection[50][50];
  
  /* stage 1: read user profiles */
  stage_one(users, &number_of_users, &thc, &ths, areFriends); 
  
  /* stage 2: compute the strength of connection between u0 and u1 */
  stage_two(number_of_users, areFriends, strength_of_connection);
  
  /* stage 3: compute the strength of connection for all user pairs */
  stage_three(number_of_users, areFriends, strength_of_connection);
  
  /* stage 4: detect communities and topics of interest */
  stage_four(users, number_of_users, thc, ths, strength_of_connection);
  
  /* all done; take some rest */
  return 0;
}

/****************************************************************/

/* add your code below; you can also modify the function return type 
   and parameter list 
*/

/* stage 1: read user profiles */
void stage_one(user_t* users[], int* number_of_users, int* thc, float* ths, int areFriends[][50]) {
  /* add code for stage 1 */
  /* print stage header */
  print_stage_header(STAGE_NUM_ONE,0);
  
  int num = 0, max_num_of_hashtags = 0, max_hashtags_user = 0;
  
  while(scanf("u%d", &num))
  {
    users[num] = (user_t*) malloc(sizeof(user_t));
    
    scanf("%d", &(users[num] -> year));
    
    char* hashtag; int number = 0;
    hashtag = (char*) malloc(21* sizeof(char));
    
    while(scanf(" #%s", hashtag))
    {
      users[num] -> hashtag[number] = hashtag;
      hashtag = (char*) malloc(21* sizeof(char));
      number++;
    } 
    users[num] -> number_of_hashtags = number;
  }
  
  *number_of_users = num + 1;
  
  for(int user_i=0; user_i < *number_of_users; ++user_i)
  {
    for(int user_j=0; user_j < *number_of_users; ++user_j)
    {
      scanf("%d", &areFriends[user_i][user_j]);
    }
  }
  
  scanf("%f%d",ths,thc);
  // Input DONE
  
  printf("Number of users: %d\n", *number_of_users);
  
  for(int num=0; num < *number_of_users; ++num)
  {
    if(users[num]->number_of_hashtags > max_num_of_hashtags)
    {
      max_num_of_hashtags = users[num]->number_of_hashtags;
      max_hashtags_user = num;
    }
  }
  
  printf("u%d has the largest number of hashtags:\n", max_hashtags_user);
  
  for(int number = 0; number < (users[max_hashtags_user] -> number_of_hashtags); ++number){
    printf("#%s ", users[max_hashtags_user] -> hashtag[number]);
  }
 
  printf("\n\n");
}

float computeSOC(int user1, int user2, int number_of_users, int areFriends[50][50]){
  if(! areFriends[user1][user2]) return 0;
  
  int common_friends = 0, total_num_of_friends = 0;
  for(int user_i=0; user_i < number_of_users; ++user_i)
  {
    if(areFriends[user1][user_i] || areFriends[user2][user_i]) total_num_of_friends++;
    if(areFriends[user1][user_i] && areFriends[user2][user_i]) common_friends++;
  }
  
  return (float) common_friends / total_num_of_friends;
}

/* stage 2: compute the strength of connection between u0 and u1 */
void stage_two(int number_of_users, int areFriends[50][50], float strength_of_connection[50][50]) {
  /* add code for stage 2 */
  /* print stage header */
  print_stage_header(STAGE_NUM_TWO,0);
  
  for(int user_i = 0; user_i < number_of_users; ++user_i){
    for(int user_j = user_i; user_j < number_of_users; ++user_j){
      strength_of_connection[user_i][user_j] 
      = strength_of_connection[user_j][user_i]
        = computeSOC(user_i, user_j, number_of_users, areFriends);
    }
  }
  
  printf("Strength of connection between u0 and u1: %4.2f\n", strength_of_connection[0][1]);
  
  printf("\n");
}

/* stage 3: compute the strength of connection for all user pairs */
void stage_three(int number_of_users, int areFriends[50][50], float strength_of_connection[50][50]) {
  /* add code for stage 3 */
  /* print stage header */
  print_stage_header(STAGE_NUM_THREE,0);
  
  for(int user_i = 0; user_i < number_of_users; ++user_i){
    for(int user_j = 0; user_j < number_of_users; ++user_j){
      printf("%4.2f ", strength_of_connection[user_i][user_j]);
    }
    printf("\n");
  }
  
  printf("\n");
}


int areCloseFriends(float ths, int user_i, int user_j, float strength_of_connection[50][50]){
  return strength_of_connection[user_i][user_j] > ths;
}

int isCoreUser(int user_i, int number_of_users, int thc, float ths, float strength_of_connection[50][50]){
  int num_of_close_friends=0;
  for(int user_j = 0; user_j < number_of_users; ++user_j){
    if(! areCloseFriends(ths, user_i, user_j, strength_of_connection)) continue;
    num_of_close_friends++;
  }
  return num_of_close_friends >= thc;
}


void find_close_friends(user_t* users[], int user_i, int number_of_users, int thc, float ths, float strength_of_connection[50][50]){
  list_t* list = make_empty_list();
  for(int num = 0; num < users[user_i] -> number_of_hashtags; ++num){
    char* data = users[user_i] -> hashtag[num];
    insert_unique_in_order(list, data);
    // print_list(list);
  }
  
  for(int user_j = 0; user_j < number_of_users; ++user_j){
    if(! areCloseFriends(ths, user_i, user_j, strength_of_connection)) continue;
    printf("u%d ", user_j);
    
    for(int num = 0; num < users[user_j] -> number_of_hashtags; ++num){
      char* data = users[user_j] -> hashtag[num];
      insert_unique_in_order(list, data);
    }
  }
  printf("\n");
  
  print_stage_header(STAGE_NUM_FOUR,2);
  printf(" Hashtags:\n");
  print_list(list);
  free_list(list);
}


/* stage 4: detect communities and topics of interest */
void stage_four(user_t* users[], int number_of_users, int thc, float ths, float strength_of_connection[50][50]) {
  /* add code for stage 4 */
  /* print stage header */
  print_stage_header(STAGE_NUM_FOUR,0);
  
  for(int user_i = 0; user_i < number_of_users; ++user_i){
    if(! isCoreUser(user_i, number_of_users, thc, ths, strength_of_connection)) continue;
    print_stage_header(STAGE_NUM_FOUR,1);
    printf(" Core user: u%d; close friends: ", user_i);
    find_close_friends(users, user_i, number_of_users, thc, ths, strength_of_connection);
  }
  
  printf("\n");
}

/****************************************************************/
/* functions provided, adapt them as appropriate */

/* print stage header given stage number */
void print_stage_header(int stage_num, int substage_num) {
  if(! substage_num) printf(STAGE_HEADER, stage_num);
  else printf("Stage %d.%d.", stage_num, substage_num);
}

/****************************************************************/
/* linked list implementation below, adapted from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c 
*/

/* create an empty list */
list_t *make_empty_list(void) {
  list_t *list;

  list = (list_t*)malloc(sizeof(*list));
  assert(list!=NULL);
  list->head = list->foot = NULL;

  return list;
}

/* free the memory allocated for a list (and its nodes) */
void free_list(list_t *list) {
  node_t *curr, *prev;

  assert(list!=NULL);
  curr = list->head;
  while (curr) {
    prev = curr;
    curr = curr->next;
    free(prev);
  }

  free(list);
}

/* insert a new data element into a linked list, keeping the
  data elements in the list unique and in alphabetical order
*/
list_t *insert_unique_in_order(list_t *list, data_t value) {
  /* the following code inserts a new node to the end of list.
    Modify it to suit Stage 4.2. Start by modifying it to 
    insert a new node while keeping an alphabetical order
    (think about how to insert in the middle of a list), 
    then, only insert when value is not in list already.
    [remove this comment to save line space if needed]
  */

  node_t* new = (node_t*) malloc(sizeof(*new));
  
  new->data = value;
  new->next = NULL;
  
  node_t* head = list -> head;
  node_t* prev;
  int ind = 0, isValueSmaller = 1;
  
  if(head){
    while(head -> data[ind] && value[ind]){
      if(value[ind] == head -> data[ind]){ ind++; continue; }      
      if(value[ind] > head -> data[ind]) isValueSmaller = 0;
      break;
    }
    if(! value[ind] && head->data[ind] == value[ind]) return list;
    if(! head -> data[ind] && value[ind]) isValueSmaller = 0;

    if(isValueSmaller){
        // insert at head
        new->next = head;
        head = new;

        list -> head = head;
        return list;
    } 

    prev = head;
    head = head -> next;
  }
  
  while(head){
    ind = 0, isValueSmaller = 1;
    while(head -> data[ind] && value[ind]){
      if(value[ind] == head -> data[ind]){ ind++; continue; }  
      if(value[ind] > head -> data[ind]) isValueSmaller = 0;
      break;
    }
    if(! value[ind] && head->data[ind] == value[ind]) return list;
    if(! head -> data[ind] && value[ind]) isValueSmaller = 0;

    if(isValueSmaller){
      // insert in between
      prev -> next = new;
      new -> next = head;
      return list;
    }
    
    prev = head;
    head = head -> next;
  }
  
  // insert at end
  
  if (list->foot==NULL) {
    /* this is the first insertion into the list */
    list->head = list->foot = new;
  } else {
    list->foot->next = new;
    list->foot = new;
  }

  return list;
}

/* print the data contents of a list */
void
print_list(list_t *list) {
  /* add code to print list */
  node_t* head = list -> head;
  int cnt = 0;
  while(head){
    printf("#%s ", head->data);
    if(++cnt == 5){
      printf("\n"); cnt = 0;
    }
    head = head -> next;
  }
  if(cnt) printf("\n");
}

/****************************************************************/
/*
  Write your time complexity analysis below for Stage 4.2, 
  assuming U users, C core users, H hashtags per user, 
  and a maximum length of T characters per hashtag:

*/
