-- Lexing ... OK
+
+
@@ Tokens @@
List[132]:
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
  void
  *
  identifier(void_ptr)
  =
  identifier(calloc)
  (
  const int(3)
  ,
  sizeof
  (
  unsigned
  int
  )
  )
  ;
  unsigned
  int
  identifier(array)
  [
  const int(3)
  ]
  =
  {
  const int(1)
  ,
  const int(2)
  ,
  const int(3)
  }
  ;
  if
  (
  identifier(void_ptr)
  ==
  const int(0)
  )
  return
  const int(1)
  ;
  if
  (
  identifier(void_ptr)
  ==
  identifier(array)
  )
  return
  const int(2)
  ;
  if
  (
  !
  (
  identifier(void_ptr)
  !=
  identifier(array)
  )
  )
  return
  const int(3)
  ;
  static
  void
  *
  identifier(null_ptr)
  =
  const int(0)
  ;
  int
  *
  identifier(my_array)
  =
  identifier(null_ptr)
  ?
  identifier(void_ptr)
  :
  identifier(array)
  ;
  int
  identifier(array_element)
  =
  identifier(my_array)
  [
  const int(1)
  ]
  ;
  if
  (
  identifier(array_element)
  !=
  const int(2)
  )
  {
  return
  const int(4)
  ;
  }
  identifier(free)
  (
  identifier(void_ptr)
  )
  ;
  return
  const int(0)
  ;
  }
