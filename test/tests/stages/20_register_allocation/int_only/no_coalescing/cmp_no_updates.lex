-- Lexing ... OK
+
+
@@ Tokens @@
List[297]:
  int
  identifier(glob0)
  =
  const int(0)
  ;
  int
  identifier(glob1)
  =
  const int(1)
  ;
  int
  identifier(glob2)
  =
  const int(2)
  ;
  int
  identifier(glob3)
  =
  const int(3)
  ;
  int
  identifier(glob4)
  =
  const int(4)
  ;
  int
  identifier(increment_globals)
  (
  void
  )
  {
  identifier(glob0)
  =
  identifier(glob0)
  +
  const int(1)
  ;
  identifier(glob1)
  =
  identifier(glob1)
  +
  const int(1)
  ;
  identifier(glob2)
  =
  identifier(glob2)
  +
  const int(1)
  ;
  identifier(glob3)
  =
  identifier(glob3)
  +
  const int(1)
  ;
  identifier(glob4)
  =
  identifier(glob4)
  +
  const int(1)
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(validate)
  (
  int
  identifier(zero)
  ,
  int
  identifier(one)
  ,
  int
  identifier(two)
  ,
  int
  identifier(three)
  ,
  int
  identifier(four)
  )
  ;
  int
  identifier(target)
  (
  void
  )
  {
  int
  identifier(a)
  =
  identifier(glob0)
  ;
  int
  identifier(b)
  =
  identifier(glob1)
  ;
  int
  identifier(c)
  =
  identifier(glob2)
  ;
  int
  identifier(d)
  =
  identifier(glob3)
  ;
  int
  identifier(e)
  =
  identifier(glob4)
  ;
  identifier(increment_globals)
  (
  )
  ;
  int
  identifier(x)
  =
  identifier(a)
  ;
  if
  (
  identifier(a)
  >
  identifier(b)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(b)
  )
  {
  identifier(x)
  =
  const int(100)
  ;
  }
  identifier(increment_globals)
  (
  )
  ;
  return
  identifier(validate)
  (
  identifier(x)
  ,
  identifier(b)
  ,
  identifier(c)
  ,
  identifier(d)
  ,
  identifier(e)
  )
  ;
  }
  int
  identifier(validate)
  (
  int
  identifier(hundred)
  ,
  int
  identifier(one)
  ,
  int
  identifier(two)
  ,
  int
  identifier(three)
  ,
  int
  identifier(four)
  )
  {
  if
  (
  identifier(glob0)
  !=
  const int(2)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(glob1)
  !=
  const int(3)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(glob2)
  !=
  const int(4)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(glob3)
  !=
  const int(5)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(glob4)
  !=
  const int(6)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(hundred)
  !=
  const int(100)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(one)
  !=
  const int(1)
  )
  {
  return
  const int(8)
  ;
  }
  if
  (
  identifier(two)
  !=
  const int(2)
  )
  {
  return
  const int(9)
  ;
  }
  if
  (
  identifier(three)
  !=
  const int(3)
  )
  {
  return
  const int(10)
  ;
  }
  if
  (
  identifier(four)
  !=
  const int(4)
  )
  {
  return
  const int(11)
  ;
  }
  return
  const int(0)
  ;
  }
