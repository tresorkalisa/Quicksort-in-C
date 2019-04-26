#include "stdlib.h"
#include "stdio.h"
#include "omp.h"

// parallel partition
int ParPartition(int *a, int p, int r) {
    int b[r-p];
    int key = *(a+r);
    int lt[r-p];
    int gt[r-p];
    int cnt_lt = 0; // count 1 in the lt array
    int cnt_gt = 0; // count 1 in the gt array
    int j=p;
    int k = 0;
    // deal with gt and lt array
#pragma omp parallel for
    for ( j=p; j<r; ++j) {
        b[j-p] = *(a+j);
        if (*(a+j) < key) {
            lt[j-p] = 1;
            gt[j-p] = 0;
        } else {
            lt[j-p] = 0;
            gt[j-p] = 1;
        }
    }
    // calculate the new position of the elements
    for ( j=0; j<(r-p); ++j) {
        if (lt[j]) {
            ++cnt_lt;
            lt[j] = cnt_lt;
        } else
            lt[j] = cnt_lt;
        if (gt[j]) {
            ++cnt_gt;
            gt[j] = cnt_gt;
        } else
            gt[j] = cnt_gt;
    }

    k = lt[r-p-1];
    *(a+p+k) = key;
    // move elements to their new positon
#pragma omp parallel for
    for ( j=p; j<r; ++j) {
        if (b[j-p] < key)
            *(a+p+lt[j-p]-1) = b[j-p];
        else if (b[j-p] > key)
            *(a+k+gt[j-p]) = b[j-p];
    }
    return (k+p);
}

void ParQuickSort(int *a, int p, int r) {
    int q;
    if (p<r) {
        q = ParPartition(a, p, r);
#pragma omp parallel sections
        {
#pragma omp section
            ParQuickSort(a, p, q-1);
#pragma omp section
            ParQuickSort(a, q+1, r);
        }
    }
}

int main() {
    int a[10] = {5, 3, 8, 4, 0, 9, 2, 1, 7, 6};
    ParQuickSort(a, 0, 9);
    int i=0;
    for (; i!=10; ++i)
        printf("%d\t", a[i]);
    printf("\n");
    return 0;
}
