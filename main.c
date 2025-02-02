#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>

#define MAX_USERS 100
#define PAGE_SIZE 5
#define MAX_ROOMS 6
#define MIN_WIDTH 8
#define MIN_HEIGHT 7
#define MAX_WIDTH 17
#define MAX_HEIGHT 14
#define MAX_HEALTH 100

// User structure
#define DATA_FILE "users_data.txt"
typedef struct {
    char username[50];
    char password[50];
    char email[100];
    int total_scores;
    int total_gold;
    int completed_game;
    time_t first_game_time;
} User;
typedef struct {
    int x, y; // Top-left corner
} point;
typedef struct {
    char type[15];
    int damage;
    point position;
    int count;
} Weapon;
typedef struct {
point position;
int damage;
int color;
}Trap;
typedef struct {
point position;
int amount;
}gold;
typedef struct {
point position;
char type[10];
int value;
}food;
typedef struct {
point position;
char type[10];
}spell;
typedef struct {
point position;
char type;
int health;
int damage;
int range;
int current_range;
}monster;
// Structure to represent a room
typedef struct {
    int x, y; // Top-left corner
    int width, height;
    point doors[10];
     int door_count;
     point windows[4];
     int window_count;
     point obstacles[10];
     int obstacle_count;
     point hiddenDoors[10];
     int hiddenDoor_count;
     Trap traps[15];
     int trap_count;
     gold golds[10];
     int gold_count;
     Weapon weapons[10]; // Up to 5 weapons per room
     int weapon_count;
     food foods[10];
     int food_count;
     spell spells[10];
     int spell_count;
     monster monsters[2];
     int monster_count;

     int color;
     char theme[100];
 } Room;
typedef struct {
point positions;
int health;
int gold;
int hunger;
int speed_spells;
int health_spells;
int damage_spells;
food food_inventory[5];
int food_count;
Weapon active_weapon;
//Weapon owned_weapons[20];
 //int weapons_count;
 int dagger_count;
 int sword_count;
 int Mace_count;
 int magic_wound_count;
 int arrow_count;
 int active_spell;
 int active_spell_timer;
}Player;
// Array to store users

User users[MAX_USERS];
char current_username[50];
int user_count = 0;
int guest=0;
int successful_entry=0;
int dificulty_mode=3;
int level=0; 
int win=0;
int viewing_whole_map=0; 
int skip_pickup=0;
char map[4][1000][1000];
int visibility[4][1000][1000];
int max_y, max_x;
    Room rooms[40];
point stair[4];
 int random_stair;
 Player * player;
WINDOW *message_window;
WINDOW *status_window;
time_t last_update_time;

int song_choice = 1;
const char *songs[3] = {  // Only 3 valid songs
    "song1.mp3",
    "song2.mp3",
    "song3.mp3"
};



int valid_password(char *password);
int valid_email(char *email);
void reset_password(char *username);
void create_new_user();
int login_user();
void main_menu();
void pre_game_menu();

void load_users_fromfile();
void save_users_tofile();
void game_settings();
void manage_profile();
void start_new_game();
void continue_previous_game();

void set_difficulty();
void set_charcter_color();
int compare_scores(const void *a, const void *b);
void navigate_scoreboard();
void display_scoreboard(int current_page);

void save_game();
void load_game();

void song_menu();
void stop_song();
void play_song(const char *song);

void draw_from_file(const char *filename);
//////////////////////////////////////////////////////////////////////////////////
void update_message_window(const char *message);
void update_clear_message_swindow();
void set_up_screen_message();
void update_status_window(Player *player);

void generate_doors(Room *room);
void generate_obstacles(Room *room);
void generate_traps();
void generate_windows(Room *room);
int rooms_overlap(Room r1, Room r2);
int room_overlaps_existing(Room *rooms, int count, Room new_room);
void generate_path(int x1, int y1, int x2, int y2);
void generate_hidden_doors_assign_themes();
void update_room_doors(Room rooms[], int room_count);
void make_map();

point valid_empty_place(int leve,int room_index);
int get_current_room_id(int x, int y);
void reveal_room(int currentx, int currenty);
Player * playersetup();
void update_visibility_on_move(int oldx,int oldy,int currentx,int currenty);
int right_path(int x1, int y1, int x2, int y2);
void check_valid_move (Player * player,int newx,int newy);
void check_traps (Player * player);
void print_map();
void handle_input(int input,Player *player);
void check_hidden_doors(int x,int y);
int is_in_enchanted_room(Player *player);
void adjust_color();
void print_whole_map();
void search_traps_and_hidden_doors(int x,int y);
void check_traps_placement(int x,int y);

void place_special_items();
void process_gold(int x,int y);
void process_food(int x, int y);
void display_food_menu(Player * player);
void check_hunger_and_health();
void process_spell(int x, int y);
void display_spell_menu(Player * player);
void process_weapon(int x, int y);
void display_weapon_menu(Player * player);

void generate_monsters();
void move_monsters(Player *player);
int monster_exist(int x, int y);
void update_monster_range();
void attack_monster(Player * player);
////////////////////////////////////////////////////////
int main(){
    setlocale(LC_ALL,"");
    initscr();
    cbreak();
    //noecho();
    curs_set(FALSE);
    keypad(stdscr,TRUE);
   start_color();
   init_color(9,1000,500,0);
   init_pair(1,COLOR_GREEN,COLOR_BLACK);
   init_pair(2,COLOR_RED,COLOR_BLACK);
   init_pair(3,COLOR_WHITE,COLOR_BLACK);
   init_pair(4,COLOR_YELLOW,COLOR_BLACK);
   init_pair(5,COLOR_CYAN,COLOR_BLACK);
   init_pair(6,COLOR_MAGENTA,COLOR_BLACK);
   init_pair(7,COLOR_GREEN,COLOR_BLACK);
   init_pair(8,COLOR_BLUE,COLOR_BLACK);
   init_pair(9,9,COLOR_BLACK);
    // Seed random number generator
    srand(time(NULL));
    load_users_fromfile();
    main_menu();

    save_users_tofile();
    endwin();
    return 0;
}
int valid_password(char *password) {
    int has_capital = 0, has_small = 0, has_digit = 0;
    if (strlen(password) < 7) return 0;
    for (int i = 0; password[i] != '\0'; i++) {
        if (password[i]>='A' && password[i]<='Z') has_capital = 1;
        if (password[i]>='a' && password[i]<='z') has_small = 1;
        if (password[i]>='0' && password[i]<='9') has_digit = 1;

    }
    return has_capital && has_small && has_digit;
}
int valid_email(char *email) {
    char *atsign = strchr(email, '@');
    if (atsign == NULL) return 0;
    char *dot = strchr(atsign, '.');
    if (dot==NULL || dot == atsign + 1 || *(dot+1)=='\0') return 0;
    return 1;
}
void reset_password(char *username) {
            char new_password[10];
            const char all_chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
            srand(time(NULL));
            new_password[0]=all_chars[rand() % (26)];
            new_password[1]=all_chars[(rand() % (26))+26];
            new_password[2]=all_chars[rand() % (9)+52];
            for (int j = 3; j < 9; j++) {
                new_password[j] = all_chars[rand() % (sizeof(all_chars) - 1)];
            }
            new_password[9] = '\0';
            attron(COLOR_PAIR(5));
            printw("Your new password is: %s\n", new_password);
            attroff(COLOR_PAIR(5));
            return;
}
void create_new_user() {
    char username[50], password[50], email[100];
    
    echo();
    printw("Enter username: ");
    getstr(username);
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            attron(COLOR_PAIR(9));
            printw("Username already exists. Try again.\n");
            attroff(COLOR_PAIR(9));
            return;
        }
    }
    printw("Do you to random generate a valid password? (y/n): ");
     char answ=getch();
    printw("\n");
    if(answ=='y'){
      reset_password(username);   
    }
    printw("Enter password (min 7 characters): ");
    getstr(password);
    if (!valid_password(password)) {
        attron(COLOR_PAIR(9));
        printw("Invalid password.Password should have at least 1 capital letter, 1 small letter and 1 digit. Try again.\n");
        attroff(COLOR_PAIR(9));
        return;

    }
    printw("Enter email: ");
    getstr(email);
    if (!valid_email(email)) {
        attron(COLOR_PAIR(9));
        printw("Invalid email format. Try again.\n");
        attroff(COLOR_PAIR(9));
        return;
    }
    strcpy(users[user_count].username, username);
    strcpy(users[user_count].password, password);
    strcpy(users[user_count].email, email);
    users[user_count].total_scores=0;
    users[user_count].total_gold=0;
    users[user_count].completed_game=0;
    users[user_count].first_game_time=0;
    user_count++;
    attron(COLOR_PAIR(5));
    printw("User created successfully!\n");
    attroff(COLOR_PAIR(5));
}
int login_user() {
    char username[50], password[50],correct_password[50];
    int found=0;
    echo();
    printw("Enter as a guest? (y/n) ");
    char wanna_be_guest=getch();
    printw("\n");
    if(wanna_be_guest=='y'){
      guest=1;return 1;   
    }
    guest=0;
    printw("Enter username: ");
    getstr(username);
   for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0 ) {
            strcpy(current_username,users[i].username);
           found=1;
           strcpy(correct_password,users[i].password);
           break;
        }
    }
    if(!found){
        attron(COLOR_PAIR(9));
        printw("Invalid username. Try again.\n");
        attroff(COLOR_PAIR(9));
        return 0;}

    printw("Forgot password? (y/n): ");
    char answer=getch();
    printw("\n");
    if(answer=='y'){
        attron(COLOR_PAIR(5));
      printw("Your password is %s\n",correct_password); 
      attroff(COLOR_PAIR(5));  
    }
    printw("Enter password: ");
    getstr(password);
        if ( strcmp(correct_password, password) == 0) {
             attron(COLOR_PAIR(5));
             printw("Login successful! Welcome, %s.\n", username);
             attroff(COLOR_PAIR(5));  
             return 1;
        }
    attron(COLOR_PAIR(9));  
    printw("Invalid password. Try again.\n");
    attroff(COLOR_PAIR(9));  
    return 0;
}
void load_users_fromfile() {
    
    FILE *file = fopen(DATA_FILE, "r");
    if (!file) return;
    while (fscanf(file, "%49s %49s %d %d %d %ld", 
                  users[user_count].username, 
                  users[user_count].password, 
                  &users[user_count].total_scores,
                  &users[user_count].total_gold,
                  &users[user_count].completed_game,
                  &users[user_count].first_game_time) == 6) {
        user_count++;
    }
    fclose(file);
}
void save_users_tofile() {
    FILE *file = fopen(DATA_FILE, "w");
    if (!file) {
        printw("Error saving users to file.\n");
        return;
    }
    for (int i = 0; i < user_count; i++) {
        fprintf(file, "%s %s %d %d %d %ld\n", users[i].username, users[i].password,users[i].total_scores,users[i].total_gold,users[i].completed_game,users[i].first_game_time);
    }
    fclose(file);
}
void main_menu() {
    int choice;
    
    echo();
    while (1) {
        clear();
        attron(COLOR_PAIR(5));
        printw("=== Main Menu ===\n");
        printw("1. Create New User\n");
        printw("2. Login\n");
        printw("3. Exit\n");
        printw("Enter your choice: ");
        attroff(COLOR_PAIR(5));
        scanw("%d", &choice);
       
        switch (choice) {
            case 1:
                clear();
                create_new_user();
                break;

            case 2:
                clear();
                if (login_user()) {
                    attron(COLOR_PAIR(5));  
                    printw("You can now access the game!\n");
                    attroff(COLOR_PAIR(5));  
                    refresh();
                    usleep(2000000);
                    successful_entry=1;
                    clear();
                   pre_game_menu();
                }
                break;
            case 3:
                endwin();
                return;

            default:
                attron(COLOR_PAIR(9));
                printw("Invalid Command.Try again.\n");
                attroff(COLOR_PAIR(9));

        }
        // if(guest==1 || successful_entry==1){break;}
        printw("\nPress any key to continue...");
        getch();
        
    }

}

