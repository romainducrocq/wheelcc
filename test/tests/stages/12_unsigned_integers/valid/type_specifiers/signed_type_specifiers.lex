-- Lexing ... OK
+
+
@@ Tokens @@
List[122]:
  static
  int
  identifier(i)
  ;
  signed
  extern
  identifier(i)
  ;
  int
  static
  signed
  identifier(i)
  =
  const int(5)
  ;
  signed
  int
  static
  identifier(i)
  ;
  long
  signed
  identifier(l)
  ;
  long
  identifier(l)
  =
  const int(7)
  ;
  int
  long
  identifier(l)
  ;
  signed
  long
  int
  identifier(l)
  ;
  int
  identifier(main)
  (
  void
  )
  {
  int
  signed
  extern
  identifier(i)
  ;
  extern
  signed
  long
  identifier(l)
  ;
  if
  (
  identifier(i)
  !=
  const int(5)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(l)
  !=
  const int(7)
  )
  {
  return
  const int(2)
  ;
  }
  int
  identifier(counter)
  =
  const int(0)
  ;
  for
  (
  signed
  int
  identifier(index)
  =
  const int(10)
  ;
  identifier(index)
  >
  const int(0)
  ;
  identifier(index)
  =
  identifier(index)
  -
  const int(1)
  )
  {
  identifier(counter)
  =
  identifier(counter)
  +
  const int(1)
  ;
  }
  if
  (
  identifier(counter)
  !=
  const int(10)
  )
  {
  return
  const int(3)
  ;
  }
  return
  const int(0)
  ;
  }
