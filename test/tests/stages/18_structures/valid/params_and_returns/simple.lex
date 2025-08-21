-- Lexing ... OK
+
+
@@ Tokens @@
List[101]:
  struct
  identifier(pair)
  {
  int
  identifier(x)
  ;
  char
  identifier(y)
  ;
  }
  ;
  struct
  identifier(pair2)
  {
  double
  identifier(d)
  ;
  long
  identifier(l)
  ;
  }
  ;
  struct
  identifier(pair2)
  identifier(double_members)
  (
  struct
  identifier(pair)
  identifier(p)
  )
  {
  struct
  identifier(pair2)
  identifier(retval)
  =
  {
  identifier(p)
  .
  identifier(x)
  *
  const int(2)
  ,
  identifier(p)
  .
  identifier(y)
  *
  const int(2)
  }
  ;
  return
  identifier(retval)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  struct
  identifier(pair)
  identifier(arg)
  =
  {
  const int(1)
  ,
  const int(4)
  }
  ;
  struct
  identifier(pair2)
  identifier(result)
  =
  identifier(double_members)
  (
  identifier(arg)
  )
  ;
  if
  (
  identifier(result)
  .
  identifier(d)
  !=
  const double(2.0)
  ||
  identifier(result)
  .
  identifier(l)
  !=
  const int(8)
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
