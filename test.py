import unittest
import random
from main  import hyrmis_sort

class TestHyrmisSort(unittest.TestCase):
    def test_insert_sort(self):
        # Test case 1: Sorting a list of integers using HyRMIS Insertion Sort
        data = [170, -45, 75, -90, 802, -24, 2, 66, -1, 0, 123, -500]
        sorted_data = hyrmis_sort(data)
        self.assertEqual(sorted_data, [-500, -90, -45, -24, -1, 0, 2, 66, 75, 123, 170, 802])

    def test_merge_sort(self):
        # Test case 2: Sorting a list of integers using HyRMIS Merge Sort
        data = [170, -45, 75, -90, 802, -24, 2, 66, -1, 0, 123, -500, 10000, -10000, 1000000, -1000000, 1000000000, -1000000000]
        sorted_data = hyrmis_sort(data)
        self.assertEqual(sorted_data, [-1000000000, -1000000, -10000, -500, -90, -45, -24, -1, 0, 2, 66, 75, 123, 170, 802, 10000, 1000000, 1000000000])

    # Test case 3: Sorting a list of integers using HyRMIS Radix Sort
    def test_radix_sort(self):
        # Create a list of random integers for testing Radix Sort
        data = [random.randint(-10000, 10000) for _ in range(10000)]

        # Copy the data to a new list to avoid modifying the original data
        data_copy = data.copy()
        # Sort the data using the built-in sorted function
        data_copy.sort()
        # Sort the data using HyRMIS Radix Sort
        sorted_data = hyrmis_sort(data)
        # Check if the sorted data is sorted and matche the built-in sorted function
        self.assertEqual(sorted_data, data_copy)
if __name__ == '__main__':
    unittest.main()
