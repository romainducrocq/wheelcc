-- Lexing ... OK
+
+
@@ Tokens @@
List[183]:
  int
  identifier(foo)
  (
  long
  identifier(a)
  ,
  int
  identifier(b)
  ,
  int
  identifier(c)
  ,
  int
  identifier(d)
  ,
  long
  identifier(e)
  ,
  int
  identifier(f)
  ,
  long
  identifier(g)
  ,
  int
  identifier(h)
  )
  {
  if
  (
  identifier(a)
  !=
  -
  const long(1l)
  )
  return
  const int(1)
  ;
  if
  (
  identifier(b)
  !=
  const int(2)
  )
  return
  const int(2)
  ;
  if
  (
  identifier(c)
  !=
  const int(0)
  )
  return
  const int(3)
  ;
  if
  (
  identifier(d)
  !=
  -
  const int(5)
  )
  return
  const int(4)
  ;
  if
  (
  identifier(e)
  !=
  -
  const long(101l)
  )
  return
  const int(5)
  ;
  if
  (
  identifier(f)
  !=
  -
  const int(123)
  )
  return
  const int(6)
  ;
  if
  (
  identifier(g)
  !=
  -
  const long(10l)
  )
  return
  const int(7)
  ;
  if
  (
  identifier(h)
  !=
  const int(1234)
  )
  return
  const int(8)
  ;
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
  int
  identifier(a)
  =
  -
  const int(1)
  ;
  long
  int
  identifier(b)
  =
  const int(4294967298)
  ;
  long
  identifier(c)
  =
  -
  const int(4294967296)
  ;
  long
  identifier(d)
  =
  const int(21474836475)
  ;
  int
  identifier(e)
  =
  -
  const int(101)
  ;
  long
  identifier(f)
  =
  -
  const int(123)
  ;
  int
  identifier(g)
  =
  -
  const int(10)
  ;
  long
  identifier(h)
  =
  -
  const int(9223372036854774574)
  ;
  return
  identifier(foo)
  (
  identifier(a)
  ,
  identifier(b)
  ,
  identifier(c)
  ,
  identifier(d)
  ,
  identifier(e)
  ,
  identifier(f)
  ,
  identifier(g)
  ,
  identifier(h)
  )
  ;
  }
