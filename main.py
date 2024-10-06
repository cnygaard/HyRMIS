def hyrmis_sort(input_list, upper_threshold=1000, lower_threshold=16):
    """
    HyRMIS Sort: Hybrid Radix Merge Insertion Sort.

    Parameters:
    input_list (list): List of integers to sort.
    upper_threshold (int): Threshold to switch to radix sort.
    lower_threshold (int): Threshold to switch to insertion sort.

    Returns:
    list: Sorted list of integers.
    """
    n = len(input_list)
    if n <= lower_threshold:
        # Use insertion sort for small input_listays
        insertion_sort(input_list)
    elif n <= upper_threshold:
        # Use merge sort for medium input_listays
        input_list[:] = merge_sort(input_list)
    else:
        # Use radix sort for large input_listays
        input_list[:] = radix_sort(input_list)
    return input_list

def insertion_sort(input_list):
    """
    Insertion Sort implementation.
    """
    for i in range(1, len(input_list)):
        key = input_list[i]
        j = i - 1
        # Move elements greater than key one position ahead
        while j >= 0 and input_list[j] > key:
            input_list[j + 1] = input_list[j]
            j -= 1
        input_list[j + 1] = key

def merge_sort(input_list):
    """
    Merge Sort implementation.
    """
    if len(input_list) <= 1:
        return input_list
    mid = len(input_list) // 2
    left = merge_sort(input_list[:mid])
    right = merge_sort(input_list[mid:])
    return merge(left, right)

def merge(left, right):
    """
    Merges two sorted lists.
    """
    result = []
    i = j = 0
    # Merge the two halves
    while i < len(left) and j < len(right):
        if left[i] <= right[j]:
            result.append(left[i])
            i += 1
        else:
            result.append(right[j])
            j += 1
    # Append any remaining elements
    result.extend(left[i:])
    result.extend(right[j:])
    return result

def radix_sort(input_list):
    """
    Radix Sort implementation for integers, including negatives.
    """
    if not input_list:
        return input_list

    # Separate positive and negative numbers
    positives = [num for num in input_list if num >= 0]
    negatives = [-num for num in input_list if num < 0]

    # Sort positives and negatives separately
    if positives:
        positives = _radix_sort_positive(positives)
    if negatives:
        negatives = _radix_sort_positive(negatives)
        negatives = [-num for num in reversed(negatives)]

    # Combine negatives and positives
    return negatives + positives

def _radix_sort_positive(input_list):
    """
    Helper function to perform radix sort on non-negative integers.
    """
    max_num = max(input_list)
    exp = 1
    while max_num // exp > 0:
        counting_sort(input_list, exp)
        exp *= 10
    return input_list

def counting_sort(input_list, exp):
    """
    Counting Sort used by Radix Sort.
    """
    n = len(input_list)
    output = [0] * n
    count = [0] * 10  # Base 10 digits (0 to 9)

    # Store count of occurrences
    for i in range(n):
        index = (input_list[i] // exp) % 10
        count[index] += 1

    # Update count[i] to contain actual positions
    for i in range(1, 10):
        count[i] += count[i - 1]

    # Build the output list
    i = n - 1
    while i >= 0:
        index = (input_list[i] // exp) % 10
        output[count[index] - 1] = input_list[i]
        count[index] -= 1
        i -= 1

    # Copy the output input_listay to input_list
    for i in range(n):
        input_list[i] = output[i]

# Example usage:
if __name__ == "__main__":
    data = [170, -45, 75, -90, 802, -24, 2, 66, -1, 0, 123, -500]

    sorted_data = hyrmis_sort(data)
    print("Sorted input_list:", sorted_data)