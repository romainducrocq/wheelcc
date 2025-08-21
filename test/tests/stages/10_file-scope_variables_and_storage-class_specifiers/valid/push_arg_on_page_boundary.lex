-- Lexing ... OK
+
+
@@ Tokens @@
List[60]:
  extern
  int
  identifier(zed)
  ;
  int
  identifier(foo)
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
  )
  {
  return
  identifier(g)
  +
  const int(1)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  return
  identifier(foo)
  (
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
  identifier(zed)
  )
  ;
  }
