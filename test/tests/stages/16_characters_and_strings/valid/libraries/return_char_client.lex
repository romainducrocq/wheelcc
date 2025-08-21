-- Lexing ... OK
+
+
@@ Tokens @@
List[259]:
  char
  identifier(return_char)
  (
  void
  )
  ;
  signed
  char
  identifier(return_schar)
  (
  void
  )
  ;
  unsigned
  char
  identifier(return_uchar)
  (
  void
  )
  ;
  int
  identifier(main)
  (
  void
  )
  {
  char
  identifier(char_array)
  [
  const int(3)
  ]
  =
  {
  const int(121)
  ,
  -
  const int(122)
  ,
  -
  const int(3)
  }
  ;
  char
  identifier(retval_c)
  =
  identifier(return_char)
  (
  )
  ;
  char
  identifier(char_array2)
  [
  const int(3)
  ]
  =
  {
  -
  const int(5)
  ,
  const int(88)
  ,
  -
  const int(100)
  }
  ;
  signed
  char
  identifier(retval_sc)
  =
  identifier(return_schar)
  (
  )
  ;
  char
  identifier(char_array3)
  [
  const int(3)
  ]
  =
  {
  const int(10)
  ,
  const int(11)
  ,
  const int(12)
  }
  ;
  unsigned
  char
  identifier(retval_uc)
  =
  identifier(return_uchar)
  (
  )
  ;
  char
  identifier(char_array4)
  [
  const int(2)
  ]
  =
  {
  -
  const int(5)
  ,
  -
  const int(6)
  }
  ;
  if
  (
  identifier(char_array)
  [
  const int(0)
  ]
  !=
  const int(121)
  ||
  identifier(char_array)
  [
  const int(1)
  ]
  !=
  -
  const int(122)
  ||
  identifier(char_array)
  [
  const int(2)
  ]
  !=
  -
  const int(3)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(retval_c)
  !=
  -
  const int(10)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(char_array2)
  [
  const int(0)
  ]
  !=
  -
  const int(5)
  ||
  identifier(char_array2)
  [
  const int(1)
  ]
  !=
  const int(88)
  ||
  identifier(char_array2)
  [
  const int(2)
  ]
  !=
  -
  const int(100)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(retval_sc)
  !=
  -
  const int(10)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(char_array3)
  [
  const int(0)
  ]
  !=
  const int(10)
  ||
  identifier(char_array3)
  [
  const int(1)
  ]
  !=
  const int(11)
  ||
  identifier(char_array3)
  [
  const int(2)
  ]
  !=
  const int(12)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(retval_uc)
  !=
  const int(246)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(char_array4)
  [
  const int(0)
  ]
  !=
  -
  const int(5)
  ||
  identifier(char_array4)
  [
  const int(1)
  ]
  !=
  -
  const int(6)
  )
  {
  return
  const int(7)
  ;
  }
  return
  const int(0)
  ;
  }
