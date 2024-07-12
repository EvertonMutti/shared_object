#include <Python.h>
#include "../libcsv.h"

static PyObject *pModule = NULL, *pFuncProcessCsvData = NULL, *pFuncProcessCsvFile = NULL;

void init_python() {
    Py_Initialize();
    PyRun_SimpleString("import sys; sys.path.append('.')");
    pModule = PyImport_ImportModule("dependencies.csv_processor");
    if (!pModule) {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"csv_processor\" module\n");
        return;
    }
    pFuncProcessCsvData = PyObject_GetAttrString(pModule, "process_csv_data");
    pFuncProcessCsvFile = PyObject_GetAttrString(pModule, "process_csv_file");
    if (!pFuncProcessCsvData || !PyCallable_Check(pFuncProcessCsvData) ||
        !pFuncProcessCsvFile || !PyCallable_Check(pFuncProcessCsvFile)) {
        PyErr_Print();
        fprintf(stderr, "Failed to find \"process_csv_data\" or \"process_csv_file\" function\n");
        Py_XDECREF(pFuncProcessCsvData);
        Py_XDECREF(pFuncProcessCsvFile);
        Py_DECREF(pModule);
        pModule = NULL;
        return;
    }
}

void call_python_function(PyObject *pFunc, const char *arg1, const char *arg2, const char *arg3) {
    if (!pFunc) {
        fprintf(stderr, "Python function not initialized\n");
        return;
    }

    PyObject *pArgs = PyTuple_Pack(3, PyUnicode_FromString(arg1), PyUnicode_FromString(arg2), PyUnicode_FromString(arg3));
    if (!pArgs) {
        PyErr_Print();
        fprintf(stderr, "Failed to create arguments tuple\n");
        return;
    }

    PyObject *pValue = PyObject_CallObject(pFunc, pArgs);
    Py_DECREF(pArgs);

    if (pValue != NULL) {
        printf("%s\n", PyUnicode_AsUTF8(pValue));
        Py_DECREF(pValue);
    } else {
        PyErr_Print();
        fprintf(stderr, "Call to Python function failed\n");
    }
}

void processCsv(const char csv[], const char selectedColumns[], const char rowFilterDefinitions[]) {
    call_python_function(pFuncProcessCsvData, csv, selectedColumns, rowFilterDefinitions);
}

void processCsvFile(const char csvFilePath[], const char selectedColumns[], const char rowFilterDefinitions[]) {
    call_python_function(pFuncProcessCsvFile, csvFilePath, selectedColumns, rowFilterDefinitions);
}

__attribute__((constructor))
void initialize() {
    init_python();
}

__attribute__((destructor))
void finalize() {
    if (pFuncProcessCsvData) Py_DECREF(pFuncProcessCsvData);
    if (pFuncProcessCsvFile) Py_DECREF(pFuncProcessCsvFile);
    if (pModule) Py_DECREF(pModule);
    Py_Finalize();
}
