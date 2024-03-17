#include <sys/time.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int row1_size = 0, col1_size = 0, row2_size = 0, col2_size = 0;
int mat1[20][20];
int mat2[20][20];
int mat_output[20][20];

typedef struct
{
    int row;
    int col;
} pos;

void thread_per_matrix()
{
    for (int i = 0; i < row1_size; i++)
    {
        for (int j = 0; j < col2_size; j++)
        {
            mat_output[i][j] = 0;
            for (int k = 0; k < col1_size; k++)
            {
                mat_output[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }
}
void write_result(char *path)
{
    printf("%s\n", path);
    FILE *file = fopen(path, "a");
    fprintf(file, "row=%d col=%d\n", row1_size, col2_size);
    for (int i = 0; i < row1_size; i++)
    {
        for (int j = 0; j < col2_size; j++)
        {
            fprintf(file, "%d ", mat_output[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}
void *multiplication_per_row(void *ptr)
{
    pos *pos_element = (pos *)ptr;
    int i = pos_element->row;
    for (int j = 0; j < col2_size; j++)
    {
        mat_output[i][j] = 0;
        for (int k = 0; k < col1_size; k++)
        {
            mat_output[i][j] += mat1[i][k] * mat2[k][j];
        }
    }
}
void thread_per_row()
{
    pthread_t thread[row1_size];
    for (int i = 0; i < row1_size; i++)
    {
        pos *pos_row = malloc(sizeof(pos));
        pos_row->row = i;

        pthread_create(&thread[i], NULL, multiplication_per_row, (void *)pos_row);
    }

    for (int i = 0; i < row1_size; i++)
    {
        pthread_join(thread[i], NULL);
    }
}
void *multiplication_per_element(void *ptr)
{
    pos *pos_element = (pos *)ptr;
    int i = pos_element->row;
    int j = pos_element->col;
    mat_output[i][j] = 0;
    for (int k = 0; k < col1_size; k++)
    {
        mat_output[i][j] += mat1[i][k] * mat2[k][j];
    }
    return NULL;
}
void thread_per_element()
{
    pthread_t thread[row1_size][col2_size];
    for (int i = 0; i < row1_size; i++)
    {
        for (int j = 0; j < col2_size; j++)
        {
            pos *pos_element = malloc(sizeof(pos));
            pos_element->row = i;
            pos_element->col = j;
            pthread_create(&thread[i][j], NULL, multiplication_per_element, (void *)pos_element);
        }
    }
    for (int i = 0; i < row1_size; i++)
    {
        for (int j = 0; j < col2_size; j++)
        {
            pthread_join(thread[i][j], NULL);
        }
    }
}
int *read_matrix_size(FILE *file)
{
    char line[100];
    char *arr[50] = {};
    int *size = malloc(2 * sizeof(int));
    fgets(line, sizeof(line), file);
    if (line != NULL)
    {
        arr[0] = strtok(line, " ");
        for (int i = 1; arr[i - 1] != NULL; i++)
        {
            arr[i] = strtok(NULL, " ");
        }
    }
    size[0] = atoi(strchr(arr[0], '=') + 1);
    size[1] = atoi(strchr(arr[1], '=') + 1);
    return size;
}
void file_not_exist(FILE *file)
{
    if (file == NULL)
    {
        printf("file doesn't exist!");
        exit(0);
    }
}
void get_files_names(char *args[])
{
    static char line[100];
    fgets(line, sizeof(line), stdin);
    if (line != NULL)
    {
        int indx = strcspn(line, "\n");
        line[indx] = '\0';
        args[0] = strtok(line, " ");
        for (int i = 1; args[i - 1] != NULL; i++)
        {
            args[i] = strtok(NULL, " ");
        }
    }
}
void read_matrix_1(FILE *file)
{
    int counter = 0;
    char row[256];
    char *arr[50];
    while (counter < row1_size)
    {
        fgets(row, 100, file);
        arr[0] = strtok(row, " ");
        mat1[counter][0] = atoi(arr[0]);
        for (int i = 1; i < col1_size; i++)
        {
            arr[i] = strtok(NULL, " ");
            mat1[counter][i] = atoi(arr[i]);
        }
        counter++;
    }
}
void read_matrix_2(FILE *file)
{
    int counter = 0;
    char row[256];
    char *arr[50];
    while (counter < row2_size)
    {
        fgets(row, 100, file);
        arr[0] = strtok(row, " ");
        mat2[counter][0] = atoi(arr[0]);
        for (int i = 1; i < col2_size; i++)
        {
            arr[i] = strtok(NULL, " ");
            mat2[counter][i] = atoi(arr[i]);
        }
        counter++;
    }
}
void menu(char *filename)
{
    while (1)
    {
        char path[100];
        strcpy(path, filename);
        int choice;
        printf("1 -> thread per matrix\n2 -> thread per row\n3 -> thread per element\n0 -> exit\n");
        scanf("%d", &choice);
        struct timeval stop, start;
        gettimeofday(&start, NULL); // start checking time
        switch (choice)
        {
        case 1:
            thread_per_matrix();
            strcat(path, "_per_matrix.txt");
            break;
        case 2:
            thread_per_row();
            strcat(path, "_per_row.txt");
            break;
        case 3:
            thread_per_element();
            strcat(path, "_per_element.txt");
            break;
        case 0:
            exit(0);
            break;
        default:
            break;
        }

        gettimeofday(&stop, NULL); // end checking time
        write_result(path);
        printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
        printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    }
}
int main()
{
    FILE *file;

    char *args[50], path[50];
    args[1] = "a";
    args[2] = "b";
    args[3] = "c";
    // get files name
    get_files_names(args);

    // open first file
    strcpy(path, args[1]);
    strcat(path, ".txt");

    file = fopen(path, "r");
    file_not_exist(file);
    int *size1 = read_matrix_size(file);
    row1_size = size1[0];
    col1_size = size1[1];
    read_matrix_1(file);

    fclose(file);
    // open second file
    strcpy(path, args[2]);
    strcat(path, ".txt");

    file = fopen(path, "r");
    file_not_exist(file);
    int *size2 = read_matrix_size(file);
    row2_size = size2[0];
    col2_size = size2[1];
    read_matrix_2(file);
    fclose(file);

    if (col1_size != row2_size)
    {
        printf("Multiplication is not allowed");
        exit(0);
    }
    menu(args[3]);
    return 0;
}
