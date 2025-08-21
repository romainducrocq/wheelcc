-- Lexing ... OK
+
+
@@ Tokens @@
List[105]:
  extern
  long
  identifier(arr)
  [
  const int(4)
  ]
  ;
  int
  identifier(double_each_element)
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
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(4)
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
  identifier(arr)
  [
  identifier(i)
  ]
  !=
  identifier(i)
  +
  const int(1)
  )
  {
  return
  identifier(i)
  +
  const int(1)
  ;
  }
  }
  identifier(double_each_element)
  (
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
  const int(4)
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
  identifier(arr)
  [
  identifier(i)
  ]
  !=
  (
  identifier(i)
  +
  const int(1)
  )
  *
  const int(2)
  )
  {
  return
  identifier(i)
  +
  const int(5)
  ;
  }
  }
  return
  const int(0)
  ;
  }
