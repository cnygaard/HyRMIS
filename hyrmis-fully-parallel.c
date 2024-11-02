// hyrmis.c
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// Optimized threshold values
#define LOWER_THRESHOLD 32
#define UPPER_THRESHOLD 2048
#define CACHE_BLOCK_SIZE 1024
#define PARALLEL_THRESHOLD 4096

// Forward declarations
static void insertion_sort(long arr[], Py_ssize_t n);
static void merge_sort(long arr[], Py_ssize_t left, Py_ssize_t right, long temp[]);
static void merge(long arr[], Py_ssize_t left, Py_ssize_t mid, Py_ssize_t right, long temp[]);
static void radix_sort(long arr[], Py_ssize_t n);
static void parallel_counting_sort(long arr[], Py_ssize_t n, long exp);

// Insertion sort implementation
static void insertion_sort(long arr[], Py_ssize_t n) {
    for (Py_ssize_t i = 1; i < n; i++) {
        long key = arr[i];
        Py_ssize_t j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// Main sorting function
static PyObject* hyrmis_sort(PyObject* self, PyObject* args) {
    PyObject *input_obj;
    PyObject *item;
    Py_ssize_t i, n;
    long *arr, *temp;

    if (!PyArg_ParseTuple(args, "O", &input_obj)) {
        return NULL;
    }

    if (!PyList_Check(input_obj)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be a list");
        return NULL;
    }

    n = PyList_Size(input_obj);
    arr = (long*)malloc(n * sizeof(long));
    if (arr == NULL) {
        PyErr_NoMemory();
        return NULL;
    }

    // Convert Python list to C array
    for (i = 0; i < n; i++) {
        item = PyList_GetItem(input_obj, i);
        if (!PyLong_Check(item)) {
            free(arr);
            PyErr_SetString(PyExc_TypeError, "All elements must be integers");
            return NULL;
        }
        arr[i] = PyLong_AsLong(item);
    }

    // Choose sorting algorithm based on size
    if (n <= LOWER_THRESHOLD) {
        insertion_sort(arr, n);
    } else if (n <= UPPER_THRESHOLD) {
        temp = (long*)malloc(n * sizeof(long));
        if (temp == NULL) {
            free(arr);
            PyErr_NoMemory();
            return NULL;
        }
        merge_sort(arr, 0, n - 1, temp);
        free(temp);
    } else {
        radix_sort(arr, n);
    }

    // Convert back to Python list
    for (i = 0; i < n; i++) {
        item = PyLong_FromLong(arr[i]);
        if (item == NULL) {
            free(arr);
            return NULL;
        }
        PyList_SetItem(input_obj, i, item);
    }

    free(arr);
    Py_RETURN_NONE;
}

// Merge sort implementation with parallel sections
static void merge_sort(long arr[], Py_ssize_t left, Py_ssize_t right, long temp[]) {
    if (right - left > LOWER_THRESHOLD) {
        Py_ssize_t mid = (left + right) / 2;
        
        if (right - left > PARALLEL_THRESHOLD) {
            #pragma omp parallel sections
            {
                #pragma omp section
                merge_sort(arr, left, mid, temp);
                #pragma omp section
                merge_sort(arr, mid + 1, right, temp);
            }
        } else {
            merge_sort(arr, left, mid, temp);
            merge_sort(arr, mid + 1, right, temp);
        }
        merge(arr, left, mid, right, temp);
    } else {
        insertion_sort(arr + left, right - left + 1);
    }
}

// Cache-friendly parallel merge
static void merge(long arr[], Py_ssize_t left, Py_ssize_t mid, Py_ssize_t right, long temp[]) {
    Py_ssize_t i = left, j = mid + 1, k = left;
    Py_ssize_t len = right - left + 1;

    if (len > PARALLEL_THRESHOLD) {
        #pragma omp parallel
        {
            int num_threads = omp_get_num_threads();
            int thread_id = omp_get_thread_num();
            Py_ssize_t chunk_size = (len + num_threads - 1) / num_threads;
            Py_ssize_t start = left + thread_id * chunk_size;
            Py_ssize_t end = start + chunk_size <= right + 1 ? start + chunk_size : right + 1;
            
            // Local merge
            Py_ssize_t local_i = start <= mid ? start : mid + 1;
            Py_ssize_t local_j = start <= mid ? mid + 1 : start;
            Py_ssize_t local_k = start;
            
            while (local_i <= mid && local_j <= right && local_k < end) {
                temp[local_k++] = (arr[local_i] <= arr[local_j]) ? arr[local_i++] : arr[local_j++];
            }
            
            while (local_i <= mid && local_k < end) temp[local_k++] = arr[local_i++];
            while (local_j <= right && local_k < end) temp[local_k++] = arr[local_j++];
            
            #pragma omp barrier
            
            // Copy back in parallel
            #pragma omp for
            for (Py_ssize_t idx = left; idx <= right; idx++) {
                arr[idx] = temp[idx];
            }
        }
    } else {
        while (i <= mid && j <= right) {
            Py_ssize_t i_end = (i + CACHE_BLOCK_SIZE <= mid) ? i + CACHE_BLOCK_SIZE : mid + 1;
            Py_ssize_t j_end = (j + CACHE_BLOCK_SIZE <= right) ? j + CACHE_BLOCK_SIZE : right + 1;
            
            while (i < i_end && j < j_end) {
                temp[k++] = (arr[i] <= arr[j]) ? arr[i++] : arr[j++];
            }
            
            if (i >= i_end) continue;
            if (j >= j_end) break;
        }

        while (i <= mid) temp[k++] = arr[i++];
        while (j <= right) temp[k++] = arr[j++];

        memcpy(arr + left, temp + left, len * sizeof(long));
    }
}

// Parallel radix sort implementation
static void radix_sort(long arr[], Py_ssize_t n) {
    if (n <= 1) return;

    // Find range in parallel
    long max = arr[0], min = arr[0];
    #pragma omp parallel
    {
        long local_max = LONG_MIN;
        long local_min = LONG_MAX;

        #pragma omp for nowait
        for (Py_ssize_t i = 0; i < n; i++) {
            if (arr[i] > local_max) local_max = arr[i];
            if (arr[i] < local_min) local_min = arr[i];
        }

        #pragma omp critical
        {
            if (local_max > max) max = local_max;
            if (local_min < min) min = local_min;
        }
    }

    // Handle negative numbers
    if (min < 0) {
        #pragma omp parallel for
        for (Py_ssize_t i = 0; i < n; i++) {
            arr[i] -= min;
        }
        max -= min;
    }

    // Parallel counting sort for each digit
    for (long exp = 1; max / exp > 0; exp *= 10) {
        parallel_counting_sort(arr, n, exp);
    }

    // Restore original values
    if (min < 0) {
        #pragma omp parallel for
        for (Py_ssize_t i = 0; i < n; i++) {
            arr[i] += min;
        }
    }
}

// Parallel counting sort implementation
static void parallel_counting_sort(long arr[], Py_ssize_t n, long exp) {
    long *output = malloc(n * sizeof(long));
    if (!output) return;

    int num_threads = omp_get_max_threads();
    long **local_counts = malloc(num_threads * sizeof(long*));
    if (!local_counts) {
        free(output);
        return;
    }

    // Initialize local count arrays
    for (int t = 0; t < num_threads; t++) {
        local_counts[t] = calloc(10, sizeof(long));
        if (!local_counts[t]) {
            for (int i = 0; i < t; i++) {
                free(local_counts[i]);
            }
            free(local_counts);
            free(output);
            return;
        }
    }

    // Parallel counting phase
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        #pragma omp for
        for (Py_ssize_t i = 0; i < n; i++) {
            int digit = (arr[i] / exp) % 10;
            local_counts[tid][digit]++;
        }
    }

    // Combine counts and calculate positions
    long total[10] = {0};
    for (int t = 0; t < num_threads; t++) {
        for (int d = 0; d < 10; d++) {
            total[d] += local_counts[t][d];
        }
    }

    for (int d = 1; d < 10; d++) {
        total[d] += total[d-1];
    }

    // Parallel distribution phase
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        long positions[10];
        memcpy(positions, total, sizeof(positions));

        #pragma omp for
        for (Py_ssize_t i = n - 1; i >= 0; i--) {
            int digit = (arr[i] / exp) % 10;
            long pos;
            #pragma omp atomic capture
            pos = --positions[digit];
            output[pos] = arr[i];
        }
    }

    // Parallel copy back
    #pragma omp parallel for
    for (Py_ssize_t i = 0; i < n; i++) {
        arr[i] = output[i];
    }

    // Cleanup
    for (int t = 0; t < num_threads; t++) {
        free(local_counts[t]);
    }
    free(local_counts);
    free(output);
}

// Module method definition
static PyMethodDef HyrmisMethods[] = {
    {"sort", hyrmis_sort, METH_VARARGS, "Sort a list using parallel HyRMIS algorithm"},
    {NULL, NULL, 0, NULL}
};

// Module definition
static struct PyModuleDef hyrmismodule = {
    PyModuleDef_HEAD_INIT,
    "hyrmis",
    "Hybrid Radix Merge Insertion Sort implementation",
    -1,
    HyrmisMethods
};

// Module initialization function
PyMODINIT_FUNC PyInit_hyrmis(void) {
    omp_set_dynamic(0);  // Disable dynamic adjustment of threads
    omp_set_num_threads(omp_get_num_procs());  // Use all available cores
    return PyModule_Create(&hyrmismodule);
}
