﻿//  120210400 조 용 현
//  main_SoA_AoS.cpp
//
//  Written for CSEG437_CSE5437
//  Department of Computer Science and Engineering
//  Copyright © 2021 Sogang University. All rights reserved.
//
#pragma warning(disable: 4996)
#pragma warning(disable: 6386)

#include "Context_SoA_AoS.h"
#include "Config_SoA_AoS.h"

Context context;

////////////////////////////////////////
//CLContext context;
cl_int errcode_ret;

FILE* fp_stat;
char tmp_string[512];

__int64 _start, _freq, _end;
float compute_time;

void set_sobel_x_filter(int filter_size) {
    context.sobel_filter_x.width = filter_size;
    context.sobel_filter_x.weights = Sobel_x;
}

void set_sobel_y_filter(int filter_size) {
    context.sobel_filter_y.width = filter_size;
    context.sobel_filter_y.weights = Sobel_y;
}

int main(int argc, char* argv[]) {
    char program_name[] = "Sogang CSEG475_5475 Kernel_Optimization_120210400";
    fprintf(stdout, "\n###  %s\n\n", program_name);
    fprintf(stdout, "/////////////////////////////////////////////////////////////////////////\n");
    fprintf(stdout, "### INPUT FILE NAME = \t\t%s\n", INPUT_FILE_NAME);
    fprintf(stdout, "### OUTPUT FILE NAME = \t\t%s\n\n", OUTPUT_FILE_NAME);

    fprintf(stdout, "### IMAGE OPERATION = ");
    switch (IMAGE_OPERATION) {
    case SoA_GS_CPU:
        fprintf(stdout, "Structure of Arrays (GRAYSCALE) on CPU\n");
        break;
    case SoA_SO_CPU:
        fprintf(stdout, "Structure of Arrays (SOBEL) on CPU\n");
        break;
    case AoS_GS_CPU:
        fprintf(stdout, "Arrays of Structures (GRAYSCALE) on CPU\n");
        break;
    case AoS_SO_CPU:
        fprintf(stdout, "Arrays of Structures (SOBEL) on CPU\n");
        break;
    case SoA_GS_GPU:
        fprintf(stdout, "Structure of Arrays (GRAYSCALE) on GPU\n");
        break;
    case SoA_SO_GPU:
        fprintf(stdout, "Structure of Arrays (SOBEL) on GPU\n");
        break;
    case AoS_GS_GPU:
        fprintf(stdout, "Arrays of Structures (GRAYSCALE) on GPU\n");
        break;
    case AoS_SO_GPU:
        fprintf(stdout, "Arrays of Structures (SOBEL) on GPU\n");
        break;
    case Kernel_Naive:
        fprintf(stdout, "Kernel Naive version on GPU\n");
        break;
    case Kernel_Advanced:
        fprintf(stdout, "Kernel Advanced version on GPU\n");
        break;
    case Kernel_Optimized:
        fprintf(stdout, "Kernel Optimized version on GPU\n");
        break;
    default:
        fprintf(stderr, "*** Error: unknown image operation...\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "/////////////////////////////////////////////////////////////////////////\n\n");

    read_input_image_from_file32(INPUT_FILE_NAME);   
    prepare_output_image();
    set_sobel_x_filter(SOBEL_FILTER_SIZE);
    set_sobel_y_filter(SOBEL_FILTER_SIZE);

    //printf("KS: %d \n", KERNEL_SELECTION); printf("IO: %d \n", IMAGE_OPERATION);
    // (0) [RGBA 영상을 GrayScale 영상으로 변환] SoA_GS_CPU 수행.
    if (IMAGE_OPERATION == SoA_GS_CPU) {
        prepare_SoA_input_and_output();         // input과 output에 대한 메모리 할당을 준비한다.
        CHECK_TIME_START(_start, _freq);
        convert_to_greyscale_image_SoA_CPU();  // Write a GPU version of this function.
        CHECK_TIME_END(_start, _end, _freq, compute_time);
        fprintf(stdout, "\n^^^ Conversion time by host clock = %.3fms\n\n", compute_time);
        convert_SoA_output_to_output_image_data();  // SoA output을 output_image_data로 변환해준다.
        write_output_image_to_file32(OUTPUT_FILE_NAME);

        free(context.SoA_image_input.R_plane);
        free(context.SoA_image_input.G_plane);
        free(context.SoA_image_input.B_plane);
        free(context.SoA_image_input.A_plane);
        free(context.SoA_image_output.R_plane);
        free(context.SoA_image_output.G_plane);
        free(context.SoA_image_output.B_plane);
        free(context.SoA_image_output.A_plane);
    }
    // (1) [RGBA 영상을 GrayScale 영상으로 변환] AoS_GS_CPU 수행.
    else if (IMAGE_OPERATION == AoS_GS_CPU) {
        prepare_AoS_input_and_output();
        CHECK_TIME_START(_start, _freq);
        convert_to_greyscale_image_AoS_CPU();  // Write a GPU version of this function.
        CHECK_TIME_END(_start, _end, _freq, compute_time);
        fprintf(stdout, "\n^^^ Conversion time by host clock = %.3fms\n\n", compute_time);
        convert_AoS_output_to_output_image_data();
        write_output_image_to_file32(OUTPUT_FILE_NAME);

        free(context.AoS_image_input);
        free(context.AoS_image_output);
    }
    // (2) [RGBA 영상을 GrayScale로 변환 후, Sobel operator를 적용] SoA_SO_CPU 수행.
    else if (IMAGE_OPERATION == SoA_SO_CPU) {
        prepare_SoA_input_and_output_SO();         // input과 output에 대한 메모리 할당을 준비한다.
        CHECK_TIME_START(_start, _freq);
        // My function.
        apply_sobel_operator_to_image_SoA_CPU();    // Write a GPU version of this function.
        CHECK_TIME_END(_start, _end, _freq, compute_time);
        fprintf(stdout, "\n^^^ Conversion time by host clock = %.3fms\n\n", compute_time);
        convert_SoA_output_to_output_image_data();  // SoA output을 output_image_data로 변환해준다.
        write_output_image_to_file32(OUTPUT_FILE_NAME);

        free(context.SoA_image_input.R_plane);
        free(context.SoA_image_input.G_plane);
        free(context.SoA_image_input.B_plane);
        free(context.SoA_image_input.A_plane);
        free(context.SoA_image_midput.R_plane);
        free(context.SoA_image_midput.G_plane);
        free(context.SoA_image_midput.B_plane);
        free(context.SoA_image_midput.A_plane);
        free(context.SoA_image_output.R_plane);
        free(context.SoA_image_output.G_plane);
        free(context.SoA_image_output.B_plane);
        free(context.SoA_image_output.A_plane);
    }
    // (3) [RGBA 영상을 GrayScale로 변환 후, Sobel operator를 적용] AoS_SO_CPU 수행.
    else if (IMAGE_OPERATION == AoS_SO_CPU) {
        prepare_AoS_input_and_output_SO();
        CHECK_TIME_START(_start, _freq);
        // My function.
        apply_sobel_operator_to_image_AoS_CPU();    // Write a GPU version of this function.
        CHECK_TIME_END(_start, _end, _freq, compute_time);
        fprintf(stdout, "\n^^^ Conversion time by host clock = %.3fms\n\n", compute_time);
        convert_AoS_output_to_output_image_data();
        write_output_image_to_file32(OUTPUT_FILE_NAME);

        free(context.AoS_image_input);
        free(context.AoS_image_midput);
        free(context.AoS_image_output);
    }

    //////////////////////////////////////////// GPU Commanding codes //////////////////////////////////////////////////
    // (4) [SoA_GS_GPU] 수행.
    else if (IMAGE_OPERATION == SoA_GS_GPU) {
        fprintf(stdout, "\n/////////////////////////////////////////////////////////////////////////\n");
        fprintf(stdout, "### NUMBER OF EXECUTIONS = \t%d\n\n", N_EXECUTIONS);
        fprintf(stdout, "### WORK-GROUP SIZE = \t\t(%d, %d)\n", LOCAL_WORK_SIZE_0, LOCAL_WORK_SIZE_1);
        fprintf(stdout, "### KERNEL SELECTION = \t\t%d\n", KERNEL_SELECTION);
        fprintf(stdout, "/////////////////////////////////////////////////////////////////////////\n\n");
        prepare_SoA_input_and_output();

        int flag = initialize_OpenCL_SoA();
        if (flag) goto finish;
        flag = set_local_work_size_and_kernel_arguments_SoA();
        if (flag) goto finish;
        flag = run_OpenCL_kernel_SoA();
        if (flag) goto finish;

        convert_SoA_output_to_output_image_data();  // SoA output을 output_image_data로 변환해준다.
        write_output_image_to_file32(OUTPUT_FILE_NAME);

    finish:
        free(context.SoA_image_input.R_plane);
        free(context.SoA_image_input.G_plane);
        free(context.SoA_image_input.B_plane);
        free(context.SoA_image_input.A_plane);
        free(context.SoA_image_output.R_plane);
        free(context.SoA_image_output.G_plane);
        free(context.SoA_image_output.B_plane);
        free(context.SoA_image_output.A_plane);
        clean_up_system();
    }
    // (5) [AoS_GS_GPU] 수행.
    else if (IMAGE_OPERATION == AoS_GS_GPU) {
        fprintf(stdout, "\n/////////////////////////////////////////////////////////////////////////\n");
        fprintf(stdout, "### NUMBER OF EXECUTIONS = \t%d\n\n", N_EXECUTIONS);
        fprintf(stdout, "### WORK-GROUP SIZE = \t\t(%d, %d)\n", LOCAL_WORK_SIZE_0, LOCAL_WORK_SIZE_1);
        fprintf(stdout, "### KERNEL SELECTION = \t\t%d\n", KERNEL_SELECTION);
        fprintf(stdout, "/////////////////////////////////////////////////////////////////////////\n\n");
        prepare_AoS_input_and_output();

        int flag = initialize_OpenCL_AoS();
        if (flag) goto finish2;
        flag = set_local_work_size_and_kernel_arguments_AoS();
        if (flag) goto finish2;
        flag = run_OpenCL_kernel_AoS();
        if (flag) goto finish2;

        convert_AoS_output_to_output_image_data();
        write_output_image_to_file32(OUTPUT_FILE_NAME);

    finish2:
        free(context.AoS_image_input);
        free(context.AoS_image_output);
        clean_up_system(); 
    }
    // (6) [SoA_SO_GPU] 수행.
    else if (IMAGE_OPERATION == SoA_SO_GPU) {
        fprintf(stdout, "\n/////////////////////////////////////////////////////////////////////////\n");
        fprintf(stdout, "### NUMBER OF EXECUTIONS = \t%d\n\n", N_EXECUTIONS);
        fprintf(stdout, "### WORK-GROUP SIZE = \t\t(%d, %d)\n", LOCAL_WORK_SIZE_0, LOCAL_WORK_SIZE_1);
        fprintf(stdout, "### KERNEL SELECTION = \t\t%d\n", KERNEL_SELECTION);
        fprintf(stdout, "/////////////////////////////////////////////////////////////////////////\n\n");
        prepare_SoA_input_and_output_SO();

        int flag = initialize_OpenCL_SoA_SO();
        if (flag) goto finish3;
        flag = set_local_work_size_and_kernel_arguments_SoA_SO();
        if (flag) goto finish3;
        flag = run_OpenCL_kernel_SoA_SO();
        if (flag) goto finish3;

        convert_SoA_output_to_output_image_data();  // SoA output을 output_image_data로 변환해준다.
        write_output_image_to_file32(OUTPUT_FILE_NAME);

    finish3:
        free(context.SoA_image_input.R_plane);
        free(context.SoA_image_input.G_plane);
        free(context.SoA_image_input.B_plane);
        free(context.SoA_image_input.A_plane);
        free(context.SoA_image_midput.R_plane);
        free(context.SoA_image_midput.G_plane);
        free(context.SoA_image_midput.B_plane);
        free(context.SoA_image_midput.A_plane);
        free(context.SoA_image_output.R_plane);
        free(context.SoA_image_output.G_plane);
        free(context.SoA_image_output.B_plane);
        free(context.SoA_image_output.A_plane);
        clean_up_system();
    }
    // (7) [AoS_SO_GPU] 수행.
    else if (IMAGE_OPERATION == AoS_SO_GPU) {
        fprintf(stdout, "\n/////////////////////////////////////////////////////////////////////////\n");
        fprintf(stdout, "### NUMBER OF EXECUTIONS = \t%d\n\n", N_EXECUTIONS);
        fprintf(stdout, "### WORK-GROUP SIZE = \t\t(%d, %d)\n", LOCAL_WORK_SIZE_0, LOCAL_WORK_SIZE_1);
        fprintf(stdout, "### KERNEL SELECTION = \t\t%d\n", KERNEL_SELECTION);
        fprintf(stdout, "/////////////////////////////////////////////////////////////////////////\n\n");
        prepare_AoS_input_and_output_SO();

        int flag = initialize_OpenCL_AoS_SO();
        if (flag) goto finish4;
        flag = set_local_work_size_and_kernel_arguments_AoS_SO();
        if (flag) goto finish4;
        flag = run_OpenCL_kernel_AoS_SO();
        if (flag) goto finish4;

        convert_AoS_output_to_output_image_data();
        write_output_image_to_file32(OUTPUT_FILE_NAME);

    finish4:
        free(context.AoS_image_input);
        free(context.AoS_image_midput);
        free(context.AoS_image_output);
        clean_up_system();
    }

    /////////////////////////////////////// [HW2] GPU 커널 최적화. /////////////////////////////////////////
    // (8) Kernel-Naive.
    else if (IMAGE_OPERATION == Kernel_Naive) {
    fprintf(stdout, "\n/////////////////////////////////////////////////////////////////////////\n");
    fprintf(stdout, "### NUMBER OF EXECUTIONS = \t%d\n\n", N_EXECUTIONS);
    fprintf(stdout, "### WORK-GROUP SIZE = \t\t(%d, %d)\n", LOCAL_WORK_SIZE_0, LOCAL_WORK_SIZE_1);
    fprintf(stdout, "### KERNEL SELECTION = \t\t%d\n", KERNEL_SELECTION);
    fprintf(stdout, "/////////////////////////////////////////////////////////////////////////\n\n");
    prepare_AoS_input_and_output_SO();

    int flag = initialize_OpenCL_AoS_SO();
    if (flag) goto finish5;
    flag = set_local_work_size_and_kernel_arguments_AoS_SO_KO();
    if (flag) goto finish5;
    flag = run_OpenCL_kernel_AoS_SO();
    if (flag) goto finish5;

    convert_AoS_output_to_output_image_data();
    write_output_image_to_file32(OUTPUT_FILE_NAME);

finish5:
    free(context.AoS_image_input);
    free(context.AoS_image_midput);
    free(context.AoS_image_output);
    clean_up_system();
    }
    // (9) Intermediate.
    else if (IMAGE_OPERATION == Kernel_Advanced) {
    fprintf(stdout, "\n/////////////////////////////////////////////////////////////////////////\n");
    fprintf(stdout, "### NUMBER OF EXECUTIONS = \t%d\n\n", N_EXECUTIONS);
    fprintf(stdout, "### WORK-GROUP SIZE = \t\t(%d, %d)\n", LOCAL_WORK_SIZE_0, LOCAL_WORK_SIZE_1);
    fprintf(stdout, "### KERNEL SELECTION = \t\t%d\n", KERNEL_SELECTION);
    fprintf(stdout, "/////////////////////////////////////////////////////////////////////////\n\n");
    prepare_AoS_input_and_output_SO();

    int flag = initialize_OpenCL_AoS_SO();
    if (flag) goto finish6;
    flag = set_local_work_size_and_kernel_arguments_AoS_SO_KO();
    if (flag) goto finish6;
    flag = run_OpenCL_kernel_AoS_SO();
    if (flag) goto finish6;

    convert_AoS_output_to_output_image_data();
    write_output_image_to_file32(OUTPUT_FILE_NAME);

finish6:
    free(context.AoS_image_input);
    free(context.AoS_image_midput);
    free(context.AoS_image_output);
    clean_up_system();
    }
    // (10) Kernel-Optimized.
    else if (IMAGE_OPERATION == Kernel_Optimized) {
    fprintf(stdout, "\n/////////////////////////////////////////////////////////////////////////\n");
    fprintf(stdout, "### NUMBER OF EXECUTIONS = \t%d\n\n", N_EXECUTIONS);
    fprintf(stdout, "### WORK-GROUP SIZE = \t\t(%d, %d)\n", LOCAL_WORK_SIZE_0, LOCAL_WORK_SIZE_1);
    fprintf(stdout, "### KERNEL SELECTION = \t\t%d\n", KERNEL_SELECTION);
    fprintf(stdout, "/////////////////////////////////////////////////////////////////////////\n\n");
    prepare_AoS_input_and_output_SO();

    int flag = initialize_OpenCL_AoS_SO();
    if (flag) goto finish7;
    flag = set_local_work_size_and_kernel_arguments_AoS_SO_KO_local();
    if (flag) goto finish7;
    flag = run_OpenCL_kernel_AoS_SO();
    if (flag) goto finish7;

    convert_AoS_output_to_output_image_data();
    write_output_image_to_file32(OUTPUT_FILE_NAME);

finish7:
    free(context.AoS_image_input);
    free(context.AoS_image_midput);
    free(context.AoS_image_output);
    clean_up_system();
    }
    else {
        fprintf(stdout, "^^^ Nothing has been done!\n");
    }

    return 0;
}

void clean_up_system(void) {
    // Free OpenCL and other resources. 
    if (context.prog_src.string) free(context.prog_src.string);
    if (context.BO_input) clReleaseMemObject(context.BO_input);
    if (context.BO_input) clReleaseMemObject(context.BO_midput);    // 추가.
    if (context.BO_input) clReleaseMemObject(context.BO_output);

    if (context.kernel) clReleaseKernel(context.kernel);
    if (context.program) clReleaseProgram(context.program);
    if (context.cmd_queue) clReleaseCommandQueue(context.cmd_queue);
    if (context.device_id) clReleaseDevice(context.device_id);
    if (context.context) clReleaseContext(context.context);
    if (context.event_for_timing) clReleaseEvent(context.event_for_timing);
    // what else?
}

// [AoS]
int initialize_OpenCL_AoS(void) {
    /* Get the first platform. */
    errcode_ret = clGetPlatformIDs(1, &context.platform_id, NULL);
    // You may skip error checking if you think it is unnecessary.
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Get the first GPU device. */
    errcode_ret = clGetDeviceIDs(context.platform_id, CL_DEVICE_TYPE_GPU, 1, &context.device_id, NULL);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    fprintf(stdout, "\n^^^ The first GPU device on the platform ^^^\n");
    print_device_0(context.device_id);

    /* Create a context with the devices. */
    context.context = clCreateContext(NULL, 1, &context.device_id, NULL, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Create a command-queue for the GPU device. */
    // Use clCreateCommandQueueWithProperties() for OpenCL 2.0.
    context.cmd_queue = clCreateCommandQueue(context.context, context.device_id,
        CL_QUEUE_PROFILING_ENABLE, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Create a program from OpenCL C source code. */
    context.prog_src.length = read_kernel_from_file(OPENCL_C_PROG_FILE_NAME,
        &context.prog_src.string);
    context.program = clCreateProgramWithSource(context.context, 1,
        (const char**)&context.prog_src.string, &context.prog_src.length, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Build a program executable from the program object. */
    const char options[] = "-cl-std=CL1.2";
    errcode_ret = clBuildProgram(context.program, 1, &context.device_id, options, NULL, NULL);
    if (errcode_ret != CL_SUCCESS) {
        print_build_log(context.program, context.device_id, "GPU");
        return 1;
    }

    /* Create the kernel from the program. */
    context.kernel = clCreateKernel(context.program, KERNEL_NAME, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Create input and output buffer objects. */
    context.BO_input = clCreateBuffer(context.context, CL_MEM_READ_ONLY,
        sizeof(Pixel_Channels) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    context.BO_output = clCreateBuffer(context.context, CL_MEM_WRITE_ONLY,
        sizeof(Pixel_Channels) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    fprintf(stdout, "    [Data Transfer to GPU] \n");

    CHECK_TIME_START(_start, _freq);
    // Move the input data from the host memory to the GPU device memory.
    errcode_ret = clEnqueueWriteBuffer(context.cmd_queue, context.BO_input, CL_FALSE, 0,
        sizeof(Pixel_Channels) * context.image_height * context.image_width, context.AoS_image_input, 0, NULL, NULL);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Wait until all data transfers finish. */
    clFinish(context.cmd_queue);
    CHECK_TIME_END(_start, _end, _freq, compute_time);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    fprintf(stdout, "      * Time by host clock = %.3fms\n\n", compute_time);
    return 0;
}

// [AoS] context.BO_midput에 대한 처리가 추가.
int initialize_OpenCL_AoS_SO(void) {
    /* Get the first platform. */
    errcode_ret = clGetPlatformIDs(1, &context.platform_id, NULL);
    // You may skip error checking if you think it is unnecessary.
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Get the first GPU device. */
    errcode_ret = clGetDeviceIDs(context.platform_id, CL_DEVICE_TYPE_GPU, 1, &context.device_id, NULL);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    fprintf(stdout, "\n^^^ The first GPU device on the platform ^^^\n");
    print_device_0(context.device_id);

    /* Create a context with the devices. */
    context.context = clCreateContext(NULL, 1, &context.device_id, NULL, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Create a command-queue for the GPU device. */
    // Use clCreateCommandQueueWithProperties() for OpenCL 2.0.
    context.cmd_queue = clCreateCommandQueue(context.context, context.device_id,
        CL_QUEUE_PROFILING_ENABLE, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Create a program from OpenCL C source code. */
    context.prog_src.length = read_kernel_from_file(OPENCL_C_PROG_FILE_NAME,
        &context.prog_src.string);
    context.program = clCreateProgramWithSource(context.context, 1,
        (const char**)&context.prog_src.string, &context.prog_src.length, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Build a program executable from the program object. */
    const char options[] = "-cl-std=CL1.2";
    errcode_ret = clBuildProgram(context.program, 1, &context.device_id, options, NULL, NULL);
    if (errcode_ret != CL_SUCCESS) {
        print_build_log(context.program, context.device_id, "GPU");
        return 1;
    }

    /* Create the kernel from the program. */
    context.kernel = clCreateKernel(context.program, KERNEL_NAME, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Create input and output buffer objects. */
    context.BO_input = clCreateBuffer(context.context, CL_MEM_READ_ONLY,
        sizeof(Pixel_Channels) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    context.BO_midput = clCreateBuffer(context.context, CL_MEM_READ_ONLY,
        sizeof(Pixel_Channels) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    context.BO_output = clCreateBuffer(context.context, CL_MEM_WRITE_ONLY,
        sizeof(Pixel_Channels) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;


    context.BO_filter_x = clCreateBuffer(context.context, CL_MEM_READ_ONLY,
        sizeof(int) * 5 * 5, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    context.BO_filter_y = clCreateBuffer(context.context, CL_MEM_READ_ONLY,
        sizeof(int) * 5 * 5, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    fprintf(stdout, "    [Data Transfer to GPU] \n");

    CHECK_TIME_START(_start, _freq);
    // Move the input data from the host memory to the GPU device memory.
    errcode_ret = clEnqueueWriteBuffer(context.cmd_queue, context.BO_input, CL_FALSE, 0,
        sizeof(Pixel_Channels) * context.image_height * context.image_width, context.AoS_image_input, 0, NULL, NULL);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;


    errcode_ret = clEnqueueWriteBuffer(context.cmd_queue, context.BO_filter_x, CL_FALSE, 0,
        sizeof(int) * 5 * 5, context.sobel_filter_x.weights, 0, NULL, NULL);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    errcode_ret = clEnqueueWriteBuffer(context.cmd_queue, context.BO_filter_y, CL_FALSE, 0,
        sizeof(int) * 5 * 5, context.sobel_filter_y.weights, 0, NULL, NULL);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Wait until all data transfers finish. */
    clFinish(context.cmd_queue);
    CHECK_TIME_END(_start, _end, _freq, compute_time);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    fprintf(stdout, "      * Time by host clock = %.3fms\n\n", compute_time);
    return 0;
}

// [SoA]
int initialize_OpenCL_SoA(void) {
    /* Get the first platform. */
    errcode_ret = clGetPlatformIDs(1, &context.platform_id, NULL);
    // You may skip error checking if you think it is unnecessary.
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Get the first GPU device. */
    errcode_ret = clGetDeviceIDs(context.platform_id, CL_DEVICE_TYPE_GPU, 1, &context.device_id, NULL);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    fprintf(stdout, "\n^^^ The first GPU device on the platform ^^^\n");
    print_device_0(context.device_id);

    /* Create a context with the devices. */
    context.context = clCreateContext(NULL, 1, &context.device_id, NULL, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Create a command-queue for the GPU device. */
    // Use clCreateCommandQueueWithProperties() for OpenCL 2.0.
    context.cmd_queue = clCreateCommandQueue(context.context, context.device_id,
        CL_QUEUE_PROFILING_ENABLE, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Create a program from OpenCL C source code. */
    context.prog_src.length = read_kernel_from_file(OPENCL_C_PROG_FILE_NAME,
        &context.prog_src.string);
    context.program = clCreateProgramWithSource(context.context, 1,
        (const char**)&context.prog_src.string, &context.prog_src.length, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Build a program executable from the program object. */
    const char options[] = "-cl-std=CL1.2";
    errcode_ret = clBuildProgram(context.program, 1, &context.device_id, options, NULL, NULL);
    if (errcode_ret != CL_SUCCESS) {
        print_build_log(context.program, context.device_id, "GPU");
        return 1;
    }

    /* Create the kernel from the program. */
    context.kernel = clCreateKernel(context.program, KERNEL_NAME, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Create input and output buffer objects. */
    context.BO_input_R = clCreateBuffer(context.context, CL_MEM_READ_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    context.BO_output_R = clCreateBuffer(context.context, CL_MEM_WRITE_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    context.BO_input_G = clCreateBuffer(context.context, CL_MEM_READ_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    context.BO_output_G = clCreateBuffer(context.context, CL_MEM_WRITE_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    context.BO_input_B = clCreateBuffer(context.context, CL_MEM_READ_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    context.BO_output_B = clCreateBuffer(context.context, CL_MEM_WRITE_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    context.BO_input_A = clCreateBuffer(context.context, CL_MEM_READ_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    context.BO_output_A = clCreateBuffer(context.context, CL_MEM_WRITE_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    fprintf(stdout, "    [Data Transfer to GPU] \n");

    CHECK_TIME_START(_start, _freq);
    // Move the input data from the host memory to the GPU device memory.
    errcode_ret = clEnqueueWriteBuffer(context.cmd_queue, context.BO_input_R, CL_FALSE, 0,
        sizeof(BYTE) * context.image_height * context.image_width, context.SoA_image_input.R_plane, 0, NULL, NULL);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    errcode_ret = clEnqueueWriteBuffer(context.cmd_queue, context.BO_input_G, CL_FALSE, 0,
        sizeof(BYTE) * context.image_height * context.image_width, context.SoA_image_input.G_plane, 0, NULL, NULL);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    errcode_ret = clEnqueueWriteBuffer(context.cmd_queue, context.BO_input_B, CL_FALSE, 0,
        sizeof(BYTE) * context.image_height * context.image_width, context.SoA_image_input.B_plane, 0, NULL, NULL);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    errcode_ret = clEnqueueWriteBuffer(context.cmd_queue, context.BO_input_A, CL_FALSE, 0,
        sizeof(BYTE) * context.image_height * context.image_width, context.SoA_image_input.A_plane, 0, NULL, NULL);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Wait until all data transfers finish. */
    clFinish(context.cmd_queue);
    CHECK_TIME_END(_start, _end, _freq, compute_time);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    fprintf(stdout, "      * Time by host clock = %.3fms\n\n", compute_time);
    return 0;
}

// [SoA] context.BO_midput에 대한 처리가 추가.
int initialize_OpenCL_SoA_SO(void) {
    /* Get the first platform. */
    errcode_ret = clGetPlatformIDs(1, &context.platform_id, NULL);
    // You may skip error checking if you think it is unnecessary.
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Get the first GPU device. */
    errcode_ret = clGetDeviceIDs(context.platform_id, CL_DEVICE_TYPE_GPU, 1, &context.device_id, NULL);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    fprintf(stdout, "\n^^^ The first GPU device on the platform ^^^\n");
    print_device_0(context.device_id);

    /* Create a context with the devices. */
    context.context = clCreateContext(NULL, 1, &context.device_id, NULL, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Create a command-queue for the GPU device. */
    // Use clCreateCommandQueueWithProperties() for OpenCL 2.0.
    context.cmd_queue = clCreateCommandQueue(context.context, context.device_id,
        CL_QUEUE_PROFILING_ENABLE, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Create a program from OpenCL C source code. */
    context.prog_src.length = read_kernel_from_file(OPENCL_C_PROG_FILE_NAME,
        &context.prog_src.string);
    context.program = clCreateProgramWithSource(context.context, 1,
        (const char**)&context.prog_src.string, &context.prog_src.length, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Build a program executable from the program object. */
    const char options[] = "-cl-std=CL1.2";
    errcode_ret = clBuildProgram(context.program, 1, &context.device_id, options, NULL, NULL);
    if (errcode_ret != CL_SUCCESS) {
        print_build_log(context.program, context.device_id, "GPU");
        return 1;
    }

    /* Create the kernel from the program. */
    context.kernel = clCreateKernel(context.program, KERNEL_NAME, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Create input and output buffer objects. */
    context.BO_input_R = clCreateBuffer(context.context, CL_MEM_READ_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    context.BO_midput_R = clCreateBuffer(context.context, CL_MEM_WRITE_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    context.BO_output_R = clCreateBuffer(context.context, CL_MEM_WRITE_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    context.BO_input_G = clCreateBuffer(context.context, CL_MEM_READ_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    context.BO_midput_G = clCreateBuffer(context.context, CL_MEM_WRITE_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    context.BO_output_G = clCreateBuffer(context.context, CL_MEM_WRITE_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    context.BO_input_B = clCreateBuffer(context.context, CL_MEM_READ_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    context.BO_midput_B = clCreateBuffer(context.context, CL_MEM_READ_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    context.BO_output_B = clCreateBuffer(context.context, CL_MEM_WRITE_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    context.BO_input_A = clCreateBuffer(context.context, CL_MEM_READ_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    context.BO_midput_A = clCreateBuffer(context.context, CL_MEM_READ_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    context.BO_output_A = clCreateBuffer(context.context, CL_MEM_WRITE_ONLY,
        sizeof(BYTE) * context.image_height * context.image_width, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;


    context.BO_filter_x = clCreateBuffer(context.context, CL_MEM_READ_ONLY,
        sizeof(int) * 5 * 5, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    context.BO_filter_y = clCreateBuffer(context.context, CL_MEM_READ_ONLY,
        sizeof(int) * 5 * 5, NULL, &errcode_ret);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    fprintf(stdout, "    [Data Transfer to GPU] \n");

    CHECK_TIME_START(_start, _freq);
    // Move the input data from the host memory to the GPU device memory.
    errcode_ret = clEnqueueWriteBuffer(context.cmd_queue, context.BO_input_R, CL_FALSE, 0,
        sizeof(BYTE) * context.image_height * context.image_width, context.SoA_image_input.R_plane, 0, NULL, NULL);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    errcode_ret = clEnqueueWriteBuffer(context.cmd_queue, context.BO_input_G, CL_FALSE, 0,
        sizeof(BYTE) * context.image_height * context.image_width, context.SoA_image_input.G_plane, 0, NULL, NULL);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    errcode_ret = clEnqueueWriteBuffer(context.cmd_queue, context.BO_input_B, CL_FALSE, 0,
        sizeof(BYTE) * context.image_height * context.image_width, context.SoA_image_input.B_plane, 0, NULL, NULL);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;
    errcode_ret = clEnqueueWriteBuffer(context.cmd_queue, context.BO_input_A, CL_FALSE, 0,
        sizeof(BYTE) * context.image_height * context.image_width, context.SoA_image_input.A_plane, 0, NULL, NULL);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;


    errcode_ret = clEnqueueWriteBuffer(context.cmd_queue, context.BO_filter_x, CL_FALSE, 0,
        sizeof(int) * 5 * 5, context.sobel_filter_x.weights, 0, NULL, NULL);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    errcode_ret = clEnqueueWriteBuffer(context.cmd_queue, context.BO_filter_y, CL_FALSE, 0,
        sizeof(int) * 5 * 5, context.sobel_filter_y.weights, 0, NULL, NULL);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    /* Wait until all data transfers finish. */
    clFinish(context.cmd_queue);
    CHECK_TIME_END(_start, _end, _freq, compute_time);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    fprintf(stdout, "      * Time by host clock = %.3fms\n\n", compute_time);
    return 0;
}

// Work Size와 Kernel source code에 들어갈 argument에 대한 정보를 setting하는 함수.
int set_local_work_size_and_kernel_arguments_AoS(void) {
    context.global_work_size[0] = context.image_width;
    context.global_work_size[1] = context.image_height;

    context.local_work_size[0] = LOCAL_WORK_SIZE_0;
    context.local_work_size[1] = LOCAL_WORK_SIZE_1;

    /* Set the kenel arguments. */
    errcode_ret = clSetKernelArg(context.kernel, 0, sizeof(cl_mem), &context.BO_input);
    errcode_ret |= clSetKernelArg(context.kernel, 1, sizeof(cl_mem), &context.BO_output);
    errcode_ret |= clSetKernelArg(context.kernel, 2, sizeof(int), &context.image_width);
    errcode_ret |= clSetKernelArg(context.kernel, 3, sizeof(int), &context.image_height);

    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    printf_KernelWorkGroupInfo(context.kernel, context.device_id);

    return 0;
}

// Work Size와 Kernel source code에 들어갈 argument에 대한 정보를 setting하는 함수.
int set_local_work_size_and_kernel_arguments_AoS_SO(void) {
    context.global_work_size[0] = context.image_width;
    context.global_work_size[1] = context.image_height;

    context.local_work_size[0] = LOCAL_WORK_SIZE_0;
    context.local_work_size[1] = LOCAL_WORK_SIZE_1;

    /* Set the kenel arguments. */
    errcode_ret = clSetKernelArg(context.kernel, 0, sizeof(cl_mem), &context.BO_input);
    errcode_ret |= clSetKernelArg(context.kernel, 1, sizeof(cl_mem), &context.BO_midput);
    errcode_ret |= clSetKernelArg(context.kernel, 2, sizeof(cl_mem), &context.BO_output);
    errcode_ret |= clSetKernelArg(context.kernel, 3, sizeof(int), &context.image_width);
    errcode_ret |= clSetKernelArg(context.kernel, 4, sizeof(int), &context.image_height);

    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    printf_KernelWorkGroupInfo(context.kernel, context.device_id);

    return 0;
}

// Work Size와 Kernel source code에 들어갈 argument에 대한 정보를 setting하는 함수.
int set_local_work_size_and_kernel_arguments_AoS_SO_KO(void) {
    context.global_work_size[0] = context.image_width;
    context.global_work_size[1] = context.image_height;

    context.local_work_size[0] = LOCAL_WORK_SIZE_0;
    context.local_work_size[1] = LOCAL_WORK_SIZE_1;

    /* Set the kenel arguments. */
    errcode_ret = clSetKernelArg(context.kernel, 0, sizeof(cl_mem), &context.BO_input);
    errcode_ret |= clSetKernelArg(context.kernel, 1, sizeof(cl_mem), &context.BO_midput);
    errcode_ret |= clSetKernelArg(context.kernel, 2, sizeof(cl_mem), &context.BO_output);
    errcode_ret |= clSetKernelArg(context.kernel, 3, sizeof(int), &context.image_width);
    errcode_ret |= clSetKernelArg(context.kernel, 4, sizeof(int), &context.image_height);

    errcode_ret |= clSetKernelArg(context.kernel, 5, sizeof(cl_mem), &context.BO_filter_x);
    errcode_ret |= clSetKernelArg(context.kernel, 6, sizeof(cl_mem), &context.BO_filter_y);
    errcode_ret |= clSetKernelArg(context.kernel, 7, sizeof(int), &context.sobel_filter_x.width);
    errcode_ret |= clSetKernelArg(context.kernel, 8, sizeof(int), &context.sobel_filter_y.width);

    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    printf_KernelWorkGroupInfo(context.kernel, context.device_id);

    return 0;
}

// Work Size와 Kernel source code에 들어갈 argument에 대한 정보를 setting하는 함수.
int set_local_work_size_and_kernel_arguments_AoS_SO_KO_local(void) {
    context.global_work_size[0] = context.image_width;
    context.global_work_size[1] = context.image_height;

    context.local_work_size[0] = LOCAL_WORK_SIZE_0;
    context.local_work_size[1] = LOCAL_WORK_SIZE_1;

    /* Set the kenel arguments. */
    errcode_ret = clSetKernelArg(context.kernel, 0, sizeof(cl_mem), &context.BO_input);
    errcode_ret |= clSetKernelArg(context.kernel, 1, sizeof(cl_mem), &context.BO_output);
    errcode_ret |= clSetKernelArg(context.kernel, 2, sizeof(int), &context.image_width);
    errcode_ret |= clSetKernelArg(context.kernel, 3, sizeof(int), &context.image_height);

    errcode_ret |= clSetKernelArg(context.kernel, 4, sizeof(cl_mem), &context.BO_filter_x);
    errcode_ret |= clSetKernelArg(context.kernel, 5, sizeof(cl_mem), &context.BO_filter_y);
    errcode_ret |= clSetKernelArg(context.kernel, 6, sizeof(int), &context.sobel_filter_x.width);
    errcode_ret |= clSetKernelArg(context.kernel, 7, sizeof(int), &context.sobel_filter_y.width);

    int twice_half_filter_width = 2 * (context.sobel_filter_x.width / 2);
    size_t local_mem_size = sizeof(cl_uchar4)
        * (context.local_work_size[0] + twice_half_filter_width)
        * (context.local_work_size[1] + twice_half_filter_width);
    errcode_ret |= clSetKernelArg(context.kernel, 8, local_mem_size, NULL);
    fprintf(stdout, "^^^ Necessary local memory = %d bytes (%d, %d, %d) ^^^\n\n", local_mem_size,
        sizeof(cl_uchar4), context.local_work_size[0] + twice_half_filter_width,
        context.local_work_size[1] + twice_half_filter_width);

    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    printf_KernelWorkGroupInfo(context.kernel, context.device_id);

    return 0;
}

// Work Size와 Kernel source code에 들어갈 argument에 대한 정보를 setting하는 함수.
int set_local_work_size_and_kernel_arguments_SoA(void) {
    context.global_work_size[0] = context.image_width;
    context.global_work_size[1] = context.image_height;

    context.local_work_size[0] = LOCAL_WORK_SIZE_0;
    context.local_work_size[1] = LOCAL_WORK_SIZE_1;

    /* Set the kenel arguments. */
    errcode_ret = clSetKernelArg(context.kernel, 0, sizeof(cl_mem), &context.BO_input_R);
    errcode_ret |= clSetKernelArg(context.kernel, 1, sizeof(cl_mem), &context.BO_output_R);
    errcode_ret |= clSetKernelArg(context.kernel, 2, sizeof(cl_mem), &context.BO_input_G);
    errcode_ret |= clSetKernelArg(context.kernel, 3, sizeof(cl_mem), &context.BO_output_G);
    errcode_ret |= clSetKernelArg(context.kernel, 4, sizeof(cl_mem), &context.BO_input_B);
    errcode_ret |= clSetKernelArg(context.kernel, 5, sizeof(cl_mem), &context.BO_output_B);
    errcode_ret |= clSetKernelArg(context.kernel, 6, sizeof(cl_mem), &context.BO_input_A);
    errcode_ret |= clSetKernelArg(context.kernel, 7, sizeof(cl_mem), &context.BO_output_A);
    errcode_ret |= clSetKernelArg(context.kernel, 8, sizeof(int), &context.image_width);
    errcode_ret |= clSetKernelArg(context.kernel, 9, sizeof(int), &context.image_height);

    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    printf_KernelWorkGroupInfo(context.kernel, context.device_id);

    return 0;
}

// Work Size와 Kernel source code에 들어갈 argument에 대한 정보를 setting하는 함수.
int set_local_work_size_and_kernel_arguments_SoA_SO(void) {
    context.global_work_size[0] = context.image_width;
    context.global_work_size[1] = context.image_height;

    context.local_work_size[0] = LOCAL_WORK_SIZE_0;
    context.local_work_size[1] = LOCAL_WORK_SIZE_1;

    /* Set the kenel arguments. */
    errcode_ret = clSetKernelArg(context.kernel, 0, sizeof(cl_mem), &context.BO_input_R);
    errcode_ret |= clSetKernelArg(context.kernel, 1, sizeof(cl_mem), &context.BO_midput_R);
    errcode_ret |= clSetKernelArg(context.kernel, 2, sizeof(cl_mem), &context.BO_output_R);
    errcode_ret |= clSetKernelArg(context.kernel, 3, sizeof(cl_mem), &context.BO_input_G);
    errcode_ret |= clSetKernelArg(context.kernel, 4, sizeof(cl_mem), &context.BO_midput_G);
    errcode_ret |= clSetKernelArg(context.kernel, 5, sizeof(cl_mem), &context.BO_output_G);
    errcode_ret |= clSetKernelArg(context.kernel, 6, sizeof(cl_mem), &context.BO_input_B);
    errcode_ret |= clSetKernelArg(context.kernel, 7, sizeof(cl_mem), &context.BO_midput_B);
    errcode_ret |= clSetKernelArg(context.kernel, 8, sizeof(cl_mem), &context.BO_output_B);
    errcode_ret |= clSetKernelArg(context.kernel, 9, sizeof(cl_mem), &context.BO_input_A);
    errcode_ret |= clSetKernelArg(context.kernel, 10, sizeof(cl_mem), &context.BO_midput_A);
    errcode_ret |= clSetKernelArg(context.kernel, 11, sizeof(cl_mem), &context.BO_output_A);
    errcode_ret |= clSetKernelArg(context.kernel, 12, sizeof(int), &context.image_width);
    errcode_ret |= clSetKernelArg(context.kernel, 13, sizeof(int), &context.image_height);

    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    printf_KernelWorkGroupInfo(context.kernel, context.device_id);

    return 0;
}

// Work Size와 Kernel source code에 들어갈 argument에 대한 정보를 setting하는 함수.
int set_local_work_size_and_kernel_arguments_SoA_SO_KO(void) {
    context.global_work_size[0] = context.image_width;
    context.global_work_size[1] = context.image_height;

    context.local_work_size[0] = LOCAL_WORK_SIZE_0;
    context.local_work_size[1] = LOCAL_WORK_SIZE_1;

    /* Set the kenel arguments. */
    errcode_ret = clSetKernelArg(context.kernel, 0, sizeof(cl_mem), &context.BO_input_R);
    errcode_ret |= clSetKernelArg(context.kernel, 1, sizeof(cl_mem), &context.BO_midput_R);
    errcode_ret |= clSetKernelArg(context.kernel, 2, sizeof(cl_mem), &context.BO_output_R);
    errcode_ret |= clSetKernelArg(context.kernel, 3, sizeof(cl_mem), &context.BO_input_G);
    errcode_ret |= clSetKernelArg(context.kernel, 4, sizeof(cl_mem), &context.BO_midput_G);
    errcode_ret |= clSetKernelArg(context.kernel, 5, sizeof(cl_mem), &context.BO_output_G);
    errcode_ret |= clSetKernelArg(context.kernel, 6, sizeof(cl_mem), &context.BO_input_B);
    errcode_ret |= clSetKernelArg(context.kernel, 7, sizeof(cl_mem), &context.BO_midput_B);
    errcode_ret |= clSetKernelArg(context.kernel, 8, sizeof(cl_mem), &context.BO_output_B);
    errcode_ret |= clSetKernelArg(context.kernel, 9, sizeof(cl_mem), &context.BO_input_A);
    errcode_ret |= clSetKernelArg(context.kernel, 10, sizeof(cl_mem), &context.BO_midput_A);
    errcode_ret |= clSetKernelArg(context.kernel, 11, sizeof(cl_mem), &context.BO_output_A);
    errcode_ret |= clSetKernelArg(context.kernel, 12, sizeof(int), &context.image_width);
    errcode_ret |= clSetKernelArg(context.kernel, 13, sizeof(int), &context.image_height);
    errcode_ret |= clSetKernelArg(context.kernel, 14, sizeof(cl_mem), &context.BO_filter_x);
    errcode_ret |= clSetKernelArg(context.kernel, 15, sizeof(cl_mem), &context.BO_filter_y);
    errcode_ret |= clSetKernelArg(context.kernel, 16, sizeof(int), &context.sobel_filter_x.width);
    errcode_ret |= clSetKernelArg(context.kernel, 17, sizeof(int), &context.sobel_filter_y.width);

    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    printf_KernelWorkGroupInfo(context.kernel, context.device_id);

    return 0;
}

int run_OpenCL_kernel_AoS(void) {
    fprintf(stdout, "    [Kernel Execution] \n");

    fp_stat = util_open_stat_file_append(STAT_FILE_NAME);
    util_stamp_stat_file_device_name_and_time(fp_stat, context.device_id);
    util_reset_event_time();

    CHECK_TIME_START(_start, _freq);
    /* Execute the kernel on the device. */
    for (int i = 0; i < N_EXECUTIONS; i++) {
        //printf("gws[0]: %d, gws[1]: %d lws: %d \n", context.global_work_size[0], context.global_work_size[1], context.local_work_size[0]);
        errcode_ret = clEnqueueNDRangeKernel(context.cmd_queue, context.kernel, 2, NULL,
            context.global_work_size, context.local_work_size, 0, NULL, &context.event_for_timing);
        if (CHECK_ERROR_CODE(errcode_ret)) return 1;
        clWaitForEvents(1, &context.event_for_timing);
        if (CHECK_ERROR_CODE(errcode_ret)) return 1;
        util_accumulate_event_times_1_2(context.event_for_timing);
    }
    CHECK_TIME_END(_start, _end, _freq, compute_time);

    fprintf(stdout, "      * Time by host clock = %.3fms\n\n", compute_time);
    util_print_accumulated_device_time_1_2(N_EXECUTIONS);
 //   MAKE_STAT_ITEM_LIST_CBO(tmp_string, context_CL.global_work_size, context_CL.local_work_size);
    util_stamp_stat_file_ave_device_time_START_to_END_1_2_string(fp_stat, tmp_string);
    util_close_stat_file_append(fp_stat);

    fprintf(stdout, "    [Data Transfer] \n");

    /* Read back the device buffer to the host array. */
    CHECK_TIME_START(_start, _freq);
    errcode_ret = clEnqueueReadBuffer(context.cmd_queue, context.BO_output, CL_TRUE, 0,
        sizeof(Pixel_Channels) * context.image_height * context.image_width, context.AoS_image_output, 0, NULL,
        &context.event_for_timing);
    CHECK_TIME_END(_start, _end, _freq, compute_time);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    fprintf(stdout, "      * Time by host clock = %.3fms\n\n", compute_time);
    print_device_time(context.event_for_timing);

    return 0;
}

int run_OpenCL_kernel_AoS_SO(void) {
    fprintf(stdout, "    [Kernel Execution] \n");

    fp_stat = util_open_stat_file_append(STAT_FILE_NAME);
    util_stamp_stat_file_device_name_and_time(fp_stat, context.device_id);
    util_reset_event_time();

    CHECK_TIME_START(_start, _freq);
    /* Execute the kernel on the device. */
    for (int i = 0; i < N_EXECUTIONS; i++) {
        //printf("gws[0]: %d, gws[1]: %d lws: %d \n", context.global_work_size[0], context.global_work_size[1], context.local_work_size[0]);
        errcode_ret = clEnqueueNDRangeKernel(context.cmd_queue, context.kernel, 2, NULL,
            context.global_work_size, context.local_work_size, 0, NULL, &context.event_for_timing);
        if (CHECK_ERROR_CODE(errcode_ret)) return 1;
        clWaitForEvents(1, &context.event_for_timing);
        if (CHECK_ERROR_CODE(errcode_ret)) return 1;
        util_accumulate_event_times_1_2(context.event_for_timing);
    }
    CHECK_TIME_END(_start, _end, _freq, compute_time);

    fprintf(stdout, "      * Time by host clock = %.3fms\n\n", compute_time);
    util_print_accumulated_device_time_1_2(N_EXECUTIONS);
    //   MAKE_STAT_ITEM_LIST_CBO(tmp_string, context_CL.global_work_size, context_CL.local_work_size);
    util_stamp_stat_file_ave_device_time_START_to_END_1_2_string(fp_stat, tmp_string);
    util_close_stat_file_append(fp_stat);

    fprintf(stdout, "    [Data Transfer] \n");

    /* Read back the device buffer to the host array. */
    CHECK_TIME_START(_start, _freq);
 /*   errcode_ret = clEnqueueReadBuffer(context.cmd_queue, context.BO_midput, CL_TRUE, 0,
        sizeof(Pixel_Channels) * context.image_height * context.image_width, context.AoS_image_midput, 0, NULL,
        &context.event_for_timing);*/
    errcode_ret = clEnqueueReadBuffer(context.cmd_queue, context.BO_output, CL_TRUE, 0,
        sizeof(Pixel_Channels) * context.image_height * context.image_width, context.AoS_image_output, 0, NULL,
        &context.event_for_timing);
    CHECK_TIME_END(_start, _end, _freq, compute_time);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    fprintf(stdout, "      * Time by host clock = %.3fms\n\n", compute_time);
    print_device_time(context.event_for_timing);

    return 0;
}

int run_OpenCL_kernel_SoA(void) {
    fprintf(stdout, "    [Kernel Execution] \n");

    fp_stat = util_open_stat_file_append(STAT_FILE_NAME);
    util_stamp_stat_file_device_name_and_time(fp_stat, context.device_id);
    util_reset_event_time();

    CHECK_TIME_START(_start, _freq);
    /* Execute the kernel on the device. */
    for (int i = 0; i < N_EXECUTIONS; i++) {
        //printf("gws[0]: %d, gws[1]: %d lws: %d \n", context.global_work_size[0], context.global_work_size[1], context.local_work_size[0]);
        errcode_ret = clEnqueueNDRangeKernel(context.cmd_queue, context.kernel, 2, NULL,
            context.global_work_size, context.local_work_size, 0, NULL, &context.event_for_timing);
        if (CHECK_ERROR_CODE(errcode_ret)) return 1;
        clWaitForEvents(1, &context.event_for_timing);
        if (CHECK_ERROR_CODE(errcode_ret)) return 1;
        util_accumulate_event_times_1_2(context.event_for_timing);
    }
    CHECK_TIME_END(_start, _end, _freq, compute_time);

    fprintf(stdout, "      * Time by host clock = %.3fms\n\n", compute_time);
    util_print_accumulated_device_time_1_2(N_EXECUTIONS);
    //   MAKE_STAT_ITEM_LIST_CBO(tmp_string, context_CL.global_work_size, context_CL.local_work_size);
    util_stamp_stat_file_ave_device_time_START_to_END_1_2_string(fp_stat, tmp_string);
    util_close_stat_file_append(fp_stat);

    fprintf(stdout, "    [Data Transfer] \n");

    /* Read back the device buffer to the host array. */
    CHECK_TIME_START(_start, _freq);
    errcode_ret = clEnqueueReadBuffer(context.cmd_queue, context.BO_output_R, CL_TRUE, 0,
        sizeof(BYTE) * context.image_height * context.image_width, context.SoA_image_output.R_plane, 0, NULL,
        &context.event_for_timing);
    errcode_ret = clEnqueueReadBuffer(context.cmd_queue, context.BO_output_G, CL_TRUE, 0,
        sizeof(BYTE) * context.image_height * context.image_width, context.SoA_image_output.G_plane, 0, NULL,
        &context.event_for_timing);
    errcode_ret = clEnqueueReadBuffer(context.cmd_queue, context.BO_output_B, CL_TRUE, 0,
        sizeof(BYTE) * context.image_height * context.image_width, context.SoA_image_output.B_plane, 0, NULL,
        &context.event_for_timing);
    errcode_ret = clEnqueueReadBuffer(context.cmd_queue, context.BO_output_A, CL_TRUE, 0,
        sizeof(BYTE) * context.image_height * context.image_width, context.SoA_image_output.A_plane, 0, NULL,
        &context.event_for_timing);
    CHECK_TIME_END(_start, _end, _freq, compute_time);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    fprintf(stdout, "      * Time by host clock = %.3fms\n\n", compute_time);
    print_device_time(context.event_for_timing);

    return 0;
}

int run_OpenCL_kernel_SoA_SO(void) {
    fprintf(stdout, "    [Kernel Execution] \n");

    fp_stat = util_open_stat_file_append(STAT_FILE_NAME);
    util_stamp_stat_file_device_name_and_time(fp_stat, context.device_id);
    util_reset_event_time();

    CHECK_TIME_START(_start, _freq);
    /* Execute the kernel on the device. */
    for (int i = 0; i < N_EXECUTIONS; i++) {
        //printf("gws[0]: %d, gws[1]: %d lws: %d \n", context.global_work_size[0], context.global_work_size[1], context.local_work_size[0]);
        errcode_ret = clEnqueueNDRangeKernel(context.cmd_queue, context.kernel, 2, NULL,
            context.global_work_size, context.local_work_size, 0, NULL, &context.event_for_timing);
        if (CHECK_ERROR_CODE(errcode_ret)) return 1;
        clWaitForEvents(1, &context.event_for_timing);
        if (CHECK_ERROR_CODE(errcode_ret)) return 1;
        util_accumulate_event_times_1_2(context.event_for_timing);
    }
    CHECK_TIME_END(_start, _end, _freq, compute_time);

    fprintf(stdout, "      * Time by host clock = %.3fms\n\n", compute_time);
    util_print_accumulated_device_time_1_2(N_EXECUTIONS);
 //   MAKE_STAT_ITEM_LIST_CBO(tmp_string, context.global_work_size, context.local_work_size);
    util_stamp_stat_file_ave_device_time_START_to_END_1_2_string(fp_stat, tmp_string);
    util_close_stat_file_append(fp_stat);

    fprintf(stdout, "    [Data Transfer] \n");

    /* Read back the device buffer to the host array. */
    CHECK_TIME_START(_start, _freq);
    errcode_ret = clEnqueueReadBuffer(context.cmd_queue, context.BO_output_R, CL_TRUE, 0,
        sizeof(BYTE) * context.image_height * context.image_width, context.SoA_image_output.R_plane, 0, NULL,
        &context.event_for_timing);
    errcode_ret = clEnqueueReadBuffer(context.cmd_queue, context.BO_output_G, CL_TRUE, 0,
        sizeof(BYTE) * context.image_height * context.image_width, context.SoA_image_output.G_plane, 0, NULL,
        &context.event_for_timing);
    errcode_ret = clEnqueueReadBuffer(context.cmd_queue, context.BO_output_B, CL_TRUE, 0,
        sizeof(BYTE) * context.image_height * context.image_width, context.SoA_image_output.B_plane, 0, NULL,
        &context.event_for_timing);
    errcode_ret = clEnqueueReadBuffer(context.cmd_queue, context.BO_output_A, CL_TRUE, 0,
        sizeof(BYTE) * context.image_height * context.image_width, context.SoA_image_output.A_plane, 0, NULL,
        &context.event_for_timing);
    CHECK_TIME_END(_start, _end, _freq, compute_time);
    if (CHECK_ERROR_CODE(errcode_ret)) return 1;

    fprintf(stdout, "      * Time by host clock = %.3fms\n\n", compute_time);
    print_device_time(context.event_for_timing);

    return 0;
}