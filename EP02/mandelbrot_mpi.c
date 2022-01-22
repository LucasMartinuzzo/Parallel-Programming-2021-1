#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <mpi.h>
#include <time.h>

#define  RANK_ZERO     0
// MPI sending and listening TAGs
#define  TAG_1      1
#define  TAG_2      2

static double rtclock() {
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  return t.tv_sec + t.tv_nsec * 1e-9;
}

// Seahorse Valley
/*int image_size = 1024;
double c_x_min = -0.8; 
double c_x_max = -0.7; 
double c_y_min = 0.05; 
double c_y_max = 0.15; 
int i_x_max = 1024; 
int i_y_max = 1024;*/

// Elephant
/*int image_size = 1024;
double c_x_min = 0.175; 
double c_x_max = 0.375; 
double c_y_min = -0.1; 
double c_y_max = 0.1; 
int i_x_max = 1024; 
int i_y_max = 1024;*/


// triple spiral valley
// -0.188 -0.012 0.554 0.754
int image_size = 4096;
double c_x_min = -0.188; 
double c_x_max = -0.012; 
double c_y_min = 0.554; 
double c_y_max = 0.754; 
int i_x_max = 4096;
int i_y_max = 4096;


 
// full picture
// -2.5 1.5 -2.0 2.0
//int image_size = 1024;
//double c_x_min = -2.5;
//double c_x_max = 1.5;
//double c_y_min = -2.0;
//double c_y_max = 2.0;
//int i_x_max = 1024;
//int i_y_max = 1024;

int number_of_process;

// double c_x_min;
// double c_x_max;
// double c_y_min;
// double c_y_max;

double pixel_width;
double pixel_height;
double escape_radius_squared = 4;

int iteration_max = 200;

// int image_size;
unsigned char *image_buffer;

// int i_x_max; 
// int i_y_max;
// int image_buffer_size;

int gradient_size = 16;
int colors[17][3] = {
    {66, 30, 15},
    {25, 7, 26},
    {9, 1, 47},
    {4, 4, 73},
    {0, 7, 100},
    {12, 44, 138},
    {24, 82, 177},
    {57, 125, 209},
    {134, 181, 229},
    {211, 236, 248},
    {241, 233, 191},
    {248, 201, 95},
    {255, 170, 0},
    {204, 128, 0},
    {153, 87, 0},
    {106, 52, 3},
    {16, 16, 16},
};


void allocate_image_buffer(){
    int image_buffer_size = image_size * image_size;
    int rgb_size = 3;
    
    image_buffer = (unsigned char *) malloc(sizeof(unsigned char ) * image_buffer_size * rgb_size);
};

/*
void init(int argc, char *argv[]){
    if(argc < 2){
        printf("usage: mpirun -np number_of_process ./mandelbrot_mpi number_of_process\n");
        printf("example:\n");
        printf("mpirun -np 4 ./mandelbrot_mpi\n");
        exit(0);
    }
    else{
        sscanf(argv[1], "%d", &number_of_process);
    };
};*/

void update_rgb_buffer(int iteration, int x, int y){
    int color;
    int rgb_size = 3;

    if(iteration == iteration_max){
        image_buffer[((i_y_max * y) + x) * rgb_size + 0] = colors[gradient_size][0];
        image_buffer[((i_y_max * y) + x) * rgb_size + 1] = colors[gradient_size][1];
        image_buffer[((i_y_max * y) + x) * rgb_size + 2] = colors[gradient_size][2];
    }
    else{
        color = iteration % gradient_size;

        image_buffer[((i_y_max * y) + x) * rgb_size + 0] = colors[color][0];
        image_buffer[((i_y_max * y) + x) * rgb_size + 1] = colors[color][1];
        image_buffer[((i_y_max * y) + x) * rgb_size + 2] = colors[color][2];
    };
};

void write_to_file(){
    FILE * file;
    char * filename               = "output.ppm";
    char * comment                = "# ";
    int image_buffer_size = image_size * image_size;
    int max_color_component_value = 255;
    int rgb_size = 3;
    
    file = fopen(filename,"wb");

    fprintf(file, "P6\n %s\n %d\n %d\n %d\n", comment,
            i_x_max, i_y_max, max_color_component_value);

    for(int i = 0; i < image_buffer_size * rgb_size; i+=1){  
        fwrite(image_buffer + i, 1 , 1, file);
    };

    fclose(file);
};

void compute_mandelbrot(int starting_from, int ending_to, uint8_t *result_array){
    pixel_width = (c_x_max - c_x_min) / i_x_max;
    pixel_height = (c_y_max - c_y_min) / i_y_max;

    uint8_t iteration;
    for(int i_y = starting_from; i_y < ending_to; i_y++){
        double c_y = c_y_min + i_y * pixel_height;

        if(fabs(c_y) < pixel_height / 2){
            c_y = 0.0;
        };

        for(int i_x = 0; i_x < i_x_max; i_x++){
            double c_x         = c_x_min + i_x * pixel_width;

            double z_x         = 0.0;
            double z_y         = 0.0;

            double z_x_squared = 0.0;
            double z_y_squared = 0.0;
            for(iteration = 0;
                iteration < iteration_max && \
            ((z_x_squared + z_y_squared) < escape_radius_squared);
                iteration++){
                z_y         = 2 * z_x * z_y + c_y;
                z_x         = z_x_squared - z_y_squared + c_x;

                z_x_squared = z_x * z_x;
                z_y_squared = z_y * z_y;
            };
            
            result_array[(i_y - starting_from)*i_y_max+i_x] = iteration;

        };
    };
};