void start_new_game() {
    noecho();
    clear();
    win=0;
     User *current_user;
    if(guest==0){

      for (int i = 0; i < user_count; i++) {
        if(strcmp(users[i].username,current_username)==0){current_user=&users[i];
        if(current_user->completed_game==0)current_user->first_game_time=time(NULL);
        break;}
    }}
     last_update_time=time(NULL);
     getmaxyx(stdscr, max_y, max_x);
      max_y-=3;max_x-=2;
      set_up_screen_message();
     level=0;
     make_map();
     generate_traps();
     generate_hidden_doors_assign_themes();
     place_special_items();
     generate_monsters();
       level=0;
     player=playersetup();
     adjust_color();
     attron(COLOR_PAIR(1));
      mvprintw(player->positions.y,player->positions.x,"@");    
     attroff(COLOR_PAIR(1));
     if (song_choice == 0) {
        stop_song();
    } else {
        play_song(songs[song_choice - 1]);
    }
     while(1){
         int ch=getch();
         if(ch=='q'){break;}
         handle_input(ch,player);
         if((win==1)||(player->health<=0)){break;}
         check_hunger_and_health();

        }
        if (song_choice != 0) {
        stop_song();} 
    int score=player->gold+((player->health)*(4-dificulty_mode)*(4-dificulty_mode));
    refresh();
    clear();
    if(win==1){
        if(guest==0){ current_user->completed_game++;current_user->total_scores+=score;current_user->total_gold+=player->gold;}
       
        mvprintw(0,0,"You won!\nScore:%d\n",score);
        draw_from_file("trophy.txt");}
    else if((player->health<=0)){mvprintw(0,0,"You lost!\n");
    draw_from_file("skull.txt");
    if(guest==0){ current_user->completed_game++;current_user->total_scores+=score;current_user->total_gold+=player->gold;}
       }
    else{mvprintw(0,0,"You quited the game!\nScore:%d\n",score);
    draw_from_file("dragon.txt");
    if(guest==0) save_game();
      
    }
   // getch();
    endwin();

}
void continue_previous_game() {
    if(guest==1)return;
    noecho();
    clear();
    User *current_user;
     for (int i = 0; i < user_count; i++) {
        if(strcmp(users[i].username,current_username)==0){current_user=&users[i];
        if(current_user->completed_game==0)current_user->first_game_time=time(NULL);
        break;}
    }
   load_game();
     last_update_time=time(NULL);
     getmaxyx(stdscr, max_y, max_x);
      max_y-=3;max_x-=2;
      set_up_screen_message();
      update_status_window(player);
       adjust_color();
     attron(COLOR_PAIR(1));
      mvprintw(player->positions.y,player->positions.x,"@");    
     attroff(COLOR_PAIR(1));
     if (song_choice == 0) {
        stop_song();
    } else {
        play_song(songs[song_choice - 1]);
    }
     while(1){
         int ch=getch();
         if(ch=='q'){break;}
         handle_input(ch,player);
         if((win==1)||(player->health<=0)){break;}
         check_hunger_and_health();

        }
        if (song_choice != 0) {
        stop_song();} 
    int score=player->gold+((player->health)*(4-dificulty_mode)*(4-dificulty_mode));
    refresh();
    clear();
    if(win==1){
        current_user->completed_game++;current_user->total_scores+=score;current_user->total_gold+=player->gold;
       
        mvprintw(0,0,"You won!\nScore:%d\n",score);}
    else if((player->health<=0)){mvprintw(0,0,"You lost\n!");
     current_user->completed_game++;current_user->total_scores+=score;current_user->total_gold+=player->gold;
       }
    else{mvprintw(0,0,"You quited the game!\nScore:%d\n",score);
    save_game();
    }
   // getch();
    endwin();    


}



void game_settings() {
   int choice;
   while (1) {
    echo();
    clear();
    printw("=== Game Settings ===\n");
    printw("1. Set Difficulty Level\n");
    printw("2. Change Character Color\n");
    printw("3. Select Background Music\n");
     printw("4.Go back\n");
    printw("Enter your choice: ");
   scanw("%d", &choice);
   switch (choice) {
            case 1:
                clear();
                set_difficulty();
                break;

            case 2:
             clear();
             set_charcter_color();
             break;
            case 3:
            clear();
             song_menu();
                break;
            case 4:
               // endwin();
                return;

            default:
                printw("Invalid Command.Try again.\n");

        }
        // if(guest==1 || successful_entry==1){break;}
        printw("\nPress any key to continue...");
        getch();
        
    }
   }
    


void manage_profile() {
if(guest==1)return;
 User *current_user;
    if(guest==0){
      for (int i = 0; i < user_count; i++) {
        if(strcmp(users[i].username,current_username)==0){current_user=&users[i];
        break;}
    }}
    clear();
    time_t current_time = time(NULL);
    double experience = (current_user->first_game_time != 0) ? difftime(current_time, current_user->first_game_time) : 0;
     experience/=360; 
    attron(COLOR_PAIR(6));  
    printw("=== Profile Management ===\n");

    printw("\n1. User name: %s\n",current_user->username);
    printw("2. Password: %s\n",current_user->password);
    printw("3. Completed games: %d\n",current_user->completed_game);
    printw("4. Total golds: %d\n",current_user->total_gold);
    printw("5. Total scores: %d\n",current_user->total_scores);
    printw("6. Experience(hours): %.2f\n",experience);
    attroff(COLOR_PAIR(6));  

}
void pre_game_menu() {
    echo();
    int choice;
    while (1) {
        clear();
        attron(COLOR_PAIR(5));
        printw("=== Pre-Game Menu ===\n");
        printw("1. Start New Game\n");
        printw("2. Continue Previous Game\n");
        printw("3. Scoreboard\n");
        printw("4. Settings\n");
        printw("5. Profile\n");
       printw("6. Back to Main Menu\n");
        printw("Enter your choice: ");
        attroff(COLOR_PAIR(5));
        scanw("%d", &choice);

        switch (choice) {

            case 1:
                start_new_game();
                break;
            case 2:
                continue_previous_game();
                break;
            case 3:
                //display_scoreboard();
                 navigate_scoreboard();
                break;
            case 4:
                game_settings();
                break;
            case 5:
                manage_profile();
                break;
            case 6:
                //main_menu();
                return;

            default:
                printw("Invalid choice. Try again.\n");

        }
        printw("\nPress any key to continue...");
        getch();

    }

}
void update_message_window(const char *message) {
    // Clear the message window
    wclear(message_window);
    // Redraw the border of the message window
    box(message_window, 0, 0);
    // Print the new message inside the window
    mvwprintw(message_window, 1, 1, "%s", message);
    // Refresh the message window to show the updates
    wrefresh(message_window);
}
void update_clear_message_swindow() {
    // Clear the message window
    wclear(message_window);
    box(message_window, 0, 0);
    wrefresh(message_window);
}
void generate_doors(Room *room) {

    //srand(time(NULL)); // Seed the random generator
    room->door_count = 0;
    // Random number of doors (1 to MAX_DOORS)
    int num_doors = 2;
    for (int i = 0; i < num_doors; i++) {
        int wall = rand() % 4; // Randomly select a wall (0: top, 1: bottom, 2: left, 3: right)
        point new_door;
        switch (wall) {
        case 0: // Top wall
            new_door.x = room->x + 1 + rand() % (room->width -1);
            new_door.y = room->y;
            break;
        case 1: // Bottom wall
            new_door.x = room->x + 1 + rand() % (room->width-1 );
            new_door.y = room->y + room->height - 1;
            break;
        case 2: // Left wall
            new_door.x = room->x;
            new_door.y = room->y + 1 + rand() % (room->height-1);
            break;
        case 3: // Right wall
            new_door.x = room->x + room->width - 1;
            new_door.y = room->y + 1 + rand() % (room->height-1);
            break;
        }
        // Add the door to the room
        room->doors[room->door_count++] = new_door;
    }

}
void generate_obstacles(Room *room) {
    room->obstacle_count = 0;
    int num_obstacle = rand() % 4;
    for (int i = 0; i < num_obstacle; i++) {
        point new_obstacle;
       
            new_obstacle.x = room->x + 2 + rand() % (room->width -4);
            new_obstacle.y = room->y +2 + rand() % (room->height-4);;
         
        room->obstacles[room->obstacle_count++] = new_obstacle;
    }

}
void generate_windows(Room *room) {

    room->window_count = 0;
    
        int wall = rand() % 4; 
        point new_window;
        switch (wall) {
        case 0: // Top wall
            new_window.x = room->x + 1 + rand() % (room->width -1);
            new_window.y = room->y;
            break;
        case 1: // Bottom wall
            new_window.x = room->x + 1 + rand() % (room->width-1 );
            new_window.y = room->y + room->height - 1;
            break;
        case 2: // Left wall
            new_window.x = room->x;
            new_window.y = room->y + 1 + rand() % (room->height-1);
            break;
        case 3: // Right wall
            new_window.x = room->x + room->width - 1;
            new_window.y = room->y + 1 + rand() % (room->height-1);
            break;
        }
        // Add the door to the room
        room->windows[room->window_count++] = new_window;

}
point valid_empty_place(int leve,int room_index){
    int success=0;
    point valid_placement;
    while(!success){
            valid_placement.x = rooms[(leve*6)+room_index].x + 2 + rand() % (rooms[(leve*6)+room_index].width -4);
            valid_placement.y = rooms[(leve*6)+room_index].y +2 + rand() % (rooms[(leve*6)+room_index].height-4);;
         if(map[leve][valid_placement.y][valid_placement.x]=='.'&&map[leve][valid_placement.y][valid_placement.x+1]=='.'){
            success=1;}
       // room->obstacles[room->obstacle_count++] = valid_placement;
    }
    return valid_placement;
}
void generate_traps(){
    for(int j=0;j<24;j++){
   rooms[j].trap_count = 0;
    int num_trap = (rooms[j].width*rooms[j].height<80)?1+rand()%2:2+rand() % 6;
    if(j==20)num_trap=12;
    for (int i = 0; i < num_trap; i++) {
        Trap new_trap;
            point starting_position=valid_empty_place(j/6,j%6);
           // if(map[level][starting_position.y][starting_position.x]!='.'){continue;}
           new_trap.position.x=starting_position.x;
            new_trap.position.y=starting_position.y; 
             new_trap.damage=1+rand()%5;
              new_trap.color=2;
        rooms[j].traps[rooms[j].trap_count++] = new_trap;
    }
    
     }
}
void place_special_items(){
    for(int j=0;j<24 ;j++){
       // if(j%6!=5){
   rooms[j].gold_count = 0;
    int num_gold = (rooms[j].width*rooms[j].height<70)?1+rand() % 2:1+rand() % 3;
    if(j==20)num_gold=4+rand()%4;
    for (int i = 0; i < num_gold; i++) {
        gold new_gold;
            point starting_position=valid_empty_place(j/6,j%6);
           // if(map[level][starting_position.y][starting_position.x]!='.'){continue;}
           new_gold.position.x=starting_position.x;
            new_gold.position.y=starting_position.y; 
             new_gold.amount=1+rand()%5;
             map[j/6][new_gold.position.y][new_gold.position.x]='%';
        rooms[j].golds[rooms[j].gold_count++] = new_gold;
    } 
    //}
    }
   for(int j=0;j<24 ;j++){
   if(j%6!=5){
    int num_gold = rand() % 2;
    if(j==20)num_gold = 1+rand() % 2;
    for (int i = 0; i < num_gold; i++) {
        gold new_gold;
            point starting_position=valid_empty_place(j/6,j%6);
           // if(map[level][starting_position.y][starting_position.x]!='.'){continue;}
             new_gold.position.x=starting_position.x;
             new_gold.position.y=starting_position.y; 
             new_gold.amount=15+((rand()%3)*10);
             map[j/6][new_gold.position.y][new_gold.position.x]='$';
             rooms[j].golds[rooms[j].gold_count++] = new_gold;
    } 
    
    rooms[j].food_count=(rooms[j].width*rooms[j].height<64)?rand()%2:1+rand()%2;
    for(int i=0;i<rooms[j].food_count;i++){
    food newfood;
    newfood.position=valid_empty_place(j/6,j%6);
    int type=rand()%4;
    switch (type)
      {
      case 0:
      strcpy( newfood.type,"normal");
      map[j/6][newfood.position.y][newfood.position.x]='f';
      break;
      case 1:
     strcpy( newfood.type,"bad");
      map[j/6][newfood.position.y][newfood.position.x]='f';
      break;
      case 2:
     strcpy( newfood.type,"magic");
      map[j/6][newfood.position.y][newfood.position.x]='i';
      break;
      case 3:
      strcpy( newfood.type,"gourmet");
      map[j/6][newfood.position.y][newfood.position.x]='e';
      break;
      }
    newfood.value=20+rand()%11;
     rooms[j].foods[i] = newfood;
    }
    rooms[j].spell_count=0;
    rooms[j].weapon_count=0;
    if(j==20)continue;
if(rooms[j].width*rooms[j].height>70){
    
     if(j%6==4 || j%6==1){rooms[j].spell_count=(rooms[j].width*rooms[j].height<70)?1+rand()%2:3+rand()%4;}
     else {rooms[j].spell_count=(rooms[j].width*rooms[j].height<70)?rand()%2:1+rand()%3;}
     for(int i=0;i<rooms[j].spell_count;i++){
        spell newspell;
      newspell.position=valid_empty_place(j/6,j%6);
      int type=rand()%3;
      switch (type)
      {
      case 0:strcpy(newspell.type,"speed");
      map[j/6][newspell.position.y][newspell.position.x]='x';
     rooms[j].spells[i] = newspell;
      break;
      case 1:strcpy(newspell.type,"damage");
      map[j/6][newspell.position.y][newspell.position.x]='y';
     rooms[j].spells[i] = newspell;
      break;
      case 2:strcpy(newspell.type,"health");
      map[j/6][newspell.position.y][newspell.position.x]='z';
     rooms[j].spells[i] = newspell;
      break;
      }

     }
   }
     
if(rooms[j].width*rooms[j].height>70){
    rooms[j].weapon_count=1+rand()%3;
     for(int i=0;i<rooms[j].weapon_count;i++){
        Weapon newweapon;
      newweapon.position=valid_empty_place(j/6,j%6);
      int type=rand()%4;
      switch (type)
      {
      case 0:strcpy(newweapon.type,"arrow");
      newweapon.damage=5;
      map[j/6][newweapon.position.y][newweapon.position.x]='a';
      newweapon.count=20;
     rooms[j].weapons[i] = newweapon;
      break;
      case 1:strcpy(newweapon.type,"dagger");
       newweapon.damage=12;
      map[j/6][newweapon.position.y][newweapon.position.x]='d';
       newweapon.count=10;
     rooms[j].weapons[i] = newweapon;
      break;
      case 2:strcpy(newweapon.type,"magic_wound");
      newweapon.damage=15;
       newweapon.count=8;
      map[j/6][newweapon.position.y][newweapon.position.x]='m';
     rooms[j].weapons[i] = newweapon;
      break;
      case 3:strcpy(newweapon.type,"sword");
       newweapon.damage=10;
      map[j/6][newweapon.position.y][newweapon.position.x]='s';
     rooms[j].weapons[i] = newweapon;
      break;
      }
     }

   }

   }
}}
void generate_hidden_doors_assign_themes(){
    for(int i=0;i<24;i+=3){
         rooms[i].hiddenDoor_count=0;
       int num_hidden_door=1 +rand()%(rooms[i].door_count);
    for (int k=0;k<num_hidden_door;k++){
    rooms[i].hiddenDoors[rooms[i].hiddenDoor_count].x=rooms[i].doors[k].x;
    rooms[i].hiddenDoors[rooms[i].hiddenDoor_count].y=rooms[i].doors[k].y;
    rooms[i].hiddenDoor_count++;
    int doorx=rooms[i].doors[k].x;int doory=rooms[i].doors[k].y;
    int level=i/6;
    if((map[level][doory][doorx-1]=='#'||map[level][doory][doorx-1]==' '||map[level][doory][doorx+1]=='#'||map[level][doory][doorx+1]==' ')&&(map[level][doory-1][doorx]!='#')&&(map[level][doory-1][doorx]!=' ')){
     map[level][doory][doorx]='|';
    }else{map[level][doory][doorx]='_';}}
    }
    for(int i=1;i<24;i+=3){
         rooms[i].hiddenDoor_count=0;
       int num_hidden_door=(rooms[i].door_count);
       strcpy(rooms[i].theme,"enchant");
    for (int k=0;k<num_hidden_door;k++){
    rooms[i].hiddenDoors[rooms[i].hiddenDoor_count].x=rooms[i].doors[k].x;
    rooms[i].hiddenDoors[rooms[i].hiddenDoor_count].y=rooms[i].doors[k].y;
    rooms[i].hiddenDoor_count++;
    int doorx=rooms[i].doors[k].x;int doory=rooms[i].doors[k].y;
    int level=i/6;
    if((map[level][doory][doorx-1]=='#'||map[level][doory][doorx-1]==' '||map[level][doory][doorx+1]=='#'||map[level][doory][doorx+1]==' ')&&(map[level][doory-1][doorx]!='#')&&(map[level][doory-1][doorx]!=' ')){
     map[level][doory][doorx]='|';
    }else{map[level][doory][doorx]='_';}}
    }
    for(int q=0;q<4;q++){
     strcpy(rooms[(q*6)+5].theme,"nightmare");
     strcpy(rooms[(q*6)].theme,"regular");
     (q!=3)?strcpy(rooms[(q*6)+2].theme,"regular"):strcpy(rooms[(q*6)+2].theme,"treasure");
     strcpy(rooms[(q*6)+3].theme,"regular");
    }
     point starting_position=valid_empty_place(3,2);
     map[3][starting_position.y][starting_position.x]='E';
    for(int j=0;j<24;j++){
     if(strcmp(rooms[j].theme,"enchant")==0){
      rooms[j].color=6;
     }else if(strcmp(rooms[j].theme,"nightmare")==0){
      rooms[j].color=2;
     }else if(strcmp(rooms[j].theme,"treasure")==0){
      rooms[j].color=4;
     }else{
      rooms[j].color=3;
     }
    }
    }
