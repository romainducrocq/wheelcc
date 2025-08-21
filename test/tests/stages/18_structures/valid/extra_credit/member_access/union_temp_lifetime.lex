-- Lexing ... OK
+
+
@@ Tokens @@
List[124]:
  struct
  identifier(has_char_array)
  {
  char
  identifier(arr)
  [
  const int(8)
  ]
  ;
  }
  ;
  union
  identifier(has_array)
  {
  long
  identifier(l)
  ;
  struct
  identifier(has_char_array)
  identifier(s)
  ;
  }
  ;
  int
  identifier(get_flag)
  (
  void
  )
  {
  static
  int
  identifier(flag)
  =
  const int(0)
  ;
  identifier(flag)
  =
  !
  identifier(flag)
  ;
  return
  identifier(flag)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  union
  identifier(has_array)
  identifier(union1)
  =
  {
  const long(9876543210l)
  }
  ;
  union
  identifier(has_array)
  identifier(union2)
  =
  {
  const long(1234567890l)
  }
  ;
  if
  (
  (
  identifier(get_flag)
  (
  )
  ?
  identifier(union1)
  :
  identifier(union2)
  )
  .
  identifier(s)
  .
  identifier(arr)
  [
  const int(0)
  ]
  !=
  -
  const int(22)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  (
  identifier(get_flag)
  (
  )
  ?
  identifier(union1)
  :
  identifier(union2)
  )
  .
  identifier(s)
  .
  identifier(arr)
  [
  const int(0)
  ]
  !=
  -
  const int(46)
  )
  {
  return
  const int(2)
  ;
  }
  return
  const int(0)
  ;
  }
