//
//  ApplyPatch.cpp
//  imgpatchtools
//
//  Created by Erfan Abdi on 7/19/17.
//  Copyright © 2017 Erfan Abdi. All rights reserved.
//

#include <iostream>
#include "applypatch/applypatch.h"
#include "edify/expr.h"

#include <memory>
#include <string>
#include <vector>

#include <android-base/parseint.h>
#include <android-base/strings.h>
#include <android-base/stringprintf.h>

// <file> <target> <tgt_sha1> <size> <init_sha1> <patch>
int ApplyPatchFn(const char* name, State* state, int argc, char * argv[]) {
    char* source_filename = argv[1];
    char* target_filename = argv[2];
    char* target_sha1 = argv[3];
    char* target_size_str = argv[4];
    
    size_t target_size;
    if (!android::base::ParseUint(target_size_str, &target_size)) {
        printf("%s(): can't parse \"%s\" as byte count\n",
                   name, target_size_str);
        free(source_filename);
        free(target_filename);
        free(target_sha1);
        free(target_size_str);
        return kArgsParsingFailure;
    }
    
    int patchcount = (argc-4) / 2;
    
    std::vector<char*> patch_sha_str;
    std::vector<Value*> patch_ptrs;
    
    for (int i = 0; i < patchcount * 2; i += 2) {
        Value patch_value;
        patch_value.type = VAL_BLOB;
        
        FILE *rm;
        int length;
        rm = fopen(argv[i+6], "r");
        fseek (rm, 0, SEEK_END);
        length = ftell (rm);
        fseek (rm, 0, SEEK_SET);
        patch_value.data = (char*)malloc ((length+1)*sizeof(char));
        
        if (patch_value.data)
        {
            fread (patch_value.data, sizeof(char), length, rm);
        }
        fclose (rm);
        
        patch_value.size = length;
        
        patch_sha_str.push_back(argv[i+5]);
        patch_ptrs.push_back(&patch_value);
    }
    
    int result = applypatch(source_filename, target_filename,
                            target_sha1, target_size,
                            patchcount, patch_sha_str.data(), patch_ptrs.data(), NULL);
    
    return result;
}

int main(int argc, char * argv[]) {
    if (argc < 7 || (argc % 2) == 0) {
        printf("usage: %s <file> <target> <tgt_sha1> <size> <init_sha1(s)> <patch(s)>\n",argv[0]);
        return 0;
    }
    
    State* state;
    int i;
    i=ApplyPatchFn("ApplyPatchFn", state, argc, argv);
    
    std::cout << "Done with error code : " << i << "\n";
    return 0;
}
