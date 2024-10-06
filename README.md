
# HyRMIS Sort: Hybrid Radix Merge Insertion Sort

HyRMIS Sort is an efficient hybrid sorting algorithm that combines the strengths of Radix Sort, Merge Sort, and Insertion Sort to achieve optimal performance across different input sizes. This algorithm was cocreated with artificial intelligence (AI) to provide a more efficient sorting solution.

## Algorithm Overview

HyRMIS Sort adapts its sorting strategy based on the size of the input list:

1. For small lists (size <= 16 by default), it uses Insertion Sort.
2. For medium-sized lists (16 < size <= 1000 by default), it uses Merge Sort.
3. For large lists (size > 1000 by default), it uses Radix Sort.

This hybrid approach allows HyRMIS Sort to maintain good performance across various input sizes and distributions.

## Usage

```python
from main import hyrmis_sort

# Example usage
list = [64, 34, 25, 12, 22, 11, 90]
sorted_list = hyrmis_sort(list)
print(sorted_list)
```

## Time Complexity

The time complexity of HyRMIS Sort varies depending on the size of the input:

1. Small lists (size <= 16):
   - Uses Insertion Sort
   - Time complexity: O(n^2)

2. Medium-sized lists (16 < size <= 1000):
   - Uses Merge Sort
   - Time complexity: O(n log n)

3. Large lists (size > 1000):
   - Uses Radix Sort
   - Time complexity: O(d * (n + k)), where d is the number of digits in the maximum number, and k is the range of the input (10 for decimal numbers)

Overall time complexity:
- Best Case: O(n) when the input is already sorted and small enough to use Insertion Sort
- Average Case: O(n log n) for medium-sized lists using Merge Sort
- Worst Case: O(d * (n + k)) for large lists using Radix Sort

The hybrid nature of HyRMIS Sort allows it to achieve optimal performance across different input sizes by leveraging the strengths of each sorting algorithm in its respective range.

## Testing

The HyRMIS Sort algorithm comes with a set of unit tests to ensure its correctness. The tests are located in the `test.py` file and cover different scenarios for the sorting algorithm.

### Test Cases

1. **Insertion Sort Test**: Tests the sorting of a small list of integers using the Insertion Sort component of HyRMIS.
2. **Merge Sort Test**: Tests the sorting of a medium-sized list of integers using the Merge Sort component of HyRMIS.
3. **Radix Sort Test**: Tests the sorting of a large list of random integers using the Radix Sort component of HyRMIS.

### Running the Tests

To run the tests, follow these steps:

1. Ensure you have Python installed on your system.
2. Navigate to the directory containing the `test.py` file.
3. Run the following command in your terminal:

```bash
python test.py
```

## Inspiration

HyRMIS Sort draws inspiration from Timsort, a hybrid sorting algorithm for use in Python. 