// Function to check if two rooms overlap
int rooms_overlap(Room r1, Room r2) {
    return !(r1.x + r1.width +10 <= r2.x || // r1 is to the left of r2
             r1.x >= r2.x + r2.width+10 || // r1 is to the right of r2
             r1.y + r1.height +10<= r2.y || // r1 is above r2
             r1.y >= r2.y + r2.height+10); // r1 is below r2
}

// Function to check if a room overlaps with any existing rooms
int room_overlaps_existing(Room *rooms, int count, Room new_room) {
    for (int i = 0; i < count; i++) {
        if (rooms_overlap(rooms[(level*6)+i], new_room)) {
            return 1;
        }
    }
    return 0;
}

void generate_path(int x1, int y1, int x2, int y2) {
    int x = x1, y = y1;
    // Horizontal and vertical distances
    int dx = x2 - x1;
    int temp_x=(dx>0)? 1 : -1;
    int dy = y2 - y1;
    int temp_y=(dy>0)? 1 : -1;
    // Move horizontally first
    while (dx != 0) {
        x += (dx > 0) ? 1 : -1;
        dx += (dx > 0) ? -1 : 1;
        if (map[level][y][x] == ' '||map[level][y][x] == '#') {
            map[level][y][x] = '#'; // Mark path
        }
         else if (map[level][y][x] == '|'||map[level][y][x] == '_'||map[level][y][x] == '=') {
           if(map[level][y][x-temp_x]=='#' ||map[level][y][x+temp_x]==' '||map[level][y][x+temp_x]=='.') map[level][y][x] = '+'; // Mark path
          //if(map[y][x+( (dx > 0) ? 1 : -1)]=='#'&&map[y][x-( (dx > 0) ? 1 : -1)]=='_') map[y][x] = '+';
          if(map[level][y][x-temp_x]=='#' && map[level][y][x+temp_x]=='|') map[level][y][x] = '+';
        } 
    }
    // Then move vertically
    while (dy != 0) {
        y += (dy > 0) ? 1 : -1;
        dy += (dy > 0) ? -1 : 1;
         if (map[level][y][x] == ' '||map[level][y][x] == '#') {
            map[level][y][x] = '#'; // Mark path
        }
        else if (map[level][y][x] == '|'||map[level][y][x] == '_'||map[level][y][x] == '=') {
           if(map[level][y-temp_y][x]=='#'||map[level][y+temp_y][x]==' '||map[level][y+temp_y][x]=='.') map[level][y][x] = '+'; // Mark path
            if(map[level][y-temp_y][x]=='#' && map[level][y+temp_y][x]=='_')map[level][y][x] = '+';
        } 
    }
}
void update_room_doors(Room rooms[], int room_count) {
    for (int i = 0; i < room_count; i++) {
        Room *room = &rooms[i+(level*6)];
        room->door_count = 0; 
        
        // Reset door count
        // Check the borders of the room for doors
        for (int x = room->x; x < room->x + room->width; x++) {
            // Top border
            if (map[level][room->y][x] == '+') {
                room->doors[room->door_count].x = x;
                room->doors[room->door_count].y = room->y;
                room->door_count++;
            }
            // Bottom border
            if (map[level][room->y + room->height - 1][x] == '+') {
                room->doors[room->door_count].x = x;
                room->doors[room->door_count].y = room->y + room->height - 1;
                room->door_count++;
            }
        }
        for (int y = room->y; y < room->y + room->height; y++) {
            // Left border
            if (map[level][y][room->x] == '+') {
                room->doors[room->door_count].x = room->x;
                room->doors[room->door_count].y = y;
                room->door_count++;
            }
            // Right border
            if (map[level][y][room->x + room->width - 1] == '+') {
                room->doors[room->door_count].x = room->x + room->width - 1;
                room->doors[room->door_count].y = y;
                room->door_count++;
            }
        }
        room->hiddenDoor_count=0;
        room->weapon_count=0;
        room->gold_count=0;
    }

}
 
void make_map(){
    while(level<4){
    int room_count = 0;
   
 while (room_count < MAX_ROOMS) {
        Room new_room;
         if(level!=0 && room_count == 0 ){
         new_room.width = rooms[random_stair].width;
        new_room.height = rooms[random_stair].height;
        new_room.x = rooms[random_stair].x;
        new_room.y =rooms[random_stair].y;
    }else{
        new_room.width = rand() % (MAX_WIDTH - MIN_WIDTH + 1) + MIN_WIDTH;
        new_room.height = rand() % (MAX_HEIGHT - MIN_HEIGHT + 1) + MIN_HEIGHT;
        new_room.x = rand() % (max_x -2- new_room.width)+3;
        new_room.y = rand() % (max_y -2- new_room.height)+3;
    }
        if (!room_overlaps_existing(rooms, room_count, new_room)) {
            generate_windows(&new_room);
            generate_obstacles(&new_room);
            generate_doors(&new_room);
            rooms[(level*6)+room_count++] = new_room;

        }
    }
    for (int y = 0; y < max_y; y++) {
        for (int x = 0; x < max_x; x++) {
            map[level][y][x] = ' '; // Initialize all cells as empty
           visibility[level][y][x] = 0;
        }
    }
    // Display the rooms
    for (int i = 0; i < room_count; i++) {
        Room r = rooms[(level*6)+i];
        for (int y = 0; y < r.height; y++) {
            for (int x = 0; x < r.width; x++) {
                if(y==0 && (x==0||x==r.width-1 )){
                map[level][r.y + y][r.x + x] = '_';}
                else if(x==0 || x==r.width-1){
                map[level][r.y + y][r.x + x] = '|';}
                else if(y==0 || y==r.height-1){
                map[level][r.y + y][r.x + x] = '_';}
                else{
                    map[level][r.y + y][r.x + x] = '.';}
            }
        }
         for (int i = 0; i < r.window_count; i++) {
         map[level][r.windows[i].y][r.windows[i].x] = '=';}
          for (int i = 0; i < r.obstacle_count; i++) {
         map[level][r.obstacles[i].y][r.obstacles[i].x] = 'O';}
        for (int i = 0; i < r.door_count; i++) {
         map[level][r.doors[i].y][r.doors[i].x] = '+';}
    }
for(int i=0;i<room_count-1;i++){
   generate_path(rooms[(6*level)+i].doors[1].x,rooms[(6*level)+i].doors[1].y,rooms[(6*level)+i+1].doors[0].x,rooms[(6*level)+i+1].doors[0].y);
    }

    for (int y = 0; y <max_y; y++) {
        for (int x = 0; x < max_x; x++) {
            if (map[level][y][x] == '+') {
                int has_path = 0;
                if (y > 0 && map[level][y - 1][x] == '#') has_path = 1;  // Above
                if (y < max_y - 1 && map[level][y + 1][x] == '#') has_path = 1;  // Below
                if (x > 0 && map[level][y][x - 1] == '#') has_path = 1;  // Left
                if (x < max_x - 1 && map[level][y][x + 1] == '#') has_path = 1;  // Right
                if (!has_path) {
                 map[level][y][x] = (map[level][y - 1][x] == '|' || map[level][y + 1][x] == '|' || map[level][y - 1][x] == '+' || map[level][y + 1][x] == '+'|| map[level][y - 1][x] == '=' || map[level][y + 1][x] == '=') ? '|' : '_';
                  if(map[level][y][x] == '|' &&  (map[level][y-1][x] == ' ' ||map[level][y-1][x] == '#') )map[level][y][x] = '_';
                }
            }
        }
    }
     update_room_doors(rooms,MAX_ROOMS);

    //clear();
    if(level==0){
 
    random_stair= rand() % 6;
            stair[level].x = rooms[random_stair].x + 2 + rand() % (rooms[random_stair].width -4);
            stair[level].y = rooms[random_stair].y +2 + rand() % (rooms[random_stair].height-4);
}
       map[level][stair[0].y][stair[0].x]='<';

if(level!=0){
    for (int y = rooms[random_stair].y; y < rooms[random_stair].y + rooms[random_stair].height; y++) {
        for (int x = rooms[random_stair].x; x < rooms[random_stair].x + rooms[random_stair].width; x++) {
            visibility[level][y][x] = 1;  // Set to visible
        }
    }
}


    level++;}
}


int get_current_room_id(int x, int y) {
    for (int i = 0; i < 6; i++) {
        Room r = rooms[i+(level*6)];
        if (x >= r.x && x < r.x + r.width &&
            y >= r.y && y < r.y + r.height) {
            return i;  // Return the room ID
        }
    }
    return -1;  // If not in any room
}
void reveal_room(int currentx, int currenty) {
    int show_message=0;
 //update_message_window("you have entered a new room!");
    int room_id = get_current_room_id(currentx, currenty);
 if(room_id!=-1){
    Room r = rooms[(level * 6) + room_id];
 if(strcmp(r.theme,"nightmare")!=0){
    for (int y = r.y; y < r.y + r.height; y++) {
        for (int x = r.x; x < r.x + r.width; x++) {
            if(visibility[level][y][x] == 0){
               show_message=1; 
            visibility[level][y][x] = 1;  // Set to visible
           mvprintw(y,x,"%c",map[level][y][x]);}
        }
    }}
    if(show_message)update_message_window("you have entered a new room!");
 }
}

