#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

//record and section
typedef struct record{
    char *key;
    char *value;
    struct record *next;
} record_t;

typedef struct section{
    char * name;
    record_t *firstRecord;
    struct section *next;
} section_t;


//user input
typedef struct  user_args{
    char *file_path;
    char *section;
    char *key;
    bool is_valid;
} user_args_t;

typedef struct user_args_expr{
    char *file_path;
    char *firstSection;
    char *firstKey;
    char *secondSection;
    char *secondKey;
    char *operator;
    bool is_valid;
} user_args_expr_t;


//record functions

record_t * search_record(char *key, record_t *head){
    record_t *currentRecord = head;
    while(currentRecord != NULL){
        if(strcmp(currentRecord->key,key)==0){
            return currentRecord;
        }
        currentRecord = currentRecord->next;
    }
    return NULL;
}


//section functions
section_t * search_section(char *name, section_t *head){
    section_t *currentSection = head;
    while(currentSection!= NULL){
        if (strcmp(currentSection->name,name) == 0){
            return currentSection;
        }
        currentSection = currentSection->next;
    }
    return NULL;
}

void create_record_from_file(char *key, char *value, section_t *head_section){
    record_t *new_record = (record_t*)malloc(sizeof(record_t));
    new_record->key = key;
    new_record->value = value;
    new_record->next = head_section->firstRecord;
    head_section->firstRecord = new_record;
}


section_t *create_section_from_file(char *file_line, section_t *head_section){
    section_t* new_section = (section_t*)malloc(sizeof(section_t));
    new_section->name = file_line;
    new_section->next = head_section;
    new_section->firstRecord = NULL;
    return new_section;
}


//string functions
char *strip_right(char *text, char stripChar){
    int len = strlen(text);
    while (text[len-1] == stripChar)
    {
        text[len-1]='\0';
        len--;
    }
    return text;
}

char *strip_left(char *text, char stripChar){
    int i = 0;
    while(text[i] == stripChar){
        i++;
    }
    memcpy(text, text+i, strlen(text)-i+1);
    return text;
}

char *strip_newline (char *text){
    text = strip_right(text, '\n');
    text = strip_right(text, '\r');
    return text;
}


bool is_correct(char *text){
    for(unsigned long long i=0; i<strlen(text);i++){
        if(isalnum(text[i]) || text[i] == '-' || text[i] == ' '){
            continue;
        }else{
            return false;
        }
    }
    return true;
}

bool is_integer(char *text){
    for(unsigned long long i = 0; i<strlen(text); i++){
        if(!isdigit(text[i])){
            return false;
        }
    }
    return true;
}


//input function
user_args_t handle_console_input_normal(int argc, char *argv[]){
    user_args_t user_args;
    if(argc!=3){
        printf("Input correct arguments in the form PATH_TO_INI_FILE section.key or PATH_TO_INI_FILE expression \" section.key {+, -, *, /} section.key\"\n");
        user_args.is_valid = false;
        return user_args;
    }
    char *file_path = argv[1];
    char *sectionAndKey = argv[2];
    char *section_name = strtok(sectionAndKey,".");
    if(!is_correct(section_name)){
        printf("Section %s contains prohibited characters!\n",section_name);
        user_args.is_valid = false;
    }else{
        char *given_key = strtok(NULL, ".");
        if(!is_correct(given_key)){
            printf("Key %s contatins prohibited characters!", given_key);
            user_args.is_valid = false;

        } else{
        user_args.file_path = file_path;
        user_args.section = section_name;
        user_args.key = given_key;
        user_args.is_valid = true;
    }
    }
    return user_args;
}

user_args_expr_t handle_console_input_expression(int argc, char *argv[]){
    user_args_expr_t user_args;
    if(argc!=4){
        printf("Input correct arguments in the form PATH_TO_INI_FILE section.key or PATH_TO_INI_FILE expression \" section.key {+, -, *, /} section.key\"\n");
        user_args.is_valid = false;
        return user_args;
    }
    else{
        char *file_path = argv[1];
        char *expression = argv[3];
        expression = strip_left(expression, '\"');
        expression = strip_right(expression, '\"');
        user_args.file_path = file_path;
        
        char *first_section_and_key = strtok(expression, " ");
        char *operator = strtok(NULL, " ");
        user_args.operator = operator;
        char *second_section_and_key = strtok(NULL, " ");

        //first secion and key splitting
        char *section_name = strtok(first_section_and_key,"."); 
        if(!is_correct(section_name)){
            printf("Section %s contains prohibited characters!\n", section_name);
            user_args.is_valid = false;
            return user_args;
        } else{
            char *given_key = strtok(NULL, ".");
            if(!is_correct(given_key)){
                printf("Key %s contains prohibited characters!\n", given_key);
                user_args.is_valid = false;
                return user_args;
            } else{
                user_args.firstSection = section_name;
                user_args.firstKey = given_key;
            }
        }
        //second secion and key splitting
        char *second_section_name = strtok(second_section_and_key,".");
         if(!is_correct(second_section_name)){
            printf("Section %s contains prohibited characters!\n", section_name);
            user_args.is_valid = false;
            return user_args;
        } else{
            char *second_given_key = strtok(NULL, ".");
            if(!is_correct(second_given_key)){
                printf("Key %s contains prohibited characters!\n", second_given_key);
                user_args.is_valid = false;
                return user_args;
            } else{
                user_args.secondSection = second_section_name;
                user_args.secondKey = second_given_key;
            }
        }
        if (!(*operator =='+' || *operator =='-' || *operator =='*' || *operator =='/')){
            printf("%s is not a valid operator!\n", user_args.operator);
            user_args.is_valid = false;
            return user_args;
        }
        user_args.is_valid = true;
        return user_args;
    }
}


