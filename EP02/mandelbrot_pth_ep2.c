#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<pthread.h>
#include <time.h>

static double rtclock() {
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  return t.tv_sec + t.tv_nsec * 1e-9;
}

int number_of_threads;
int number_of_process;

double c_x_min;
double c_x_max;
double c_y_min;
double c_y_max;

double pixel_width;
double pixel_height;

int iteration_max = 200;
double escape_radius_squared = 4;

int image_size;
unsigned char **image_buffer;

int i_x_max;
int i_y_max;
int image_buffer_size;

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
    int rgb_size = 3;
    image_buffer = (unsigned char **) malloc(sizeof(unsigned char *) * image_buffer_size);

    for(int i = 0; i < image_buffer_size; i++){
        image_buffer[i] = (unsigned char *) malloc(sizeof(unsigned char) * rgb_size);
    };
};

void init(int argc, char *argv[]){
    if(argc < 6){
        printf("usage: ./mandelbrot_pth_ep2 c_x_min c_x_max c_y_min c_y_max image_size\n");
        printf("Triple Spiral Valley: ./mandelbrot_pth_ep2 -0.188 -0.012 0.554 0.754 4096\n");
        exit(0);
    }
    else{
        sscanf(argv[1], "%lf", &c_x_min);
        sscanf(argv[2], "%lf", &c_x_max);
        sscanf(argv[3], "%lf", &c_y_min);
        sscanf(argv[4], "%lf", &c_y_max);
        sscanf(argv[5], "%d", &image_size);
        //sscanf(argv[6], "%d", &number_of_threads);

        i_x_max           = image_size;
        i_y_max           = image_size;
        image_buffer_size = image_size * image_size;

        pixel_width       = (c_x_max - c_x_min) / i_x_max;
        pixel_height      = (c_y_max - c_y_min) / i_y_max;
    };
};

void update_rgb_buffer(int iteration, int x, int y){
    int color;

    if(iteration == iteration_max){
        image_buffer[(i_y_max * y) + x][0] = colors[gradient_size][0];
        image_buffer[(i_y_max * y) + x][1] = colors[gradient_size][1];
        image_buffer[(i_y_max * y) + x][2] = colors[gradient_size][2];
    }
    else{
        color = iteration % gradient_size;

        image_buffer[(i_y_max * y) + x][0] = colors[color][0];
        image_buffer[(i_y_max * y) + x][1] = colors[color][1];
        image_buffer[(i_y_max * y) + x][2] = colors[color][2];
    };
};

void write_to_file(){
    FILE * file;
    char * filename               = "output_pth.ppm";
    char * comment                = "# ";

    int max_color_component_value = 255;

    file = fopen(filename,"wb");

    fprintf(file, "P6\n %s\n %d\n %d\n %d\n", comment,
            i_x_max, i_y_max, max_color_component_value);

    for(int i = 0; i < image_buffer_size; i++){
        fwrite(image_buffer[i], 1 , 3, file);
    };

    fclose(file);
};

struct Thread_Data {
    int start_index;
    int end_index;
};

void compute_y_axis(void *_data){
    struct Thread_Data *data = _data;

    int start_index = data->start_index;
    int end_index = data->end_index;

    double z_x;
    double z_y;
    double z_x_squared;
    double z_y_squared;

    int iteration;
    int i_x;

    double c_x;
    double c_y;

    for(int i_y = start_index; i_y < end_index; i_y++){
        c_y = c_y_min + i_y * pixel_height;

        if(fabs(c_y) < pixel_height / 2){
            c_y = 0.0;
        };

        for(i_x = 0; i_x < i_x_max; i_x++){
            c_x         = c_x_min + i_x * pixel_width;

            z_x         = 0.0;
            z_y         = 0.0;

            z_x_squared = 0.0;
            z_y_squared = 0.0;

            for(iteration = 0;
                iteration < iteration_max && \
                ((z_x_squared + z_y_squared) < escape_radius_squared);
                iteration++){
                z_y         = 2 * z_x * z_y + c_y;
                z_x         = z_x_squared - z_y_squared + c_x;

                z_x_squared = z_x * z_x;
                z_y_squared = z_y * z_y;
            };

            update_rgb_buffer(iteration, i_x, i_y);
        };
    };
}

void compute_mandelbrot(){

    pthread_t *threads = malloc(sizeof(pthread_t)*number_of_threads);
    struct Thread_Data *thread_datas = malloc(sizeof(struct Thread_Data)*number_of_threads);

	int bulk_size = i_y_max/number_of_threads;
	int offset = 0;
	for(int i = 0; i < number_of_threads; i++) {
		thread_datas[i].start_index = offset;
		thread_datas[i].end_index = offset + bulk_size;
		offset += bulk_size;
	}
	
    int remaining = i_y_max%number_of_threads;
	thread_datas[number_of_threads-1].end_index += remaining;

    for(int i = 0; i < number_of_threads; i++) {
		pthread_create(threads+i, NULL, (void *) compute_y_axis, thread_datas+i);
	}

    for(int i = 0; i < number_of_threads; i++) {
		pthread_join(threads[i], NULL);
	}

};

int main(int argc, char *argv[]){
    init(argc, argv);
    allocate_image_buffer();
    number_of_threads = 8;
    number_of_process = 1;
    double start_time = rtclock();
    compute_mandelbrot();
    double end_time = rtclock();
    //printf("Image size\tExecution Time\tNumber of Threads\n");
    printf("%f;%d;%d\n", end_time - start_time,number_of_process, number_of_threads);
    write_to_file();

    return 0;
};