Player * playersetup(){
    Player * newplayer;
newplayer=malloc(sizeof(Player));
point starting_position=valid_empty_place(0,0);
newplayer->positions.x=starting_position.x;
newplayer->positions.y=starting_position.y;
newplayer->health=MAX_HEALTH *dificulty_mode;
newplayer->gold=0;
newplayer->hunger=0;
newplayer->damage_spells=0;
newplayer->health_spells=0;
newplayer->speed_spells=0;

strcpy(newplayer->active_weapon.type,"mace");
newplayer->active_weapon.damage=5;
//newplayer->owned_weapons[0]=newplayer->active_weapon;
//newplayer->weapons_count=1;
newplayer->dagger_count=0;
newplayer->Mace_count=1;
newplayer->sword_count=0;
newplayer->magic_wound_count=0;
newplayer->arrow_count=0;
newplayer->active_spell=0;
newplayer->active_spell_timer=0;
reveal_room(newplayer->positions.x,newplayer->positions.y );
attron(COLOR_PAIR(1));
   mvprintw(newplayer->positions.y,newplayer->positions.x,"@");    
  attroff(COLOR_PAIR(1));
  update_status_window(newplayer);
 return newplayer;   
}
void update_visibility_on_move(int oldx,int oldy,int currentx,int currenty) {
    int x = currentx;
    int y = currenty;
    int room_id = get_current_room_id(currentx, currenty);
    Room r2 = rooms[(level * 6) + 5];
         for (int y = r2.y; y < r2.y + r2.height; y++) {
          for (int x = r2.x; x < r2.x + r2.width; x++) {mvprintw(y, x, " ");}}
    if(room_id==5){
         Room r = rooms[(level * 6) + room_id];
         for (int y = r.y; y < r.y + r.height; y++) {
          for (int x = r.x; x < r.x + r.width; x++) {mvprintw(y, x, " ");}}
             for (int dy = -3; dy <= 3; dy++) {
             for (int dx = -3; dx <= 3; dx++) {
              int nx = x + dx;
              int ny = y + dy;
               // Ensure we're within bounds
                if (nx >= 0 && nx < max_x && ny >= 0 && ny < max_y) {
                // Only update tiles that are not already visible
                if (visibility[level][ny][nx] == 0 &&( map[level][ny][nx] != '#' && map[level][ny][nx] != ' ')) {
                   // visibility[level][ny][nx] = 1;
                    if(map[level][ny][nx]=='%'){attron(COLOR_PAIR(4));}else{attron(COLOR_PAIR(2));}
                    mvprintw(ny, nx, "%c", map[level][ny][nx]);  // Print the newly visible tile
                    if(map[level][ny][nx]=='%'){attroff(COLOR_PAIR(4));}else{attroff(COLOR_PAIR(2));}
                }
            }
        }
    }
}else{
    // First, mark the current tile as visible
    if (visibility[level][y][x] == 0) {
        visibility[level][y][x] = 1;
        mvprintw(y, x, "%c", map[level][y][x]);  // Print the newly visible tile
    }
    // Update the next 5 tiles in the path (only for movement on a path)
//    int direction_x = 0;
//     int direction_y = 0;
//   direction_x=currentx-oldx;
//   direction_y=currenty-oldy;
//     // Now update the next 5 tiles in the path
//     for (int i = 1; i <= 5; i++) {
//         int next_x = x + direction_x * i;
//         int next_y = y + direction_y * i;
//         // Ensure we're within bounds
//         if (next_x >= 0 && next_x < max_x && next_y >= 0 && next_y < max_y) {

//             if (visibility[level][next_y][next_x] == 0 && map[level][next_y][next_x] != '.') {
//                 visibility[level][next_y][next_x] = 1;
//                 mvprintw(next_y, next_x, "%c", map[level][next_y][next_x]);  // Print the newly visible tile
//             }
//         }
//     }
if((map[level][y][x]=='#' || map[level][y][x]=='+'|| map[level][y][x]=='?')){
    if(map[level][y][x]=='#' )update_monster_range();
for (int dy = -3; dy <= 3; dy++) {
        for (int dx = -3; dx <= 3; dx++) {
            int nx = x + dx;
            int ny = y + dy;
            // Ensure we're within bounds
            if (nx >= 0 && nx < max_x && ny >= 0 && ny < max_y) {
                // Only update tiles that are not already visible

                if (visibility[level][ny][nx] == 0 &&( map[level][ny][nx] == '#'||map[level][ny][nx] == '+')) {
                    if(right_path(nx,ny,x,y) || right_path(x,y,nx,ny)){
                                        visibility[level][ny][nx] = 1;

                                        mvprintw(ny, nx, "%c", map[level][ny][nx]); } // Print the newly visible tile

                                    }
                }
            }
        }
    }

    // If the player enters a door, reveal the whole room
    if (map[level][y][x] == '+'||map[level][y][x] == '?') {

        reveal_room(x, y);  // Call function to reveal the whole room

    }
  }
}

void check_traps(Player *player) {
    int room_id = get_current_room_id(player->positions.x, player->positions.y);
    if (room_id != -1) {
        Room r = rooms[(level * 6) + room_id];
        for (int i = 0; i < r.trap_count; i++) {
        Trap current = r.traps[i];
            if (current.position.x == player->positions.x &&  current.position.y == player->positions.y) {
                player->health -= current.damage*(4-dificulty_mode); // Trap damage
                update_message_window("You stepped on a trap!");
                map[level][current.position.y][current.position.x]='^';
                update_status_window(player);
                break;

            }

        }
    }
}
int monster_exist(int x, int y){
    int room_id = get_current_room_id(x, y);
 if (room_id != -1) {
  Room r = rooms[(level * 6) + room_id];
    for (int i = 0; i < r.monster_count; i++) {
    monster current = r.monsters[i];
            if (current.health>0 && current.position.x == x &&  current.position.y == y) {
              //  update_message_window("there is a monster you cant move");
                return 1;
            }else{return 0;}
    }
 }
 return 0;
}
void check_hidden_doors(int newx,int newy) {
    int room_id = get_current_room_id(newx, newy);
    if (room_id != -1) {
        Room r = rooms[(level * 6) + room_id];
        for (int i = 0; i < r.hiddenDoor_count; i++) {
        point current = r.hiddenDoors[i];
            if (current.x == newx &&  current.y == newy) {
               update_message_window("You discovered a hidden door!");
                map[level][current.y][current.x]='?';
                attron(COLOR_PAIR(7));
                mvprintw(current.y,current.x,"?");
                attroff(COLOR_PAIR(7));
                refresh();
                break;
            }
        }
    }
}
void search_traps_and_hidden_doors(int x,int y){
for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            int nx = x + dx;
            int ny = y + dy;
             check_hidden_doors(nx,ny);
             check_traps_placement(nx,ny);  
        }
    }
}
void check_traps_placement(int x,int y) {
    int room_id = get_current_room_id(x,y);
    if (room_id != -1) {
        Room r = rooms[(level * 6) + room_id];
        for (int i = 0; i < r.trap_count; i++) {
        Trap current = r.traps[i];
            if (current.position.x == x &&  current.position.y == y) {
                map[level][current.position.y][current.position.x]='^';
               attron(COLOR_PAIR(2));
                mvprintw(current.position.y,current.position.x,"^");
                attroff(COLOR_PAIR(2));
                refresh();
                break;
                

            }

        }
    }
}
void check_valid_move (Player * player,int newx,int newy){

char past_character=map[level][player->positions.y][player->positions.x];
// switch (character)
// {
// case '.':
//    playermove(y,x,);
//     break;
//  case '#':
   
//     break;
//  case '>':
   
//  break; 
// default:
//     break;
// }

char character=map[level][newy][newx];
//character=map[level][newy][newx];
 
if(monster_exist( newx, newy)){
    adjust_color();
 attron(COLOR_PAIR(1));
   mvprintw(player->positions.y,player->positions.x,"@");    
  attroff(COLOR_PAIR(1));
     return;}
else if((!monster_exist( newx, newy))&&(character=='.' ||character=='#' ||character=='<' ||character=='+' ||character=='^'||character=='?'||character=='%' ||character=='$'||character=='f'||character=='i'||character=='e'||character=='x'||character=='y'||character=='z'||character=='a'||character=='d'||character=='s'||character=='m'||character=='c'||character=='D'||character=='M'||character=='A')){
   
    mvprintw(player->positions.y,player->positions.x,"%c",past_character);
    if(character=='%' ||character=='$'){process_gold(newx,newy);}
    if(skip_pickup==0){
    if(character=='f'||character=='i'||character=='e'){process_food( newx,newy);}
    if(character=='x'|| character=='y'||character=='z'){process_spell( newx,newy);}
    if(character=='A'||character=='D'||character=='M'||character=='a'||character=='d'||character=='s'||character=='m'||character=='c'){process_weapon( newx,newy);}
    }
    if(skip_pickup==1){skip_pickup=0;}
    update_visibility_on_move(player->positions.x,player->positions.y,newx,newy);
     adjust_color();
   player->positions.y=newy;
   player->positions.x=newx;
   attron(COLOR_PAIR(1));
   mvprintw(player->positions.y,player->positions.x,"@");    
  attroff(COLOR_PAIR(1));
  if(character=='.'||character=='^') check_traps(player);
}
// else if(character=='%' ||character=='$'){
//  mvprintw(player->positions.y,player->positions.x,"%c",past_character);
//    process_item( newx,newy);
//     if (visibility[level][player->positions.y][player->positions.x] == 0) {
//         visibility[level][player->positions.y][player->positions.x] = 1;
//         mvprintw(player->positions.y, player->positions.x, "."); 
//     }
//      adjust_color();
//    player->positions.y=newy;
//    player->positions.x=newx;
//    attron(COLOR_PAIR(1));
//    mvprintw(player->positions.y,player->positions.x,"@");    
//   attroff(COLOR_PAIR(1));
// }
else if(map[level][newy][newx]=='|'|| map[level][newy][newx]=='_'){
check_hidden_doors(newx,newy);
}
else if(character=='E'){
        // char message[50];
        // sprintf(message, "Collect all golds and kill the monsters! %d gold and %d monster left.",rooms[20].gold_count,rooms[20].monster_count);
        // update_message_window(message);
   if((rooms[20].monster_count<=0) && (rooms[20].gold_count<=0)){ player->gold+=200;win=1;}}

  // Check if the character is a weapon
// int room_id = get_current_room_id(player->positions.x,player->positions.y);
// Room room = rooms[(level *6) + room_id ];
//   // Check if the player's position matches a weapon's position
// for (int j = 0; j < room.weapon_count; j++) {

//  Weapon weapon = room.weapons[j];
//  if ((weapon.position.x == player->positions.x ||weapon.position.x+1 == player->positions.x)&& weapon.position.y == player->positions.y) {

//     // Add the weapon to the player's inventory
//     player->owned_weapons[player->weapons_count++] = weapon;
//     // mvprintw(1, 1, "Collected %s!", weapon->name);
//     // Remove the weapon from the map
//     map[level][weapon.position.y][weapon.position.x] = '.';
//     map[level][weapon.position.y][weapon.position.x+1] = '.';
//     // Remove the weapon from the room's weapons list
//     for (int k = j; k < room.weapon_count - 1; k++) {
//       room.weapons[k] = room.weapons[k + 1];}
//      room.weapon_count--;
//      break;
   // }
 //  }
 
}
void process_food(int x, int y) {
    int room_id = get_current_room_id(x, y);
    Room *room = &rooms[(level * 6) + room_id];
    for (int i = 0; i < room->food_count; i++) {
        food current_food = room->foods[i];
        if (current_food.position.x == x && current_food.position.y == y) {
            if (player->food_count < 5) {
                player->food_inventory[player->food_count++] = current_food;
                map[level][y][x] = '.'; // Remove food from map
                update_message_window("You collected food!");
            } else {
                update_message_window("Food inventory is full!");
            }
            // Remove food from room
            for (int j = i; j < room->food_count - 1; j++) {
                room->foods[j] = room->foods[j + 1];
            }
            room->food_count--;
            break;
        }
    }
}
void process_weapon(int x, int y) {
    
    if( map[level][y][x]=='A'){player->arrow_count+=1;map[level][y][x] = '.' ;return;}
    if( map[level][y][x]=='M'){player->magic_wound_count+=1;map[level][y][x] = '.' ;return;}
    if( map[level][y][x]=='D'){player->dagger_count+=1;map[level][y][x] = '.' ;return;}


    int room_id = get_current_room_id(x, y);
    Room *room = &rooms[(level * 6) + room_id];
    for (int i = 0; i < room->weapon_count; i++) {
        Weapon current_weapon = room->weapons[i];
        if (current_weapon.position.x == x && current_weapon.position.y == y) {
            if(strcmp(current_weapon.type,"dagger")==0){ player->dagger_count+=current_weapon.count;}
            else if(strcmp(current_weapon.type,"arrow")==0) player->arrow_count+=current_weapon.count;
            else  if(strcmp(current_weapon.type,"sword")==0) player->sword_count=1;
            else  if(strcmp(current_weapon.type,"magic_wound")==0) player->magic_wound_count+=current_weapon.count;
                update_message_window("Weapon collected!");
                map[level][y][x] = '.'; // Remove food from map
               
            // Remove food from room
            for (int j = i; j < room->weapon_count - 1; j++) {
                room->weapons[j] = room->weapons[j + 1];
            }
            room->weapon_count--;
            break;
        }
    }
}
void process_spell(int x, int y) {
    int room_id = get_current_room_id(x, y);
    Room *room = &rooms[(level * 6) + room_id];
    for (int i = 0; i < room->spell_count; i++) {
        spell current_spell = room->spells[i];
        if (current_spell.position.x == x && current_spell.position.y == y) {
            if(strcmp(current_spell.type,"damage")==0) player->damage_spells++;
            else if(strcmp(current_spell.type,"health")==0) player->health_spells++;
            else  if(strcmp(current_spell.type,"speed")==0) player->speed_spells++;
                update_message_window("Spell collected!");
                map[level][y][x] = '.'; // Remove food from map
               
            // Remove food from room
            for (int j = i; j < room->spell_count - 1; j++) {
                room->spells[j] = room->spells[j + 1];
            }
            room->spell_count--;
            break;
        }
    }
}
void process_gold(int x,int y){
int room_id = get_current_room_id(x,y);
Room *room = &rooms[(level *6) + room_id ];
for (int j = 0; j < room->gold_count; j++) {
 gold new_gold = room->golds[j];
 if ((new_gold.position.x == x )&& new_gold.position.y == y) {
   if(room_id!=5) player->gold+= new_gold.amount;

    wclear(message_window);
    box(message_window, 0, 0);
    mvwprintw(message_window, 1, 1, "you picked up %d gold",new_gold.amount);
    wrefresh(message_window);
    update_status_window(player);
    map[level][y][x] = '.';
    for (int k = j; k < room->gold_count - 1; k++) {
      room->golds[k] = room->golds[k + 1];}
     room->gold_count--;
     break;
   }
  }
}
void print_map() {
    for (int y = 3; y < max_y; y++) {
        for (int x = 0; x < max_x; x++) {
            if (visibility[level][y][x] == 1) {
                mvprintw(y, x, "%c", map[level][y][x]);
            }else{mvprintw(y, x, " ");}
        }
    }
}
void adjust_color() {
for(int q=0;q<6;q++){
     Room r = rooms[(level * 6) + q];

    for (int y = r.y; y < r.y + r.height; y++) {
        for (int x = r.x; x < r.x + r.width; x++) {

            char tile = map[level][y][x];
            if (visibility[level][y][x]) {
                // Apply colors based on the feature
                if (tile == '^') { // Trap
                    attron(COLOR_PAIR(2)); // Trap color
                    mvprintw(y, x, "%c", tile);
                    attroff(COLOR_PAIR(2));
                } else if (tile == '?'|| tile == '$') { // Hidden door
                    attron(COLOR_PAIR(7)); // Hidden door color
                    mvprintw(y, x, "%c", tile);
                    attroff(COLOR_PAIR(7));
                    } else if (tile == '%') { // Hidden door
                    attron(COLOR_PAIR(4)); // Hidden door color
                    mvprintw(y, x, "%c", tile);
                    attroff(COLOR_PAIR(4));
                    } else if (tile == 'f' || tile == 'i' || tile == 'e') { // Hidden door
                    attron(COLOR_PAIR(9)); // Hidden door color
                    mvprintw(y, x, "%c", tile);
                    attroff(COLOR_PAIR(9));
                    } else if (tile == 'x'||tile == 'y'||tile == 'z') { // Hidden door
                    attron(COLOR_PAIR(8)); // Hidden door color
                    mvprintw(y, x, "%c", tile);
                    attroff(COLOR_PAIR(8));
                    } else if (tile == 'a' ||tile == 'A') { 
                    attron(COLOR_PAIR(5)); 
                    mvprintw(y, x, "\u27B3");
                    attroff(COLOR_PAIR(5));
                    } else if (tile == 's') { // Hidden door
                    attron(COLOR_PAIR(5)); // Hidden door color
                    mvprintw(y, x, "\u2625");
                    attroff(COLOR_PAIR(5));
                    } else if (tile == 'm'||tile == 'M') { // Hidden door
                    attron(COLOR_PAIR(5)); // Hidden door color
                    mvprintw(y, x, "★");
                    attroff(COLOR_PAIR(5));
                    } else if (tile == 'd' ||tile == 'D') { // Hidden door
                    attron(COLOR_PAIR(5)); // Hidden door color
                    mvprintw(y, x, "\u262D");
                    attroff(COLOR_PAIR(5));
                    } else if (tile == 'c') { // Hidden door
                    attron(COLOR_PAIR(5)); // Hidden door color
                    mvprintw(y, x, "\u2628");
                    attroff(COLOR_PAIR(5));
                    // } else if (tile == 'D'||tile == 'S'||tile == 'F'||tile == 'G'||tile == 'U') { // Hidden door
                    // attron(COLOR_PAIR(3)); // Hidden door color
                    // mvprintw(y, x, "%c", tile);
                    // attroff(COLOR_PAIR(3));
                // } else if (tile == '<' || tile == '>') { // Stairs
                //     attron(COLOR_PAIR(4)); // Stairs color
                //     mvprintw(y, x, "%c", tile);
                //     attroff(COLOR_PAIR(4));
                } else { // Default rendering
                     attron(COLOR_PAIR(r.color));
                     mvprintw(y, x, "%c", tile);
                     attroff(COLOR_PAIR(r.color));
                }
            }
        }
    }
if(r.monster_count>0 && visibility[level][r.monsters[0].position.y][r.monsters[0].position.x]==1 && r.monsters[0].health>0){
   mvprintw(r.monsters[0].position.y, r.monsters[0].position.x, "%c", r.monsters[0].type);}
   }
   for (int y = 3; y < max_y; y++) {
        for (int x = 0; x < max_x; x++) {
           if(map[level][y][x]=='#' && visibility[level][y][x]==1) mvaddch(y, x, map[level][y][x]);
        }
    } 
}
void print_whole_map(){
 for(int q=0;q<6;q++){
     Room r = rooms[(level * 6) + q];

    for (int y = r.y; y < r.y + r.height; y++) {
        for (int x = r.x; x < r.x + r.width; x++) {

            char tile = map[level][y][x];
                // Apply colors based on the feature
                if (tile == '^') { // Trap
                    attron(COLOR_PAIR(2)); // Trap color
                    mvprintw(y, x, "%c", tile);
                    attroff(COLOR_PAIR(2));
                } else if (tile == '?' || tile == '$') { // Hidden door
                    attron(COLOR_PAIR(7)); // Hidden door color
                    mvprintw(y, x, "%c", tile);
                    attroff(COLOR_PAIR(7));
                    } else if (tile == '%') { // Hidden door
                    attron(COLOR_PAIR(4)); // Hidden door color
                    mvprintw(y, x, "%c", tile);
                    attroff(COLOR_PAIR(4));
                    } else if (tile == 'f' || tile == 'i' || tile == 'e') { // Hidden door
                    attron(COLOR_PAIR(9)); // Hidden door color
                    mvprintw(y, x, "%c", tile);
                    attroff(COLOR_PAIR(9));
                    } else if (tile == 'x'||tile == 'y'||tile == 'z') { // Hidden door
                    attron(COLOR_PAIR(8)); // Hidden door color
                    mvprintw(y, x, "%c", tile);
                    attroff(COLOR_PAIR(8));
                    } else if (tile == 'a'||tile == 'A') { 
                    attron(COLOR_PAIR(5)); 
                    mvprintw(y, x, "\u27B3");
                    attroff(COLOR_PAIR(5));
                    } else if (tile == 's') { // Hidden door
                    attron(COLOR_PAIR(5)); // Hidden door color
                    mvprintw(y, x, "\u2625");
                    attroff(COLOR_PAIR(5));
                    } else if (tile == 'm'||tile == 'M') { // Hidden door
                    attron(COLOR_PAIR(5)); // Hidden door color
                    mvprintw(y, x, "★");
                    attroff(COLOR_PAIR(5));
                    } else if (tile == 'd'||tile == 'D') { // Hidden door
                    attron(COLOR_PAIR(5)); // Hidden door color
                    mvprintw(y, x, "\u262D");
                    attroff(COLOR_PAIR(5));
                    } else if (tile == 'c') { // Hidden door
                    attron(COLOR_PAIR(5)); // Hidden door color
                    mvprintw(y, x, "\u2628");
                    attroff(COLOR_PAIR(5));
                    //  } else if (tile == 'D'||tile == 'S'||tile == 'F'||tile == 'G'||tile == 'U') { // Hidden door
                    // attron(COLOR_PAIR(3)); // Hidden door color
                    // mvprintw(y, x, "%c", tile);
                    // attroff(COLOR_PAIR(3));
                // } else if (tile == '<' || tile == '>') { // Stairs
                //     attron(COLOR_PAIR(4)); // Stairs color
                //     mvprintw(y, x, "%c", tile);
                //     attroff(COLOR_PAIR(4));
                } else { // Default rendering
                     attron(COLOR_PAIR(r.color));
                     mvprintw(y, x, "%c", tile);
                     attroff(COLOR_PAIR(r.color));
                }
            
        }
    }
    if(r.monster_count>0  && r.monsters[0].health>0){
   mvprintw(r.monsters[0].position.y, r.monsters[0].position.x, "%c", r.monsters[0].type);}
}  

for (int y = 3; y < max_y; y++) {
        for (int x = 0; x < max_x; x++) {
           if(map[level][y][x]=='#') mvaddch(y, x, map[level][y][x]);
        }
    } 
}