void pre_update_rgb_buffer(uint8_t *results, int start, int size){
    int j = start-1;
    for (int i=0; i< size; i++){
        if (i % i_y_max == 0){
            j++;
        }
        update_rgb_buffer(results[i], i % i_x_max, j);
    }
}

int main(int argc, char *argv[]){
    
    //init(argc, argv);

    int rank;
    int num_process;
    
    // number of data
    int count = 1;
    
    // destination and source for message send-receive
    int destination, source;
    
    // Each row of image starting_from and ending_to
    int offset = 0;
    int starting_from;
    int ending_to;
    
    int rows;
    
    MPI_Init(&argc, &argv);

    // Get this process' rank (process within a communicator)
    // MPI_COMM_WORLD is the default communicator
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    // Get the total number ranks in this communicator
    MPI_Comm_size(MPI_COMM_WORLD, &num_process);
    
    // link: https://youtu.be/c0C9mQaxsD4?t=1599
    /* 
    dividing the processes into two groups. One process sending 
    to another the other process listening to the first
    if statement if for checking this rank is from the type of rank 0,
    which first send and then receive, or the other ranks which first receive and then send 
    */
    double start_time = rtclock();
    if (rank == RANK_ZERO){
        allocate_image_buffer();
        
        if(num_process == 1){
            uint8_t *result_array = (uint8_t *)malloc(image_size * image_size * sizeof(uint8_t));
            compute_mandelbrot(0, image_size, result_array);
            pre_update_rgb_buffer(result_array, 0, image_size * image_size);
            
            MPI_Finalize();
        } else {
            int num_workers = num_process-1;
        
            int num_rows = image_size/num_workers;
            int mod_row = image_size % num_workers;
            rows = num_rows;
            
            uint8_t *result_array = (uint8_t *)malloc(rows*i_x_max*sizeof(uint8_t));
            
            // Sender: Sending information to workers
            for (int i=1; i <= num_workers; i++){
                        destination = i;
                        //TAG_1
                        MPI_Send(&offset, count, MPI_INT, destination, TAG_1, MPI_COMM_WORLD);
                        MPI_Send(&rows, count, MPI_INT, destination, TAG_1, MPI_COMM_WORLD);
                        
                        // each worker receive a new row of data
                        offset = offset + rows;
            }
      
            // Receiver: Waiting to receive results from all workers
            for (int i=1; i<=num_workers; i++){
                        source = i;
                        //TAG_2
                        MPI_Recv(&offset, count, MPI_INT, source, TAG_2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        MPI_Recv(&rows, count, MPI_INT, source, TAG_2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        MPI_Recv(result_array, num_rows*i_y_max, MPI_BYTE, source, TAG_2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        
                        pre_update_rgb_buffer(result_array, offset, i_x_max*rows);
            }

            MPI_Finalize();

            // if there is a mod, then we put outside MPI_Init and MPI_Finalize
            if (mod_row != 0){
                    uint8_t *result_array;
                    result_array = (uint8_t *)malloc(mod_row*i_x_max*sizeof(uint8_t));
                    compute_mandelbrot(image_size - mod_row,image_size, result_array);
                    pre_update_rgb_buffer(result_array, image_size - mod_row, i_x_max*mod_row);
            }
        }
    } else {
            source = RANK_ZERO;
            // TAG_1
            MPI_Recv(&offset, count, MPI_INT, source, TAG_1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&rows, count, MPI_INT, source, TAG_1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            starting_from=offset;
            ending_to=offset+rows; 
              
            uint8_t *result_array;
            result_array = (uint8_t *)malloc(rows*i_x_max*sizeof(uint8_t));
              
            compute_mandelbrot(starting_from,ending_to, result_array);
              
            // TAG_2
            MPI_Send(&offset, count, MPI_INT, RANK_ZERO, TAG_2, MPI_COMM_WORLD);
            MPI_Send(&rows, count, MPI_INT, RANK_ZERO, TAG_2, MPI_COMM_WORLD);        
            MPI_Send(result_array, rows*i_y_max, MPI_BYTE, RANK_ZERO, TAG_2, MPI_COMM_WORLD);
            

            MPI_Finalize();
    }
    int number_of_threads = 1;
    if(rank == RANK_ZERO){
        double end_time = rtclock();
        write_to_file();
        printf("%f;%d;%d\n", end_time - start_time, num_process, number_of_threads);
    }
    
    return 0;
};
