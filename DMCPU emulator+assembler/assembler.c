#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <direct.h>
#include <stdint.h>

#define MAX_LABELS 128
#define LABEL_LEN 32

typedef struct {
    char name[LABEL_LEN];
    int address;
} Label;

Label labels[MAX_LABELS];
int labelCount = 0;

// Trim leading and trailing whitespace in-place
void trim(char* str) {
    if (!str) return;
    // Trim leading
    char* p = str;
    while (isspace((unsigned char)*p)) p++;
    if (p != str) memmove(str, p, strlen(p) + 1);
    // Trim trailing
    char* end = str + strlen(str) - 1;
    while (end >= str && isspace((unsigned char)*end)) *end-- = '\0';
}

int findLabel(const char* name) {
    char temp[LABEL_LEN];
    strncpy(temp, name, LABEL_LEN-1);
    temp[LABEL_LEN-1] = '\0';
    trim(temp);
    for (int i = 0; i < labelCount; i++) {
        if (strcmp(labels[i].name, temp) == 0)
            return labels[i].address;
    }
    return -1; // Not found
}

int decodeFunc(const char* func) {
    if (strcmp(func, "add") == 0) return 0;
    else if (strcmp(func, "sub") == 0) return 1;
    else if (strcmp(func, "or") == 0) return 2;
    else if (strcmp(func, "nor") == 0) return 3;
    else if (strcmp(func, "and") == 0) return 4;
    else if (strcmp(func, "nand") == 0) return 5;
    else if (strcmp(func, "xor") == 0) return 6;
    else if (strcmp(func, "xnor") == 0) return 7;
    else if (strcmp(func, "shl") == 0) return 8;
    else if (strcmp(func, "shr") == 0) return 9;
    else if (strcmp(func, "rol") == 0) return 10;
    else if (strcmp(func, "ror") == 0) return 11;
    else if (strcmp(func, "beq") == 0) return 12;
    else if (strcmp(func, "bne") == 0) return 13;
    else if (strcmp(func, "bnl") == 0) return 14;
    else if (strcmp(func, "bls") == 0) return 15;
    else if (strcmp(func, "bgr") == 0) return 16;
    else if (strcmp(func, "bng") == 0) return 17;
    else if (strcmp(func, "bic") == 0) return 18;
    else if (strcmp(func, "bnc") == 0) return 19;
    else if (strcmp(func, "bin") == 0) return 20;
    else if (strcmp(func, "bnn") == 0) return 21;
    else if (strcmp(func, "biz") == 0) return 22;
    else if (strcmp(func, "bnz") == 0) return 23;
    else if (strcmp(func, "brk") == 0) return 63;
    else return 0; // Unknown function
}

int decodeReg(const char* str) {
    char temp[LABEL_LEN];
    strncpy(temp, str, LABEL_LEN-1);
    temp[LABEL_LEN-1] = '\0';
    trim(temp);
    if (strcmp(temp, "reg0") == 0) return 0;
    else if (strcmp(temp, "reg1") == 0) return 1;
    else if (strcmp(temp, "reg2") == 0) return 2;
    else if (strcmp(temp, "reg3") == 0) return 3;
    else if (strcmp(temp, "reg4") == 0) return 4;
    else if (strcmp(temp, "reg5") == 0) return 5;
    else if (strcmp(temp, "reg6") == 0) return 6;
    else if (strcmp(temp, "ram") == 0) return 7;
    else if (strcmp(temp, "stk") == 0) return 8;
    else if (strcmp(temp, "clk") == 0) return 9;
    else if (isdigit((unsigned char)temp[0])) return atoi(temp);
    else {
        int addr = findLabel(temp);
        if (addr >= 0) return addr;
        else return 0; // Invalid string
    }
}

// Returns 1 if the string is a pure number (not regX, stk, ram, clk, or a label)
int is_number(const char* str) {
    if (!str || !*str) return 0;
    if (strncmp(str, "reg", 3) == 0 ||
        strcmp(str, "stk") == 0 ||
        strcmp(str, "ram") == 0 ||
        strcmp(str, "clk") == 0)
        return 0;
    for (const char* p = str; *p; ++p) {
        if (!isdigit((unsigned char)*p)) return 0;
    }
    return 1;
}

int is_reg_or_special(const char* str) {
    return strncmp(str, "reg", 3) == 0 ||
           strcmp(str, "stk") == 0 ||
           strcmp(str, "ram") == 0 ||
           strcmp(str, "clk") == 0;
}