int is_in_enchanted_room(Player *player) {
    int room_id = get_current_room_id(player->positions.x, player->positions.y);
    if (room_id != -1) {
        Room r = rooms[(level * 6) + room_id];
        if (strcmp(r.theme, "enchant") == 0) {
            return 1; // Player is in a dangerous room
        }
    }
    return 0; // Player is not in a dangerous room
}
// void handle_input(int input,Player *player){
//     int newx=player->positions.x;
//     int newy=player->positions.y;
//      int fast_move = 0;
//      //wclear(message_window);
//     //box(message_window, 0, 0);
//     //wrefresh(message_window);
//     //update_clear_message_swindow();
//      if (input == 'f') {
  //      fast_move = 1;  // Enable fast movement
   //     input = getch();  // Read the next input for direction
  //  }
//     switch(input){
        
//         case KEY_RIGHT:
//         if(map[level][newy][newx]=='<' && level<3){level++;

//          print_map();
//          adjust_color();
//          update_message_window("you have ascended to the next floor!");
//          update_status_window(player);
//          attron(COLOR_PAIR(1)); 
//          mvprintw(player->positions.y,player->positions.x,"@");  
//          attroff(COLOR_PAIR(1)); }
//         break;

//          case KEY_LEFT:
//          if(map[level][newy][newx]=='<' && level>0){level--;
//           print_map();
//           adjust_color();
//           update_message_window("you have returned to the previous floor!");
//           update_status_window(player);
//          attron(COLOR_PAIR(1));
//           mvprintw(player->positions.y,player->positions.x,"@");    
//           attroff(COLOR_PAIR(1)); }
//         break;
//         //left
//      case 'h': newx--; break;
//       //up
//      case 'j': newy--; break;
//      //down
//      case 'k': newy++; break;
//      //right
//      case 'l': newx++; break;
//      //up_left
//      case 'y': newx--; newy--; break;
//      //up_right
//      case 'u': newx++; newy--; break;
//      //down_left
//      case 'b': newx--; newy++; break;
//      //down_right
//      case 'n': newx++; newy++; break;
//      //view whole map

//      case 'm': 
//      if(viewing_whole_map==0){
//         viewing_whole_map=1;print_whole_map();
//          attron(COLOR_PAIR(1));
//           mvprintw(player->positions.y,player->positions.x,"@");    
//           attroff(COLOR_PAIR(1));
//          break;}
//      if(viewing_whole_map==1){
//         viewing_whole_map=0;print_map();
//           adjust_color();
//           attron(COLOR_PAIR(1));
//           mvprintw(player->positions.y,player->positions.x,"@");    
//           attroff(COLOR_PAIR(1));
//            break;}
      
