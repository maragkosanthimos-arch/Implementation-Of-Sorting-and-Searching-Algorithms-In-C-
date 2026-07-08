Part I: Sorting and Searching Algorithms

The first part of the program implements four distinct operations:Sorting records by Date in ascending order based on the Cumulative value, using Counting Sort and Merge Sort for experimental comparison.  

Sorting records by Date in ascending order based on the Value field, comparing Heap Sort and Quick Sort.  

Searching for a specific Date to retrieve its Value or Cumulative fields, comparing the average-case performance of Binary Search and Interpolation Search.  

Implementing Binary Interpolation Search (BIS) and its worst-case improvement variant (BIS*) to verify their time complexities.  

Bonus implementations include running BIS and BIS* in parallel threads if the correct $\sqrt{n}$ element is not found after 3 jumps, and comparing BIS against LA-BIS using threading.  


Part II: BSTs and HashingThe second part introduces a unified menu where the user can choose to load the dataset into a Balanced Search Tree or a Hashing structure.  

Balanced Search TreesThe application uses dynamic memory allocation for the BST, with preferred implementations being AVL, red-black, or (2,4) trees.  

BST ordered by Date: Supports inorder traversal, searching for a Cumulative value by Date, modifying the Cumulative value, and deleting a record.  

BST ordered by Cumulative value: Used to find the specific days that recorded the minimum and maximum Cumulative values.  

Hashing with Chaining  The hash function calculates the modulo of the sum of the ASCII codes of the characters in the Date string, divided by an odd number of buckets.  

Supports searching, modifying the Cumulative field, and deleting records based on the user-provided Date.  
