-- Lexing ... OK
+
+
@@ Tokens @@
List[82]:
  int
  identifier(target)
  (
  void
  )
  {
  unsigned
  char
  identifier(uc)
  =
  const int(250)
  ;
  int
  identifier(i)
  =
  identifier(uc)
  *
  const int(2)
  ;
  double
  identifier(d)
  =
  identifier(i)
  *
  const double(1000.)
  ;
  unsigned
  long
  identifier(ul)
  =
  identifier(d)
  /
  const double(6.0)
  ;
  identifier(d)
  =
  identifier(ul)
  +
  const double(5.0)
  ;
  long
  identifier(l)
  =
  -
  identifier(i)
  ;
  char
  identifier(c)
  =
  identifier(l)
  ;
  return
  identifier(d)
  +
  identifier(i)
  -
  identifier(c)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(target)
  (
  )
  !=
  const int(83826)
  )
  {
  return
  const int(1)
  ;
  }
  return
  const int(0)
  ;
  }
