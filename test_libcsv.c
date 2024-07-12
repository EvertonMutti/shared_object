#include <unity.h>
#include "libcsv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char csvFilePath[] = "./data.csv";
const char outputCsvFilePath[] = "output/processed_data.csv";

void setUp(void) {}

void tearDown(void) {}

char *readFileContent(const char *filename) {
    FILE *output_file = fopen(filename, "r");
    if (output_file == NULL) {
        TEST_FAIL_MESSAGE("Failed to open output file for reading.");
        return NULL;
    }

    fseek(output_file, 0, SEEK_END);
    long file_size = ftell(output_file);
    fseek(output_file, 0, SEEK_SET);

    char *file_content = (char *)malloc(file_size + 1);
    if (file_content == NULL) {
        TEST_FAIL_MESSAGE("Failed to allocate memory for file content.");
        fclose(output_file);
        return NULL;
    }

    size_t bytes_read = fread(file_content, 1, file_size, output_file);
    fclose(output_file);

    file_content[bytes_read] = '\0';

    return file_content;
}

void test_processCsv_no_filter(void) {
    const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
    const char selectedColumns[] = "";
    const char rowFilterDefinitions[] = "";

    processCsv(csv, selectedColumns, rowFilterDefinitions);

    char *file_content = readFileContent(outputCsvFilePath);
    if (file_content == NULL) {
        TEST_FAIL_MESSAGE("Failed to read output file.");
        return;
    }

    char expected_content[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";

    TEST_ASSERT_EQUAL_STRING(expected_content, file_content);
    free(file_content);
}

void test_processCsv_with_filters(void) {
    const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
    const char selectedColumns[] = "header3,header1";
    const char rowFilterDefinitions[] = "header1>1\nheader3<9";
    processCsv(csv, selectedColumns, rowFilterDefinitions);

    char *file_content = readFileContent(outputCsvFilePath);
    if (file_content == NULL) {
        TEST_FAIL_MESSAGE("Failed to read output file.");
        return;
    }

    char expected_content[] = "header1,header3\n4,6";

    TEST_ASSERT_EQUAL_STRING(expected_content, file_content);
    free(file_content);
}

void test_processCsvFile_no_filter(void) {
    const char selectedColumns[] = "";
    const char rowFilterDefinitions[] = "";
    processCsvFile(csvFilePath, selectedColumns, rowFilterDefinitions);

    char *file_content = readFileContent(outputCsvFilePath);
    if (file_content == NULL) {
        TEST_FAIL_MESSAGE("Failed to read output file.");
        return;
    }

    char expected_content[] = "col1,col2,col3,col4,col5,col6,col7\nl1c1,l1c2,l1c3,l1c4,l1c5,l1c6,l1c7\nl1c1,l1c2,l1c3,l1c4,l1c5,l1c6,l1c7\nl2c1,l2c2,l2c3,l2c4,l2c5,l2c6,l2c7\nl3c1,l3c2,l3c3,l3c4,l3c5,l3c6,l3c7";

    TEST_ASSERT_EQUAL_STRING(expected_content, file_content);
    free(file_content);
}

void test_processCsvFile_with_filters(void) {
    const char selectedColumns[] = "col1,col3,col4,col7";
    const char rowFilterDefinitions[] = "col1>l1c1\ncol3!=l1c3";
    processCsvFile(csvFilePath, selectedColumns, rowFilterDefinitions);

    char *file_content = readFileContent(outputCsvFilePath);
    if (file_content == NULL) {
        TEST_FAIL_MESSAGE("Failed to read output file.");
        return;
    }

    char expected_content[] = "col1,col3,col4,col7\nl2c1,l2c3,l2c4,l2c7\nl3c1,l3c3,l3c4,l3c7";

    TEST_ASSERT_EQUAL_STRING(expected_content, file_content);
    free(file_content);
}


void test_processCsvFile_selected_columns_and_specific_filters(void) {
    const char selectedColumns[] = "col1,col3,col4,col7";
    const char rowFilterDefinitions[] = "col1>l1c1\ncol3>l2c3";
    processCsvFile(csvFilePath, selectedColumns, rowFilterDefinitions);

    char *file_content = readFileContent(outputCsvFilePath);
    if (file_content == NULL) {
        TEST_FAIL_MESSAGE("Failed to read output file.");
        return;
    }

    char expected_content[] = "col1,col3,col4,col7\nl3c1,l3c3,l3c4,l3c7";

    TEST_ASSERT_EQUAL_STRING(expected_content, file_content);
    free(file_content);
}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_processCsv_no_filter);
    RUN_TEST(test_processCsv_with_filters);
    RUN_TEST(test_processCsvFile_no_filter);
    RUN_TEST(test_processCsvFile_with_filters);
    RUN_TEST(test_processCsvFile_selected_columns_and_specific_filters);
    return UNITY_END();
}
