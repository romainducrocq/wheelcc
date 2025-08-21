-- Lexing ... OK
+
+
@@ Tokens @@
List[86]:
  int
  identifier(test_sum)
  (
  long
  identifier(a)
  ,
  long
  identifier(b)
  ,
  int
  identifier(c)
  ,
  int
  identifier(d)
  ,
  int
  identifier(e)
  ,
  int
  identifier(f)
  ,
  int
  identifier(g)
  ,
  int
  identifier(h)
  ,
  long
  identifier(i)
  )
  {
  if
  (
  identifier(a)
  +
  identifier(b)
  <
  const long(100l)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(i)
  <
  const long(100l)
  )
  return
  const int(2)
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
  return
  identifier(test_sum)
  (
  const long(34359738368l)
  ,
  const long(34359738368l)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const int(0)
  ,
  const long(34359738368l)
  )
  ;
  }