//       default: break;
//     }
// if(input!='m')check_valid_move (player,newx,newy);
// if(is_in_enchanted_room(player)){player->health-=1;
// update_status_window(player);
// //update_message_window("your health is deacreasing!");
// }
// refresh();
// }
void handle_input(int input, Player *player) {
    int newx = player->positions.x;
    int newy = player->positions.y;
    int fast_move = 0;  // Flag to determine if fast movement is enabled
    //wclear(message_window);
    //box(message_window, 0, 0);
    //wrefresh(message_window);
    //update_clear_message_swindow();
    if (input == 'f') {
        fast_move = 1;  // Enable fast movement
        input = getch();  // Read the next input for direction
    }else if (input == 'g') {
        skip_pickup = 1;  // Enable fast movement
        input = getch();
    }else if(input == 's'){
        search_traps_and_hidden_doors(player->positions.x,player->positions.y);
    return;
    }
    else if(input == ' '){
        move_monsters(player);
     attack_monster(player);
     adjust_color();
      attron(COLOR_PAIR(1));
     mvprintw(player->positions.y, player->positions.x, "@");
     attroff(COLOR_PAIR(1));
     return;
    }
    else if(input == 'e'){
     display_food_menu(player);
    }
    else if(input == 'z'){
     display_spell_menu(player);
    }
    else if(input == 'i'){
     display_weapon_menu(player);
    }
    switch (input) {

        case KEY_RIGHT:

            if (map[level][newy][newx] == '<' && level < 3) {
                level++;
                update_status_window(player);
                print_map();
                adjust_color();
                update_message_window("You have ascended to the next floor!");
                attron(COLOR_PAIR(1));
                mvprintw(player->positions.y, player->positions.x, "@");
                attroff(COLOR_PAIR(1));
            }
            break;
        case KEY_LEFT:

            if (map[level][newy][newx] == '<' && level > 0) {
                level--;
                update_status_window(player);
                print_map();
                adjust_color();
                update_message_window("You have returned to the previous floor!");
                attron(COLOR_PAIR(1));
                mvprintw(player->positions.y, player->positions.x, "@");
                attroff(COLOR_PAIR(1));
            }
            break;
        // Movement directions
    case 'h': newx -= (player->active_spell == 1) ? 2 : 1; break;  // Left
    case 'j': newy -= (player->active_spell == 1) ? 2 : 1; break;  // Up
    case 'k': newy += (player->active_spell == 1) ? 2 : 1; break;  // Down
    case 'l': newx += (player->active_spell == 1) ? 2 : 1; break;  // Right
    case 'y': newx -= (player->active_spell == 1) ? 2 : 1; newy -= (player->active_spell == 1) ? 2 : 1; break;  // Up-left
    case 'u': newx += (player->active_spell == 1) ? 2 : 1; newy -= (player->active_spell == 1) ? 2 : 1; break;  // Up-right
    case 'b': newx -= (player->active_spell == 1) ? 2 : 1; newy += (player->active_spell == 1) ? 2 : 1; break;  // Down-left
    case 'n': newx += (player->active_spell == 1) ? 2 : 1; newy += (player->active_spell == 1) ? 2 : 1; break;  // Down-right
       //view whole map

        case 'm': 
           if(viewing_whole_map==0){
             viewing_whole_map=1;print_whole_map();
             attron(COLOR_PAIR(1));
             mvprintw(player->positions.y,player->positions.x,"@");    
             attroff(COLOR_PAIR(1));
            break;}
          if(viewing_whole_map==1){
             viewing_whole_map=0;print_map();
             adjust_color();
             attron(COLOR_PAIR(1));
             mvprintw(player->positions.y,player->positions.x,"@");    
             attroff(COLOR_PAIR(1));
           break;}

        default: break;

    }
    // Handle fast movement
    if (fast_move) {

        while (1) {

            char character = map[level][newy][newx];
            if (character == '.' || character == '#' || character == '+' || character == '^' || character == '?') {
                check_valid_move(player, newx, newy);
                if (is_in_enchanted_room(player)) {
                    player->health -= 1;
                    update_status_window(player);
                    //update_message_window("Your health is decreasing4!");
                }
                // Update newx and newy for the next step in the same direction
                switch (input) {
                    case 'h': newx--; break;  // Left
                    case 'j': newy--; break;  // Up
                    case 'k': newy++; break;  // Down
                    case 'l': newx++; break;  // Right
                    case 'y': newx--; newy--; break;  // Up-left
                    case 'u': newx++; newy--; break;  // Up-right
                    case 'b': newx--; newy++; break;  // Down-left
                    case 'n': newx++; newy++; break;  // Down-right
                    default: break;
                }
            } else {
                // Stop when encountering a wall or an item
                break;
            }
        }
    } else {
        // Regular single movement
       if(input!='m' && input!='i'&& input!='e'&& input!='z')move_monsters(player);
         if(input!='m')check_valid_move (player,newx,newy);
         //move_monsters(player);
        if (is_in_enchanted_room(player)) {
            player->health -= 1;
            update_status_window(player);
           // update_message_window("Your health is decreasing3!");
        }
    }
    refresh();
}


// void save_visibility_state() {

//     FILE *file = fopen("visibility_state.txt", "wb");

//     fwrite(visibility, sizeof(int), MAX_X * MAX_Y * 4, file);  // Adjust size as needed

//     fclose(file);

// }



// void load_visibility_state() {

//     FILE *file = fopen("visibility_state.txt", "rb");

//     fread(visibility, sizeof(int), MAX_X * MAX_Y * 4, file);  // Adjust size as needed

//     fclose(file);

// }
void set_up_screen_message(){
 message_window = newwin(3,max_x, 0, 0); 
 status_window = newwin(1, max_x, max_y, 0);
      refresh();
    box(message_window, 0, 0);
    
    mvwprintw(message_window, 1, 1, "Welcome to the game!"); 
    wrefresh(message_window);

   // mvwprintw(status_window, max_y, 1, "Health: 1000  Gold: 0");  // Initial display
    wrefresh(status_window);
}
void update_status_window(Player *player) {

    wclear(status_window);
    wattron(status_window,COLOR_PAIR(5));
    mvwprintw(status_window, 0, 0, "  Health: %d   Gold: %d  Level:%d", player->health, player->gold,level+1);
    wattroff(status_window,COLOR_PAIR(5));
    wrefresh(status_window);

}
void display_food_menu(Player *player) {
    clear();
    attron(COLOR_PAIR(5));
    mvprintw(0, 0, "Food Menu:");
    // Display each food item in the inventory
    for (int i = 0; i < player->food_count; i++) {
      if(strcmp(player->food_inventory[i].type,"normal")==0 || strcmp(player->food_inventory[i].type,"bad")==0){ mvprintw(i + 1, 0, "%d) Food: Restores %d health", i + 1, player->food_inventory[i].value);}
      else{mvprintw(i + 1, 0, "%d) %s food: Restores %d health",i+1,player->food_inventory[i].type, player->food_inventory[i].value);}
    }
    // Display the hunger bar (ribbon)
    mvprintw(player->food_count + 2, 0, "Hunger: [");
    int bar_length = player->hunger / 10; // Scale hunger (max 100) to a 10-unit bar
    for (int i = 0; i < 10; i++) {
        if (i < bar_length) {
            addch('|'); // Filled section of the bar
        } else {
            addch(' '); // Empty section of the bar
        }
    }
    addch(']');
    mvprintw(player->food_count + 4, 0, "Press 1-5 to consume food, or 'q' to return.");
    refresh();
    // Wait for player input to consume foo
    int ch;
    while ((ch = getch()) != 'q') {
        if (ch >= '1' && ch <= '5') {
            int index = ch - '1';
            if (index < player->food_count) {
                // Consume food
                if(strcmp(player->food_inventory[index].type,"bad")==0){player->health -= player->food_inventory[index].value;}
                else{
                player->health += player->food_inventory[index].value;
                player->hunger -= 20;}  
                if(strcmp(player->food_inventory[index].type,"magic")==0){player->active_spell_timer=10;player->active_spell=1;}
                if(strcmp(player->food_inventory[index].type,"gourmet")==0){player->active_weapon.damage*=2;player->active_spell_timer=10;player->active_spell=2;}
                if (player->hunger < 0) { player->hunger = 0;}
                if (player->health > MAX_HEALTH*dificulty_mode) { player->health = MAX_HEALTH*dificulty_mode;}
                // Remove the consumed food from inventory
                for (int j = index; j < player->food_count - 1; j++) {
                    player->food_inventory[j] = player->food_inventory[j + 1];
                }
                player->food_count--;
                // Update the menu to reflect the change
                display_food_menu(player);
                break;
            }
        }
    }
    attroff(COLOR_PAIR(5));
    print_map();
     adjust_color();
     update_message_window("You returned to main map!");
      update_status_window(player);
}
void display_spell_menu(Player *player) {
    clear();
    attron(COLOR_PAIR(5));
    mvprintw(0, 0, "Spell Menu:");

    mvprintw( 2, 1, "1. Speed: %d", player->speed_spells);

    mvprintw( 3, 1, "2. Health: %d", player->health_spells);

    mvprintw(4, 1, "3. Damage: %d", player->damage_spells);

    mvprintw( 6, 1, "Press number to use a spell");
    refresh();
     int ch = getch();
     while (ch != 'q'){
    if (ch == '1' && player->speed_spells > 0) {
        player->speed_spells--;
        player->active_spell=1;
       player->active_spell_timer=10;
       // update_message_window("Speed spell qctivated!");
        display_spell_menu(player);break;
       // update_message_window("Speed spell used!");
    } else if (ch == '2' && player->health_spells > 0) {
       // player->health += 50; // Example effect
        player->health_spells--;
        player->active_spell=3;
        player->active_spell_timer=10;
        display_spell_menu(player);break;
       // update_message_window("Health spell used!");
    } else if (ch == '3' && player->damage_spells > 0) {
       // update_message_window("Damage spell used!");
        player->damage_spells--;
          player->active_weapon.damage*=2;
          player->active_spell=2;
          player->active_spell_timer=10;
        display_spell_menu(player); break;
    }else{break;} 
     }
    // else {

    //     update_message_window("Invalid choice or no spells available!");

    // }
    // Wait for player input to consume foo
    
    attroff(COLOR_PAIR(5));
    print_map();
     adjust_color();
     update_message_window("You returned to main map!");
      update_status_window(player);
}
// void display_spell_menu(Player *player) {
//     clear();
//     attron(COLOR_PAIR(5));
//     mvprintw(0, 0, "Spell Menu:");

//     mvprintw( 2, 1, "1. Speed: %d", player->speed_spells);

//     mvprintw( 3, 1, "2. Health: %d", player->health_spells);

//     mvprintw(4, 1, "3. Damage: %d", player->damage_spells);

//     mvprintw( 6, 1, "Press number to use a spell");
//     refresh();
//      int ch = getch();
//      while (ch != 'q'){
//     if (ch == '1' && player->speed_spells > 0) {
//         player->speed_spells--;
//         player->active_spell=1;
//         player->active_spell_timer=10;
//        // update_message_window("Speed spell qctivated!");
//         display_spell_menu(player);break;
//        // update_message_window("Speed spell used!");
//     } else if (ch == '2' && player->health_spells > 0) {
//        // player->health += 50; // Example effect
//         player->health_spells--;
//          player->active_spell=3;
//         player->active_spell_timer=10;
//        // update_message_window("Health spell activated!");
//         display_spell_menu(player);break;
//        // update_message_window("Health spell used!");
//     } else if (ch == '3' && player->damage_spells > 0) {
//        // update_message_window("Damage spell used!");
//         player->damage_spells--;
//         player->active_weapon.damage*=2;
//          player->active_spell=2;
//         player->active_spell_timer=10;
//         //update_message_window("Damage spell activated!");
//         display_spell_menu(player); break;
//     } 
//      }
//     // else {

//     //     update_message_window("Invalid choice or no spells available!");

//     // }
//     // Wait for player input to consume foo
    
