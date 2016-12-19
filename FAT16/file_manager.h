// Michael Eller mbe9a
// OS Machine Problem 3
// 22 November 2016
// file_manager.h (structs and function protos)

#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

// entry attribute definitions
#define READ_ONLY	0x01
#define HIDDEN 		0x02
#define SYSTEM 		0x04
#define VOLUME_ID 	0x08
#define DIRECTORY 	0x10
#define ARCHIVE 	0x20
#define LFN			0x0F

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <list>
#include <errno.h>
using namespace std;

// struct to hold the FAT16 boot sector
typedef struct {
    unsigned char jmp[3];
    char oem[8];
    unsigned short sector_size;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sectors;
    unsigned char number_of_fats;
    unsigned short root_dir_entries;
    unsigned short total_sectors_short; // if zero, later field is used
    unsigned char media_descriptor;
    unsigned short fat_size_sectors;
    unsigned short sectors_per_track;
    unsigned short number_of_heads;
    unsigned int hidden_sectors;
    unsigned int total_sectors_long;
    
    unsigned char drive_number;
    unsigned char current_head;
    unsigned char boot_signature;
    unsigned int volume_id;
    char volume_label[11];
    char fs_type[8];
    char boot_code[448];
    unsigned short boot_sector_signature;
} __attribute((packed)) Fat16BootSector;
 
 // struct to hold a FAT16 entry
typedef struct {
    unsigned char filename[8];
    unsigned char ext[3];
    unsigned char attributes;
    unsigned char reserved[10];
    unsigned short modify_time;
    unsigned short modify_date;
    unsigned short starting_cluster;
    unsigned int file_size;
} __attribute((packed)) Fat16Entry;

// function to retrieve the FAT value of the active cluster
bool get_fat_value(unsigned short* value);

// function to return the offset of the first data sector of a given cluster
unsigned int first_sector_of_cluster(unsigned int cluster);

// read dir entries in single cluster (directory)
void list_directory(string path);

// change_directory helper function
void next_entry(unsigned int starting_sector, unsigned short offset);

// method to change the PWD string, another helper function for 'cd'
void update_pwd(unsigned int direction);

// implementation of cd command
Fat16Entry change_directory(string path, bool ls);

// helper function for main, takes in a char* and outputs a list of arguments
list<char*> build_arguments(char* token);

// helper function for main, takes in a list<char*>* and call change_directory
Fat16Entry follow_path(list<char*>* arguments, bool ls);

// helper function to test if we are in the root directory
void check_and_set_root();

// copy out function implementation
void copy_out(char* dest, list<char*>* arguments);

// helper function for copy in, returns the offset of the dir entry written by this function
// very similar to 'ls'
unsigned int write_dir_entry(Fat16Entry dir_entry, list<char*>* arguments);

// copy in function implementation, file modify data and time are neglected, attributes are assumed 0x00
void copy_in(char* name, unsigned int* src_file, list<char*>* arguments);

#endif