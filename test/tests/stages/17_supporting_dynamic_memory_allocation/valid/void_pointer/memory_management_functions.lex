-- Lexing ... OK
+
+
@@ Tokens @@
List[250]:
  void
  *
  identifier(malloc)
  (
  unsigned
  long
  identifier(size)
  )
  ;
  void
  *
  identifier(realloc)
  (
  void
  *
  identifier(ptr)
  ,
  unsigned
  long
  identifier(size)
  )
  ;
  void
  *
  identifier(calloc)
  (
  unsigned
  long
  identifier(nmemb)
  ,
  unsigned
  long
  identifier(size)
  )
  ;
  void
  *
  identifier(aligned_alloc)
  (
  unsigned
  long
  identifier(alignment)
  ,
  unsigned
  long
  identifier(size)
  )
  ;
  void
  identifier(free)
  (
  void
  *
  identifier(ptr)
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  char
  *
  identifier(char_buffer)
  =
  identifier(malloc)
  (
  const int(50)
  )
  ;
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(50)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  identifier(char_buffer)
  [
  identifier(i)
  ]
  =
  identifier(i)
  ;
  }
  char
  *
  identifier(char_buffer2)
  =
  identifier(realloc)
  (
  identifier(char_buffer)
  ,
  const int(100)
  )
  ;
  identifier(char_buffer2)
  [
  const int(75)
  ]
  =
  const int(11)
  ;
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(50)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  if
  (
  identifier(char_buffer2)
  [
  identifier(i)
  ]
  !=
  identifier(i)
  )
  {
  return
  const int(1)
  ;
  }
  }
  if
  (
  identifier(char_buffer2)
  [
  const int(75)
  ]
  !=
  const int(11)
  )
  {
  return
  const int(2)
  ;
  }
  identifier(free)
  (
  identifier(char_buffer2)
  )
  ;
  double
  *
  identifier(double_buffer)
  =
  identifier(calloc)
  (
  const int(10)
  ,
  sizeof
  (
  double
  )
  )
  ;
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(10)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  if
  (
  identifier(double_buffer)
  [
  identifier(i)
  ]
  )
  {
  return
  const int(3)
  ;
  }
  }
  identifier(free)
  (
  identifier(double_buffer)
  )
  ;
  identifier(char_buffer)
  =
  identifier(aligned_alloc)
  (
  const int(256)
  ,
  const int(256)
  )
  ;
  if
  (
  (
  unsigned
  long
  )
  identifier(char_buffer)
  %
  const int(256)
  )
  {
  return
  const int(4)
  ;
  }
  identifier(free)
  (
  identifier(char_buffer)
  )
  ;
  return
  const int(0)
  ;
  }