//     attroff(COLOR_PAIR(5));
//     print_map();
//      adjust_color();
//      update_message_window("You returned to main map!");
//       update_status_window(player);
// }
// void display_weapon_menu(Player *player) {
//     clear();
//     attron(COLOR_PAIR(5)); // Use color for the menu
//     mvprintw(0, 0, "Weapon Menu:");
//     // Display current equipped weapon
//     mvprintw(1, 0, "Current Weapon: %s (Damage: %d)", player->active_weapon.type, player->active_weapon.damage);
//     mvprintw(3, 0, "Available Weapons:");
//     int menu_line = 5;
//         mvprintw(menu_line++, 2, "far_range weapons:");
//         mvprintw(menu_line++, 2, "d) Dagger (Count: %d, Damage: 12)", player->dagger_count);
//         mvprintw(menu_line++, 2, "a) Arrow (Count: %d, Damage: 5)", player->arrow_count);
//         mvprintw(menu_line++, 2, "m) Magic Wound (Count: %d, Damage: 15)", player->magic_wound_count);
//         menu_line++;
//         mvprintw(menu_line++, 2, "close_range weapons:");
//         mvprintw(menu_line++, 2, "s) Sword (Count: 1, Damage: 10)");
//         mvprintw(menu_line++, 2, "c) Mace (Count: %d, Damage: 5)", player->Mace_count);
//     mvprintw(menu_line + 1, 0, "Press the corresponding key to equip a weapon, or 'q' to return.");
//     refresh();
//     // Handle user input]
//     int has_weapon=0;
//     int error=0;
//     int ch = getch();
//      while (ch != 'q'){
//         if(ch=='w'){
//              strcpy(player->active_weapon.type, "none");
//             player->active_weapon.damage = 0;
//             display_weapon_menu(player); break;
//         }
//         if (ch == 'd' && player->dagger_count > 0 && strcmp(player->active_weapon.type, "none")==0) {
//             strcpy(player->active_weapon.type, "dagger");
//             player->active_weapon.damage = 12;
//             display_weapon_menu(player);
//             break;
//         } else if (ch == 'a' && player->arrow_count > 0 && strcmp(player->active_weapon.type, "none")==0) {
//             strcpy(player->active_weapon.type, "arrow");
//             player->active_weapon.damage = 5;
//             display_weapon_menu(player);
//             break;
//         } else if (ch == 's' && player->sword_count > 0 && strcmp(player->active_weapon.type, "none")==0) {
//             strcpy(player->active_weapon.type, "sword");
//             player->active_weapon.damage = 10;
//             display_weapon_menu(player);
//             break;
//         } else if (ch == 'm' && player->magic_wound_count > 0 && strcmp(player->active_weapon.type, "none")==0) {
//             strcpy(player->active_weapon.type, "magic_wound");
//             player->active_weapon.damage = 15;
//             display_weapon_menu(player);
//             break;
//         } else if (ch == 'c' && player->Mace_count > 0 && strcmp(player->active_weapon.type, "none")==0) {
//             strcpy(player->active_weapon.type, "mace");
//             player->active_weapon.damage = 5; 
//             display_weapon_menu(player);
//             break;   
//         } 
//         else if ((ch == 'm'||ch == 'c'||ch == 'a'||ch == 's'||ch == 'd')  && strcmp(player->active_weapon.type, "none")!=0) {
//             has_weapon=1;
//             break;   
//         } 
//         else{error=1;break;
//         }
//     }
//     attroff(COLOR_PAIR(5));
//     print_map();
//     adjust_color();
//     if(has_weapon){update_message_window("Put down your weapon first");}
//     else if(error ||strcmp(player->active_weapon.type, "none")==0){update_message_window("Invalid choice or weapon not available!");}
//    //else if(strcmp(player->active_weapon.type, "none")==0) {update_message_window("You returned to the main map!");}
//     else{ wclear(message_window);
//     box(message_window, 0, 0);
//     mvwprintw(message_window, 1, 1, "%s is now your weapon",player->active_weapon.type);
//     wrefresh(message_window);}
//     update_status_window(player);
// }
void display_weapon_menu(Player *player) {
    clear();
    attron(COLOR_PAIR(5)); // Use color for the menu
    mvprintw(0, 0, "Weapon Menu:");
    mvprintw(1, 0, "Current Weapon: %s (Damage: %d)", player->active_weapon.type, player->active_weapon.damage);
    mvprintw(3, 0, "Available Weapons:");
    int menu_line = 5;
    // Far-range weapons
    mvprintw(menu_line++, 2, "far_range weapons:");
    mvprintw(menu_line++, 2, "d) Dagger (Count: %d, Damage: 12)", player->dagger_count);
    mvprintw(menu_line++, 2, "a) Arrow (Count: %d, Damage: 5)", player->arrow_count);
    mvprintw(menu_line++, 2, "m) Magic Wound (Count: %d, Damage: 15)", player->magic_wound_count);
    menu_line++;
    // Close-range weapons
    mvprintw(menu_line++, 2, "close_range weapons:");
    mvprintw(menu_line++, 2, "s) Sword (Count: 1, Damage: 10)");
    mvprintw(menu_line++, 2, "c) Mace (Count: %d, Damage: 5)", player->Mace_count);
    mvprintw(menu_line + 1, 0, "Press 'w' to put down your weapon, or 'q' to return.");
    refresh();
    int has_weapon = 0; 
    int error = 0;     
    int ch = getch();
    while (ch != 'q') {

        if (ch == 'w') {

            if (strcmp(player->active_weapon.type, "none") != 0) {
                strcpy(player->active_weapon.type, "none");
                player->active_weapon.damage = 0;
            } 
            
            display_weapon_menu(player); 
            break;
        } else if (ch == 'd' && player->dagger_count > 0 && strcmp(player->active_weapon.type, "none") == 0) {
            strcpy(player->active_weapon.type, "dagger");
            player->active_weapon.damage = 12;
            break;
        } else if (ch == 'a' && player->arrow_count > 0 && strcmp(player->active_weapon.type, "none") == 0) {
            strcpy(player->active_weapon.type, "arrow");
            player->active_weapon.damage = 5;
            break;
        } else if (ch == 's' && player->sword_count > 0 && strcmp(player->active_weapon.type, "none") == 0) {
            strcpy(player->active_weapon.type, "sword");
            player->active_weapon.damage = 10;
            break;
        } else if (ch == 'm' && player->magic_wound_count > 0 && strcmp(player->active_weapon.type, "none") == 0) {
            strcpy(player->active_weapon.type, "magic_wound");
            player->active_weapon.damage = 15;
            break;
        } else if (ch == 'c' && player->Mace_count > 0 && strcmp(player->active_weapon.type, "none") == 0) {
            strcpy(player->active_weapon.type, "mace");
            player->active_weapon.damage = 5;
            break;
        } else if ((ch == 'm' || ch == 'c' || ch == 'a' || ch == 's' || ch == 'd') && strcmp(player->active_weapon.type, "none") != 0) {
            has_weapon = 1;break;
        } else { error = 1; break;
        }
    }
    attroff(COLOR_PAIR(5));
    print_map();
    adjust_color();
    if (has_weapon) {
        update_message_window("Put down your weapon first.");
    } else if (error) {
        update_message_window("Invalid choice or weapon not available!");
    } else if (strcmp(player->active_weapon.type, "none") == 0) {
        update_message_window("You returned to the main map!");
    } else {
        char message[50];
        sprintf(message, "%s is now your weapon.", player->active_weapon.type);
        update_message_window(message);
    }
    update_status_window(player);
}
void check_hunger_and_health() {
   if(player->active_spell_timer>0 ){
    player->active_spell_timer--;
    if(player->active_spell==3){player->health+=10;
     if (player->health > MAX_HEALTH*dificulty_mode) {player->health = MAX_HEALTH*dificulty_mode;}}

    if(player->active_spell_timer==0){
         if(player->active_spell==2){player->active_weapon.damage/=2;}
      player->active_spell=0;
        }
   }
   
    time_t current_time = time(NULL);
    // Check if 5 seconds have passed
   
    if (difftime(current_time, last_update_time) >= 10) {
        last_update_time = current_time;  // Update the last time the hunger check occurred
        // Now update hunger and healt
        player->hunger += 10;
         if ( player->hunger <= 20) {
        player->health += 10;
        if (player->health > MAX_HEALTH*dificulty_mode) {
            player->health = MAX_HEALTH*dificulty_mode;  // Ensure health doesn't exceed max
        }
        //update_message_window("You are healing.");
        }
         // Increase hunger over time
        else if (player->hunger >= 100) {
            player->hunger = 100;
            player->health -= 5;
            update_message_window("You are starving!");
            if (player->health <= 0) {
                update_message_window("You have starved to death!");
                player->health = 0; // Prevent negative health
            }
        }
       update_status_window(player);
    }


}
void generate_monsters() {
    for (int i = 0; i < 24; i++) {
        rooms[i].monster_count = 0;
        if (i%6!=5 && rooms[i].width * rooms[i].height > 84) {  // Big rooms only
            rooms[i].monster_count = 1;  
            for (int j = 0; j < rooms[i].monster_count; j++) {
                monster monster;
                point pos = valid_empty_place(i/6, i%6);
                monster.position = pos;
                // Randomly assign monster type
                int type = rand() % 5;
                switch (type) {
                    case 0: monster.type = 'D'; monster.health = 5; monster.damage = 1; monster.range = 10; break;
                    case 1: monster.type = 'F'; monster.health = 10; monster.damage = 2; monster.range = 10; break;
                    case 2: monster.type = 'G'; monster.health = 15; monster.damage = 3; monster.range = 15; break;
                    case 3: monster.type = 'S'; monster.health = 20; monster.damage = 4; monster.range = 999; break;
                    case 4: monster.type = 'U'; monster.health = 30; monster.damage = 5; monster.range = 15; break;
                }
                monster.current_range=monster.range;
                rooms[i].monsters[j] = monster;
               // map[i/6][pos.y][pos.x] = monster.type;
            }
        }
    }
}
// void place_special_items(){
//     for(int j=0;j<24 ;j++){
//         if(j%6!=5){
//    rooms[j].gold_count = 0;
//     int num_gold = (rooms[j].width*rooms[j].height<70)?1+rand() % 2:1+rand() % 4;
//     if(j==20)num_gold=1;
//     for (int i = 0; i < num_gold; i++) {
//         gold new_gold;
//             point starting_position=valid_empty_place(j/6,j%6);
//            // if(map[level][starting_position.y][starting_position.x]!='.'){continue;}
//            new_gold.position.x=starting_position.x;
//             new_gold.position.y=starting_position.y; 
//              new_gold.amount=1+rand()%5;
//              map[j/6][new_gold.position.y][new_gold.position.x]='%';
//         rooms[j].golds[rooms[j].gold_count++] = new_gold;
//     } }
void move_monsters(Player *player) {
    int room_id = get_current_room_id(player->positions.x, player->positions.y);
    if (room_id == -1) return;
    Room *room = &rooms[(level * 6) + room_id];
    for (int i = 0; i < room->monster_count; i++) {
        monster *monster = &room->monsters[i];
        if (monster->health <= 0) continue;  // Skip dead monsters
        // Move towards the player
        int dx = player->positions.x - monster->position.x;
        int dy = player->positions.y - monster->position.y;
        if (abs(dx)+ abs(dy) <= 1 ) {
            // Attack the player if adjacent
            player->health -= monster->damage;
            update_message_window("A monster attacked you!");
            update_status_window(player);
            continue;
        } else if( monster->current_range>0){
        //if (abs(dx) <= monster.range && abs(dy) <= monster.range) {
            // Move one step closer
            int newx=monster->position.x;int newy=monster->position.y;
            monster->current_range--;
            if (abs(dx) > abs(dy)) {
                 newx += (dx > 0) ? 1 : -1;
            } else {
                newy += (dy > 0) ? 1 : -1;
            }
          if(map[level][newy][newx]!='|' && map[level][newy][newx]!='_' && map[level][newy][newx]!='='&& map[level][newy][newx]!='+' && map[level][newy][newx]!='?'){
            monster->position.x=newx;
            monster->position.y=newy;}
        }
        
    }
}
// void move_monsters(Player *player) {
//     int room_id = get_current_room_id(player->positions.x, player->positions.y);
//     if (room_id == -1) return; // Only move monsters in the same room
//     Room *room = &rooms[(level * 6) + room_id];
//     for (int i = 0; i < room->monster_count; i++) {
//         monster *monster = &room->monsters[i];
//         if (monster->health <= 0) continue;  // Skip dead monsters
//         // Restore the tile the monster was on before moving
//         // Decide movement based on range
//         int dx = player->positions.x - monster->position.x;
//         int dy = player->positions.y - monster->position.y;
//         int distance = abs(dx) + abs(dy);
//         if (abs(dx)==1 || abs(dy)==1) {
//             // Attack if next to the player
//             player->health -= monster->damage;
//             update_message_window("A monster attacked you!");
//             update_status_window(player);
//             continue;
//         } else {
//         //if (distance <= monster->range) {
//             // Move one step closer to the player
//             int newx = monster->position.x;
//             int newy = monster->position.y;
//             if (abs(dx) > abs(dy)) {
//                 newx += (dx > 0) ? 1 : -1;
//             } else {
//                 newy += (dy > 0) ? 1 : -1;
//             }
//             // Validate movement (monsters can move onto items but not walls)
//             char target_tile = map[level][newy][newx];
//             if (target_tile != '=' && target_tile != '|' && target_tile != '_' && target_tile != '+' && target_tile != '?'){
//             // (target_tile == '.' || target_tile == '%' || target_tile == '$' || 
//             //     target_tile == 'f' || target_tile == 'a' || target_tile == 'd' || 
//             //     target_tile == 's' || target_tile == 'm' || target_tile == 'c') {
//                 // Store the original tile before moving
//                 monster->position.x = newx;
//                 monster->position.y = newy;
//             }
//         }
//     }
// }
void update_monster_range(){
for(int i=0;i<24;i++){
    if(rooms[i].monsters[0].health>0 && rooms[i].monsters[0].current_range==0 ){
        rooms[i].monsters[0].current_range=rooms[i].monsters[0].range;}
}
}
// void attack_monster(Player *player, int x, int y) {

//     int room_id = get_current_room_id(x, y);

//     if (room_id == -1) return;



//     Room *room = &rooms[(level * 6) + room_id];

//     for (int i = 0; i < room->monster_count; i++) {

//         monster *monster = &room->monsters[i];

//         if (monster->position.x == x && monster->position.y == y) {

//             monster->health -= player->active_weapon.damage;

//             if (monster->health <= 0) {

//                 update_message_window("You killed a monster!");

//                 map[level][monster->position.y][monster->position.x] = '.';

//             }

//             return;

//         }

//     }

