#include <stdio.h>
 
int main()
{
  long long array[] = {
      4, 5, 6, 2, 4,
      1, 6, 8, 9, 0,
      4, 8, 2, 3, 9,
      7, 8, 0, 1, 2
  };

  long long n = 20;
  long long c, d, swap;
 
  for (c = 0 ; c < ( n - 1 ); c++)
  {
    for (d = 0 ; d < n - c - 1; d++)
    {
      if (array[d] > array[d+1]) /* For decreasing order use < */
      {
        swap       = array[d];
        array[d]   = array[d+1];
        array[d+1] = swap;
      }
    }
  }
 
  printf("Sorted list in ascending order:\n");
 
  for ( c = 0 ; c < n ; c++ )
     printf("%lld\n", array[c]);
 
  return 0;
}