int main() {
    printf("Current working directory: %s\n", _getcwd(NULL, 0));

    uint8_t progOut[256], progIn1[256], progIn2[256], progFunc[256];
    int progIndex = 0;

    // --- FIRST PASS: COLLECT LABELS ---
    FILE *fptr = fopen("main.txt", "r");
    if (fptr == NULL) { perror("fopen error"); return 1; }

    int instructionAddr = 0;
    char line[128];
    char pendingLabels[MAX_LABELS][LABEL_LEN];
    int pendingLabelCount = 0;

    while (fgets(line, sizeof(line), fptr)) {
        char *start = line;
        while (isspace((unsigned char)*start)) start++;
        char *end = start + strlen(start) - 1;
        while (end > start && isspace((unsigned char)*end)) *end-- = '\0';

        // Remove inline comments
        char *comment = strstr(start, "//");
        if (comment) *comment = '\0';

        size_t len = strlen(start);
        if (len == 0) continue;

        // Robust label detection: allow whitespace after colon
        char *colon = strchr(start, ':');
        if (colon) {
            char *after_colon = colon + 1;
            while (isspace((unsigned char)*after_colon)) after_colon++;
            if (*after_colon == '\0') {
                *colon = '\0';
                trim(start);
                if (strlen(start) > 0 && pendingLabelCount < MAX_LABELS) {
                    strncpy(pendingLabels[pendingLabelCount], start, LABEL_LEN-1);
                    pendingLabels[pendingLabelCount][LABEL_LEN-1] = '\0';
                    pendingLabelCount++;
                }
                continue;
            }
        }

        // Only assign pending labels if this is an instruction (4 tokens)
        int tokenCount = 0;
        char lineCopy[128];
        strcpy(lineCopy, start);
        char *tok = strtok(lineCopy, " \t");
        while (tok) {
            tokenCount++;
            tok = strtok(NULL, " \t");
        }
        if (tokenCount == 4) {
            // Assign all pending labels to this instruction address
            for (int i = 0; i < pendingLabelCount; i++) {
                printf("Assigning label '%s' to address %d\n", pendingLabels[i], instructionAddr);
                strncpy(labels[labelCount].name, pendingLabels[i], LABEL_LEN-1);
                labels[labelCount].name[LABEL_LEN-1] = '\0';
                labels[labelCount].address = instructionAddr;
                labelCount++;
            }
            pendingLabelCount = 0;
            instructionAddr++;
        }
    }
    fclose(fptr);

    // Print all labels for debugging
    printf("Labels:\n");
    for (int i = 0; i < labelCount; i++) {
        printf("  '%s' -> %d\n", labels[i].name, labels[i].address);
    }

    // --- SECOND PASS: ASSEMBLE ---
    fptr = fopen("main.txt", "r");
    if (fptr == NULL) {
        perror("fopen error");
        return 1;
    }
    while (fgets(line, sizeof(line), fptr)) {
        char *start = line;
        while (isspace((unsigned char)*start)) start++;
        char *end = start + strlen(start) - 1;
        while (end > start && isspace((unsigned char)*end)) *end-- = '\0';

        // Remove inline comments
        char *comment = strstr(start, "//");
        if (comment) *comment = '\0';

        size_t len = strlen(start);
        if (len == 0) continue;

        char *colon = strchr(start, ':');
        if (colon) {
            char *after_colon = colon + 1;
            while (isspace((unsigned char)*after_colon)) after_colon++;
            if (*after_colon == '\0') continue;
        }

        int tokenCount = 0;
        char *tokens[4];
        char *tok = strtok(start, " \t");
        while (tok && tokenCount < 4) {
            tokens[tokenCount++] = tok;
            tok = strtok(NULL, " \t");
        }
        if (tokenCount != 4) continue;

        // Trim tokens before use
        for (int i = 0; i < 4; i++) trim(tokens[i]);

        int funcCode = decodeFunc(tokens[3]);
        // PATCH: set immediate flag for numbers or labels (not registers/specials)
        if (is_number(tokens[1]) || (!is_reg_or_special(tokens[1]) && findLabel(tokens[1]) >= 0)) funcCode += 64;
        if (is_number(tokens[2]) || (!is_reg_or_special(tokens[2]) && findLabel(tokens[2]) >= 0)) funcCode += 128;
        uint8_t outCode = (uint8_t)decodeReg(tokens[0]);
        uint8_t in1Code = (uint8_t)decodeReg(tokens[1]);
        uint8_t in2Code = (uint8_t)decodeReg(tokens[2]);
        uint8_t fCode = (uint8_t)funcCode;

        // Debug print to verify encoding and label resolution
        if (!is_number(tokens[1]) && !is_reg_or_special(tokens[1])) {
            printf("Label lookup: '%s' -> %d\n", tokens[1], decodeReg(tokens[1]));
        }
        if (!is_number(tokens[2]) && !is_reg_or_special(tokens[2])) {
            printf("Label lookup: '%s' -> %d\n", tokens[2], decodeReg(tokens[2]));
        }
        printf("OUT='%s' IN1='%s' IN2='%s' FUNC=%d  ->  out=%d in1=%d in2=%d func=%d\n",
            tokens[0], tokens[1], tokens[2], funcCode, outCode, in1Code, in2Code, fCode);

        progOut[progIndex] = outCode;
        progIn1[progIndex] = in1Code;
        progIn2[progIndex] = in2Code;
        progFunc[progIndex] = fCode;
        progIndex++;
    }
    fclose(fptr);

    FILE *out = fopen("program.txt", "w");
    if (!out) {
        perror("fopen error for program.txt");
        return 1;
    }
    fprintf(out, "%d\n", progIndex);
    for (int i = 0; i < progIndex; i++) {
        fprintf(out, "%d %d %d %d\n", progOut[i], progIn1[i], progIn2[i], progFunc[i]);
    }
    fclose(out);

    printf("Exported %d instructions to program.txt\n", progIndex);
    return 0;
}