// }
void attack_monster(Player * player){
if(strcmp(player->active_weapon.type,"none")==0)update_message_window("No active weapon!");
else{
if(strcmp(player->active_weapon.type,"mace")==0 ||strcmp(player->active_weapon.type,"sword")==0 ){
for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue; // Skip the player's position
            int x = player->positions.x + dx;
            int y = player->positions.y + dy;
            int room_id = get_current_room_id(x, y);
            if (room_id != -1) {
                Room *r = &rooms[(level * 6) + room_id];
                for (int j = 0; j < r->monster_count; j++) {
                    if (r->monsters[j].health>0 && r->monsters[j].position.x == x && r->monsters[j].position.y == y) {
                        r->monsters[j].health -= player->active_weapon.damage;
                          update_message_window("You hit a monster!");                   
                              if (r->monsters[j].health<= 0) { 
                              update_message_window("You killed a monster!");
                              player->gold+=r->monsters[j].damage;
                              r->monster_count--;
                             adjust_color();
                             attron(COLOR_PAIR(1));
                               mvprintw(player->positions.y,player->positions.x,"@");    
                             attroff(COLOR_PAIR(1)); 
                             update_status_window(player);
                    }
                }
            }
        }
    }
}
}else{
     if(strcmp(player->active_weapon.type,"arrow")==0 && player->arrow_count<=0){update_message_window("You dont have enough  shots!");return;}
     else  if(strcmp(player->active_weapon.type,"magic_wound")==0 && player->magic_wound_count<=0){update_message_window("You dont have enough shots!");return;}
     else  if(strcmp(player->active_weapon.type,"dagger")==0 && player->dagger_count<=0){update_message_window("You dont have enough shots!");return;}

                
int ch = getch();  // Get direction input
    int dx = 0, dy = 0;
    switch (ch) {
        case 'h': dx = -1; break; // Left
        case 'l': dx = 1; break;  // Right
        case 'j': dy = -1; break; // Up
        case 'k': dy = 1; break;  // Down
        case 'y': dx = -1; dy = -1; break; // Up-Left
        case 'u': dx = 1; dy = -1; break;  // Up-Right
        case 'b': dx = -1; dy = 1; break;  // Down-Left
        case 'n': dx = 1; dy = 1; break;   // Down-Right
        default:
            update_message_window("Invalid direction!");
            return;
    }
    int range = 0;
    if (strcmp(player->active_weapon.type, "arrow") == 0) {range = 5;player->arrow_count--;}
    else if (strcmp(player->active_weapon.type, "magic_wound") == 0) {range = 10;player->magic_wound_count--;}
   else if (strcmp(player->active_weapon.type, "dagger") == 0){ range = 5;player->dagger_count--;}
    int x = player->positions.x;
    int y = player->positions.y;
    for (int i = 0; i < range; i++) {
        x += dx;
        y += dy;
        // Check for wall collision
        if (map[level][y][x] == '|' || map[level][y][x] == '_' || map[level][y][x] == '='||map[level][y][x] == '+'||map[level][y][x] == '?') {
            if(strcmp(player->active_weapon.type,"arrow")==0){map[level][y-dy][x-dx]='A';}
           else if(strcmp(player->active_weapon.type,"magic_wound")==0){map[level][y-dy][x-dx]='M';}
           else if(strcmp(player->active_weapon.type,"dagger")==0){map[level][y-dy][x-dx]='D';} 
      
            update_message_window("Your shot hit a wall.");
            return;
        }
        // Check for monster hit
        int room_id = get_current_room_id(x, y);
        if (room_id != -1) {
            Room *r = &rooms[(level * 6) + room_id];
            for (int j = 0; j < r->monster_count; j++) {
                if (r->monsters[j].health>0 && r->monsters[j].position.x == x && r->monsters[j].position.y == y) {
                     r->monsters[j].health -= player->active_weapon.damage;
                    update_message_window("You hit a monster!");
                    if (r->monsters[j].health<= 0) { 
                             player->gold+=r->monsters[j].damage;
                             r->monster_count--;
                              update_message_window("You killed a monster!");
                             adjust_color();
                             attron(COLOR_PAIR(1));
                               mvprintw(player->positions.y,player->positions.x,"@");    
                             attroff(COLOR_PAIR(1)); 
                             update_status_window(player);
                    }
                    return;
                }
            }
        }
    }


    // If no hit, drop at max range
    if(strcmp(player->active_weapon.type,"arrow")==0){map[level][y][x]='A';}
     else if(strcmp(player->active_weapon.type,"magic_wound")==0){map[level][y][x]='M';}
       else if(strcmp(player->active_weapon.type,"dagger")==0){map[level][y][x]='D';} 
      
  
    update_message_window("Your shot fell to the ground.");


}
}
}
void set_difficulty(){
    printw("default:easy\n");
     printw("1.Easy\n");
      printw("2.Average\n");
       printw("3.Hard\n");
        printw("Enter your choice: ");
         int choice;
         scanw("%d", &choice);
         switch (choice)
         {
         case 1:
            dificulty_mode=3;
            break;
         case 2:
            dificulty_mode=2;
            break;
            case 3:
            dificulty_mode=1;
            break;
         default:
            break;
         }
}
void set_charcter_color(){
    printw("default:GREEN\n");
     printw("1.GREEN\n");
      printw("2.CYAN\n");
       printw("3.RED\n");
       printw("4.YELLOW\n");
       printw("5.WHITE\n");
       printw("6.BLUE\n");
       printw("7.MAGNETA\n");
        printw("Enter your choice: ");
         int choice;
         scanw("%d", &choice);
         switch (choice)
         {
         case 1:
            init_pair(1,COLOR_GREEN,COLOR_BLACK);
            break;
         case 2:
            init_pair(1,COLOR_CYAN,COLOR_BLACK);
            break;
            case 3:
            init_pair(1,COLOR_RED,COLOR_BLACK);
            break;
            case 4:
            init_pair(1,COLOR_YELLOW,COLOR_BLACK);
            break;
            case 5:
            init_pair(1,COLOR_WHITE,COLOR_BLACK);
            break;
            case 6:
            init_pair(1,COLOR_BLUE,COLOR_BLACK);
            break;
            case 7:
            init_pair(1,COLOR_MAGENTA,COLOR_BLACK);
            break;
         default:
            break;
         }
}
int compare_scores(const void *a, const void *b) {
    return ((User*)b)->total_scores - ((User*)a)->total_scores;
}



// void display_scoreboard() {
//     clear();

//     printw("=== Scoreboard ===\n");

//     qsort(users, user_count, sizeof(User), compare_scores);
//     time_t current_time = time(NULL);  // Get current time
//     printw("Rank | Username | Total Scores | Total Gold | Completed Games | Experience (seconds)\n");
//     printw("------------------------------------------------------------------------------------\n");
     
//     for (int i = 0; i < user_count; i++) {

//         double experience = (users[i].first_game_time!=0)?difftime(current_time, users[i].first_game_time):0;  // Calculate experience
//         if(guest==0 && strcmp(users[i].username,current_username)==0){attron(A_BOLD);}
//         if(i==0){attron(COLOR_PAIR(4));attron(A_UNDERLINE);}
//         if(i==1){attron(COLOR_PAIR(5));attron(A_UNDERLINE);}
//         if(i==2){attron(COLOR_PAIR(9));attron(A_UNDERLINE);}
//         printw("%d    | %-8s | %-12d | %-10d | %-15d | %-20.0f", 
//                 i + 1, 
//                 users[i].username, 
//                 users[i].total_scores, 
//                 users[i].total_gold, 
//                 users[i].completed_game, 
//                 experience); 
//                 if(i==0) {printw("CHAMPION  ");printw("\U0001F947");attroff(COLOR_PAIR(4));attroff(A_UNDERLINE);}
//                 if(i==1) {printw("RUNNER-UP ");printw("\U0001F948");attroff(COLOR_PAIR(5));attroff(A_UNDERLINE);}
//                 if(i==2) {printw("STRIVER   ");printw("\U0001F949");attroff(COLOR_PAIR(9));attroff(A_UNDERLINE);}
//                 printw("\n");
//      if(guest==0 && strcmp(users[i].username,current_username)==0){attroff(A_BOLD);}

//     }
     
// getch();
// }
void save_game() {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s_game.dat", current_username);  // Generate file name with username
    FILE *file = fopen(filename, "wb");  // Open the file in binary mode
    if (!file) {
        perror("Failed to open file for saving");
        return;
    }
    // Save the array of rooms
    fwrite(rooms, sizeof(Room), 24, file);
    // Save the 3D char array (map)
    fwrite(map, sizeof(char), 4 * 1000 * 1000, file);
    // Save the 3D int array (levels)
    fwrite(visibility, sizeof(int), 4 * 1000 * 1000, file);
    // Save the player struct (dereference the pointer)
    fwrite(player, sizeof(Player), 1, file);
    // Save the global variables (Level and difficulty_mode)
    fwrite(&level, sizeof(int), 1, file);
    fwrite(&dificulty_mode, sizeof(int), 1, file);
    fclose(file);  // Close the file
}

void load_game() {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s_game.dat", current_username);  // Generate file name with username
    FILE *file = fopen(filename, "rb");  // Open the file in binary mode
    if (!file) {
        perror("Failed to open file for loading");
        return;
    }
    // Load the array of rooms
    fread(rooms, sizeof(Room), 24, file);
    // Load the 3D char array (map)
    fread(map, sizeof(char), 4 * 1000 * 1000, file);
    // Load the 3D int array (levels)
    fread(visibility, sizeof(int), 4 * 1000 * 1000, file);
    // Allocate memory for the player struct if it's not already allocated
    if (player == NULL) {
        player = (Player *)malloc(sizeof(Player));
    }
    // Load the player struct
    fread(player, sizeof(Player), 1, file);
    // Load the global variables (Level and difficulty_mode)
    fread(&level, sizeof(int), 1, file);
    fread(&dificulty_mode, sizeof(int), 1, file);
    fclose(file);  // Close the file
}
void play_song(const char *song) {

    // Stop any previously playing music

    system("pkill mpg123");



    // Play the new song and suppress terminal output

    char command[256];

    snprintf(command, sizeof(command), "mpg123 %s > /dev/null 2>&1 &", song);  // Redirect stdout and stderr to /dev/null

    system(command);

}



void stop_song() {
    system("pkill mpg123");
}



void song_menu() {
    int num_songs = 3;  // Only 3 songs available
    int ch;
        clear();  // Clear the screen
        // Display menu
        mvprintw(0, 0, "Select a song to play (or press space to stop):");
        for (int i = 0; i < num_songs; i++) {
            mvprintw(i + 1, 0, "%d. %s", i + 1, songs[i]);
        }
        mvprintw(5, 0, "Enter your choice:");
        refresh();  // Update the screen
        ch = getch();  // Get user input
        switch (ch) {
            case ' ':  // Stop song
                stop_song();
                song_choice = 0;  // Reset choice to 0 to indicate no song
                break;
            case '1':  // Play song 1
               // play_song(songs[0]);
                song_choice = 1;
                break;
            case '2':  // Play song 2
               // play_song(songs[1]);
                song_choice = 2;
                break;
            case '3':  // Play song 3
               // play_song(songs[2]);
                song_choice = 3;
                break;
            // case 'q':  // Quit the program
            //     stop_song();  // Stop any music before exiting
            //     endwin();  // Close ncurses
            //     return;
        }

  //  }
getch();
}

void display_scoreboard(int current_page) {
    clear();
    printw("=== Scoreboard ===\n");
    qsort(users, user_count, sizeof(User), compare_scores);  
    time_t current_time = time(NULL); 
    printw("Rank | Username | Total Scores | Total Gold | Completed Games | Experience (seconds)\n");
    printw("------------------------------------------------------------------------------------\n");
    int start_index = current_page * PAGE_SIZE;  // Calculate the start index for the current page
    int end_index = (start_index + PAGE_SIZE < user_count) ? start_index + PAGE_SIZE : user_count;  // Ensure we don't go beyond the total number of users

    for (int i = start_index; i < end_index; i++) {

        double experience = (users[i].first_game_time != 0) ? difftime(current_time, users[i].first_game_time) : 0;

        if (guest == 0 && strcmp(users[i].username, current_username) == 0) { attron(A_BOLD); }

        if (i == 0) { attron(COLOR_PAIR(4)); attron(A_UNDERLINE);}
        if (i == 1) {attron(COLOR_PAIR(5));attron(A_UNDERLINE); }
        if (i == 2) {attron(COLOR_PAIR(9));attron(A_UNDERLINE);}
        printw("%d    | %-8s | %-12d | %-10d | %-15d | %-20.0f", 
                i + 1, 
                users[i].username, 
                users[i].total_scores, 
                users[i].total_gold, 
                users[i].completed_game, 
                experience);

        if (i == 0) { printw("CHAMPION  "); printw("\U0001F947");attroff(COLOR_PAIR(4));attroff(A_UNDERLINE); }
        if (i == 1) { printw("RUNNER-UP "); printw("\U0001F948");attroff(COLOR_PAIR(5));  attroff(A_UNDERLINE); }
        if (i == 2) {  printw("STRIVER   "); printw("\U0001F949");attroff(COLOR_PAIR(9)); attroff(A_UNDERLINE);}
        printw("\n");
        if (guest == 0 && strcmp(users[i].username, current_username) == 0) {
            attroff(A_BOLD);
        }
    }
    // Display page navigation instructions
    printw("\nPage %d of %d", current_page + 1, (user_count + PAGE_SIZE - 1) / PAGE_SIZE);
    printw("\nUse 'n' for next page, 'p' for previous page, 'q' to quit: ");
    refresh();
}

void navigate_scoreboard() {
    int current_page = 0;
    int ch;

    while (1) {
        display_scoreboard(current_page);  // Display the current page
        ch = getch();  
        if (ch == 'n' && (current_page + 1) * PAGE_SIZE < user_count) {
            current_page++;  // Go to the next page
        } else if (ch == 'p' && current_page > 0) {
            current_page--;  // Go to the previous page
        } else if (ch == 'q') {
            break;  
        }
    }
}
int right_path(int x1, int y1, int x2, int y2) {
    int x = x1, y = y1;
    // Horizontal and vertical distances
    int dx = x2 - x1;
    int dy = y2 - y1;
    // Move horizontally first
    while (dx != 0) {
        x += (dx > 0) ? 1 : -1;
        dx += (dx > 0) ? -1 : 1;
        if (map[level][y][x] != '#') {return 0; }
    }
    // Then move vertically
    while (dy != 0) {
        y += (dy > 0) ? 1 : -1;
        dy += (dy > 0) ? -1 : 1;
         if (map[level][y][x] != '#') {return 0; }
    }
   return 1;
}
void draw_from_file(const char *filename) {
    FILE *file = fopen(filename, "r"); 
    if (file == NULL) {
        printw("Error opening file!\n");
        return;
    }
    char ch;
    while ((ch = fgetc(file)) != EOF) { 
        printw("%c", ch);  
    }
    fclose(file); 
}

