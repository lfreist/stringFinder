# How to optimize search

1. GNU grep increases performance by not reading lines:
   1. do not look at avery byte -> new line chars can not be read
   2. only look for new line chars when a match was found