//file parsing functions
int line_length(FILE *const file ){
    int c, count;
    long int starting_position = ftell(file);
    count = 0;
    while(1){
        c = fgetc(file);
        if( c == EOF || c == '\n' ){
            break;
        }
        count++;
    }
    fseek(file,starting_position,SEEK_SET);
    return count+1; //+1 for \n character
}


section_t *parse_file(char *file_path){
    section_t *head_section = NULL;
    FILE *file = fopen(file_path,"r");
    if (file == NULL) {
        printf("Error while opening file %s\n",file_path);
        perror("Error");
        return head_section;
    }
    while(!feof(file)){
        bool faulty_section;
        int length = line_length(file);
        char *single_line = (char*)malloc(sizeof(char)*(length+1)); //+1 for \0 character
        fgets(single_line,length+1,file);
        if (single_line[0] == '['){
            single_line = strip_newline(single_line);
            single_line = strip_right(single_line,']');
            single_line = strip_left(single_line,'[');
            if(is_correct(single_line)){
                head_section = create_section_from_file(single_line, head_section);
                faulty_section = false;
            } else{
                printf("Section %s contains prohibited characters!\n",single_line);
                printf("Section wasn't parsed!\n");
                faulty_section = true;
            }
        }
        else if(!faulty_section){
            if(single_line[0] != ';'){ //lines with comments are ommited
                if(length!=1){ //empty line with only \n character are ommited
                    char *key = strtok(single_line,"=");
                    key = strip_right(key,' ');
                    
                    if(is_correct(key)){
                        char *value = strtok(NULL,"=");
                        value = strip_left(value,' ');
                        create_record_from_file(key, value, head_section);
                    }
                    else{
                        printf("Key %s in section %s contains prohibited characters!\n",key, head_section->name);
                        printf("Record wasn't parsed!\n");
                    }
                }
            }
        }
    }  
    fclose(file);
    return head_section;
}

char *find_value(char *section, char *key, section_t *head_section){
    if (head_section!=NULL){
        section_t *wanted_section = search_section(section,head_section);
        if (wanted_section == NULL){
            printf("Couldn't find section with name %s",section);
            return NULL;
        }
        else{
            record_t *wanted_record = search_record(key,wanted_section->firstRecord);
            if(wanted_record==NULL){
                printf("Couldn't find record with key %s in section %s",key,section);
                return NULL;
            }
            else{
                return wanted_record->value;
            }
        }
    }else{
        printf("No sections were parsed yet! Operation is impossible!");
    }
    return NULL;
}

int is_valid_operation(char* value1, char* value2, char* operator){
    if(is_integer(value1) && is_integer(value2)){
        return 1; // both are numbers - valid
    }
    else if (!is_integer(value1) && !is_integer(value2)){
        if(!strcmp(operator, "+")){
            return 2; // both strings and operator = '+' -valid
        }
        else{
            return 3; // both strings and operator != '+' - invalid
        }
    }
    else{
        return 4; // one string one number - invalid
    }
}

void operation_on_ints(char *value1, char *value2, char *operator){
    int value1_int = atoi(value1);
    int value2_int = atoi(value2);
    if (*operator == '+'){
        printf("%d\n", value1_int + value2_int);
    }
    else if(*operator == '-'){
        printf("%d\n", value1_int - value2_int);
    }
    else if(*operator == '*'){
        printf("%lld", (long long)value1_int*value2_int);
    }
    else{
        if (value2_int == 0){
            printf("Division by 0 is not possible!");
            return;
        }
        else{
        printf("%f\n", (float)value1_int / value2_int);
        }
    }
    return;
}



void expression_output(char *value1, char *value2, user_args_expr_t user_args){
    int id = is_valid_operation(value1, value2, user_args.operator);
    switch(id){
        case 1:
            operation_on_ints(value1, value2, user_args.operator);
            break; //todo
        case 2:
            printf("%s%s\n", value1, value2);
            break;
        case 3:
            printf("%s.%s %s %s.%s is a forbidden operation on strings!\n", user_args.firstSection, user_args.firstKey, user_args.operator, user_args.secondSection, user_args.secondKey);
            break;
        case 4:
            printf("%s.%s %s %s.%s expression involving operands of diffrent type!\n", user_args.firstSection, user_args.firstKey, user_args.operator, user_args.secondSection, user_args.secondKey);
            break;
        default:
            break;
    }
}

//main function
int main(int argc, char *argv[]){
    if (strcmp(argv[2],"expression") == 0){
        user_args_expr_t user_args = handle_console_input_expression(argc, argv);
        if (!user_args.is_valid){
            return -1;
        }
        section_t *head_section = parse_file(user_args.file_path);
        if (head_section == NULL){
            return -1;
        }
        char *first_value = find_value(user_args.firstSection, user_args.firstKey, head_section);
        char *second_value = find_value(user_args.secondSection, user_args.secondKey, head_section);
        if (first_value == NULL || second_value == NULL){
            return -1;
        }else{
            first_value = strip_newline(first_value);
            second_value = strip_newline(second_value);
            expression_output(first_value, second_value, user_args);
        }
        return 0; 
    }
    else{
        user_args_t user_args = handle_console_input_normal(argc, argv);
        if (!user_args.is_valid){
            return -1;
        }
        section_t *head_section = parse_file(user_args.file_path);
        if (head_section == NULL){
            return -1;
        }
        char *answer = find_value(user_args.section,user_args.key,head_section);
        if (answer != NULL){
            printf("section: %s, key: %s\n", user_args.section, user_args.key);
            printf("%s", answer);
        }
        return 0;
    }
}