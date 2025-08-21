-- Lexing ... OK
+
+
@@ Tokens @@
List[57]:
  int
  identifier(test_sum)
  (
  int
  identifier(a)
  ,
  int
  identifier(b)
  ,
  int
  identifier(c)
  ,
  long
  identifier(d)
  ,
  int
  identifier(e)
  ,
  long
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
  identifier(d)
  +
  identifier(f)
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
