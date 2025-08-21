-- Lexing ... OK
+
+
@@ Tokens @@
List[116]:
  int
  identifier(f)
  (
  void
  )
  {
  static
  int
  identifier(i)
  =
  const int(0)
  ;
  static
  int
  identifier(j)
  =
  const int(0)
  ;
  static
  int
  identifier(k)
  =
  const int(1)
  ;
  static
  int
  identifier(l)
  =
  const int(48)
  ;
  identifier(i)
  +=
  const int(1)
  ;
  identifier(j)
  -=
  identifier(i)
  ;
  identifier(k)
  *=
  identifier(j)
  ;
  identifier(l)
  /=
  const int(2)
  ;
  if
  (
  identifier(i)
  !=
  const int(3)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(j)
  !=
  -
  const int(6)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(k)
  !=
  -
  const int(18)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(l)
  !=
  const int(6)
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
  int
  identifier(main)
  (
  void
  )
  {
  identifier(f)
  (
  )
  ;
  identifier(f)
  (
  )
  ;
  return
  identifier(f)
  (
  )
  ;
  }
