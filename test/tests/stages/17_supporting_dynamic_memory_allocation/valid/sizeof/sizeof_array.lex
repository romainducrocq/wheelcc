-- Lexing ... OK
+
+
@@ Tokens @@
List[95]:
  unsigned
  long
  identifier(sizeof_adjusted_param)
  (
  int
  identifier(arr)
  [
  const int(3)
  ]
  )
  {
  return
  sizeof
  identifier(arr)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(arr)
  [
  const int(3)
  ]
  ;
  if
  (
  sizeof
  identifier(arr)
  !=
  const int(12)
  )
  {
  return
  const int(1)
  ;
  }
  static
  long
  identifier(nested_arr)
  [
  const int(4)
  ]
  [
  const int(5)
  ]
  ;
  if
  (
  sizeof
  identifier(nested_arr)
  [
  const int(2)
  ]
  !=
  const int(40)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  sizeof
  string literal("Hello, World!")
  !=
  const int(14)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(sizeof_adjusted_param)
  (
  identifier(arr)
  )
  !=
  const int(8)
  )
  {
  return
  const int(4)
  ;
  }
  return
  const int(0)
  ;
  }
